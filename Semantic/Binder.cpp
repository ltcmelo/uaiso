/******************************************************************************
 * Copyright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)
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
#include "Semantic/Builtin.h"
#include "Semantic/DeclAttrs.h"
#include "Semantic/Environment.h"
#include "Semantic/Import.h"
#include "Semantic/Precision.h"
#include "Semantic/Program.h"
#include "Semantic/Sanitizer.h"
#include "Semantic/Symbol.h"
#include "Semantic/SymbolCast.h"
#include "Semantic/Signedness.h"
#include "Semantic/Type.h"
#include "Semantic/TypeCast.h"
#include "Semantic/TypeQuals.h"
#include "Semantic/TypeSystem.h"
#include "Ast/Ast.h"
#include "Ast/AstLocator.h"
#include "Ast/AstVariety.h"
#include "Common/Assert.h"
#include "Common/FileInfo.h"
#include "Common/Trace__.h"
#include "Parsing/Factory.h"
#include "Parsing/LangId.h"
#include "Parsing/Diagnostic.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/SourceLoc.h"
#include "Parsing/Lang.h"
#include "Parsing/Token.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <stack>
#include <utility>

#define TRACE_NAME "Binder"

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
    UAISO_ASSERT(isDecl(P->sym_.top().get()), return Abort)

#define ENSURE_TOP_SYMBOL_IS_TYPEDECL \
    ENSURE_NONEMPTY_SYMBOL_STACK; \
    UAISO_ASSERT(isTypeDecl(P->sym_.top().get()), return Abort)

#define ENSURE_TOP_SYMBOL_IS_VALUEDECL \
    ENSURE_NONEMPTY_SYMBOL_STACK; \
    UAISO_ASSERT(isValueDecl(P->sym_.top().get()), return Abort)

#define ENSURE_TOP_SYMBOL_IS(SYMBOL) \
    ENSURE_NONEMPTY_SYMBOL_STACK; \
    UAISO_ASSERT(P->sym_.top()->kind() == Symbol::Kind::SYMBOL, return Abort);

#define ENSURE_NONEMPTY_ENV_STACK \
    UAISO_ASSERT(!P->tyEnv_.empty(), return Abort)

using namespace uaiso;

namespace {

/*!
 * \brief The AstToLexeme class
 *
 * Collect names in the form of string literals, simple names, identifiers.
 */
class AstToLexeme final : public AstVisitor<AstToLexeme>
{
public:
    AstToLexeme(const LexemeMap* lexs)
        : lexs_(lexs)
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
    friend class AstVisitor<AstToLexeme>;

    const LexemeMap* lexs_;
    std::vector<const Lexeme*> ids_;
    bool firstOnly_;

    void prepare()
    {
        ids_.clear();
    }

    VisitResult pushName(const Lexeme* lex)
    {
        if (!lex)
            return Abort;

        ids_.push_back(lex);
        return Continue;
    }

    VisitResult visitSimpleName(SimpleNameAst* ast)
    {
        auto name = lexs_->findAt<Ident>(ast->nameLoc_.fileName_,
                                         ast->nameLoc_.lineCol());
        return pushName(name);
    }

    VisitResult visitGenName(GenNameAst* ast)
    {
        auto name = lexs_->findAt<Ident>(ast->genLoc_.fileName_,
                                         ast->genLoc_.lineCol());
        return pushName(name);
    }

