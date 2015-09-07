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

/*---------------------------------------------------------------*/
/*---                    The UaiSo! Project                   ---*/
/*---                                                         ---*/
/*---         This file was AUTOMATICALLY GENERATED           ---*/
/*--- It can be AUTOMATICALLY UPDATED if the formatting/style ---*/
/*--- of the method declarations is preserved (it's fragile). ---*/
/*---------------------------------------------------------------*/

#ifndef UAISO_ASTDUMPER_H__
#define UAISO_ASTDUMPER_H__

#include "Ast/Ast.h"
#include "Ast/AstVisitor.h"
#include <ostream>

namespace uaiso {

class UAISO_API AstDumper final : public AstVisitor<AstDumper>
{
public:
    void dumpProgram(ProgramAst* ast);
    void dumpProgram(ProgramAst* ast, std::ostream& os);

private:
    friend class AstVisitor<AstDumper>;
    using Base = AstVisitor<AstDumper>;

    template <class AstT>
    VisitResult traverse(VisitResult (Base::*function)(AstT*), AstT*);

    VisitResult traverseName(NameAst*);
    VisitResult traverseSpec(SpecAst*);
    VisitResult traverseDecl(DeclAst*);
    VisitResult traverseExpr(ExprAst*);
    VisitResult traverseStmt(StmtAst*);

    // AST names
    VisitResult visitSimpleName(SimpleNameAst* ast);
    VisitResult visitCompletionName(CompletionNameAst* ast);
    VisitResult visitTemplateInstName(TemplateInstNameAst* ast);
    VisitResult visitGenName(GenNameAst* ast);
    VisitResult visitNestedName(NestedNameAst* ast);

    // AST specifiers
    VisitResult visitRecordSpec(RecordSpecAst* ast);
    VisitResult visitArraySpec(ArraySpecAst* ast);
    VisitResult visitBuiltinSpec(BuiltinSpecAst* ast);
    VisitResult visitChanSpec(ChanSpecAst* ast);
    VisitResult visitDecoratedSpec(DecoratedSpecAst* ast);
    VisitResult visitOpaqueSpec(OpaqueSpecAst* ast);
    VisitResult visitFuncSpec(FuncSpecAst* ast);
    VisitResult visitNamedSpec(NamedSpecAst* ast);
    VisitResult visitPtrSpec(PtrSpecAst* ast);
    VisitResult visitTypeofSpec(TypeofSpecAst* ast);
    VisitResult visitVoidSpec(VoidSpecAst* ast);
    VisitResult visitInferredSpec(InferredSpecAst* ast);

    // AST attributes
    VisitResult visitAccessAttr(VisibilityAttrAst* ast);
    VisitResult visitAnnotAttr(AnnotAttrAst* ast);
    VisitResult visitCodegenAttr(CodegenAttrAst* ast);
    VisitResult visitStorageClassAttr(StorageClassAttrAst* ast);
    VisitResult visitTypeQualAttr(TypeQualAttrAst* ast);
    VisitResult visitDeclAttr(DeclAttrAst* ast);
    VisitResult visitAutoAttr(AutoAttrAst* ast);
    VisitResult visitEvalStrategyAttr(EvalStrategyAttrAst* ast);
    VisitResult visitLinkageAttr(LinkageAttrAst* ast);
    VisitResult visitParamDirAttr(ParamDirAttrAst* ast);
    VisitResult visitVisibilityAttr(VisibilityAttrAst* ast);

