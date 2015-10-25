/******************************************************************************
 * Copyright (c) 2014-2015 Leandro T. C. Melo (ltcmelo@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *****************************************************************************/

#include "Semantic/Binder.h"
#include "Semantic/DeclAttrs.h"
#include "Semantic/Environment.h"
#include "Semantic/Precision.h"
#include "Semantic/Program.h"
#include "Semantic/Sanitizer.h"
#include "Semantic/Symbol.h"
#include "Semantic/SymbolCast.h"
#include "Semantic/Signedness.h"
#include "Semantic/Type.h"
#include "Semantic/TypeCast.h"
#include "Semantic/TypeQuals.h"
#include "Ast/Ast.h"
#include "Ast/AstLocator.h"
#include "Ast/AstVariety.h"
#include "Common/Assert.h"
#include "Common/FileInfo.h"
#include "Parsing/Factory.h"
#include "Parsing/Language.h"
#include "Parsing/Diagnostic.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/SourceLoc.h"
#include "Parsing/Syntax.h"
#include "Parsing/Token.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <stack>
#include <utility>

#define ENSURE_NAME_AVAILABLE \
    UAISO_ASSERT(!P->declId_.empty(), return Abort)

#define ENSURE_NONEMPTY_TYPE_STACK \
    UAISO_ASSERT(!P->declTy_.empty(), return Abort)

#define ENSURE_TOP_TYPE_IS(TYPE) \
    ENSURE_NONEMPTY_TYPE_STACK; \
    UAISO_ASSERT(P->declTy_.top()->kind() == Type::Kind::TYPE, return Abort)

#define ENSURE_NONEMPTY_SYMBOL_STACK \
    UAISO_ASSERT(!P->sym_.empty(), return Abort)

#define ENSURE_TOP_SYMBOL_IS_DECL \
    ENSURE_NONEMPTY_SYMBOL_STACK; \
    UAISO_ASSERT(isDeclSymbol(P->sym_.top().get()), return Abort)

#define ENSURE_TOP_SYMBOL_IS_TYPEDECL \
    ENSURE_NONEMPTY_SYMBOL_STACK; \
    UAISO_ASSERT(isTypeSymbol(P->sym_.top().get()), return Abort)

#define ENSURE_TOP_SYMBOL_IS_VALUEDECL \
    ENSURE_NONEMPTY_SYMBOL_STACK; \
    UAISO_ASSERT(isValueSymbol(P->sym_.top().get()), return Abort)

#define ENSURE_TOP_SYMBOL_IS(SYMBOL) \
    ENSURE_NONEMPTY_SYMBOL_STACK; \
    UAISO_ASSERT(P->sym_.top()->kind() == Symbol::Kind::SYMBOL, return Abort);

using namespace uaiso;

namespace {

/*!
 * \brief The CollectName class
 *
 * Collect names in the form of string literals or simple names (which
 * might be from an identifier expr).
 */
class PickupName final : public AstVisitor<PickupName>
{
public:
    PickupName(const LexemeMap* lexemes)
        : lexemes_(lexemes)
    {}

    std::vector<const Lexeme*> process(ExprAst* ast)
    {
        prepare();
        traverseExpr(ast);
        return ids_;
    }

    std::vector<const Lexeme*> process(NameAst* ast)
    {
        prepare();
        traverseName(ast);
        return ids_;
    }

private:
    friend class AstVisitor<PickupName>;

    const LexemeMap* lexemes_;
    std::vector<const Lexeme*> ids_;
    bool firstOnly_;

    void prepare()
    {
        ids_.clear();
    }

    VisitResult pushName(const Lexeme* lexeme)
    {
        if (!lexeme)
            return Abort;

        ids_.push_back(lexeme);
        return Continue;
    }

    VisitResult visitSimpleName(SimpleNameAst* ast)
    {
        auto name = lexemes_->find<Ident>(ast->nameLoc_.fileName_,
                                               ast->nameLoc_.lineCol());
        return pushName(name);
    }

    VisitResult visitGenName(GenNameAst* ast)
    {
        auto name = lexemes_->find<Ident>(ast->genLoc_.fileName_,
                                               ast->genLoc_.lineCol());
        return pushName(name);
    }

    VisitResult visitStrLitExpr(StrLitExprAst* ast)
    {
        auto name = lexemes_->find<StrLit>(ast->litLoc_.fileName_,
                                           ast->litLoc_.lineCol());
        return pushName(name);
    }
};

} // anonymous

/*!
 * \brief The uaiso::Binder::BinderImpl struct
 *
 * Binder implementation.
 */
struct uaiso::Binder::BinderImpl
{
    BinderImpl(Factory* factory)
        : lexemes_(nullptr)
        , tokens_(nullptr)
        , ownedBlocks_(0)
        , sanitizer_(factory->makeSanitizer())
        , locator_(factory->makeAstLocator())
        , syntax_(factory->makeSyntax())
        , reports_(nullptr)
    {}

