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

#ifndef UAISO_TYPECHECKER_H__
#define UAISO_TYPECHECKER_H__

#include "Ast/AstFwd.h"
#include "Ast/AstVisitor.h"
#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Common/Test.h"
#include "Parsing/Diagnostic.h"
#include "Semantic/TypeFwd.h"

namespace uaiso {

class Factory;
class LexemeMap;
class TokenMap;

/*!
 * \brief The TypeChecker class
 */
class UAISO_API TypeChecker final : public AstVisitor<TypeChecker>
{
public:
    TypeChecker(Factory* factory);
    ~TypeChecker();

    void setLexemes(const LexemeMap* lexs);

    void setTokens(const TokenMap* tokens);

    void collectDiagnostics(DiagnosticReports* reports);

    /*!
     * \brief analyse
     * \param ast
     *
     * Type check a module.
     */
    void check(ProgramAst* ast);

    /*!
     * \brief analyse
     * \param ast
     *
     * Type check a function.
     */
    void check(FuncDeclAst* ast);

private:
    DECL_PIMPL(TypeChecker)
    DECL_CLASS_TEST(TypeChecker)

    friend class AstVisitor<TypeChecker>;
    using Base = AstVisitor<TypeChecker>;

    const Type* maybeResolve(const Type* ty, const SourceLoc& loc);

    bool escapeCheck(const Type* ty) const;

    bool analyseInit(const Type* lhsTy, const Type* rhsTy, const SourceLoc& loc);
    bool analyseAssign(const Type* lhsTy, const Type* rhsTy, const SourceLoc& loc);
    bool analyseEq(const Type* lhsTy, const Type* rhsTy, const SourceLoc& loc);

    VisitResult processLogical(ExprAst* expr1, ExprAst* expr2);
    VisitResult processBitwise(ExprAst* expr1, ExprAst* expr2);
    VisitResult processArithmetic(ExprAst* expr1, ExprAst* expr2);

    template <class AstT>
    VisitResult takeAnnotatedType(AstT* ast);

    //--- Declarations ---//

    VisitResult traverseSelectiveDecl(SelectiveDeclAst* ast);
    VisitResult traverseConstraintDecl(ConstraintDeclAst* ast);
    VisitResult traverseVersionDecl(VersionDeclAst* ast);
    VisitResult traverseEnumDecl(EnumDeclAst* ast);
    VisitResult traverseFuncDecl(FuncDeclAst* ast);
    VisitResult traverseImportDecl(ImportDeclAst* ast);
    VisitResult traversePackageDecl(PackageDeclAst* ast);
    VisitResult traverseParamDecl(ParamDeclAst* ast);
    VisitResult traverseParamGroupDecl(ParamGroupDeclAst* ast);
    VisitResult traverseStaticAssertDecl(StaticAssertDeclAst* ast);
    VisitResult traverseRecordDecl(RecordDeclAst* ast);
    VisitResult traverseUnitTestDecl(UnitTestDeclAst* ast);
    VisitResult traverseVarDecl(VarDeclAst* ast);
    VisitResult traverseVarGroupDecl(VarGroupDeclAst* ast);

    //--- Expressions ---//

    // Trivial
    VisitResult visitBoolLitExpr(BoolLitExprAst* ast);
    VisitResult visitCharLitExpr(CharLitExprAst* ast);
    VisitResult visitNumLitExpr(NumLitExprAst* ast);
    VisitResult visitNullLitExpr(NullLitExprAst* ast);
    VisitResult visitStrLitExpr(StrLitExprAst* ast);
    VisitResult visitAssertExpr(AssertExprAst* ast); // precisa verificar
    VisitResult visitVoidInitExpr(VoidInitExprAst* ast);
    VisitResult visitDelExpr(DelExprAst* ast);
    VisitResult visitArrayLengthExpr(ArrayLengthExprAst* ast);