    // AST declarations
    VisitResult visitAliasDecl(AliasDeclAst* ast);
    VisitResult visitBaseDecl(BaseDeclAst* ast);
    VisitResult visitBlockDecl(BlockDeclAst* ast);
    VisitResult visitSelectiveDecl(SelectiveDeclAst* ast);
    VisitResult visitConstraintDecl(ConstraintDeclAst* ast);
    VisitResult visitVersionDecl(VersionDeclAst* ast);
    VisitResult visitEnumDecl(EnumDeclAst* ast);
    VisitResult visitErrorDecl(ErrorDeclAst* ast);
    VisitResult visitFuncDecl(FuncDeclAst* ast);
    VisitResult visitForwardDecl(ForwardDeclAst* ast);
    VisitResult visitImportDecl(ImportDeclAst* ast);
    VisitResult visitInvariantDecl(InvariantDeclAst* ast);
    VisitResult visitParamDecl(ParamDeclAst* ast);
    VisitResult visitParamClauseDecl(ParamClauseDeclAst* ast);
    VisitResult visitSectionDecl(SectionDeclAst* ast);
    VisitResult visitStaticAssertDecl(StaticAssertDeclAst* ast);
    VisitResult visitTemplateDecl(TemplateDeclAst* ast);
    VisitResult visitTemplateParamDecl(TemplateParamDeclAst* ast);
    VisitResult visitTemplateParamClauseDecl(TemplateParamClauseDeclAst* ast);
    VisitResult visitRecordDecl(RecordDeclAst* ast);
    VisitResult visitUnitTestDecl(UnitTestDeclAst* ast);
    VisitResult visitVarDecl(VarDeclAst* ast);
    VisitResult visitVarGroupDecl(VarGroupDeclAst* ast);
    VisitResult visitEnumMemberDecl(EnumMemberDeclAst* ast);
    VisitResult visitModuleDecl(ModuleDeclAst* ast);
    VisitResult visitPackageDecl(PackageDeclAst* ast);
    VisitResult visitParamGroupDecl(ParamGroupDeclAst* ast);

    // AST expressions
    VisitResult visitAddExpr(AddExprAst* ast);
    VisitResult visitAddrOfExpr(AddrOfExprAst* ast);
    VisitResult visitRecordLitExpr(RecordLitExprAst* ast);
    VisitResult visitRecordInitExpr(RecordInitExprAst* ast);
    VisitResult visitArrayInitExpr(ArrayInitExprAst* ast);
    VisitResult visitArrayLengthExpr(ArrayLengthExprAst* ast);
    VisitResult visitArraySliceExpr(ArraySliceExprAst* ast);
    VisitResult visitArrayIndexExpr(ArrayIndexExprAst* ast);
    VisitResult visitAssertExpr(AssertExprAst* ast);
    VisitResult visitAssignExpr(AssignExprAst* ast);
    VisitResult visitBinaryExpr(BinaryExprAst* ast);
    VisitResult visitBitAndExpr(BitAndExprAst* ast);
    VisitResult visitBitCompExpr(BitCompExprAst* ast);
    VisitResult visitBitOrExpr(BitOrExprAst* ast);
    VisitResult visitBitXorExpr(BitXorExprAst* ast);
    VisitResult visitBoolLitExpr(BoolLitExprAst* ast);
    VisitResult visitCallExpr(CallExprAst* ast);
    VisitResult visitCastExpr(CastExprAst* ast);
    VisitResult visitCharLitExpr(CharLitExprAst* ast);
    VisitResult visitCommaExpr(CommaExprAst* ast);
    VisitResult visitConcatExpr(ConcatExprAst* ast);
    VisitResult visitCondExpr(CondExprAst* ast);
    VisitResult visitDelExpr(DelExprAst* ast);
    VisitResult visitDesignateExpr(DesignateExprAst* ast);
    VisitResult visitDivExpr(DivExprAst* ast);
    VisitResult visitEqExpr(EqExprAst* ast);
    VisitResult visitFuncLitExpr(FuncLitExprAst* ast);
    VisitResult visitIdentExpr(IdentExprAst* ast);
    VisitResult visitInExpr(InExprAst* ast);
    VisitResult visitIncDecExpr(IncDecExprAst* ast);
    VisitResult visitIsExpr(IsExprAst* ast);
    VisitResult visitLogicAndExpr(LogicAndExprAst* ast);
    VisitResult visitLogicNotExpr(LogicNotExprAst* ast);
    VisitResult visitLogicOrExpr(LogicOrExprAst* ast);
    VisitResult visitMakeExpr(MakeExprAst* ast);
    VisitResult visitMemberAccessExpr(MemberAccessExprAst* ast);
    VisitResult visitMinusExpr(MinusExprAst* ast);
    VisitResult visitModExpr(ModExprAst* ast);
    VisitResult visitMulExpr(MulExprAst* ast);
    VisitResult visitNestedNewExpr(NestedNewExprAst* ast);
    VisitResult visitNewExpr(NewExprAst* ast);
    VisitResult visitNumLitExpr(NumLitExprAst* ast);
    VisitResult visitPlusExpr(PlusExprAst* ast);
    VisitResult visitPrimaryExpr(PrimaryExprAst* ast);
    VisitResult visitPowerExpr(PowerExprAst* ast);
    VisitResult visitPtrDerefExpr(PtrDerefExprAst* ast);
    VisitResult visitNullLitExpr(NullLitExprAst* ast);
    VisitResult visitChanExpr(ChanExprAst* ast);
    VisitResult visitRelExpr(RelExprAst* ast);
    VisitResult visitShiftExpr(ShiftExprAst* ast);
    VisitResult visitStrLitExpr(StrLitExprAst* ast);
    VisitResult visitSubExpr(SubExprAst* ast);
    VisitResult visitSuperExpr(SuperExprAst* ast);
    VisitResult visitThisExpr(ThisExprAst* ast);
    VisitResult visitTypeAssertExpr(TypeAssertExprAst* ast);
    VisitResult visitTypeidExpr(TypeidExprAst* ast);
    VisitResult visitUnaryExpr(UnaryExprAst* ast);
    VisitResult visitMixinExpr(MixinExprAst* ast);
    VisitResult visitVoidInitExpr(VoidInitExprAst* ast);
    VisitResult visitWrappedExpr(WrappedExprAst* ast);
    VisitResult visitTypeQueryExpr(TypeQueryExprAst* ast);
    VisitResult visitSubrangeExpr(SubrangeExprAst* ast);
    VisitResult visitUnpackExpr(UnpackExprAst* ast);