    void enterSubEnv()
    {
        env_ = env_.createSubEnv();
    }

    Environment leaveEnv()
    {
        Environment previousEnv = env_;
        env_ = env_.outerEnv();
        return previousEnv;
    }

    template <class TypeT = Type>
    std::unique_ptr<TypeT> popDeclType()
    {
        std::unique_ptr<TypeT> ty(static_cast<TypeT*>(declTy_.top().release()));
        declTy_.pop();
        return ty;
    }

    template <class SymbolT>
    std::unique_ptr<SymbolT> popSymbol()
    {
        std::unique_ptr<SymbolT> sym(static_cast<SymbolT*>(sym_.top().release()));
        sym_.pop();
        return sym;
    }

    template <class... Args>
    void report(Args&&... args)
    {
        if (reports_)
            reports_->add(std::forward<Args>(args)...);
    }

    const LexemeMap* lexemes_; //!< Lexeme map of all AST locations.
    const TokenMap* tokens_;   //!< Token map of all AST locations.

    //! File name corresponding the to given AST.
    std::string fileName_;

    //! Environment we're currently in.
    Environment env_;

    //! Owned blocks counter (see block stmt travesal).
    size_t ownedBlocks_;

    //! Decl identifier under process, composed by the traversed AST names.
    std::vector<const Ident*> declId_;

    //! Type of the decl under process.
    std::stack<std::unique_ptr<Type>> declTy_;

    //! Symbol under process.
    std::stack<std::unique_ptr<Symbol>> sym_;

    //! Language-specific AST sanitizer.
    std::unique_ptr<const Sanitizer> sanitizer_;

    //! Language-specific AST locator.
    std::unique_ptr<const AstLocator> locator_;

    //! Language-specific syntax.
    std::unique_ptr<const Syntax> syntax_;

    //! Diagnostic reports collected.
    DiagnosticReports* reports_;

    //! Module being constructed.
    std::unique_ptr<Program> module_;
};

Binder::Binder(Factory* factory)
    : P(new BinderImpl(factory))
{}

Binder::~Binder()
{}

void Binder::setLexemes(const LexemeMap* lexemes)
{
    P->lexemes_ = lexemes;
}

void Binder::setTokens(const TokenMap* tokens)
{
    P->tokens_ = tokens;
}

void Binder::collectDiagnostics(DiagnosticReports* reports)
{
    P->reports_ = reports;
}

std::unique_ptr<Program> Binder::bind(ProgramAst* ast,
                                      const std::string& fullFileName)
{
    UAISO_ASSERT(ast, return std::unique_ptr<Program>());
    UAISO_ASSERT(!fullFileName.empty(), return std::unique_ptr<Program>());

    P->fileName_.assign(fullFileName);
    P->module_.reset(new Program(P->fileName_));

    P->enterSubEnv();

    VisitResult result = traverseDecl(ast->module_.get());
    if (result == Continue) {
        result = traverseDecl(ast->package_.get());
        if (result == Continue) {
            for (auto decl : *ast->decls_.get()) {
                result = traverseDecl(decl);
                if (result != Continue)
                    break;
            }
            // If everything finishes alright, stacks must be empty.
            UAISO_ASSERT(P->declTy_.empty(), {});
            UAISO_ASSERT(P->sym_.empty(), {});
        }
    }

    P->module_->setEnv(P->leaveEnv());

    ast->program_ = P->module_.get();

    return std::move(P->module_);
}

    //-------------//
    //--- Names ---//
    //-------------//

Binder::VisitResult Binder::visitSimpleName(SimpleNameAst* ast)
{
    const SourceLoc& loc = ast->nameLoc_;
    auto ident = P->lexemes_->find<Ident>(loc.fileName_, loc.lineCol());
    UAISO_ASSERT(ident, return Abort, "SourceLoc:", loc);
    P->declId_.push_back(ident);

    return Continue;
}

Binder::VisitResult Binder::visitTemplateInstName(TemplateInstNameAst* ast)
{
    // TODO: Gather template args.
    return Continue;
}

Binder::VisitResult Binder::visitGenName(GenNameAst* ast)
{
    return Continue;
}

Binder::VisitResult Binder::visitNestedName(NestedNameAst* ast)
{
    // Empty
    return Continue;
}

Binder::VisitResult Binder::visitCompletionName(CompletionNameAst* ast)
{
    return Continue;
}

    //------------------//
    //--- Specifiers ---//
    //------------------//

Binder::VisitResult Binder::visitDecoratedSpec(DecoratedSpecAst* ast)
{
    // Empty
    return Continue;
}

Binder::VisitResult Binder::visitVoidSpec(VoidSpecAst* ast)
{
    P->declTy_.emplace(new VoidType);

    return Continue;
}

