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

#include "Semantic/TypeChecker.h"
#include "Semantic/Program.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Semantic/TypeSystem.h"
#include "Ast/Ast.h"
#include "Ast/AstLocator.h"
#include "Common/Assert.h"
#include "Parsing/Diagnostic.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/Token.h"
#include "Parsing/TokenMap.h"
#include <iostream>
#include <stack>

#define ENSURE_NONEMPTY_STACK \
    UAISO_ASSERT(!P->exprTy_.empty(), return Abort)

#define ENSURE_ANNOTATED_SYMBOL \
    UAISO_ASSERT(ast->sym_, return Abort, "no symbol annotated")

#define ENSURE_ANNOTATED_TYPE \
    UAISO_ASSERT(ast->ty_, return Abort, "no type annotated")

#define ENSURE_NAMED_SYMBOL \
    UAISO_ASSERT(ast->sym_->name(), return Abort, "unnamed symbol")

#define ENSURE_VALID_TYPE_SYMBOL \
    ENSURE_ANNOTATED_SYMBOL; \
    ENSURE_NAMED_SYMBOL; \
    UAISO_ASSERT(P->env_.lookUpType(ast->sym_->name()), return Abort, "type not found")

using namespace uaiso;

struct uaiso::TypeChecker::TypeCheckerImpl
{
    TypeCheckerImpl(Factory* factory)
        : lexemes_(nullptr)
        , tokens_(nullptr)
        , locator_(factory->makeAstLocator())
        , typeSystem_(factory->makeTypeSystem())
        , reports_(nullptr)
    {}

    std::unique_ptr<Type> popExprType()
    {
        std::unique_ptr<Type> ty = std::move(exprTy_.top());
        exprTy_.pop();
        return ty;
    }

    template <class TypeT>
    void pushExprType(std::unique_ptr<Type> ty)
    {
        if (ty)
            exprTy_.push(std::move(ty));
        else
            exprTy_.emplace(new TypeT);
    }

    template <class... Args>
    void report(Args&&... args)
    {
        if (reports_)
            reports_->add(std::forward<Args>(args)...);
    }

    const LexemeMap* lexemes_; //!< Lexeme map of all AST locations.
    const TokenMap* tokens_;   //!< Token map of all AST locations.

    //! Environment we're currently in.
    Environment env_;

    //! Type of the expr under process.
    std::stack<std::unique_ptr<Type>> exprTy_;

    //! Language-specific AST locator.
    std::unique_ptr<const AstLocator> locator_;

    //! Language-specific type system.
    std::unique_ptr<const TypeSystem> typeSystem_;

    //! Diagnostic reports collected.
    DiagnosticReports* reports_;
};

TypeChecker::TypeChecker(Factory* factory)
    : P(new TypeCheckerImpl(factory))
{}

TypeChecker::~TypeChecker()
{}

void TypeChecker::setLexemes(const LexemeMap* lexemes)
{
    P->lexemes_ = lexemes;
}

void TypeChecker::setTokens(const TokenMap* tokens)
{
    P->tokens_ = tokens;
}

void TypeChecker::collectDiagnostics(DiagnosticReports* reports)
{
    P->reports_ = reports;
}

void TypeChecker::check(ProgramAst *ast)
{
    UAISO_ASSERT(ast, return);
    UAISO_ASSERT(ast->program_, return);

    P->env_ = ast->program_->env();

    for (auto decl : *ast->decls_.get()) {
        if (traverseDecl(decl) != Continue)
            break;
    }
}

void TypeChecker::check(FuncDeclAst* ast)
{
    UAISO_ASSERT(ast, return);
    UAISO_ASSERT(ast->sym_, return);

    P->env_ = ast->sym_->env();

    traverseStmt(ast->stmt_.get());
}

bool TypeChecker::escapeCheck(const Type* ty) const
{
    // Inferred types currently act as a joker.
    if (ty->kind() == Type::Kind::Inferred)
        return true;

    return false;
}