    VisitResult visitStrLitExpr(StrLitExprAst* ast)
    {
        auto name = lexs_->findAt<StrLit>(ast->litLoc_.fileName_,
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
        : lexs_(nullptr)
        , tokens_(nullptr)
        , ownedBlocks_(0)
        , sanitizer_(factory->makeSanitizer())
        , locator_(factory->makeAstLocator())
        , lang_(factory->makeLang())
        , typeSystem_(factory->makeTypeSystem())
        , builtins_(factory->makeBuiltin())
        , reports_(nullptr)
        , bits_(0)
    {}

    Environment enterSubEnv()
    {
        env_ = env_.createSubEnv();
        return env_; // Return the new env as a convenience.
    }

    Environment leaveEnv()
    {
        Environment prevEnv = env_;
        env_ = env_.outerEnv();
        return prevEnv;
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

    const LexemeMap* lexs_; //!< Lexeme map of all AST locations.
    const TokenMap* tokens_;   //!< Token map of all AST locations.

    //! File name corresponding the to given AST.
    std::string fileName_;

    //! Environment we're currently in.
    Environment env_;

    //! Environment of current type (when there's one).
    std::stack<Environment> tyEnv_;

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

    //! Language-specific details.
    std::unique_ptr<const Lang> lang_;

    //! Language-specific type system.
    std::unique_ptr<const TypeSystem> typeSystem_;

    //! Language-specific builtins.
    std::unique_ptr<const Builtin> builtins_;

    //! Diagnostic reports collected.
    DiagnosticReports* reports_;

    //! Program being constructed.
    std::unique_ptr<Program> program_;

    struct BitFields
    {
        uint32_t ignoreBuiltins_    : 1;
        uint32_t ignoreAutoModules_ : 1;
    };
    union
    {
        BitFields bit_;
        uint32_t bits_;
    };
};

Binder::Binder(Factory* factory)
    : P(new BinderImpl(factory))
{}

Binder::~Binder()
{}

void Binder::setLexemes(const LexemeMap* lexs)
{
    P->lexs_ = lexs;
}

void Binder::setTokens(const TokenMap* tokens)
{
    P->tokens_ = tokens;
}

void Binder::collectDiagnostics(DiagnosticReports* reports)
{
    P->reports_ = reports;
}

void Binder::ignoreBuiltins()
{
    P->bit_.ignoreBuiltins_ = true;
}

void Binder::ignoreAutomaticModules()
{
    P->bit_.ignoreAutoModules_ = true;
}

std::unique_ptr<Program> Binder::bind(ProgramAst* progAst,
                                      const std::string& fullFileName)
{
    UAISO_ASSERT(progAst, return std::unique_ptr<Program>());
    UAISO_ASSERT(!fullFileName.empty(), return std::unique_ptr<Program>());

    P->fileName_.assign(fullFileName);
    P->program_.reset(new Program(P->fileName_));

    P->enterSubEnv();

    VisitResult result = traverseDecl(progAst->module());
    if (result == Continue) {
        result = traverseDecl(progAst->package());
        if (result == Continue) {
            result = traverseProgram(progAst, this, P->lang_);

            // If everything finishes alright, stacks must be empty.
            UAISO_ASSERT(P->declTy_.empty(), {});
            UAISO_ASSERT(P->sym_.empty(), {});
        }
    }

    if (!P->bit_.ignoreBuiltins_)
        insertBuiltins();

    if (!P->bit_.ignoreAutoModules_)
        importAutomaticModules();

    progAst->program_ = P->program_.get();
    progAst->program_->setEnv(P->leaveEnv());

    return std::move(P->program_);
}

void Binder::insertBuiltins()
{
    auto insertFunc = [this](std::vector<Builtin::FuncPtr> funcs) {
        for (auto& func : funcs) {
            UAISO_ASSERT(func, return);
            P->env_.insertTypeDecl(std::move(func));
        }
    };

    LexemeMap* lexs = const_cast<LexemeMap*>(P->lexs_);
    insertFunc(P->builtins_->createConstructors(lexs));
    insertFunc(P->builtins_->createGlobalFuncs(lexs));

    if (P->lang_->isPurelyOO()) {
        auto maybeInsert = [this](Builtin::TypeDeclPtr decl) {
            if (decl)
                P->env_.insertTypeDecl(std::move(decl));
        };

        maybeInsert(P->builtins_->createRootTypeDecl(lexs));
        maybeInsert(P->builtins_->createBasicTypeDecl(lexs, Type::Kind::Bool));
        maybeInsert(P->builtins_->createBasicTypeDecl(lexs, Type::Kind::Float));
        maybeInsert(P->builtins_->createBasicTypeDecl(lexs, Type::Kind::Int));
    }
}

void Binder::importAutomaticModules()
{
    auto names = P->builtins_->automaticModules();
    for (const auto& moduleName : names) {
        std::unique_ptr<Import> import(new Import(FileInfo(P->fileName_).fullDir(),
                                                  moduleName, nullptr, true));
        P->env_.includeImport(std::move(import));
    }
}

    //-------------//
    //--- Names ---//
    //-------------//

Binder::VisitResult Binder::visitSimpleName(SimpleNameAst* ast)
{
    const SourceLoc& loc = ast->nameLoc_;
    auto ident = P->lexs_->findAt<Ident>(loc.fileName_, loc.lineCol());
    UAISO_ASSERT(ident, return Abort, "SourceLoc:", loc);
    P->declId_.push_back(ident);

    return Continue;
}

Binder::VisitResult Binder::visitErrorName(ErrorNameAst*)
{
    P->declId_.push_back(&kErrorIdent);

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
    P->declId_.push_back(&kCompletionIdent);

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
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

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
    VIS_CALL(traverseSpec(ast->baseSpec()));
    ENSURE_NONEMPTY_TYPE_STACK;
    std::unique_ptr<ArrayType> array(new ArrayType(P->popDeclType<>()));
    array->setVariety(ast->variety());

    if (ast->exprOrSpec()) {
        if (ast->exprOrSpec()->isSpec()) {
            VIS_CALL(traverseSpec(Spec_Cast(ast->exprOrSpec())));
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
    return traverseSpec(ast->baseSpec());
}

Binder::VisitResult Binder::visitTypeofSpec(TypeofSpecAst* ast)
{
    P->declTy_.emplace(new InferredType);

    return Continue;
}

Binder::VisitResult Binder::traverseRecordSpec(RecordSpecAst* ast)
{
    P->declTy_.emplace(new RecordType);

    P->tyEnv_.push(P->enterSubEnv());
    VIS_CALL(Base::traverseRecordSpec(ast));
    ENSURE_TOP_TYPE_IS(Record);
    RecordType* ty = RecordType_Cast(P->declTy_.top().get());
    ty->setVariety(ast->variety());
    ty->setEnv(P->leaveEnv());

    return Continue;
}

Binder::VisitResult Binder::traverseFuncSpec(FuncSpecAst* ast)
{
    P->declTy_.emplace(new FuncType);

    VIS_CALL(traverseDecl(ast->param()));
    VIS_CALL(traverseDecl(ast->result()));

    if (P->lang_->requiresReturnTypeInference() || !ast->result())
        return Continue;

    // Assign the function's return type. The result must be a parameter clause.
    UAISO_ASSERT(ast->result()->kind() == Ast::Kind::ParamClauseDecl,return Abort);
    ParamClauseDeclAst* clause = ParamClauseDecl_Cast(ast->result());
    if (!clause->decls())
        return Continue;

    // TODO: Currently supports only simple types, only the first parameter
    // group is considered.
    UAISO_ASSERT(clause->decls()->head()->kind() == Ast::Kind::ParamGroupDecl,
                 return Abort);
    ParamGroupDeclAst* group = ParamGroupDecl_Cast(clause->decls()->head());

    // When as a result, a parameter group might contain actual declarations
    // (if the language has named return values) or might simply be a type
    // specifier. For the former case the return type is take from the
    // parameter symbol, while in the later the return type is taken through
    // a regular traversal over the spec.
    UAISO_ASSERT(group->decls() || group->spec(), return Abort);
    std::unique_ptr<Type> retTy;
    if (group->decls()) {
        UAISO_ASSERT(group->decls()->head()->kind() == Ast::Kind::ParamDecl,
                     return Abort);
        Param* ret = ParamDecl_Cast(group->decls()->head())->sym_;
        if (ret->valueType()
                && ret->valueType()->kind() != Type::Kind::Inferred) {
            retTy.reset(ret->valueType()->clone());
        }
    } else {
        VIS_CALL(traverseSpec(group->spec()));
        ENSURE_NONEMPTY_TYPE_STACK;
        retTy = P->popDeclType<>();
    }

    if (retTy) {
        ENSURE_TOP_TYPE_IS(Func);
        FuncType* funcTy = FuncType_Cast(P->declTy_.top().get());
        funcTy->setReturnType(std::move(retTy));
    }

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
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    Decl* sym = DeclSymbol_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateVisibility(sym->kind(), tk)) {
        P->report(Diagnostic::UnexpectedVisibilityAttribute, loc);
        return Continue;
    }

    if (sym->visibility() != TypeDecl::Visibility::Unknown) {
        P->report(Diagnostic::MultipleVisibilityAttributes, loc);
        return Continue;
    }

    switch (tk) {
    case TK_PACKAGE:
        sym->setVisibility(TypeDecl::Visibility::Package);
        break;
    case TK_PRIVATE:
        sym->setVisibility(TypeDecl::Visibility::Private);
        break;
    case TK_PROTECTED:
        sym->setVisibility(TypeDecl::Visibility::Protected);
        break;
    case TK_PUBLIC:
        sym->setVisibility(TypeDecl::Visibility::Public);
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
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    Decl* sym = TypeDecl_Cast(P->sym_.top().get());

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
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    Decl* sym = DeclSymbol_Cast(P->sym_.top().get());

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
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

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
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

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
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_DECL;
    Decl* sym = DeclSymbol_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateStorageClass(sym->kind(), tk)) {
        P->report(Diagnostic::UnexpectedStorageClassAttribute, loc);
        return Continue;
    }

    if (sym->storage() != ValueDecl::Storage::Unknown) {
        P->report(Diagnostic::MultipleStorageSpecifiers, loc);
        return Continue;
    }

    switch (tk) {
    case TK_SCOPE:
        sym->setStorage(ValueDecl::Storage::Scope);
        break;
    case TK_REF:
        sym->setStorage(ValueDecl::Storage::Ref);
        break;
    case TK_STATIC:
        sym->setStorage(ValueDecl::Storage::Static);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitLinkageAttr(LinkageAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

    ENSURE_TOP_SYMBOL_IS_VALUEDECL;
    ValueDecl* sym = ValueDecl_Cast(P->sym_.top().get());

    if (!P->sanitizer_->validateLinkage(sym->kind(), tk)) {
        P->report(Diagnostic::UnexpectedLinkageAttribute, loc);
        return Continue;
    }

    if (sym->linkage() != ValueDecl::Linkage::Unknown) {
        P->report(Diagnostic::MultipleLinkageSpecifiers, loc);
        return Continue;
    }

    switch (tk) {
    case TK_EXTERN:
        sym->setLinkage(ValueDecl::Linkage::Extern);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

Binder::VisitResult Binder::visitTypeQualAttr(TypeQualAttrAst* ast)
{
    const SourceLoc& loc = ast->keyLoc();
    Token tk = P->tokens_->findAt(loc.fileName_, loc.lineCol());

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
    alias->setSourceLoc(fullLoc(ast, P->locator_));

    ENSURE_NONEMPTY_TYPE_STACK;
    alias->setType(P->popDeclType<>());

    P->env_.insertTypeDecl(std::move(alias));

    return Continue;
}

Binder::VisitResult Binder::traverseBlockDecl(BlockDeclAst* ast)
{
    // TODO: Apply the attributes to each declaration individually.
    for (auto decl : *ast->decls())
        VIS_CALL(traverseDecl(decl));

    return Continue;
}

Binder::VisitResult Binder::traverseSelectiveDecl(SelectiveDeclAst* ast)
{
    VIS_CALL(traverseExpr(ast->expr()));

    // TODO: Pick the right clause when possible (at least for simple exprs).
    VIS_CALL(traverseDecl(ast->ifDecl()));

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
    holder->setSourceLoc(fullLoc(ast, P->locator_));

    P->env_.insertTypeDecl(std::move(holder));

    return Continue;
}

Binder::VisitResult Binder::traverseImportModuleDecl(ImportModuleDeclAst* ast)
{
    // An import is not injected into the environment during binding. But
    // it's still evaluated so the module and given namespace are collected.

    UAISO_ASSERT(ast->expr(), return Abort);
    AstToLexeme astToLexs(P->lexs_);
    const auto& lexs = astToLexs.process(ast->expr());
    if (lexs.empty()) {
        P->report(Diagnostic::UnresolvedModule, ast->expr(), P->locator_);
        return Continue;
    }

    const std::string& target =
            joinLexemes(lexs, P->lang_->packageSeparator());
    DEBUG_TRACE("import target %s\n", target.c_str());

    const Ident* localName = nullptr;
    if (ast->localName_) {
        const auto& localLexemes = astToLexs.process(ast->localName());
        if (localLexemes.empty() || localLexemes.size() > 1) {
            P->report(Diagnostic::UnresolvedModule, ast->localName(), P->locator_);
        } else {
            localName = ConstIdent_Cast(localLexemes.back());
            DEBUG_TRACE("local module name %s\n", localName->str().c_str());
        }
    }

    // By default, if local name is empty, assign the target to it (at an
    // artificial column after the import's last location).
    if (!localName) {
        localName = const_cast<LexemeMap*>(P->lexs_)->insertOrFind<Ident>(
                target, P->fileName_, P->locator_->lastLoc(ast).lineCol() + LineCol(0, 1));
        DEBUG_TRACE("no explicit local name, assign %s\n", target.c_str());
    }

    std::unique_ptr<Import> import(
                new Import(FileInfo(P->fileName_).fullDir(),
                           target,
                           localName,
                           P->sanitizer_->mayMergeImportEnv(localName)));

    // Specific exact symbols if this is a selective import.
    if (ast->items()) {
        for (auto decl : *ast->items()) {
            UAISO_ASSERT(decl->kind() == Ast::Kind::ImportItemDecl, return Abort);
            ImportItemDeclAst* item = ImportItemDecl_Cast(decl);

            const auto& actualNameLexs = astToLexs.process(item->actualName());
            if (actualNameLexs.empty() || actualNameLexs.size() > 1) {
                P->report(Diagnostic::UnexpectedName, item->actualName(),
                          P->locator_);
                continue;
            }

            const Ident* itemName = ConstIdent_Cast(actualNameLexs.back());
            if (item->alternateName()) {
                const auto& altNameLexs = astToLexs.process(item->alternateName());
                if (altNameLexs.empty() || altNameLexs.size() > 1) {
                    P->report(Diagnostic::UnexpectedName, item->alternateName(),
                              P->locator_);
                    continue;
                }
                import->addSelectedItem(itemName,
                                        ConstIdent_Cast(altNameLexs.back()));
            } else {
                import->addSelectedItem(itemName);
            }
        }
    }

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
    if (!ast->decls() || !ast->spec())
        return Continue;

    for (auto decl : *ast->decls()) {
        UAISO_ASSERT(decl->kind() == Ast::Kind::ParamDecl, return Abort);
        ParamDeclAst* paramDecl = ParamDecl_Cast(decl);

        VIS_CALL(traverseDecl(paramDecl));
        ENSURE_TOP_SYMBOL_IS(Param);
        Param* param = Param_Cast(P->sym_.top().get());

        VIS_CALL(traverseSpec(ast->spec()));
        ENSURE_NONEMPTY_TYPE_STACK;
        param->setValueType(P->popDeclType<>());

        P->env_.insertValueDecl(P->popSymbol<Param>());
    }

    return Continue;
}

Binder::VisitResult Binder::traverseParamDecl(ParamDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name()));

    if (P->declId_.empty()) {
        if (!P->sanitizer_->allowAnonymous(Symbol::Kind::Param))
            P->report(Diagnostic::IdentifierExpected, ast, P->locator_);
        return Continue;
    }

    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Param> param(new Param(P->declId_.back()));
    param->setSourceLoc(fullLoc(ast, P->locator_));

    ast->sym_ = param.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(param));

    return Continue;
}

Binder::VisitResult Binder::traverseVarGroupDecl(VarGroupDeclAst* group)
{
    if (!group->decls() || !group->spec())
        return Continue;

    for (auto decl : *group->decls()) {
        UAISO_ASSERT(decl->kind() == Ast::Kind::VarDecl, return Abort);
        VarDeclAst* varDecl = VarDecl_Cast(decl);

        VIS_CALL(traverseDecl(decl));
        ENSURE_TOP_SYMBOL_IS(Var);
        Var* var = Var_Cast(P->sym_.top().get());

        VIS_CALL(traverseSpec(group->spec()));
        ENSURE_NONEMPTY_TYPE_STACK;
        var->setValueType(P->popDeclType<>());

        // Let type-specifying exprs annotate the AST.
        VIS_CALL(traverseExpr(varDecl->init()));

        P->env_.insertValueDecl(P->popSymbol<Var>());
    }

    if (group->hasInits()) {
        // Let type-specifying exprs annotate the AST.
        for (auto expr : *group->inits())
            VIS_CALL(traverseExpr(expr));
    }

    return Continue;
}

Binder::VisitResult Binder::traverseVarDecl(VarDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name()));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Var> var(new Var(P->declId_.back()));
    var->setSourceLoc(fullLoc(ast, P->locator_));

    ast->sym_ = var.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(var));

    return Continue;
}

Binder::VisitResult Binder::traverseRecordDecl(RecordDeclAst* ast)
{
    // TODO: Proper "namespace" handling.

    VIS_CALL(traverseName(ast->name()));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Record> record(new Record(P->declId_.back()));
    record->setSourceLoc(fullLoc(ast, P->locator_));

    ast->sym_ = record.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(record));

    VIS_CALL(traverseSpec(ast->spec()));

    ENSURE_TOP_SYMBOL_IS(Record);
    record = P->popSymbol<Record>();

    ENSURE_NONEMPTY_TYPE_STACK;
    record->setType(P->popDeclType<RecordType>());

    P->env_.insertTypeDecl(std::move(record));

    return Continue;
}

Binder::VisitResult Binder::traverseBaseDecl(BaseDeclAst* ast)
{
    VIS_CALL(Base::traverseBaseDecl(ast));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<BaseRecord> base(new BaseRecord(P->declId_.back()));
    base->setSourceLoc(fullLoc(ast, P->locator_));

    ENSURE_TOP_TYPE_IS(Record);
    RecordType* ty = RecordType_Cast(P->declTy_.top().get());
    ty->addBase(std::move(base));

    return Continue;
}

Binder::VisitResult Binder::traverseEnumDecl(EnumDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name()));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<Enum> enumm(new Enum(P->declId_.back()));
    enumm->setSourceLoc(fullLoc(ast, P->locator_));

    ast->sym_ = enumm.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(enumm));
    P->enterSubEnv();

    if (ast->decls()) {
        for (auto decl : *ast->decls())
            VIS_CALL(traverseDecl(decl));

        if (ast->spec_) {
            VIS_CALL(traverseSpec(ast->spec()));
        } else {
            // If there's no underlying type spec, assume int.
            P->declTy_.emplace(new IntType);
        }
    }

    ENSURE_TOP_SYMBOL_IS(Enum);
    enumm = P->popSymbol<Enum>();

    ENSURE_NONEMPTY_TYPE_STACK;
    enumm->setUnderlyingType(P->popDeclType<>());

    EnumType* enumType = new EnumType;
    enumm->setType(std::unique_ptr<EnumType>(enumType));
    enumType->setEnv(P->leaveEnv());

    P->env_.insertTypeDecl(std::move(enumm));

    return Continue;
}