    // AST statements
    VisitResult visitBlockStmt(BlockStmtAst* ast);
    VisitResult visitBodyStmt(BodyStmtAst* ast);
    VisitResult visitContractStmt(ContractStmtAst* ast);
    VisitResult visitDeclStmt(DeclStmtAst* ast);
    VisitResult visitExprStmt(ExprStmtAst* ast);
    VisitResult visitInStmt(InStmtAst* ast);
    VisitResult visitOutStmt(OutStmtAst* ast);
    VisitResult visitEmptyStmt(EmptyStmtAst* ast);
    VisitResult visitAsyncStmt(AsyncStmtAst* ast);
    VisitResult visitBreakStmt(BreakStmtAst* ast);
    VisitResult visitCaseClauseStmt(CaseClauseStmtAst* ast);
    VisitResult visitCatchClauseStmt(CatchClauseStmtAst* ast);
    VisitResult visitSelectiveStmt(SelectiveStmtAst* ast);
    VisitResult visitContinueStmt(ContinueStmtAst* ast);
    VisitResult visitDefaultClauseStmt(DefaultClauseStmtAst* ast);
    VisitResult visitDeferredStmt(DeferredStmtAst* ast);
    VisitResult visitDoWhileStmt(DoWhileStmtAst* ast);
    VisitResult visitErrorStmt(ErrorStmtAst* ast);
    VisitResult visitFallthroughStmt(FallthroughStmtAst* ast);
    VisitResult visitFinallyClauseStmt(FinallyClauseStmtAst* ast);
    VisitResult visitForStmt(ForStmtAst* ast);
    VisitResult visitForeachStmt(ForeachStmtAst* ast);
    VisitResult visitGotoStmt(GotoStmtAst* ast);
    VisitResult visitIfStmt(IfStmtAst* ast);
    VisitResult visitLabeledStmt(LabeledStmtAst* ast);
    VisitResult visitReturnStmt(ReturnStmtAst* ast);
    VisitResult visitSyncedStmt(SyncedStmtAst* ast);
    VisitResult visitSwitchStmt(SwitchStmtAst* ast);
    VisitResult visitThrowStmt(ThrowStmtAst* ast);
    VisitResult visitTryStmt(TryStmtAst* ast);
    VisitResult visitWhileStmt(WhileStmtAst* ast);
    VisitResult visitWithStmt(WithStmtAst* ast);
};

} // namespace uaiso

#endif