const Type* TypeChecker::maybeResolve(const Type* ty, const SourceLoc& loc)
{
    if (ty->kind() != Type::Kind::Elaborate)
        return ty;

    ElaborateType* prevTy = nullptr;
    const Ident* prevName = nullptr;
    while (ty->kind() == Type::Kind::Elaborate) {
        auto elab = ConstElaborateType_Cast(ty);

        // Perhaps the type has already been resolved.
        if (elab->isResolved())
            return elab->canonicalType();

        // Avoid cyclic lookups.
        if (elab->name() == prevName) {
            P->report(Diagnostic::CircularType, loc);
            return ty;
        }
        prevName = elab->name();

        auto sym = P->env_.lookUpType(elab->name());
        if (!sym) {
            P->report(Diagnostic::UnknownType, loc);
            return ty;
        }
        prevTy = ElaborateType_ConstCast(elab);
        ty = sym->type();
        UAISO_ASSERT(ty, return ty);
    }

    // Annotate the actual type for further reference.
    prevTy->setCanonical(std::unique_ptr<Type>(ty->clone()));

    return ty;
}

bool TypeChecker::analyseInit(const Type *lhsTy,
                              const Type *rhsTy,
                              const SourceLoc& loc)
{
    // TODO
    return analyseAssign(lhsTy, rhsTy, loc);
}

bool TypeChecker::analyseAssign(const Type* lhsTy,
                                const Type* rhsTy,
                                const SourceLoc& loc)
{
    if (escapeCheck(rhsTy))
        return true;

    bool ok = false;
    rhsTy = maybeResolve(rhsTy, loc);
    switch (lhsTy->kind()) {
    case Type::Kind::Array:
        if (P->typeSystem_->isArrayFirstClass()
                && rhsTy->kind() == Type::Kind::Array) {
            auto lhsArray = ConstArrayType_Cast(lhsTy);
            auto rhsArray = ConstArrayType_Cast(rhsTy);
            ok = analyseAssign(lhsArray->baseType(),
                               rhsArray->baseType(), loc);
        }
        break;
    case Type::Kind::Bool:
        if (rhsTy->kind() == Type::Kind::Bool)
            ok = true;
        else
            ok = P->typeSystem_->isBoolConvertible(rhsTy);
        break;
    case Type::Kind::Chan:
        ok = true;
        break;
    case Type::Kind::Elaborate:
        ok = true;
        break;
    case Type::Kind::Enum:
        ok = true;
        break;
    case Type::Kind::Float:
        if (rhsTy->kind() == Type::Kind::Float) {
            ok = true;
        } else if (rhsTy->kind() == Type::Kind::Int) {
            P->report(Diagnostic::ValueMayLoosePrecision, loc);
            ok = true;
        }
        break;
    case Type::Kind::Func:
        ok = true;
        break;
    case Type::Kind::Inferred:
        ok = true;
        break;
    case Type::Kind::Int:
        if (rhsTy->kind() == Type::Kind::Int) {
            ok = true;
        } else if (rhsTy->kind() == Type::Kind::Float) {
            P->report(Diagnostic::ValueWillBeTruncated, loc);
            ok = true;
        } else {
            ok = P->typeSystem_->isIntConvertible(rhsTy);
        }
        break;
    case Type::Kind::Ptr:
        ok = true;
        break;
    case Type::Kind::Record:
        ok = true;
        break;
    case Type::Kind::Str:
        if (rhsTy->kind() == Type::Kind::Str)
            ok = true;
        break;
    case Type::Kind::Subrange:
    case Type::Kind::Void:
        break;
    default:
        UAISO_ASSERT(false, return false);
    }

    if (!ok)
        P->report(Diagnostic::IncompatibleAssignment, loc);

    return ok;
}

bool TypeChecker::analyseEq(const Type* lhsTy,
                            const Type* rhsTy,
                            const SourceLoc& loc)
{
    if (escapeCheck(lhsTy) || escapeCheck(rhsTy))
        return true;

    // TODO
    return analyseAssign(lhsTy, rhsTy, loc);
}

    //--------------------//
    //--- Declarations ---//
    //--------------------//