Binder::VisitResult Binder::visitInferredSpec(InferredSpecAst* ast)
{
    P->declTy_.emplace(new InferredType);

    return Continue;
}

Binder::VisitResult Binder::visitBuiltinSpec(BuiltinSpecAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    std::unique_ptr<Type> ty;
    switch (tk) {
    case TK_VOID:
        ty.reset(new VoidType);
        break;

    case TK_BOOL:
        ty.reset(new BoolType);
        break;

    // Integral - with certain assumptions (not always correct) about precision.
    case TK_BYTE:
    case TK_CHAR:
    case TK_INT8:
        ty.reset(new IntType(Signedness::Signed, Precision::BitSize8));
        break;
    case TK_CHAR_UTF16:
    case TK_INT16:
        ty.reset(new IntType(Signedness::Signed, Precision::BitSize16));
        break;
    case TK_CHAR_UTF32:
    case TK_INT32:
    case TK_INT:
    case TK_RUNE:
        ty.reset(new IntType(Signedness::Signed, Precision::BitSize32));
        break;
    case TK_INT64:
        ty.reset(new IntType(Signedness::Signed, Precision::BitSize64));
        break;
    case TK_CENT:
        ty.reset(new IntType(Signedness::Signed, Precision::BitSize128));
        break;
    case TK_UBYTE:
    case TK_UINT8:
        ty.reset(new IntType(Signedness::Unsigned, Precision::BitSize8));
        break;
    case TK_UINT16:
        ty.reset(new IntType(Signedness::Unsigned, Precision::BitSize16));
        break;
    case TK_UINT32:
    case TK_UINT:
        ty.reset(new IntType(Signedness::Unsigned, Precision::BitSize32));
        break;
    case TK_UINT64:
        ty.reset(new IntType(Signedness::Unsigned, Precision::BitSize64));
        break;
    case TK_UCENT:
        ty.reset(new IntType(Signedness::Unsigned, Precision::BitSize128));
        break;

    // Floating point
    case TK_FLOAT32:
        ty.reset(new FloatType(Precision::BitSize32));
        break;
    case TK_FLOAT64:
        ty.reset(new FloatType(Precision::BitSize64));
        break;
    case TK_REAL:
        ty.reset(new FloatType(Precision::BitSize128));
        break;
    case TK_COMPLEX_FLOAT32:
    case TK_COMPLEX_FLOAT64:
    case TK_COMPLEX_REAL:
        // TODO: Complex...
        ty.reset(new FloatType(Precision::BitSize128));
        break;
    case TK_IMAG_FLOAT64:
    case TK_IMAG_FLOAT32:
    case TK_IMAG_REAL:
        // TODO: Imaginary...
        ty.reset(new FloatType(Precision::BitSize128));
        break;

    default:
        UAISO_ASSERT(false, {});
        break;
    }
    UAISO_ASSERT(ty.get(), return Abort);

    P->declTy_.push(std::move(ty));

    return Continue;
}

Binder::VisitResult Binder::visitPtrSpec(PtrSpecAst* ast)
{
    ENSURE_NONEMPTY_TYPE_STACK;
    P->declTy_.emplace(new PtrType(P->popDeclType<>()));

    return Continue;
}

Binder::VisitResult Binder::visitChanSpec(ChanSpecAst* ast)
{
    ENSURE_NONEMPTY_TYPE_STACK;
    std::unique_ptr<ChanType> ty(new ChanType(P->popDeclType<>()));
    ty->setVariety(ast->variety());

    P->declTy_.push(std::move(ty));

    return Continue;
}

Binder::VisitResult Binder::traverseArraySpec(ArraySpecAst* ast)
{
    VIS_CALL(traverseSpec(ast->baseSpec_.get()));
    ENSURE_NONEMPTY_TYPE_STACK;
    std::unique_ptr<ArrayType> array(new ArrayType(P->popDeclType<>()));
    array->setVariety(ast->variety());

    if (ast->exprOrSpec_) {
        if (ast->exprOrSpec_->isSpec()) {
            VIS_CALL(traverseSpec(Spec_Cast(ast->exprOrSpec_.get())));
            ENSURE_NONEMPTY_TYPE_STACK;
            array->setKeyType(P->popDeclType<>());
        }
    } else {
        if (ast->variety() == ArrayVariety::Plain)
            array->setKeyType(std::unique_ptr<Type>(new IntType));
    }

    P->declTy_.push(std::move(array));

    return Continue;
}

Binder::VisitResult Binder::visitOpaqueSpec(OpaqueSpecAst* ast)
{
    return traverseSpec(ast->baseSpec_.get());
}

Binder::VisitResult Binder::visitTypeofSpec(TypeofSpecAst* ast)
{
    P->declTy_.emplace(new InferredType);

    return Continue;
}