Binder::VisitResult Binder::traverseEnumMemberDecl(EnumMemberDeclAst* ast)
{
    VIS_CALL(Base::traverseEnumMemberDecl(ast));
    ENSURE_NAME_AVAILABLE;
    std::unique_ptr<EnumItem> enumItem(new EnumItem(P->declId_.back()));
    enumItem->setSourceLoc(fullLoc(ast, P->locator_));

    P->env_.insertValueDecl(std::move(enumItem));

    // TODO: Initializer

    return Continue;
}

Binder::VisitResult Binder::traverseFuncDecl(FuncDeclAst* ast)
{
    VIS_CALL(traverseName(ast->name()));
    std::unique_ptr<Func> func(new Func(P->declId_.back()));
    func->setSourceLoc(fullLoc(ast, P->locator_));

    ast->sym_ = func.get(); // Annotate AST with the symbol

    P->sym_.push(std::move(func));

    if (P->lang_->hasFuncLevelScope())
        P->enterSubEnv();

    VIS_CALL(traverseSpec(ast->spec()));

    ENSURE_TOP_SYMBOL_IS(Func);
    func = P->popSymbol<Func>();

    ENSURE_NONEMPTY_TYPE_STACK;
    func->setType(P->popDeclType<FuncType>());

    if (ast->stmt_ && ast->stmt_->kind() == Ast::Kind::BlockStmt) {
        // A block stmt of a function must have the same environment of
        // its parent function, and not create its own.
        ++P->ownedBlocks_;
        VIS_CALL(traverseStmt(ast->stmt()));
        BlockStmt_Cast(ast->stmt())->env_ = P->env_;
    } else {
        VIS_CALL(traverseStmt(ast->stmt()));
    }

    if (P->lang_->hasFuncLevelScope())
        func->setEnv(P->leaveEnv());
    else
        func->setEnv(P->env_);

    P->env_.insertTypeDecl(std::move(func));

    return Continue;
}