TypeChecker::VisitResult TypeChecker::traverseEnumDecl(EnumDeclAst* ast)
{
    ENSURE_VALID_TYPE_SYMBOL;

    P->env_ = ast->sym_->type()->env();
    VIS_CALL(Base::traverseEnumDecl(ast));

    for (auto decl : *ast->decls_.get()) {
        UAISO_ASSERT(decl->kind() == Ast::Kind::EnumMemberDecl, return Skip);
        EnumMemberDeclAst* enumMemberDecl = EnumMemberDecl_Cast(decl);

        if (enumMemberDecl->hasInit()) {
            VIS_CALL(traverseExpr(enumMemberDecl->init()));
            ENSURE_NONEMPTY_STACK;
            std::unique_ptr<Type> ty = P->popExprType();

            analyseInit(ast->sym_->underlyingType(), ty.get(),
                        fullLoc(ast, P->locator_.get()));
        }
    }

    P->env_ = P->env_.outerEnv();

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseRecordDecl(RecordDeclAst *ast)
{
    ENSURE_VALID_TYPE_SYMBOL;

    P->env_ = ast->sym_->type()->env();
    VIS_CALL(Base::traverseRecordDecl(ast));
    P->env_ = P->env_.outerEnv();

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseFuncDecl(FuncDeclAst* ast)
{
    ENSURE_VALID_TYPE_SYMBOL;

    // An environment is always entered upon a `BlockStmt`, so we only need
    // to take care in the case the function doesn't have one.
    if (!ast->stmt_ || ast->stmt_->kind() != Ast::Kind::BlockStmt) {
        P->env_ = ast->sym_->env();
        VIS_CALL(Base::traverseFuncDecl(ast));
        P->env_ = P->env_.outerEnv();
    } else {
        VIS_CALL(Base::traverseFuncDecl(ast));
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseSelectiveDecl(SelectiveDeclAst* ast)
{
    // TODO
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseConstraintDecl(ConstraintDeclAst* ast)
{
    // TODO
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseVersionDecl(VersionDeclAst* ast)
{
    // TODO
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseParamDecl(ParamDeclAst* ast)
{
    // TODO
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseParamGroupDecl(ParamGroupDeclAst* ast)
{
    // TODO
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseStaticAssertDecl(StaticAssertDeclAst* ast)
{
    // TODO
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseVarDecl(VarDeclAst* ast)
{
    ENSURE_ANNOTATED_SYMBOL;

    if (ast->hasInit()) {
        VIS_CALL(traverseExpr(ast->init()));
        ENSURE_NONEMPTY_STACK;
        std::unique_ptr<Type> ty = P->popExprType();

        Var* var = ast->sym_;
        analyseInit(var->valueType(), ty.get(), fullLoc(ast, P->locator_.get()));
        if (var->valueType()->kind() == Type::Kind::Inferred)
            var->setValueType(std::unique_ptr<Type>(ty->clone()));
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseVarGroupDecl(VarGroupDeclAst* group)
{
    if (group->hasInits()) {
        auto decls = group->decls_.get();
        for (auto init : *group->inits()) {
            // There should be a decl for every initializer.
            if (!decls->head()) {
                P->report(Diagnostic::ExtraInitializer, group, P->locator_.get());
                break;
            }

            VIS_CALL(traverseExpr(init));
            ENSURE_NONEMPTY_STACK;
            std::unique_ptr<Type> ty = P->popExprType();

            UAISO_ASSERT(decls->head()->kind() == Ast::Kind::VarDecl, return Skip);
            VarDeclAst* varDecl = VarDecl_Cast(decls->head());
            UAISO_ASSERT(varDecl->sym_, return Skip);
            const Type* declTy = varDecl->sym_->valueType();

            analyseInit(declTy, ty.get(), fullLoc(varDecl, P->locator_.get()));
            if (varDecl->sym_->valueType()->kind() == Type::Kind::Inferred)
                varDecl->sym_->setValueType(std::unique_ptr<Type>(ty->clone()));

            decls = decls->subList();
        }
    } else {
        for (auto decl : *group->decls_.get())
            VIS_CALL(traverseDecl(decl));
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseImportModuleDecl(ImportModuleDeclAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traversePackageDecl(PackageDeclAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseUnitTestDecl(UnitTestDeclAst* ast)
{
    // TODO
    return Continue;
}

    //-------------------//
    //--- Expressions ---//
    //-------------------//

TypeChecker::VisitResult TypeChecker::visitNumLitExpr(NumLitExprAst* ast)
{
    Token tk = P->tokens_->find(ast->litLoc_.fileName_, ast->litLoc_.lineCol());

    switch (tk) {
    case TK_INTEGER_LITERAL:
        P->exprTy_.emplace(new IntType);
        break;
    case TK_FLOAT_LITERAL:
        P->exprTy_.emplace(new FloatType);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitBoolLitExpr(BoolLitExprAst* ast)
{
    Token tk = P->tokens_->find(ast->litLoc_.fileName_, ast->litLoc_.lineCol());

    switch (tk) {
    case TK_TRUE_LITERAL:
    case TK_FALSE_LITERAL:
        P->exprTy_.emplace(new BoolType);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitCharLitExpr(CharLitExprAst* ast)
{
    Token tk = P->tokens_->find(ast->litLoc_.fileName_, ast->litLoc_.lineCol());

    switch (tk) {
    case TK_CHAR_LITERAL:
        P->exprTy_.emplace(new IntType);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitStrLitExpr(StrLitExprAst* ast)
{
    Token tk = P->tokens_->find(ast->litLoc_.fileName_, ast->litLoc_.lineCol());

    switch (tk) {
    case TK_STRING_LITERAL:
        P->exprTy_.emplace(new StrType);
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitNullLitExpr(NullLitExprAst* ast)
{
    Token tk = P->tokens_->find(ast->litLoc_.fileName_, ast->litLoc_.lineCol());

    switch (tk) {
    case TK_NULL_LITERAL:
        P->exprTy_.emplace(new PtrType(std::unique_ptr<Type>(new InferredType)));
        break;
    default:
        UAISO_ASSERT(false, {});
        break;
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitArrayLengthExpr(ArrayLengthExprAst* ast)
{
    P->exprTy_.emplace(new IntType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::processArithmetic(ExprAst* expr1,
                                                        ExprAst* expr2)
{
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty = P->popExprType();
    if (!isNumType(ty->kind())) {
        P->report(Diagnostic::NumericValueExpected, expr2, P->locator_.get());
        ty.reset(nullptr);
    }

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();
    if (!isNumType(ty2->kind())) {
        P->report(Diagnostic::NumericValueExpected, expr1, P->locator_.get());
    } else if (!ty) {
        ty = std::move(ty2);
    }

    P->pushExprType<IntType>(std::move(ty));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseAddExpr(AddExprAst* ast)
{
    VIS_CALL(Base::traverseAddExpr(ast));

    return processArithmetic(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseSubExpr(SubExprAst* ast)
{
    VIS_CALL(Base::traverseSubExpr(ast));

    return processArithmetic(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseDivExpr(DivExprAst* ast)
{
    VIS_CALL(Base::traverseDivExpr(ast));

    return processArithmetic(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseModExpr(ModExprAst* ast)
{
    VIS_CALL(Base::traverseModExpr(ast));

    return processArithmetic(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseMulExpr(MulExprAst* ast)
{
    VIS_CALL(Base::traverseMulExpr(ast));

    return processArithmetic(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traversePowerExpr(PowerExprAst* ast)
{
    VIS_CALL(Base::traversePowerExpr(ast));

    return processArithmetic(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseConcatExpr(ConcatExprAst* ast)
{
    VIS_CALL(Base::traverseConcatExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty = P->popExprType();
    if (ty->kind() != Type::Kind::Str) {
        P->report(Diagnostic::StringValueExpected, ast->expr1_.get(), P->locator_.get());
        ty.reset(nullptr);
    }

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();
    if (ty2->kind() != Type::Kind::Str) {
        P->report(Diagnostic::StringValueExpected, ast->expr2_.get(), P->locator_.get());
    } else if (!ty) {
        ty = std::move(ty2);
    }

    P->pushExprType<StrType>(std::move(ty));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitPlusExpr(PlusExprAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitMinusExpr(MinusExprAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitIncDecExpr(IncDecExprAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitWrappedExpr(WrappedExprAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitPtrDerefExpr(PtrDerefExprAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitChanExpr(ChanExprAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitUnpackExpr(UnpackExprAst* ast)
{
    // Empty
    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseAddrOfExpr(AddrOfExprAst* ast)
{
    VIS_CALL(Base::traverseAddrOfExpr(ast));
    ENSURE_NONEMPTY_STACK;
    P->exprTy_.emplace(new PtrType(P->popExprType()));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseArrayInitExpr(ArrayInitExprAst* ast)
{
    // For simplicity (although not really correct), inspect first item only.
    if (ast->inits_) {
        VIS_CALL(traverseExpr(ast->inits_->head()));
        ENSURE_NONEMPTY_STACK;
        P->exprTy_.emplace(new ArrayType(P->popExprType()));
        return Continue;
    }

    P->exprTy_.emplace(new InferredType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseArraySliceExpr(ArraySliceExprAst* ast)
{
    VIS_CALL(Base::traverseArraySliceExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> rangeTy = P->popExprType();
    if (rangeTy->kind() != Type::Kind::Subrange)
        P->report(Diagnostic::SubrangeValueExpected, ast->range_.get(), P->locator_.get());

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> baseTy = P->popExprType();
    if (baseTy->kind() != Type::Kind::Array) {
        P->report(Diagnostic::ArrayValueExpected, ast->base_.get(), P->locator_.get());
        P->exprTy_.emplace(new InferredType);
    } else {
        P->exprTy_.push(std::move(baseTy));
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseArrayIndexExpr(ArrayIndexExprAst* ast)
{
    VIS_CALL(Base::traverseArrayIndexExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> indexTy = P->popExprType();

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> baseTy = P->popExprType();
    if (baseTy->kind() != Type::Kind::Array) {
        P->report(Diagnostic::ArrayValueExpected, ast->base_.get(), P->locator_.get());
        P->exprTy_.emplace(new InferredType);
    } else {
        ArrayType* arrTy = ArrayType_Cast(baseTy.get());
        if (arrTy->variety() == ArrayVariety::Plain
                && !isNumType(indexTy->kind())) {
            P->report(Diagnostic::IntegerValueExpected, ast->index_.get(),
                      P->locator_.get());
        }
        P->exprTy_.emplace(arrTy->baseType()->clone());
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitAssertExpr(AssertExprAst* ast)
{
    P->exprTy_.emplace(new VoidType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitVoidInitExpr(VoidInitExprAst* ast)
{
    P->exprTy_.emplace(new VoidType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitDelExpr(DelExprAst* ast)
{
    P->exprTy_.emplace(new VoidType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseAssignExpr(AssignExprAst* ast)
{
    ExprAst* prevExpr2 = nullptr;
    for (auto expr1 : *ast->exprs1_.get()) {
        traverseExpr(expr1);
        ENSURE_NONEMPTY_STACK;
        std::unique_ptr<Type> ty1 = P->popExprType();

        std::unique_ptr<Type> ty2;
        auto expr2 = ast->exprs2_->head();
        if (!expr2) {
            // It's not necessary to have an exact pairing of expressions,
            // a function might return multiple values.
            expr2 = prevExpr2;
            if (!expr2) {
                // TODO: Report error.
                return Continue;
            }

            traverseExpr(expr2);
            ENSURE_NONEMPTY_STACK;
            ty2 = P->popExprType();
        }

        if (!ty2)
            return Continue;

        analyseAssign(ty1.get(), ty2.get(), fullLoc(ast, P->locator_.get()));

        prevExpr2 = expr2;
    }

    P->exprTy_.emplace(new VoidType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseEqExpr(EqExprAst* ast)
{
    VIS_CALL(Base::traverseEqExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty1 = P->popExprType();
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();

    analyseEq(ty1.get(), ty2.get(), fullLoc(ast, P->locator_.get()));

    P->exprTy_.emplace(new BoolType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseInExpr(InExprAst* ast)
{
    VIS_CALL(Base::traverseInExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty1 = P->popExprType();
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();

    // TODO: Check types

    P->exprTy_.emplace(new BoolType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseIsExpr(IsExprAst* ast)
{
    VIS_CALL(Base::traverseIsExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty1 = P->popExprType();
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();

    // TODO: Check types

    P->exprTy_.emplace(new BoolType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseCommaExpr(CommaExprAst* ast)
{
    VIS_CALL(Base::traverseCommaExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty1 = P->popExprType();
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();

    P->exprTy_.push(std::move(ty2));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseDesignateExpr(DesignateExprAst* ast)
{
    VIS_CALL(Base::traverseDesignateExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty1 = P->popExprType();
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();

    P->exprTy_.push(std::move(ty2));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::processLogical(ExprAst* expr1, ExprAst* expr2)
{
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty = P->popExprType();
    if (ty->kind() != Type::Kind::Bool) {
        P->report(Diagnostic::BooleanValueExpected, expr2, P->locator_.get());
        ty.reset(nullptr);
    }

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();
    if (ty2->kind() != Type::Kind::Bool)
        P->report(Diagnostic::BooleanValueExpected, expr1, P->locator_.get());
    else if (!ty)
        ty = std::move(ty2);

    P->pushExprType<BoolType>(std::move(ty));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseLogicAndExpr(LogicAndExprAst* ast)
{
    VIS_CALL(Base::traverseLogicAndExpr(ast));

    return processLogical(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseLogicOrExpr(LogicOrExprAst* ast)
{
    VIS_CALL(Base::traverseLogicOrExpr(ast));

    return processLogical(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseLogicNotExpr(LogicNotExprAst* ast)
{
    VIS_CALL(Base::traverseLogicNotExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty = P->popExprType();
    if (ty->kind() != Type::Kind::Bool) {
        P->report(Diagnostic::BooleanValueExpected, ast, P->locator_.get());
        P->exprTy_.emplace(new BoolType);
    } else {
        P->exprTy_.push(std::move(ty));
    }

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseRelExpr(RelExprAst* ast)
{
    VIS_CALL(Base::traverseRelExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty1 = P->popExprType();
    if (!isNumType(ty1->kind()))
        P->report(Diagnostic::NumericValueExpected, ast->expr1_.get(), P->locator_.get());

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();
    if (!isNumType(ty2->kind()))
        P->report(Diagnostic::NumericValueExpected, ast->expr2_.get(), P->locator_.get());

    P->exprTy_.emplace(new BoolType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseCondExpr(CondExprAst* ast)
{
    VIS_CALL(Base::traverseCondExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty = P->popExprType();

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty3 = P->popExprType();
    if (ty3->kind() != Type::Kind::Bool)
        P->report(Diagnostic::BooleanValueExpected, ast->yes_.get(), P->locator_.get());

    P->exprTy_.push(std::move(ty));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::processBitwise(ExprAst* expr1, ExprAst* expr2)
{
    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty = P->popExprType();
    if (ty->kind() != Type::Kind::Int) {
        P->report(Diagnostic::IntegerValueExpected, expr2, P->locator_.get());
        ty.reset(nullptr);
    }

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty2 = P->popExprType();
    if (ty2->kind() != Type::Kind::Int)
        P->report(Diagnostic::IntegerValueExpected, expr1, P->locator_.get());
    else if (!ty)
        ty = std::move(ty2);

    P->pushExprType<IntType>(std::move(ty));

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseBitAndExpr(BitAndExprAst* ast)
{
    VIS_CALL(Base::traverseBitAndExpr(ast));

    return processBitwise(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseBitOrExpr(BitOrExprAst* ast)
{
    VIS_CALL(Base::traverseBitOrExpr(ast));

    return processBitwise(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseBitXorExpr(BitXorExprAst* ast)
{
    VIS_CALL(Base::traverseBitXorExpr(ast));

    return processBitwise(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseShiftExpr(ShiftExprAst* ast)
{
    VIS_CALL(Base::traverseShiftExpr(ast));

    return processBitwise(ast->expr1_.get(), ast->expr2_.get());
}

TypeChecker::VisitResult TypeChecker::traverseBitCompExpr(BitCompExprAst* ast)
{
    VIS_CALL(Base::traverseBitCompExpr(ast));

    ENSURE_NONEMPTY_STACK;
    std::unique_ptr<Type> ty = P->popExprType();
    if (ty->kind() != Type::Kind::Int) {
        P->report(Diagnostic::IntegerValueExpected, ast, P->locator_.get());
        ty.reset(nullptr);
    }

    P->pushExprType<IntType>(std::move(ty));

    return Continue;
}

template <class AstT>
TypeChecker::VisitResult TypeChecker::takeAnnotatedType(AstT* ast)
{
    P->exprTy_.emplace(ast->ty_.release());

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseMakeExpr(MakeExprAst* ast)
{
    ENSURE_ANNOTATED_TYPE;

    return takeAnnotatedType(ast);
}

TypeChecker::VisitResult TypeChecker::traverseNewExpr(NewExprAst* ast)
{
    ENSURE_ANNOTATED_TYPE;

    return takeAnnotatedType(ast);
}

TypeChecker::VisitResult TypeChecker::traverseNestedNewExpr(NestedNewExprAst* ast)
{
    UAISO_ASSERT(ast->nestedNew_->kind() == Ast::Kind::NewExpr, return Skip);

    NewExprAst* newExprAst = NewExpr_Cast(ast->nestedNew_.get());
    UAISO_ASSERT(newExprAst->ty_, return Abort, "no type annotated");

    return takeAnnotatedType(newExprAst);
}

TypeChecker::VisitResult TypeChecker::traverseCastExpr(CastExprAst* ast)
{
    ENSURE_ANNOTATED_TYPE;

    return takeAnnotatedType(ast);
}

TypeChecker::VisitResult TypeChecker::traverseTypeAssertExpr(TypeAssertExprAst* ast)
{
    ENSURE_ANNOTATED_TYPE;

    return takeAnnotatedType(ast);
}

TypeChecker::VisitResult TypeChecker::traverseTypeQueryExpr(TypeQueryExprAst* ast)
{
    // TODO
    P->exprTy_.emplace(new BoolType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseCallExpr(CallExprAst* ast)
{
    // TODO

    P->exprTy_.emplace(new FuncType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseRecordInitExpr(RecordInitExprAst* ast)
{
    if (ast->spec_ && ast->spec_->kind() == Ast::Kind::NamedSpec) {
        auto tySym = lookUpType(NamedSpec_Cast(ast->spec_.get())->name_.get(),
                                P->env_, P->lexemes_);
        if (tySym) {
            P->exprTy_.emplace(tySym->type()->clone());
            return Continue;
        }
    }

    P->exprTy_.emplace(new RecordType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseRecordLitExpr(RecordLitExprAst* ast)
{
    // TODO: See RecordLitExprAst's comment.

    P->exprTy_.emplace(new RecordType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseFuncLitExpr(FuncLitExprAst* ast)
{
    // TODO

    P->exprTy_.emplace(new FuncType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseTypeidExpr(TypeidExprAst* ast)
{
    P->exprTy_.emplace(new RecordType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseMixinExpr(MixinExprAst* ast)
{
    // TODO: Inject the mixin expression.

    P->exprTy_.emplace(new VoidType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseMemberAccessExpr(MemberAccessExprAst* ast)
{
    // TODO

    P->exprTy_.emplace(new InferredType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::traverseSubrangeExpr(SubrangeExprAst* ast)
{
    VIS_CALL(Base::traverseSubrangeExpr(ast));

    if (ast->low_) {
        ENSURE_NONEMPTY_STACK;
        P->popExprType();
    }

    if (ast->hi_) {
        ENSURE_NONEMPTY_STACK;
        P->popExprType();
    }

    if (ast->max_) {
        ENSURE_NONEMPTY_STACK;
        P->popExprType();
    }

    // TODO

    P->exprTy_.emplace(new InferredType);

    return Continue;
}


TypeChecker::VisitResult TypeChecker::visitSuperExpr(SuperExprAst* ast)
{
    P->exprTy_.emplace(new RecordType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitThisExpr(ThisExprAst* ast)
{
    P->exprTy_.emplace(new RecordType);

    return Continue;
}

TypeChecker::VisitResult TypeChecker::visitIdentExpr(IdentExprAst* ast)
{
    auto valSym = lookUpValue(ast->name_.get(), P->env_, P->lexemes_);
    if (!valSym || !valSym->valueType()) {
        P->report(Diagnostic::UndeclaredIdentifier, ast->name_.get(), P->locator_.get());
        P->exprTy_.emplace(new InferredType);
    } else {
        P->exprTy_.emplace(valSym->valueType()->clone());
    }

    return Continue;
}

    //------------------//
    //--- Statements ---//
    //------------------//

TypeChecker::VisitResult TypeChecker::traverseBlockStmt(BlockStmtAst* ast)
{
    // The environment of a block stmt might be the shared with its
    // parent. If that's the case, it's been entered already.
    if (P->env_ == ast->env_) {
        VIS_CALL(Base::traverseBlockStmt(ast));
    } else {
        P->env_ = ast->env_;
        VIS_CALL(Base::traverseBlockStmt(ast));
        P->env_ = P->env_.outerEnv();
    }

    return Continue;
}