    // No-op
    VisitResult visitPlusExpr(PlusExprAst* ast);
    VisitResult visitMinusExpr(MinusExprAst* ast);
    VisitResult visitIncDecExpr(IncDecExprAst* ast);
    VisitResult visitWrappedExpr(WrappedExprAst* ast);
    VisitResult visitPtrDerefExpr(PtrDerefExprAst* ast);
    VisitResult visitChanExpr(ChanExprAst* ast);
    VisitResult visitUnpackExpr(UnpackExprAst* ast);

    // Name lookup
    VisitResult visitIdentExpr(IdentExprAst* ast);

    // Arithmetic (binary)
    VisitResult traverseAddExpr(AddExprAst* ast);
    VisitResult traverseDivExpr(DivExprAst* ast);
    VisitResult traverseModExpr(ModExprAst* ast);
    VisitResult traverseMulExpr(MulExprAst* ast);
    VisitResult traversePowerExpr(PowerExprAst* ast);
    VisitResult traverseSubExpr(SubExprAst* ast);

    // String
    VisitResult traverseConcatExpr(ConcatExprAst* ast);

    // Bitwise
    VisitResult traverseBitAndExpr(BitAndExprAst* ast);
    VisitResult traverseBitOrExpr(BitOrExprAst* ast);
    VisitResult traverseBitXorExpr(BitXorExprAst* ast);
    VisitResult traverseBitCompExpr(BitCompExprAst* ast);
    VisitResult traverseShiftExpr(ShiftExprAst* ast);

    // Logical and relational
    VisitResult traverseLogicAndExpr(LogicAndExprAst* ast);
    VisitResult traverseLogicOrExpr(LogicOrExprAst* ast);
    VisitResult traverseLogicNotExpr(LogicNotExprAst* ast);
    VisitResult traverseRelExpr(RelExprAst* ast);

    // Checks
    VisitResult traverseEqExpr(EqExprAst* ast);
    VisitResult traverseInExpr(InExprAst* ast);
    VisitResult traverseIsExpr(IsExprAst* ast);
    VisitResult traverseCondExpr(CondExprAst* ast);

    // Type creation/convertion/assertion
    VisitResult traverseCastExpr(CastExprAst* ast);
    VisitResult traverseNewExpr(NewExprAst* ast);
    VisitResult traverseNestedNewExpr(NestedNewExprAst* ast);
    VisitResult traverseMakeExpr(MakeExprAst* ast);
    VisitResult traverseTypeQueryExpr(TypeQueryExprAst* ast);
    VisitResult traverseTypeAssertExpr(TypeAssertExprAst* ast);

    // Class-dependent
    VisitResult visitSuperExpr(SuperExprAst* ast);
    VisitResult visitThisExpr(ThisExprAst* ast);

    // Misc
    VisitResult traverseAddrOfExpr(AddrOfExprAst* ast);
    VisitResult traverseArrayIndexExpr(ArrayIndexExprAst* ast);
    VisitResult traverseArrayInitExpr(ArrayInitExprAst* ast);
    VisitResult traverseArraySliceExpr(ArraySliceExprAst* ast);
    VisitResult traverseAssignExpr(AssignExprAst* ast);
    VisitResult traverseCommaExpr(CommaExprAst* ast);
    VisitResult traverseDesignateExpr(DesignateExprAst* ast);
    VisitResult traverseCallExpr(CallExprAst* ast);
    VisitResult traverseTypeidExpr(TypeidExprAst* ast);
    VisitResult traverseRecordLitExpr(RecordLitExprAst* ast);
    VisitResult traverseRecordInitExpr(RecordInitExprAst* ast);
    VisitResult traverseFuncLitExpr(FuncLitExprAst* ast);
    VisitResult traverseMixinExpr(MixinExprAst* ast);
    VisitResult traverseMemberAccessExpr(MemberAccessExprAst* ast);
    VisitResult traverseSubrangeExpr(SubrangeExprAst* ast);

    //--- Statements ---//

    VisitResult traverseBlockStmt(BlockStmtAst* ast);
};

} // namespace uaiso

#endif