Binder::VisitResult Binder::traverseModuleDecl(ModuleDeclAst* ast)
{
    VIS_CALL(Base::traverseModuleDecl(ast));

    // An explicit module decl is not required.
    if (!P->declId_.empty()) {
        std::string moduleName = P->declId_.back()->str();
        if (!P->sanitizer_->moduleMatchesFile(P->fileName_, moduleName)) {
            P->report(Diagnostic::ModuleNameDoesNotMatchFileName, ast->name(),
                      P->locator_);
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
            P->report(Diagnostic::PackageNameDoesNotMatchDirName, ast->name(),
                      P->locator_);
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
    VIS_CALL(traverseSpec(ast->spec()));
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

    return traverseNewExpr(NewExpr_Cast(ast->nestedNew()));
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
    if (ast->exprOrSpec()->isSpec()) {
        VIS_CALL(traverseSpec(Spec_Cast(ast->exprOrSpec())));
        ENSURE_NONEMPTY_TYPE_STACK;
        P->popDeclType<>();
    }

    return Continue;
}

Binder::VisitResult Binder::traverseAssignExpr(AssignExprAst* ast)
{
    VIS_CALL(Base::traverseAssignExpr(ast));

    if (!P->typeSystem_->isDynamic())
        return Continue;

    for (auto expr : *ast->exprs1()) {
        Environment env = P->env_;
        NameAst* name = nullptr;
        if (expr->kind() == Ast::Kind::IdentExpr) {
            name = IdentExpr_Cast(expr)->name();
        } else if (expr->kind() == Ast::Kind::MemberAccessExpr) {
            auto member = MemberAccessExpr_Cast(expr);
            if (!member->exprOrSpec()
                    || member->exprOrSpec()->kind() != Ast::Kind::IdentExpr) {
                continue;
            }
            auto baseName = IdentExpr_Cast(member->exprOrSpec())->name();

            // Retrieve the lex of the base name and check whether
            // it's a "self". If so, bind the corresponding name.
            AstToLexeme pickupName(P->lexs_);
            const auto& baseLexemes = pickupName.process(baseName);
            if (baseLexemes.empty() ||
                    baseLexemes[0] != P->lexs_->self()) {
                continue;
            }

            name = member->name();
            if (P->tyEnv_.empty())
                P->report(Diagnostic::InvalidReferenceToSelf, fullLoc(name, P->locator_));
            else
                env = P->tyEnv_.top();
        } else {
            continue;
        }

        VIS_CALL(traverseName(name));
        ENSURE_NAME_AVAILABLE;
        std::unique_ptr<Var> var(new Var(P->declId_.back()));
        var->setSourceLoc(fullLoc(name, P->locator_));
        var->setValueType(std::unique_ptr<Type>(new InferredType));

        ast->syms_.push_back(var.get()); // Annotate AST with symbol

        env.insertValueDecl(std::unique_ptr<ValueDecl>(var.release()));
    }

    return Continue;
}

    //------------------//
    //--- Statements ---//
    //------------------//

Binder::VisitResult Binder::traverseBlockStmt(BlockStmtAst* ast)
{
    bool selfEnv = false;
    if (P->ownedBlocks_ > 0) {
        --P->ownedBlocks_;
    } else if (P->lang_->hasBlockLevelScope()) {
        P->enterSubEnv();
        selfEnv = true;
    }

    VIS_CALL(Base::traverseBlockStmt(ast));

    if (selfEnv) {
        ast->env_ = P->leaveEnv(); // Annotate AST with the environment
        ast->env_.nestIntoOuterEnv();
    }

    return Continue;
}

Binder::VisitResult Binder::traverseTypeSwitchStmt(TypeSwitchStmtAst* ast)
{
    // TODO: Don't traverse spec yet (type of decl won't be popped).

    VIS_CALL(traverseStmt(ast->stmt()));

    return Continue;
}