Binder::VisitResult Binder::traverseRecordSpec(RecordSpecAst* ast)
{
    P->declTy_.emplace(new RecordType);
    P->enterSubEnv();
    VIS_CALL(Base::traverseRecordSpec(ast));

    ENSURE_TOP_TYPE_IS(Record);
    RecordType* ty = RecordType_Cast(P->declTy_.top().get());
    ty->setVariety(ast->variety());
    ty->setEnv(P->leaveEnv());

    return Continue;
}

Binder::VisitResult Binder::traverseFuncSpec(FuncSpecAst* ast)
{
    UAISO_ASSERT(ast->param_.get(), return Continue);
    UAISO_ASSERT(ast->result_.get(), return Continue,
                 "result spec required, make it `VoidSpecAst` if empty");

    P->declTy_.emplace(new FuncType);

    // TODO: Assign function's signature as ty.

    VIS_CALL(traverseDecl(ast->param_.get()));
    VIS_CALL(traverseDecl(ast->result_.get()));

    return Continue;
}

Binder::VisitResult Binder::traverseNamedSpec(NamedSpecAst* ast)
{
    VIS_CALL(Base::traverseNamedSpec(ast));

    ENSURE_NAME_AVAILABLE;
    P->declTy_.emplace(new ElaborateType(P->declId_.back()));

    return Continue;
}

    //------------------//
    //--- Attributes ---//
    //------------------//

