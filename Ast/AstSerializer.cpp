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

/*---------------------------------------------------------------*/
/*---                    The UaiSo! Project                   ---*/
/*---                                                         ---*/
/*---         This file was AUTOMATICALLY GENERATED           ---*/
/*--- It can be AUTOMATICALLY UPDATED if the formatting/style ---*/
/*--- of the method declarations is preserved (it's fragile). ---*/
/*---------------------------------------------------------------*/

#include "Ast/AstSerializer.h"
#include <sstream>

using namespace uaiso;

namespace {

std::ostream* output__ = nullptr;

} // anonymous

namespace uaiso {

std::string serialize(ProgramAst* ast)
{
    std::ostringstream oss;
    AstSerializer().serializeProgram(ast, oss);
    return oss.str();
}

std::unique_ptr<ProgramAst> deserialize(const std::string& s)
{
    throw 1; // Not implemented
    return std::unique_ptr<ProgramAst>(nullptr);
}

} // namespace uaiso

template <class AstT> AstSerializer::VisitResult
AstSerializer::traverse(VisitResult (Base::*function)(AstT*), AstT* ast)
{
    if (!ast)
        *output__ << "!";

    return ((this)->*(function))(ast);
}

AstSerializer::VisitResult AstSerializer::traverseName(NameAst* ast)
{
    return traverse(&Base::traverseName, ast);
}

AstSerializer::VisitResult AstSerializer::traverseSpec(SpecAst* ast)
{
    return traverse(&Base::traverseSpec, ast);
}

AstSerializer::VisitResult AstSerializer::traverseAttr(AttrAst* ast)
{
    return traverse(&Base::traverseAttr, ast);
}

AstSerializer::VisitResult AstSerializer::traverseDecl(DeclAst* ast)
{
    return traverse(&Base::traverseDecl, ast);
}

AstSerializer::VisitResult AstSerializer::traverseExpr(ExprAst* ast)
{
    return traverse(&Base::traverseExpr, ast);
}

AstSerializer::VisitResult AstSerializer::traverseStmt(StmtAst* ast)
{
    return traverse(&Base::traverseStmt, ast);
}

void AstSerializer::serializeProgram(ProgramAst *ast)
{
    serializeProgram(ast, std::cout);
}

void AstSerializer::serializeProgram(ProgramAst *ast, std::ostream& os)
{
    output__ = &os;

    traverseDecl(ast->module_.get());
    traverseDecl(ast->package_.get());
    std::for_each(ast->decls_->begin(), ast->decls_->end(),
                  [this] (DeclAst* decl) { traverseDecl(decl); });

    *output__ << std::endl;
}

void AstSerializer::enterList()
{
    *output__ << "[";
}

void AstSerializer::leaveList()
{
    *output__ << "]";
}

