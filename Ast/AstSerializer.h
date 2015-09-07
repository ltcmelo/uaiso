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

#ifndef UAISO_ASTSERIALIZER_H__
#define UAISO_ASTSERIALIZER_H__

#include "Ast/Ast.h"
#include "Ast/AstVisitor.h"
#include <ostream>

namespace uaiso {

class UAISO_API AstSerializer final : public AstVisitor<AstSerializer>
{
public:
    void serializeProgram(ProgramAst* ast);
    void serializeProgram(ProgramAst* ast, std::ostream& os);

private:
    friend class AstVisitor<AstSerializer>;
    using Base = AstVisitor<AstSerializer>;

    template <class AstT>
    VisitResult traverse(VisitResult (Base::*function)(AstT*), AstT*);

    VisitResult traverseName(NameAst*);
    VisitResult traverseSpec(SpecAst*);
    VisitResult traverseAttr(AttrAst*);
    VisitResult traverseDecl(DeclAst*);
    VisitResult traverseExpr(ExprAst*);
    VisitResult traverseStmt(StmtAst*);

    void enterList();
    void leaveList();

    //--- Names ---//

    VisitResult traverseSimpleName(SimpleNameAst* ast);
    VisitResult traverseTemplateInstName(TemplateInstNameAst* ast);
    VisitResult traverseGenName(GenNameAst* ast);
    VisitResult traverseNestedName(NestedNameAst* ast);

    //--- Specifiers ---//

    VisitResult traverseRecordSpec(RecordSpecAst* ast);
    VisitResult traverseArraySpec(ArraySpecAst* ast);
    VisitResult traverseBuiltinSpec(BuiltinSpecAst* ast);
    VisitResult traverseChanSpec(ChanSpecAst* ast);
    VisitResult traverseDecoratedSpec(DecoratedSpecAst* ast);
    VisitResult traverseOpaqueSpec(OpaqueSpecAst* ast);
    VisitResult traverseFuncSpec(FuncSpecAst* ast);
    VisitResult traverseInferredSpec(InferredSpecAst* ast);
    VisitResult traverseNamedSpec(NamedSpecAst* ast);
    VisitResult traversePtrSpec(PtrSpecAst* ast);
    VisitResult traverseTypeofSpec(TypeofSpecAst* ast);
    VisitResult traverseVoidSpec(VoidSpecAst* ast);

    //--- Attributes ---//

    VisitResult traverseAnnotAttr(AnnotAttrAst* ast);
    VisitResult traverseDeclAttr(DeclAttrAst* ast);
    VisitResult traverseAutoAttr(AutoAttrAst* ast);
    VisitResult traverseCodegenAttr(CodegenAttrAst* ast);
    VisitResult traverseEvalStrategyAttr(EvalStrategyAttrAst* ast);
    VisitResult traverseLinkageAttr(LinkageAttrAst* ast);
    VisitResult traverseParamDirAttr(ParamDirAttrAst* ast);
    VisitResult traverseStorageClassAttr(StorageClassAttrAst* ast);
    VisitResult traverseTypeQualAttr(TypeQualAttrAst* ast);
    VisitResult traverseVisibilityAttr(VisibilityAttrAst* ast);

    VisitResult traverseAliasDecl(AliasDeclAst* ast);
    VisitResult traverseBaseDecl(BaseDeclAst* ast);
    VisitResult traverseSelectiveDecl(SelectiveDeclAst* ast);
    VisitResult traverseConstraintDecl(ConstraintDeclAst* ast);
    VisitResult traverseVersionDecl(VersionDeclAst* ast);
    VisitResult traverseEnumDecl(EnumDeclAst* ast);
    VisitResult traverseEnumMemberDecl(EnumMemberDeclAst* ast);
    VisitResult traverseBlockDecl(BlockDeclAst* ast);
    VisitResult traverseErrorDecl(ErrorDeclAst* ast);
    VisitResult traverseFuncDecl(FuncDeclAst* ast);
    VisitResult traverseForwardDecl(ForwardDeclAst* ast);
    VisitResult traverseImportDecl(ImportDeclAst* ast);
    VisitResult traverseInvariantDecl(InvariantDeclAst* ast);
    VisitResult traverseModuleDecl(ModuleDeclAst* ast);
    VisitResult traversePackageDecl(PackageDeclAst* ast);
    VisitResult traverseParamDecl(ParamDeclAst* ast);
    VisitResult traverseParamGroupDecl(ParamGroupDeclAst* ast);
    VisitResult traverseParamClauseDecl(ParamClauseDeclAst* ast);
    VisitResult traverseSectionDecl(SectionDeclAst* ast);
    VisitResult traverseStaticAssertDecl(StaticAssertDeclAst* ast);
    VisitResult traverseTemplateDecl(TemplateDeclAst* ast);
    VisitResult traverseTemplateParamDecl(TemplateParamDeclAst* ast);
    VisitResult traverseTemplateParamClauseDecl(TemplateParamClauseDeclAst* ast);
    VisitResult traverseRecordDecl(RecordDeclAst* ast);
    VisitResult traverseUnitTestDecl(UnitTestDeclAst* ast);
    VisitResult traverseVarDecl(VarDeclAst* ast);
    VisitResult traverseVarGroupDecl(VarGroupDeclAst* ast);

    //--- Expressions ---//