Binder::VisitResult Binder::visitVisibilityAttr(VisibilityAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    DeclSymbol* sym = DeclSymbol_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateVisibility(sym->kind(), tk)) {
        P->report(Diagnostic::UnexpectedVisibilityAttribute, loc);
        return Continue;
    }

    if (sym->visibility() != TypeSymbol::Visibility::Unknown) {
        P->report(Diagnostic::MultipleVisibilityAttributes, loc);
        return Continue;
    }

    switch (tk) {
    case TK_PACKAGE:
        sym->setVisibility(TypeSymbol::Visibility::Package);
        break;
    case TK_PRIVATE:
        sym->setVisibility(TypeSymbol::Visibility::Private);
        break;
    case TK_PROTECTED:
        sym->setVisibility(TypeSymbol::Visibility::Protected);
        break;
    case TK_PUBLIC:
        sym->setVisibility(TypeSymbol::Visibility::Public);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitAnnotAttr(AnnotAttrAst* ast)
{
    // TODO...
    return Continue;
}

Binder::VisitResult Binder::visitDeclAttr(DeclAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    DeclSymbol* sym = TypeSymbol_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateDeclAttr(sym->kind(), tk)) {
        P->report(Diagnostic::UnexpectedDeclarationAttribute, loc);
        return Continue;
    }

    if (!P->sanitizer_->checkDeclAttrCoherence(sym, tk)) {
        P->report(Diagnostic::IncompatibleDeclarationAttributes, loc);
        return Continue;
    }

    DeclAttrFlags flags = sym->declAttrs();
    switch (tk) {
    case TK_ABSTRACT:
        if (flags & DeclAttrFlag::Abstract)
            P->report(Diagnostic::DuplicateDeclarationAttribute, loc);
        else
            sym->setDeclAttrs(flags | DeclAttrFlag::Abstract);
        break;
    case TK_FINAL:
        if (flags & DeclAttrFlag::Final)
            P->report(Diagnostic::DuplicateDeclarationAttribute, loc);
        else
            sym->setDeclAttrs(flags | DeclAttrFlag::Final);
        break;
    case TK_NOTHROW:
        if (flags & DeclAttrFlag::Nothrow)
            P->report(Diagnostic::DuplicateDeclarationAttribute, loc);
        else
            sym->setDeclAttrs(flags | DeclAttrFlag::Nothrow);
        break;
    case TK_OVERRIDE:
        if (flags & DeclAttrFlag::Override)
            P->report(Diagnostic::DuplicateDeclarationAttribute, loc);
        else
            sym->setDeclAttrs(flags | DeclAttrFlag::Override);
        break;
    case TK_PURE:
        if (flags & DeclAttrFlag::Pure)
            P->report(Diagnostic::DuplicateDeclarationAttribute, loc);
        else
            sym->setDeclAttrs(flags | DeclAttrFlag::Pure);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitAutoAttr(AutoAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    DeclSymbol* sym = DeclSymbol_Cast(P->sym_.top().get());

    switch (tk) {
    case TK_AUTO:
        if (sym->isMarkedAuto())
            P->report(Diagnostic::DuplicateDeclarationAttribute, loc);
        else
            sym->markAsAuto();
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitCodegenAttr(CodegenAttrAst* ast)
{
    // TODO...
    return Continue;
}

Binder::VisitResult Binder::visitParamDirAttr(ParamDirAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS(Param);
    Param* param = Param_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateDirection(param->kind(), tk)) {
        P->report(Diagnostic::UnexpectedDirectionAttribute, loc);
        return Continue;
    }

    if (param->direction() != Param::Direction::Unknown) {
        P->report(Diagnostic::MultipleDirectionAttributes, loc);
        return Continue;
    }

    switch (tk) {
    case TK_IN:
        param->setDirection(Param::Direction::In);
        break;
    case TK_OUT:
        param->setDirection(Param::Direction::Out);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitEvalStrategyAttr(EvalStrategyAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS(Param);
    Param* param = Param_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateEvalStrategy(param->kind(), tk)) {
        P->report(Diagnostic::UnexpectedEvaluationStrategyAttribute, loc);
        return Continue;
    }

    if (param->evalStrategy() != Param::EvalStrategy::Unknown) {
        P->report(Diagnostic::MultipleEvaluationStrategyAttributes, loc);
        return Continue;
    }

    switch (tk) {
    case TK_LAZY:
        param->setEvalStrategy(Param::EvalStrategy::Lazy);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitStorageClassAttr(StorageClassAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    DeclSymbol* sym = DeclSymbol_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateStorageClass(sym->kind(), tk)) {
        P->report(Diagnostic::UnexpectedStorageClassAttribute, loc);
        return Continue;
    }

    if (sym->storage() != ValueSymbol::Storage::Unknown) {
        P->report(Diagnostic::MultipleStorageSpecifiers, loc);
        return Continue;
    }

    switch (tk) {
    case TK_SCOPE:
        sym->setStorage(ValueSymbol::Storage::Scope);
        break;
    case TK_REF:
        sym->setStorage(ValueSymbol::Storage::Ref);
        break;
    case TK_STATIC:
        sym->setStorage(ValueSymbol::Storage::Static);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitLinkageAttr(LinkageAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_VALUEDECL;
    ValueSymbol* sym = ValueSymbol_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateLinkage(sym->kind(), tk)) {
        P->report(Diagnostic::UnexpectedLinkageAttribute, loc);
        return Continue;
    }

    if (sym->linkage() != ValueSymbol::Linkage::Unknown) {
        P->report(Diagnostic::MultipleLinkageSpecifiers, loc);
        return Continue;
    }

    switch (tk) {
    case TK_EXTERN:
        sym->setLinkage(ValueSymbol::Linkage::Extern);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitTypeQualAttr(TypeQualAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc_;
    Token tk = P->tokens_->find(loc.fileName_, loc.lineCol());

    ENSURE_NONEMPTY_TYPE_STACK;
    Type* ty = P->declTy_.top().get();

    if (!P->sanitizer_->validateTypeQual(ty->kind(), tk)) {
        P->report(Diagnostic::UnexpectedTypeQualifier, loc);
        return Continue;
    }

    if (!P->sanitizer_->checkTypeQualCoherence(ty, tk)) {
        P->report(Diagnostic::IncompatibleTypeQualifiers, loc);
        return Continue;
    }

    TypeQualFlags flags = ty->typeQuals();
    switch (tk) {
    case TK_CONST:
        if (flags & TypeQualFlag::Const)
            P->report(Diagnostic::DuplicateTypeQualifier, loc);
        else
            ty->setTypeQuals(flags | TypeQualFlag::Const);
        break;
    case TK_IMMUTABLE:
        if (flags & TypeQualFlag::Immutable)
            P->report(Diagnostic::DuplicateTypeQualifier, loc);
        else
            ty->setTypeQuals(flags | TypeQualFlag::Immutable);
        break;
    case TK_SHARED:
        if (flags & TypeQualFlag::Shared)
            P->report(Diagnostic::DuplicateTypeQualifier, loc);
        else
            ty->setTypeQuals(flags | TypeQualFlag::Shared);
        break;
    case TK_VOLATILE:
        if (flags & TypeQualFlag::Volatile)
            P->report(Diagnostic::DuplicateTypeQualifier, loc);
        else
            ty->setTypeQuals(flags | TypeQualFlag::Volatile);
        break;
    case TK_INOUT:
        break; // TODO: Consider um nome melhor para esse token (inout eh de D).
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

    //--------------------//
    //--- Declarations ---//
    //--------------------//

Binder::VisitResult Binder::traverseAliasDecl(AliasDeclAst* ast)
{
    VIS_CALL(Base::traverseAliasDecl(ast));

    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Alias> alias(new Alias(P->declId_.back()));
    alias->setSourceLoc(fullLoc(ast, P->locator_.get()));

    ENSURE_NONEMPTY_TYPE_STACK;
    alias->setType(P->popDeclType<>());

    P->env_.insertType(std::move(alias));

    return Continue;
}

Binder::VisitResult Binder::traverseBlockDecl(BlockDeclAst* ast)
{
    // TODO: Apply the attributes to each declaration individually.
    for (auto decl : *ast->decls_.get())
        VIS_CALL(traverseDecl(decl));

    return Continue;
}

Binder::VisitResult Binder::traverseSelectiveDecl(SelectiveDeclAst* ast)
{
    VIS_CALL(traverseExpr(ast->expr_.get()));

    // TODO: Pick the right clause when possible (at least for simple exprs).
    VIS_CALL(traverseDecl(ast->ifDecl_.get()));

    return Continue;
}

Binder::VisitResult Binder::traverseConstraintDecl(ConstraintDeclAst* ast)
{
    // TODO...
    return Continue;
}

Binder::VisitResult Binder::traverseVersionDecl(VersionDeclAst* ast)
{
    // TODO...
    return Continue;
}

Binder::VisitResult Binder::traverseForwardDecl(ForwardDeclAst* ast)
{
    VIS_CALL(Base::traverseForwardDecl(ast));

    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Placeholder> holder(new Placeholder(P->declId_.back()));
    holder->setSourceLoc(fullLoc(ast, P->locator_.get()));

    P->env_.insertType(std::move(holder));

    return Continue;
}

Binder::VisitResult Binder::traverseImportModuleDecl(ImportModuleDeclAst* ast)
{
    // An import is not injected into the environment during binding. But
    // it's still evaluated so the module and given namespace are collected.

    UAISO_ASSERT(ast->expr_.get(), return Abort);
    PickupName pickupName(P->lexemes_);
    const auto& lexemes = pickupName.process(ast->expr_.get());

    if (lexemes.empty()) {
        P->report(Diagnostic::UnresolvedModule, ast->expr_.get(), P->locator_.get());
        return Continue;
    }

    const std::string& moduleName =
            joinLexemes(lexemes, P->syntax_->packageSeparator());

    const Ident* localName = nullptr;
    if (ast->localName_) {
        const auto& local = pickupName.process(ast->localName_.get());
        if (local.empty() || local.size() > 1) {
            P->report(Diagnostic::UnresolvedModule, ast->localName_.get(),
                      P->locator_.get());
        } else {
            localName = ConstIdent_Cast(local.back());
        }
    }

    // By default, if local name is empty, assign the module name to it.
    if (!localName) {
        auto pos = moduleName.find_last_of(P->syntax_->packageSeparator());
        auto ident = moduleName.substr(pos == std::string::npos ? 0 : pos);
        localName =
                const_cast<LexemeMap*>(P->lexemes_)->insertOrFind<Ident>(
                        ident, P->fileName_, ast->asLoc_.lineCol());
    }

    std::unique_ptr<Import> import(
                new Import(FileInfo(P->fileName_).fullDir(),
                           moduleName,
                           localName,
                           P->sanitizer_->mergeImportEnv(localName)));
    import->setSourceLoc(fullLoc(ast, P->locator_.get()));

    P->env_.includeImport(std::move(import));

    return Continue;
}

Binder::VisitResult Binder::traverseTemplateDecl(TemplateDeclAst* ast)
{
    // TODO...
    return Continue;
}

Binder::VisitResult Binder::traverseTemplateParamDecl(TemplateParamDeclAst* ast)
{
    // TODO...
    return Continue;
}

Binder::VisitResult Binder::traverseTemplateParamClauseDecl(TemplateParamClauseDeclAst* ast)
{
    // TODO...
    return Continue;
}

Binder::VisitResult Binder::traverseParamGroupDecl(ParamGroupDeclAst* ast)
{
    if (!ast->decls_ || !ast->spec_)
        return Continue;

    for (auto decl : *(ast->decls_.get())) {
        UAISO_ASSERT(decl->kind() == Ast::Kind::ParamDecl, return Abort);
        ParamDeclAst* paramDecl = ParamDecl_Cast(decl);

        VIS_CALL(traverseDecl(paramDecl));
        ENSURE_TOP_SYMBOL_IS(Param);
        Param* param = Param_Cast(P->sym_.top().get());

        VIS_CALL(traverseSpec(ast->spec_.get()));
        ENSURE_NONEMPTY_TYPE_STACK;
        param->setValueType(P->popDeclType<>());

        P->env_.insertValue(P->popSymbol<Param>());
    }

    return Continue;
}

Binder::VisitResult Binder::traverseParamDecl(ParamDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name_.get()));

    if (P->declId_.empty()) {
        if (!P->sanitizer_->acceptsAnonymous(Symbol::Kind::Param))
            P->report(Diagnostic::NameRequired, ast, P->locator_.get());
        return Continue;
    }

    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Param> param(new Param(P->declId_.back()));
    param->setSourceLoc(fullLoc(ast, P->locator_.get()));

    P->sym_.push(std::move(param));

    return Continue;
}

Binder::VisitResult Binder::traverseVarGroupDecl(VarGroupDeclAst* group)
{
    if (!group->decls_ || !group->spec_)
        return Continue;

    for (auto decl : *(group->decls_.get())) {
        UAISO_ASSERT(decl->kind() == Ast::Kind::VarDecl, return Abort);
        VarDeclAst* varDecl = VarDecl_Cast(decl);

        VIS_CALL(traverseDecl(decl));
        ENSURE_TOP_SYMBOL_IS(Var);
        Var* var = Var_Cast(P->sym_.top().get());

        VIS_CALL(traverseSpec(group->spec_.get()));
        ENSURE_NONEMPTY_TYPE_STACK;
        var->setValueType(P->popDeclType<>());

        // Let type-specifying exprs annotate the AST with the type.
        VIS_CALL(traverseExpr(varDecl->init()));

        P->env_.insertValue(P->popSymbol<Var>());
    }

    // Let type-specifying exprs annotate the AST with the type.
    for (auto expr : *group->inits())
        VIS_CALL(traverseExpr(expr));

    return Continue;
}

Binder::VisitResult Binder::traverseVarDecl(VarDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name_.get()));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Var> var(new Var(P->declId_.back()));
    var->setSourceLoc(fullLoc(ast, P->locator_.get()));

    ast->sym_ = var.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(var));

    return Continue;
}

Binder::VisitResult Binder::traverseRecordDecl(RecordDeclAst* ast)
{
    // TODO: Proper "namespace" handling.

    VIS_CALL(traverseName(ast->name_.get()));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Record> record(new Record(P->declId_.back()));
    record->setSourceLoc(fullLoc(ast, P->locator_.get()));

    ast->sym_ = record.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(record));

    VIS_CALL(traverseSpec(ast->spec_.get()));

    ENSURE_TOP_SYMBOL_IS(Record);
    record = P->popSymbol<Record>();

    ENSURE_NONEMPTY_TYPE_STACK;
    record->setType(P->popDeclType<RecordType>());

    P->env_.insertType(std::move(record));

    return Continue;
}

Binder::VisitResult Binder::traverseBaseDecl(BaseDeclAst* ast)
{
    VIS_CALL(Base::traverseBaseDecl(ast));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<BaseRecord> base(new BaseRecord(P->declId_.back()));
    base->setSourceLoc(fullLoc(ast, P->locator_.get()));

    ENSURE_TOP_TYPE_IS(Record);
    RecordType* ty = RecordType_Cast(P->declTy_.top().get());
    ty->addBase(std::move(base));

    return Continue;
}

Binder::VisitResult Binder::traverseEnumDecl(EnumDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name_.get()));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Enum> enumm(new Enum(P->declId_.back()));
    enumm->setSourceLoc(fullLoc(ast, P->locator_.get()));

    ast->sym_ = enumm.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(enumm));
    P->enterSubEnv();

    for (auto decl : *ast->decls_.get())
        VIS_CALL(traverseDecl(decl));

    if (ast->spec_) {
        VIS_CALL(traverseSpec(ast->spec_.get()));
    } else {
        // If there's no underlying type spec, assume int.
        P->declTy_.emplace(new IntType);
    }

    ENSURE_TOP_SYMBOL_IS(Enum);
    enumm = P->popSymbol<Enum>();

    ENSURE_NONEMPTY_TYPE_STACK;
    enumm->setUnderlyingType(P->popDeclType<>());

    EnumType* enumType = new EnumType;
    enumm->setType(std::unique_ptr<EnumType>(enumType));
    enumType->setEnv(P->leaveEnv());

    P->env_.insertType(std::move(enumm));

    return Continue;
}

Binder::VisitResult Binder::traverseEnumMemberDecl(EnumMemberDeclAst* ast)
{
    VIS_CALL(Base::traverseEnumMemberDecl(ast));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<EnumItem> enumItem(new EnumItem(P->declId_.back()));
    enumItem->setSourceLoc(fullLoc(ast, P->locator_.get()));

    P->env_.insertValue(std::move(enumItem));

    // TODO: Initializer

    return Continue;
}

Binder::VisitResult Binder::traverseFuncDecl(FuncDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name_.get()));
    std::unique_ptr<Func> func(new Func(P->declId_.back()));
    func->setSourceLoc(fullLoc(ast, P->locator_.get()));

    ast->sym_ = func.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(func));
    P->enterSubEnv();

    VIS_CALL(traverseSpec(ast->spec_.get()));

    ENSURE_TOP_SYMBOL_IS(Func);
    func = P->popSymbol<Func>();

    ENSURE_NONEMPTY_TYPE_STACK;
    func->setType(P->popDeclType<FuncType>());

    Environment signatureEnv = P->leaveEnv();

    // Body environment is taken from block (if any).
    if (ast->stmt_
            && ast->stmt_->kind() == Ast::Kind::BlockStmt) {
        ++P->ownedBlocks_;
    }
    VIS_CALL(traverseStmt(ast->stmt_.get()));
    if (ast->stmt_
            && ast->stmt_->kind() == Ast::Kind::BlockStmt) {
        Environment blockEnv = BlockStmt_Cast(ast->stmt_.get())->env_;
        blockEnv.takeOver(signatureEnv);
        func->setEnv(blockEnv);
    } else {
        func->setEnv(signatureEnv);
    }

    P->env_.insertType(std::move(func));

    return Continue;
}

