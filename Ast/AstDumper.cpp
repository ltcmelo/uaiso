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

#include "Ast/AstDumper.h"
#include <iostream>

using namespace uaiso;

namespace {

std::size_t level__ = 0;

std::ostream* output__ = nullptr;

std::ostream& level(std::ostream& os)
{
    std::string s(level__, ' ');
    os << s;
    return os;
}

} // anonymous

template <class AstT> AstDumper::VisitResult
AstDumper::traverse(VisitResult (Base::*function)(AstT*), AstT* ast)
{
    ++level__;
    VisitResult result = ((this)->*(function))(ast);
    --level__;
    return result;
}

AstDumper::VisitResult AstDumper::traverseName(NameAst* ast)
{
    return traverse(&Base::traverseName, ast);
}

AstDumper::VisitResult AstDumper::traverseSpec(SpecAst* ast)
{
    return traverse(&Base::traverseSpec, ast);
}

AstDumper::VisitResult AstDumper::traverseDecl(DeclAst* ast)
{
    return traverse(&Base::traverseDecl, ast);
}

AstDumper::VisitResult AstDumper::traverseExpr(ExprAst* ast)
{
    return traverse(&Base::traverseExpr, ast);
}

AstDumper::VisitResult AstDumper::traverseStmt(StmtAst* ast)
{
    return traverse(&Base::traverseStmt, ast);
}

void AstDumper::dumpProgram(ProgramAst* ast)
{
    dumpProgram(ast, std::cout);
}

void AstDumper::dumpProgram(ProgramAst* ast, std::ostream& os)
{
    output__ = &os;

    traverse(&Base::traverseDecl, ast->module_.get());
    traverse(&Base::traverseDecl, ast->package_.get());
    std::for_each(ast->decls_->begin(), ast->decls_->end(),
                  [this] (DeclAst* decl) { traverse(&Base::traverseDecl, decl); });
    std::for_each(ast->stmts_->begin(), ast->stmts_->end(),
                  [this] (StmtAst* stmt) { traverse(&Base::traverseStmt, stmt); });

    *output__ << std::endl;
}

