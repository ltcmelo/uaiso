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

#ifndef UAISO_BINDER_H__
#define UAISO_BINDER_H__

#include "Ast/Ast.h"
#include "Ast/AstVisitor.h"
#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Common/Test.h"
#include "Parsing/Diagnostic.h"
#include "Semantic/SymbolFwd.h"
#include <vector>

namespace uaiso {

class Factory;
class LexemeMap;
class Program;
class TokenMap;

class UAISO_API Binder final : public AstVisitor<Binder>
{
public:
    Binder(Factory* factory);
    ~Binder();

    void setLexemes(const LexemeMap* lexemes);

    void setTokens(const TokenMap* tokens);

    void collectDiagnostics(DiagnosticReports* reports);

    /*!
     * \brief bindProgram
     * \param decl
     * \param fullFileName
     * \return
     */
    std::unique_ptr<Program> bind(ProgramAst* decl, const std::string& fullFileName);

private:
    DECL_PIMPL(Binder)
    DECL_CLASS_TEST(Binder)

    friend class AstVisitor<Binder>;
    using Base = AstVisitor<Binder>;

    template <class AstT>
    VisitResult keepTypeOfExprSpec(AstT* ast);

    //--- Names ---//

    VisitResult visitSimpleName(SimpleNameAst* ast);
    VisitResult visitCompletionName(CompletionNameAst* ast);
    VisitResult visitTemplateInstName(TemplateInstNameAst* ast);
    VisitResult visitGenName(GenNameAst* ast);
    VisitResult visitNestedName(NestedNameAst* ast);

    //--- Specifiers ---//

    VisitResult visitBuiltinSpec(BuiltinSpecAst* ast);
    VisitResult visitChanSpec(ChanSpecAst* ast);
    VisitResult visitDecoratedSpec(DecoratedSpecAst* ast);
    VisitResult visitOpaqueSpec(OpaqueSpecAst* ast);
    VisitResult visitInferredSpec(InferredSpecAst* ast);
    VisitResult visitPtrSpec(PtrSpecAst* ast);
    VisitResult visitTypeofSpec(TypeofSpecAst* ast);
    VisitResult visitVoidSpec(VoidSpecAst* ast);

    VisitResult traverseNamedSpec(NamedSpecAst* ast);
    VisitResult traverseRecordSpec(RecordSpecAst* ast);
    VisitResult traverseFuncSpec(FuncSpecAst* ast);
    VisitResult traverseArraySpec(ArraySpecAst* ast);

    //--- Attributes ---//

    VisitResult visitVisibilityAttr(VisibilityAttrAst* ast);
    VisitResult visitAnnotAttr(AnnotAttrAst* ast);
    VisitResult visitDeclAttr(DeclAttrAst* ast);
    VisitResult visitAutoAttr(AutoAttrAst* ast);
    VisitResult visitCodegenAttr(CodegenAttrAst* ast);
    VisitResult visitParamDirAttr(ParamDirAttrAst* ast);
    VisitResult visitEvalStrategyAttr(EvalStrategyAttrAst* ast);
    VisitResult visitStorageClassAttr(StorageClassAttrAst* ast);
    VisitResult visitLinkageAttr(LinkageAttrAst* ast);
    VisitResult visitTypeQualAttr(TypeQualAttrAst* ast);

    //--- Declarations ---//

    VisitResult traverseAliasDecl(AliasDeclAst* ast);
    VisitResult traverseBaseDecl(BaseDeclAst* ast);
    VisitResult traverseSelectiveDecl(SelectiveDeclAst* ast);
    VisitResult traverseConstraintDecl(ConstraintDeclAst* ast);
    VisitResult traverseVersionDecl(VersionDeclAst* ast);
    VisitResult traverseEnumDecl(EnumDeclAst* ast);
    VisitResult traverseEnumMemberDecl(EnumMemberDeclAst* ast);
    VisitResult traverseBlockDecl(BlockDeclAst* ast);
    VisitResult traverseForwardDecl(ForwardDeclAst* ast);
    VisitResult traverseFuncDecl(FuncDeclAst* ast);
    VisitResult traverseImportDecl(ImportDeclAst* ast);
    VisitResult traverseModuleDecl(ModuleDeclAst* ast);
    VisitResult traversePackageDecl(PackageDeclAst* ast);
    VisitResult traverseParamGroupDecl(ParamGroupDeclAst* ast);
    VisitResult traverseParamDecl(ParamDeclAst* ast);
    VisitResult traverseTemplateDecl(TemplateDeclAst* ast);
    VisitResult traverseTemplateParamDecl(TemplateParamDeclAst* ast);
    VisitResult traverseTemplateParamClauseDecl(TemplateParamClauseDeclAst* ast);
    VisitResult traverseRecordDecl(RecordDeclAst* ast);
    VisitResult traverseUnitTestDecl(UnitTestDeclAst* ast);
    VisitResult traverseVarGroupDecl(VarGroupDeclAst* ast);
    VisitResult traverseVarDecl(VarDeclAst* ast);

    //--- Expressions ---//

    VisitResult visitAddExpr(AddExprAst* ast);
    VisitResult visitAddrOfExpr(AddrOfExprAst* ast);
    VisitResult visitRecordLitExpr(RecordLitExprAst* ast);
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
    VisitResult visitMemberAccessExpr(MemberAccessExprAst* ast);
    VisitResult visitMinusExpr(MinusExprAst* ast);
    VisitResult visitMixinExpr(MixinExprAst* ast);
    VisitResult visitModExpr(ModExprAst* ast);
    VisitResult visitMulExpr(MulExprAst* ast);
    VisitResult visitNullLitExpr(NullLitExprAst* ast);
    VisitResult visitNumLitExpr(NumLitExprAst* ast);
    VisitResult visitPlusExpr(PlusExprAst* ast);
    VisitResult visitPrimaryExpr(PrimaryExprAst* ast);
    VisitResult visitPowerExpr(PowerExprAst* ast);
    VisitResult visitPtrDerefExpr(PtrDerefExprAst* ast);
    VisitResult visitChanExpr(ChanExprAst* ast);
    VisitResult visitRecordInitExpr(RecordInitExprAst* ast);
    VisitResult visitRelExpr(RelExprAst* ast);
    VisitResult visitShiftExpr(ShiftExprAst* ast);
    VisitResult visitStrLitExpr(StrLitExprAst* ast);
    VisitResult visitSubExpr(SubExprAst* ast);
    VisitResult visitSubrangeExpr(SubrangeExprAst* ast);
    VisitResult visitSuperExpr(SuperExprAst* ast);
    VisitResult visitThisExpr(ThisExprAst* ast);
    VisitResult visitTypeidExpr(TypeidExprAst* ast);
    VisitResult visitUnaryExpr(UnaryExprAst* ast);
    VisitResult visitUnpackExpr(UnpackExprAst* ast);
    VisitResult visitVoidInitExpr(VoidInitExprAst* ast);
    VisitResult visitWrappedExpr(WrappedExprAst* ast);

    // Annotate type of type-specifying exprs.
    VisitResult traverseMakeExpr(MakeExprAst* ast);
    VisitResult traverseNewExpr(NewExprAst* ast);
    VisitResult traverseNestedNewExpr(NestedNewExprAst* ast);
    VisitResult traverseCastExpr(CastExprAst* ast);
    VisitResult traverseTypeQueryExpr(TypeQueryExprAst* ast);
    VisitResult traverseTypeAssertExpr(TypeAssertExprAst* ast);

    //--- Statements ---//

    VisitResult traverseBlockStmt(BlockStmtAst* ast);
};

} // namespace uaiso

#endif