Binder::VisitResult Binder::traverseModuleDecl(ModuleDeclAst* ast)
{
    VIS_CALL(Base::traverseModuleDecl(ast));

    // An explicit module decl is not required.
    if (!P->declId_.empty()) {
        std::string moduleName = P->declId_.back()->str();
        if (!P->sanitizer_->moduleMatchesFile(P->fileName_, moduleName)) {
            P->report(Diagnostic::ModuleNameDoesNotMatchFileName,
                      ast->name_.get(), P->locator_.get());
        }
    }

    return Continue;
}

Binder::VisitResult Binder::traversePackageDecl(PackageDeclAst *ast)
{
    VIS_CALL(Base::traversePackageDecl(ast));

    // An explicit package decl is not required.
    if (!P->declId_.empty()) {
        std::string packageName = P->declId_.back()->str();
        if (!P->sanitizer_->packageMatchesDir(P->fileName_, packageName)) {
            P->report(Diagnostic::PackageNameDoesNotMatchDirName,
                      ast->name_.get(), P->locator_.get());
        }
    }

    return Continue;
}

Binder::VisitResult Binder::traverseUnitTestDecl(UnitTestDeclAst* ast)
{
    // TODO...
    return Continue;
}

    //-------------------//
    //--- Expressions ---//
    //-------------------//