    VisitResult traverseAddExpr(AddExprAst* ast);
    VisitResult traverseAddrOfExpr(AddrOfExprAst* ast);
    VisitResult traverseRecordLitExpr(RecordLitExprAst* ast);
    VisitResult traverseRecordInitExpr(RecordInitExprAst* ast);
    VisitResult traverseArrayInitExpr(ArrayInitExprAst* ast);
    VisitResult traverseArrayLengthExpr(ArrayLengthExprAst* ast);
    VisitResult traverseArraySliceExpr(ArraySliceExprAst* ast);
    VisitResult traverseArrayIndexExpr(ArrayIndexExprAst* ast);
    VisitResult traverseAssertExpr(AssertExprAst* ast);
    VisitResult traverseAssignExpr(AssignExprAst* ast);
    VisitResult traverseBinaryExpr(BinaryExprAst* ast);
    VisitResult traverseBitAndExpr(BitAndExprAst* ast);
    VisitResult traverseBitCompExpr(BitCompExprAst* ast);
    VisitResult traverseBitOrExpr(BitOrExprAst* ast);
    VisitResult traverseBitXorExpr(BitXorExprAst* ast);
    VisitResult traverseBoolLitExpr(BoolLitExprAst* ast);
    VisitResult traverseCallExpr(CallExprAst* ast);
    VisitResult traverseCastExpr(CastExprAst* ast);
    VisitResult traverseChanExpr(ChanExprAst* ast);
    VisitResult traverseCharLitExpr(CharLitExprAst* ast);
    VisitResult traverseCommaExpr(CommaExprAst* ast);
    VisitResult traverseConcatExpr(ConcatExprAst* ast);
    VisitResult traverseCondExpr(CondExprAst* ast);
    VisitResult traverseDelExpr(DelExprAst* ast);
    VisitResult traverseDesignateExpr(DesignateExprAst* ast);
    VisitResult traverseDivExpr(DivExprAst* ast);
    VisitResult traverseEqExpr(EqExprAst* ast);
    VisitResult traverseFuncLitExpr(FuncLitExprAst* ast);
    VisitResult traverseIdentExpr(IdentExprAst* ast);
    VisitResult traverseInExpr(InExprAst* ast);
    VisitResult traverseIncDecExpr(IncDecExprAst* ast);
    VisitResult traverseTypeQueryExpr(TypeQueryExprAst* ast);
    VisitResult traverseIsExpr(IsExprAst* ast);
    VisitResult traverseLogicAndExpr(LogicAndExprAst* ast);
    VisitResult traverseLogicNotExpr(LogicNotExprAst* ast);
    VisitResult traverseLogicOrExpr(LogicOrExprAst* ast);
    VisitResult traverseMakeExpr(MakeExprAst* ast);
    VisitResult traverseMemberAccessExpr(MemberAccessExprAst* ast);
    VisitResult traverseMinusExpr(MinusExprAst* ast);
    VisitResult traverseModExpr(ModExprAst* ast);
    VisitResult traverseMulExpr(MulExprAst* ast);
    VisitResult traverseNestedNewExpr(NestedNewExprAst* ast);
    VisitResult traverseNewExpr(NewExprAst* ast);
    VisitResult traverseNumLitExpr(NumLitExprAst* ast);
    VisitResult traversePlusExpr(PlusExprAst* ast);
    VisitResult traversePrimaryExpr(PrimaryExprAst* ast);
    VisitResult traversePowerExpr(PowerExprAst* ast);
    VisitResult traversePtrDerefExpr(PtrDerefExprAst* ast);
    VisitResult traverseNullLitExpr(NullLitExprAst* ast);
    VisitResult traverseRelExpr(RelExprAst* ast);
    VisitResult traverseShiftExpr(ShiftExprAst* ast);
    VisitResult traverseStrLitExpr(StrLitExprAst* ast);
    VisitResult traverseSubExpr(SubExprAst* ast);
    VisitResult traverseSuperExpr(SuperExprAst* ast);
    VisitResult traverseThisExpr(ThisExprAst* ast);
    VisitResult traverseTypeAssertExpr(TypeAssertExprAst* ast);
    VisitResult traverseTypeidExpr(TypeidExprAst* ast);
    VisitResult traverseUnaryExpr(UnaryExprAst* ast);
    VisitResult traverseMixinExpr(MixinExprAst* ast);
    VisitResult traverseVoidInitExpr(VoidInitExprAst* ast);
    VisitResult traverseWrappedExpr(WrappedExprAst* ast);

    //--- Statements ---//

    VisitResult traverseBlockStmt(BlockStmtAst* ast);
    VisitResult traverseBodyStmt(BodyStmtAst* ast);
    VisitResult traverseContractStmt(ContractStmtAst* ast);
    VisitResult traverseDeclStmt(DeclStmtAst* ast);
    VisitResult traverseExprStmt(ExprStmtAst* ast);
    VisitResult traverseInStmt(InStmtAst* ast);
    VisitResult traverseOutStmt(OutStmtAst* ast);
    VisitResult traverseEmptyStmt(EmptyStmtAst* ast);
};

/*!
 * \brief serialize
 * \param ast
 * \return
 *
 * Serialize a ProgramAst.
 */
std::string serialize(ProgramAst* ast);

/*!
 * \brief deserialize
 * \param s
 * \return
 *
 * Deserialize a ProgramAst.
 */
std::unique_ptr<ProgramAst> deserialize(const std::string& s);

} // namespace uaiso

#endif