AstSerializer::VisitResult AstSerializer::traverseSimpleName(SimpleNameAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseSimpleName(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTemplateInstName(TemplateInstNameAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTemplateInstName(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseGenName(GenNameAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseGenName(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseNestedName(NestedNameAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseNestedName(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseRecordSpec(RecordSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseRecordSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseArraySpec(ArraySpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseArraySpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBuiltinSpec(BuiltinSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBuiltinSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseChanSpec(ChanSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseChanSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseDecoratedSpec(DecoratedSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseDecoratedSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseOpaqueSpec(OpaqueSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseOpaqueSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseFuncSpec(FuncSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseFuncSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseInferredSpec(InferredSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseInferredSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseNamedSpec(NamedSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseNamedSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traversePtrSpec(PtrSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traversePtrSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTypeofSpec(TypeofSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTypeofSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseVoidSpec(VoidSpecAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseVoidSpec(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseAnnotAttr(AnnotAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseAnnotAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseDeclAttr(DeclAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseDeclAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseAutoAttr(AutoAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseAutoAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseCodegenAttr(CodegenAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseCodegenAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseEvalStrategyAttr(EvalStrategyAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseEvalStrategyAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseLinkageAttr(LinkageAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseLinkageAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseParamDirAttr(ParamDirAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseParamDirAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseStorageClassAttr(StorageClassAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseStorageClassAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTypeQualAttr(TypeQualAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTypeQualAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseVisibilityAttr(VisibilityAttrAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseVisibilityAttr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseAliasDecl(AliasDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseAliasDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBaseDecl(BaseDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBaseDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseSelectiveDecl(SelectiveDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseSelectiveDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseConstraintDecl(ConstraintDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseConstraintDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseVersionDecl(VersionDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseVersionDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseEnumDecl(EnumDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseEnumDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseEnumMemberDecl(EnumMemberDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseEnumMemberDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBlockDecl(BlockDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBlockDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseErrorDecl(ErrorDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseErrorDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseFuncDecl(FuncDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseFuncDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseForwardDecl(ForwardDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseForwardDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseImportDecl(ImportDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseImportDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseInvariantDecl(InvariantDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseInvariantDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseModuleDecl(ModuleDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseModuleDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traversePackageDecl(PackageDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traversePackageDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseParamDecl(ParamDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseParamDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseParamGroupDecl(ParamGroupDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseParamGroupDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseParamClauseDecl(ParamClauseDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseParamClauseDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseSectionDecl(SectionDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseSectionDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseStaticAssertDecl(StaticAssertDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseStaticAssertDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTemplateDecl(TemplateDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTemplateDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTemplateParamDecl(TemplateParamDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTemplateParamDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTemplateParamClauseDecl(TemplateParamClauseDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTemplateParamClauseDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseRecordDecl(RecordDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseRecordDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseUnitTestDecl(UnitTestDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseUnitTestDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseVarDecl(VarDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseVarDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseVarGroupDecl(VarGroupDeclAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseVarGroupDecl(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseAddExpr(AddExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseAddExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseAddrOfExpr(AddrOfExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseAddrOfExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseRecordLitExpr(RecordLitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseRecordLitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseRecordInitExpr(RecordInitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseRecordInitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseArrayInitExpr(ArrayInitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseArrayInitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseArrayLengthExpr(ArrayLengthExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseArrayLengthExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseArraySliceExpr(ArraySliceExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseArraySliceExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseArrayIndexExpr(ArrayIndexExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseArrayIndexExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseAssertExpr(AssertExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseAssertExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseAssignExpr(AssignExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseAssignExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBinExpr(BinExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBinExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBitAndExpr(BitAndExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBitAndExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBitCompExpr(BitCompExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBitCompExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBitOrExpr(BitOrExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBitOrExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBitXorExpr(BitXorExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBitXorExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBoolLitExpr(BoolLitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBoolLitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseCallExpr(CallExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseCallExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseCastExpr(CastExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseCastExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseChanExpr(ChanExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseChanExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseCharLitExpr(CharLitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseCharLitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseCommaExpr(CommaExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseCommaExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseConcatExpr(ConcatExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseConcatExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTerExpr(TerExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTerExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseDelExpr(DelExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseDelExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseDesignateExpr(DesignateExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseDesignateExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseDivExpr(DivExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseDivExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseEqExpr(EqExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseEqExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseFuncLitExpr(FuncLitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseFuncLitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseIdentExpr(IdentExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseIdentExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseInExpr(InExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseInExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseIncDecExpr(IncDecExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseIncDecExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTypeQueryExpr(TypeQueryExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTypeQueryExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseIsExpr(IsExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseIsExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseLogicAndExpr(LogicAndExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseLogicAndExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseLogicNotExpr(LogicNotExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseLogicNotExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseLogicOrExpr(LogicOrExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseLogicOrExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseMakeExpr(MakeExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseMakeExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseMemberAccessExpr(MemberAccessExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseMemberAccessExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseMinusExpr(MinusExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseMinusExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseModExpr(ModExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseModExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseMulExpr(MulExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseMulExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseNestedNewExpr(NestedNewExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseNestedNewExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseNewExpr(NewExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseNewExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseNumLitExpr(NumLitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseNumLitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traversePlusExpr(PlusExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traversePlusExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traversePriExpr(PriExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traversePriExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traversePowerExpr(PowerExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traversePowerExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traversePtrDerefExpr(PtrDerefExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traversePtrDerefExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseNullLitExpr(NullLitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseNullLitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseRelExpr(RelExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseRelExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseShiftExpr(ShiftExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseShiftExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseStrLitExpr(StrLitExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseStrLitExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseSubExpr(SubExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseSubExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseSuperExpr(SuperExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseSuperExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseThisExpr(ThisExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseThisExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTypeAssertExpr(TypeAssertExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTypeAssertExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseTypeidExpr(TypeidExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseTypeidExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseUnaryExpr(UnaryExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseUnaryExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseMixinExpr(MixinExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseMixinExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseWrappedExpr(WrappedExprAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseWrappedExpr(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBlockStmt(BlockStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBlockStmt(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseBodyStmt(BodyStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseBodyStmt(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseContractStmt(ContractStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseContractStmt(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseExprStmt(ExprStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseExprStmt(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseDeclStmt(DeclStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseDeclStmt(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseInStmt(InStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseInStmt(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseOutStmt(OutStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseOutStmt(ast);
    *output__ << ")";
    return Continue;
}

AstSerializer::VisitResult AstSerializer::traverseEmptyStmt(EmptyStmtAst* ast)
{
    *output__ << "(" << static_cast<size_t>(ast->kind());
    Base::traverseEmptyStmt(ast);
    *output__ << ")";
    return Continue;
}