Binder::VisitResult Binder::visitFuncLitExpr(FuncLitExprAst* ast)
{
    return Skip;
}

Binder::VisitResult Binder::visitMemberAccessExpr(MemberAccessExprAst* ast)
{
    return Skip;
}

Binder::VisitResult Binder::visitRecordInitExpr(RecordInitExprAst* ast)
{
    return Skip;
}

Binder::VisitResult Binder::visitRecordLitExpr(RecordLitExprAst* ast)
{
    return Skip;
}

template <class AstT>
Binder::VisitResult Binder::keepTypeOfExprSpec(AstT* ast)
{
    VIS_CALL(traverseSpec(ast->spec_.get()));
    ENSURE_NONEMPTY_TYPE_STACK;
    ast->ty_ = P->popDeclType<>();

    return Continue;
}

Binder::VisitResult Binder::traverseMakeExpr(MakeExprAst* ast)
{
    return keepTypeOfExprSpec(ast);
}

Binder::VisitResult Binder::traverseNewExpr(NewExprAst* ast)
{
    return keepTypeOfExprSpec(ast);
}

Binder::VisitResult Binder::traverseNestedNewExpr(NestedNewExprAst* ast)
{
    UAISO_ASSERT(ast->nestedNew_->kind() == Ast::Kind::NewExpr, return Skip);

    return traverseNewExpr(NewExpr_Cast(ast->nestedNew_.get()));
}