AstDumper::VisitResult AstDumper::visitSimpleName(SimpleNameAst* ast)
{
    *output__ << level << "SimpleNameAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCompletionName(CompletionNameAst* ast)
{
    *output__ << level << "CompletionNameAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTemplateInstName(TemplateInstNameAst* ast)
{
    *output__ << level << "TemplateInstNameAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitGenName(GenNameAst* ast)
{
    *output__ << level << "GenNameAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitNestedName(NestedNameAst* ast)
{
    *output__ << level << "NestedNameAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitRecordSpec(RecordSpecAst* ast)
{
    *output__ << level << "RecordSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitArraySpec(ArraySpecAst* ast)
{
    *output__ << level << "ArraySpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBuiltinSpec(BuiltinSpecAst* ast)
{
    *output__ << level << "BuiltinSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitChanSpec(ChanSpecAst* ast)
{
    *output__ << level << "ChanSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDecoratedSpec(DecoratedSpecAst* ast)
{
    *output__ << level << "DecoratedSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitOpaqueSpec(OpaqueSpecAst* ast)
{
    *output__ << level << "OpaqueSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitFuncSpec(FuncSpecAst* ast)
{
    *output__ << level << "FuncSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitNamedSpec(NamedSpecAst* ast)
{
    *output__ << level << "NamedSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitPtrSpec(PtrSpecAst* ast)
{
    *output__ << level << "PtrSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTypeofSpec(TypeofSpecAst* ast)
{
    *output__ << level << "TypeofSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitVoidSpec(VoidSpecAst* ast)
{
    *output__ << level << "VoidSpecAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAccessAttr(VisibilityAttrAst* ast)
{
    *output__ << level << "VisibilityAttrAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAnnotAttr(AnnotAttrAst* ast)
{
    *output__ << level << "AnnotAttrAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCodegenAttr(CodegenAttrAst* ast)
{
    *output__ << level << "CodegenAttrAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitStorageClassAttr(StorageClassAttrAst* ast)
{
    *output__ << level << "StorageClassAttrAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTypeQualAttr(TypeQualAttrAst* ast)
{
    *output__ << level << "TypeQualAttrAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAliasDecl(AliasDeclAst* ast)
{
    *output__ << level << "AliasDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBaseDecl(BaseDeclAst* ast)
{
    *output__ << level << "BaseDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBlockDecl(BlockDeclAst* ast)
{
    *output__ << level << "BlockDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSelectiveDecl(SelectiveDeclAst* ast)
{
    *output__ << level << "SelectiveDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitConstraintDecl(ConstraintDeclAst* ast)
{
    *output__ << level << "ConstraintDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitVersionDecl(VersionDeclAst* ast)
{
    *output__ << level << "VersionDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitEnumDecl(EnumDeclAst* ast)
{
    *output__ << level << "EnumDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitErrorDecl(ErrorDeclAst* ast)
{
    *output__ << level << "ErrorDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitFuncDecl(FuncDeclAst* ast)
{
    *output__ << level << "FuncDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitForwardDecl(ForwardDeclAst* ast)
{
    *output__ << level << "ForwardDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitVarDecl(VarDeclAst* ast)
{
    *output__ << level << "VarDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitImportDecl(ImportDeclAst* ast)
{
    *output__ << level << "ImportDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitInvariantDecl(InvariantDeclAst* ast)
{
    *output__ << level << "InvariantDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitParamDecl(ParamDeclAst* ast)
{
    *output__ << level << "ParamDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitParamClauseDecl(ParamClauseDeclAst* ast)
{
    *output__ << level << "ParamClauseDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSectionDecl(SectionDeclAst* ast)
{
    *output__ << level << "SectionDeclAst";
    if (ast->variety() == SectionVariety::Vars)
        *output__ << "[Var]";
    else if (ast->variety() == SectionVariety::Types)
        *output__ << "[Type]";
    *output__ << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitStaticAssertDecl(StaticAssertDeclAst* ast)
{
    *output__ << level << "StaticAssertDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTemplateDecl(TemplateDeclAst* ast)
{
    *output__ << level << "TemplateDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTemplateParamDecl(TemplateParamDeclAst* ast)
{
    *output__ << level << "TemplateParamDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTemplateParamClauseDecl(TemplateParamClauseDeclAst* ast)
{
    *output__ << level << "TemplateParamClauseDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitRecordDecl(RecordDeclAst* ast)
{
    *output__ << level << "RecordDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitUnitTestDecl(UnitTestDeclAst* ast)
{
    *output__ << level << "UnitTestDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitVarGroupDecl(VarGroupDeclAst* ast)
{
    *output__ << level << "VarGroupDeclAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAddExpr(AddExprAst* ast)
{
    *output__ << level << "AddExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAddrOfExpr(AddrOfExprAst* ast)
{
    *output__ << level << "AddrOfExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitRecordLitExpr(RecordLitExprAst* ast)
{
    *output__ << level << "RecordLitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitRecordInitExpr(RecordInitExprAst* ast)
{
    *output__ << level << "RecordInitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitArrayInitExpr(ArrayInitExprAst* ast)
{
    *output__ << level << "ArrayInitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitArrayLengthExpr(ArrayLengthExprAst* ast)
{
    *output__ << level << "ArrayLengthExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitArraySliceExpr(ArraySliceExprAst* ast)
{
    *output__ << level << "ArraySliceExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitArrayIndexExpr(ArrayIndexExprAst* ast)
{
    *output__ << level << "ArrayIndexExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAssertExpr(AssertExprAst* ast)
{
    *output__ << level << "AssertExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAssignExpr(AssignExprAst* ast)
{
    *output__ << level << "AssignExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBinExpr(BinExprAst* ast)
{
    *output__ << level << "BinExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBitAndExpr(BitAndExprAst* ast)
{
    *output__ << level << "BitAndExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBitCompExpr(BitCompExprAst* ast)
{
    *output__ << level << "BitCompExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBitOrExpr(BitOrExprAst* ast)
{
    *output__ << level << "BitOrExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBitXorExpr(BitXorExprAst* ast)
{
    *output__ << level << "BitXorExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBoolLitExpr(BoolLitExprAst* ast)
{
    *output__ << level << "BoolLitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCallExpr(CallExprAst* ast)
{
    *output__ << level << "CallExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCastExpr(CastExprAst* ast)
{
    *output__ << level << "CastExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCharLitExpr(CharLitExprAst* ast)
{
    *output__ << level << "CharLitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCommaExpr(CommaExprAst* ast)
{
    *output__ << level << "CommaExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitConcatExpr(ConcatExprAst* ast)
{
    *output__ << level << "ConcatExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTerExpr(TerExprAst* ast)
{
    *output__ << level << "TerExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDelExpr(DelExprAst* ast)
{
    *output__ << level << "DelExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDesignateExpr(DesignateExprAst* ast)
{
    *output__ << level << "DesignateExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDivExpr(DivExprAst* ast)
{
    *output__ << level << "DivExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitEqExpr(EqExprAst* ast)
{
    *output__ << level << "EqExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitLambdaExpr(LambdaExprAst* ast)
{
    *output__ << level << "LambdaExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitIdentExpr(IdentExprAst* ast)
{
    *output__ << level << "IdentExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitInExpr(InExprAst* ast)
{
    *output__ << level << "InExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitIncDecExpr(IncDecExprAst* ast)
{
    *output__ << level << "IncDecExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitIsExpr(IsExprAst* ast)
{
    *output__ << level << "IsExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitLogicAndExpr(LogicAndExprAst* ast)
{
    *output__ << level << "LogicAndExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitLogicNotExpr(LogicNotExprAst* ast)
{
    *output__ << level << "LogicNotExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitLogicOrExpr(LogicOrExprAst* ast)
{
    *output__ << level << "LogicOrExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitMakeExpr(MakeExprAst* ast)
{
    *output__ << level << "MakeExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitMemberAccessExpr(MemberAccessExprAst* ast)
{
    *output__ << level << "MemberAccessExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitMinusExpr(MinusExprAst* ast)
{
    *output__ << level << "MinusExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitModExpr(ModExprAst* ast)
{
    *output__ << level << "ModExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitMulExpr(MulExprAst* ast)
{
    *output__ << level << "MulExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitNestedNewExpr(NestedNewExprAst* ast)
{
    *output__ << level << "NestedNewExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitNewExpr(NewExprAst* ast)
{
    *output__ << level << "NewExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitNumLitExpr(NumLitExprAst* ast)
{
    *output__ << level << "NumLitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitPlusExpr(PlusExprAst* ast)
{
    *output__ << level << "PlusExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitPriExpr(PriExprAst* ast)
{
    *output__ << level << "PriExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitPowerExpr(PowerExprAst* ast)
{
    *output__ << level << "PowerExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitPtrDerefExpr(PtrDerefExprAst* ast)
{
    *output__ << level << "PtrDerefExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitNullLitExpr(NullLitExprAst* ast)
{
    *output__ << level << "NullLitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitChanExpr(ChanExprAst* ast)
{
    *output__ << level << "ChanExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitRelExpr(RelExprAst* ast)
{
    *output__ << level << "RelExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitShiftExpr(ShiftExprAst* ast)
{
    *output__ << level << "ShiftExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitStrLitExpr(StrLitExprAst* ast)
{
    *output__ << level << "StrLitExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSubExpr(SubExprAst* ast)
{
    *output__ << level << "SubExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSuperExpr(SuperExprAst* ast)
{
    *output__ << level << "SuperExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitThisExpr(ThisExprAst* ast)
{
    *output__ << level << "ThisExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTypeAssertExpr(TypeAssertExprAst* ast)
{
    *output__ << level << "TypeAssertExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTypeidExpr(TypeidExprAst* ast)
{
    *output__ << level << "TypeidExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitUnaryExpr(UnaryExprAst* ast)
{
    *output__ << level << "UnaryExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitMixinExpr(MixinExprAst* ast)
{
    *output__ << level << "MixinExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitWrappedExpr(WrappedExprAst* ast)
{
    *output__ << level << "WrappedExprAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBlockStmt(BlockStmtAst* ast)
{
    *output__ << level << "BlockStmtAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBodyStmt(BodyStmtAst* ast)
{
    *output__ << level << "BodyStmtAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitContractStmt(ContractStmtAst* ast)
{
    *output__ << level << "ContractStmtAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDeclStmt(DeclStmtAst* ast)
{
    *output__ << level << "DeclStmtAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitExprStmt(ExprStmtAst* ast)
{
    *output__ << level << "ExprStmtAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitInStmt(InStmtAst* ast)
{
    *output__ << level << "InStmtAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitOutStmt(OutStmtAst* ast)
{
    *output__ << level << "OutStmtAst" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitEmptyStmt(EmptyStmtAst* ast)
{
    *output__ << level << "EmptyStmtAst" << std::endl;
    return Continue;
}
AstDumper::VisitResult AstDumper::visitInferredSpec(InferredSpecAst* ast)
{
    *output__ << level << "InferredSpec" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDeclAttr(DeclAttrAst* ast)
{
    *output__ << level << "DeclAttr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAutoAttr(AutoAttrAst* ast)
{
    *output__ << level << "AutoAttr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitEvalStrategyAttr(EvalStrategyAttrAst* ast)
{
    *output__ << level << "EvalStrategyAttr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitLinkageAttr(LinkageAttrAst* ast)
{
    *output__ << level << "LinkageAttr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitParamDirAttr(ParamDirAttrAst* ast)
{
    *output__ << level << "ParamDirAttr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitVisibilityAttr(VisibilityAttrAst* ast)
{
    *output__ << level << "VisibilityAttr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitEnumMemberDecl(EnumMemberDeclAst* ast)
{
    *output__ << level << "EnumMemberDecl" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitModuleDecl(ModuleDeclAst* ast)
{
    *output__ << level << "ModuleDecl" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitPackageDecl(PackageDeclAst* ast)
{
    *output__ << level << "PackageDecl" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitParamGroupDecl(ParamGroupDeclAst* ast)
{
    *output__ << level << "ParamGroupDecl" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTypeQueryExpr(TypeQueryExprAst* ast)
{
    *output__ << level << "TypeQueryExpr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSubrangeExpr(SubrangeExprAst* ast)
{
    *output__ << level << "SubrangeExpr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitUnpackExpr(UnpackExprAst* ast)
{
    *output__ << level << "UnpackExpr" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitAsyncStmt(AsyncStmtAst* ast)
{
    *output__ << level << "AsyncStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitBreakStmt(BreakStmtAst* ast)
{
    *output__ << level << "BreakStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCaseClauseStmt(CaseClauseStmtAst* ast)
{
    *output__ << level << "CaseClauseStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitCatchClauseStmt(CatchClauseStmtAst* ast)
{
    *output__ << level << "CatchClauseStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSelectiveStmt(SelectiveStmtAst* ast)
{
    *output__ << level << "SelectiveStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitContinueStmt(ContinueStmtAst* ast)
{
    *output__ << level << "ContinueStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDefaultClauseStmt(DefaultClauseStmtAst* ast)
{
    *output__ << level << "DefaultClauseStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDeferredStmt(DeferredStmtAst* ast)
{
    *output__ << level << "DeferredStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitDoWhileStmt(DoWhileStmtAst* ast)
{
    *output__ << level << "DoWhileStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitErrorStmt(ErrorStmtAst* ast)
{
    *output__ << level << "ErrorStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitFallthroughStmt(FallthroughStmtAst* ast)
{
    *output__ << level << "FallthroughStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitFinallyClauseStmt(FinallyClauseStmtAst* ast)
{
    *output__ << level << "FinallyClauseStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitForStmt(ForStmtAst* ast)
{
    *output__ << level << "ForStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitForeachStmt(ForeachStmtAst* ast)
{
    *output__ << level << "ForeachStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitGotoStmt(GotoStmtAst* ast)
{
    *output__ << level << "GotoStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitIfStmt(IfStmtAst* ast)
{
    *output__ << level << "IfStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitLabeledStmt(LabeledStmtAst* ast)
{
    *output__ << level << "LabeledStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitReturnStmt(ReturnStmtAst* ast)
{
    *output__ << level << "ReturnStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSyncedStmt(SyncedStmtAst* ast)
{
    *output__ << level << "SyncedStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitSwitchStmt(SwitchStmtAst* ast)
{
    *output__ << level << "SwitchStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitThrowStmt(ThrowStmtAst* ast)
{
    *output__ << level << "ThrowStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitTryStmt(TryStmtAst* ast)
{
    *output__ << level << "TryStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitWhileStmt(WhileStmtAst* ast)
{
    *output__ << level << "WhileStmt" << std::endl;
    return Continue;
}

AstDumper::VisitResult AstDumper::visitWithStmt(WithStmtAst* ast)
{
    *output__ << level << "WithStmt" << std::endl;
    return Continue;
}