Binder::VisitResult Binder::traverseCastExpr(CastExprAst* ast)
{
    return keepTypeOfExprSpec(ast);
}

Binder::VisitResult Binder::traverseTypeQueryExpr(TypeQueryExprAst* ast)
{
    return keepTypeOfExprSpec(ast);
}

Binder::VisitResult Binder::traverseTypeAssertExpr(TypeAssertExprAst* ast)
{
    return keepTypeOfExprSpec(ast);
}

Binder::VisitResult Binder::traverseTypeidExpr(TypeidExprAst* ast)
{
    // TODO: See comment in TypeidExprAst.
    if (ast->exprOrSpec_->isSpec()) {
        VIS_CALL(traverseSpec(Spec_Cast(ast->exprOrSpec_.get())));
        ENSURE_NONEMPTY_TYPE_STACK;
        P->popDeclType<>();
    }

    return Continue;
}

    //------------------//
    //--- Statements ---//
    //------------------//

Binder::VisitResult Binder::traverseBlockStmt(BlockStmtAst* ast)
{
    bool nestBlockEnv = true;
    if (P->ownedBlocks_ > 0) {
        --P->ownedBlocks_;
        nestBlockEnv = false;
    }

    P->enterSubEnv();
    VIS_CALL(Base::traverseBlockStmt(ast));
    ast->env_ = P->leaveEnv(); // Annotate AST with the environment

    if (nestBlockEnv)
        ast->env_.nestIntoOuterEnv();

    return Continue;
}

Binder::VisitResult Binder::traverseTypeSwitchStmt(TypeSwitchStmtAst* ast)
{
    VIS_CALL(traverseStmt(ast->stmt_.get()));
    return Skip;
}
