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

/*--------------------------*/
/*--- The UaiSo! Project ---*/
/*--------------------------*/

#include "Ast/AstLocator.h"
#include "Ast/Ast.h"
#include "Ast/AstDefs.h"
#include "Common/Assert.h"
#include "Parsing/SourceLoc.h"

using namespace uaiso;

AstLocator::~AstLocator()
{}

const SourceLoc& AstLocator::loc(Ast* ast) const \
{
    if (!ast)
        return kEmptyLoc;

#define MAKE_CASE(AST_NODE, AST_KIND) \
    case Ast::Kind::AST_NODE##AST_KIND: \
        return loc(static_cast<AST_NODE##AST_KIND##Ast*>(ast));

    switch (ast->kind()) {
    NAME_AST_MIXIN(MAKE_NAME_CASE)
    SPEC_AST_MIXIN(MAKE_SPEC_CASE)
    ATTR_AST_MIXIN(MAKE_ATTR_CASE)
    EXPR_AST_MIXIN(MAKE_EXPR_CASE)
    DECL_AST_MIXIN(MAKE_DECL_CASE)
    STMT_AST_MIXIN(MAKE_STMT_CASE)
    default:
        UAISO_ASSERT(false, return kEmptyLoc);
        return kEmptyLoc;
    }

#undef MAKE_CASE
}

const SourceLoc& AstLocator::lastLoc(Ast* ast) const \
{
    if (!ast)
        return kEmptyLoc;

#define MAKE_CASE(AST_NODE, AST_KIND) \
    case Ast::Kind::AST_NODE##AST_KIND: \
        return lastLoc(static_cast<AST_NODE##AST_KIND##Ast*>(ast));

    switch (ast->kind()) {
    NAME_AST_MIXIN(MAKE_NAME_CASE)
    SPEC_AST_MIXIN(MAKE_SPEC_CASE)
    ATTR_AST_MIXIN(MAKE_ATTR_CASE)
    EXPR_AST_MIXIN(MAKE_EXPR_CASE)
    DECL_AST_MIXIN(MAKE_DECL_CASE)
    STMT_AST_MIXIN(MAKE_STMT_CASE)
    default:
        UAISO_ASSERT(false, return kEmptyLoc);
        return kEmptyLoc;
    }

#undef MAKE_CASE
}

const SourceLoc& AstLocator::loc(SimpleNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::lastLoc(SimpleNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::loc(PuncNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::lastLoc(PuncNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::loc(SpecialNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::lastLoc(SpecialNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::loc(ErrorNameAst* ast) const
{
    return ast->errorLoc_;
}

const SourceLoc& AstLocator::lastLoc(ErrorNameAst* ast) const
{
    return ast->errorLoc_;
}

const SourceLoc& AstLocator::loc(CompletionNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::lastLoc(CompletionNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::loc(TemplateInstNameAst* ast) const
{
    return loc(ast->name_.get());
}

const SourceLoc& AstLocator::lastLoc(TemplateInstNameAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(GenNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::lastLoc(GenNameAst* ast) const
{
    return ast->nameLoc_;
}

const SourceLoc& AstLocator::loc(NestedNameAst* ast) const
{
    return loc(ast->names_->front());
}

const SourceLoc& AstLocator::lastLoc(NestedNameAst* ast) const
{
    return lastLoc(ast->names_->back());
}

const SourceLoc& AstLocator::loc(RecordSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(RecordSpecAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ArraySpecAst* ast) const
{
    return loc(ast->baseSpec_.get());
}

const SourceLoc& AstLocator::lastLoc(ArraySpecAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(BuiltinSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(BuiltinSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(ChanSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ChanSpecAst* ast) const
{
    return lastLoc(ast->baseSpec_.get());
}

const SourceLoc& AstLocator::loc(DecoratedSpecAst* ast) const
{
    return loc(ast->attrs_->front());
}

const SourceLoc& AstLocator::lastLoc(DecoratedSpecAst* ast) const
{
    return lastLoc(ast->spec_.get());
}

const SourceLoc& AstLocator::loc(OpaqueSpecAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::lastLoc(OpaqueSpecAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::loc(FuncSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(FuncSpecAst* ast) const
{
    return lastLoc(ast->result_.get());
}

const SourceLoc& AstLocator::loc(InferredSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(InferredSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(NamedSpecAst* ast) const
{
    return loc(ast->name_.get());
}

const SourceLoc& AstLocator::lastLoc(NamedSpecAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(PtrSpecAst* ast) const
{
    return loc(ast->baseSpec_.get());
}

const SourceLoc& AstLocator::lastLoc(PtrSpecAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::loc(TypeofSpecAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(TypeofSpecAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(VoidSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(VoidSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(AnnotAttrAst* ast) const
{
    return ast->atLoc_;
}

const SourceLoc& AstLocator::lastLoc(AnnotAttrAst* ast) const
{
    return ast->textLoc_;
}

const SourceLoc& AstLocator::loc(DeclAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(DeclAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(AutoAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(AutoAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(CodegenAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(CodegenAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(ParamDirAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ParamDirAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(EvalStrategyAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(EvalStrategyAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(LinkageAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(LinkageAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(StorageClassAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(StorageClassAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(TypeQualAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(TypeQualAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(VisibilityAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(VisibilityAttrAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(EmptyDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(EmptyDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(AliasDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(AliasDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(BaseDeclAst* ast) const
{
    return loc(ast->name_.get());
}

const SourceLoc& AstLocator::lastLoc(BaseDeclAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(SelectiveDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(SelectiveDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ConstraintDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ConstraintDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(VersionDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(VersionDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(DebugDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(DebugDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(EnumDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(EnumDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(EnumMemberDeclAst* ast) const
{
    return loc(ast->name_.get());
}

const SourceLoc& AstLocator::lastLoc(EnumMemberDeclAst* ast) const
{
    if (ast->hasInit())
        return lastLoc(ast->init());
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(BlockDeclAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(BlockDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ErrorDeclAst* ast) const
{
    return kEmptyLoc;
}

const SourceLoc& AstLocator::lastLoc(ErrorDeclAst* ast) const
{
    return kEmptyLoc;
}

const SourceLoc& AstLocator::loc(FuncDeclAst* ast) const
{
    if (ast->name())
        return loc(ast->name_.get());
    return loc(ast->spec());
}

const SourceLoc& AstLocator::loc(PatDeclAst* ast) const
{
    return loc(ast->name());
}

const SourceLoc& AstLocator::lastLoc(PatDeclAst* ast) const
{
    return lastLoc(ast->name());
}

const SourceLoc& AstLocator::loc(TrivialPatDeclAst* ast) const
{
    return loc(ast->expr());
}

const SourceLoc& AstLocator::lastLoc(TrivialPatDeclAst* ast) const
{
    return lastLoc(ast->expr());
}

const SourceLoc& AstLocator::loc(DestructPatDeclAst* ast) const
{
    return kEmptyLoc;
}

const SourceLoc& AstLocator::lastLoc(DestructPatDeclAst* ast) const
{
    return kEmptyLoc;
}

const SourceLoc& AstLocator::loc(WrappedPatDeclAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(WrappedPatDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(TuplePatDeclAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(TuplePatDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ListPatDeclAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(ListPatDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(AsPatDeclAst* ast) const
{
    return loc(ast->name());
}

const SourceLoc& AstLocator::lastLoc(AsPatDeclAst* ast) const
{
    return lastLoc(ast->pat());
}

const SourceLoc& AstLocator::loc(PatBindDeclAst* ast) const
{
    return loc(ast->pat());
}

const SourceLoc& AstLocator::lastLoc(PatBindDeclAst* ast) const
{
    return lastLoc(ast->bind());
}

const SourceLoc& AstLocator::loc(IrrefutPatDeclAst* ast) const
{
    return loc(ast->pat());
}

const SourceLoc& AstLocator::lastLoc(IrrefutPatDeclAst* ast) const
{
    return lastLoc(ast->pat());
}

const SourceLoc& AstLocator::loc(WildCardPatDeclAst* ast) const
{
    return ast->keyLoc();
}

const SourceLoc& AstLocator::lastLoc(WildCardPatDeclAst* ast) const
{
    return ast->keyLoc();
}

const SourceLoc& AstLocator::lastLoc(FuncDeclAst* ast) const
{
    return lastLoc(ast->spec_.get());
}

const SourceLoc& AstLocator::loc(ForwardDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ForwardDeclAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(ImportGroupDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ImportGroupDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(ImportDeclAst* ast) const
{
    return loc(ast->target_.get());
}

const SourceLoc& AstLocator::lastLoc(ImportDeclAst* ast) const
{
    if (ast->selections_)
        return lastLoc(ast->selections_->back());
    if (ast->localName_)
        return lastLoc(ast->localName_.get());
    return lastLoc(ast->target_.get());
}

const SourceLoc& AstLocator::loc(ImportSelectionDeclAst* ast) const
{
    return loc(ast->actualName_.get());
}

const SourceLoc& AstLocator::lastLoc(ImportSelectionDeclAst* ast) const
{
    if (ast->alternateName_)
        return lastLoc(ast->alternateName_.get());
    return lastLoc(ast->actualName_.get());
}

const SourceLoc& AstLocator::loc(ExportDeclAst* ast) const
{
    return loc(ast->selections_->front());
}

const SourceLoc& AstLocator::lastLoc(ExportDeclAst* ast) const
{
    return lastLoc(ast->selections_->back());
}

const SourceLoc& AstLocator::loc(ExportSelectionDeclAst* ast) const
{
    return loc(ast->name_.get());
}

const SourceLoc& AstLocator::lastLoc(ExportSelectionDeclAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(InvariantDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(InvariantDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ModuleDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ModuleDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(PackageDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(PackageDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(ParamDeclAst* ast) const
{
    UAISO_ASSERT(ast->name_.get() || ast->isVariadic(),
                 return kEmptyLoc,
                 "parameter must either have a name or be variadic");

    if (ast->name_)
        return loc(ast->name_.get());
    return ast->variadicLoc();
}

const SourceLoc& AstLocator::lastLoc(ParamDeclAst* ast) const
{
    UAISO_ASSERT(ast->name_.get() || ast->isVariadic(),
                 return kEmptyLoc,
                 "parameter must either have a name or be variadic");

    if (ast->name_)
        return lastLoc(ast->name_.get());
    return ast->variadicLoc();
}

const SourceLoc& AstLocator::loc(ParamGroupDeclAst* ast) const
{
    return loc(ast->spec_.get());
}

const SourceLoc& AstLocator::lastLoc(ParamGroupDeclAst* ast) const
{
    if (ast->decls_)
        return lastLoc(ast->decls_->back());
    return loc(ast->spec_.get());
}

const SourceLoc& AstLocator::loc(ParamClauseDeclAst* ast) const
{
    if (!ast->lDelimLoc_.isEmpty())
        return ast->lDelimLoc_;
    if (ast->decls_)
        return loc(ast->decls_->front());
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(ParamClauseDeclAst* ast) const
{
    if (!ast->rDelimLoc_.isEmpty())
        return ast->rDelimLoc_;
    if (ast->decls_)
        return lastLoc(ast->decls_->back());
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::loc(SectionDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(SectionDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(StaticAssertDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(StaticAssertDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(TemplateDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(TemplateDeclAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(TemplateParamDeclAst* ast) const
{
    return loc(ast->spec_.get());
}

const SourceLoc& AstLocator::lastLoc(TemplateParamDeclAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(TemplateParamClauseDeclAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(TemplateParamClauseDeclAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(RecordDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(RecordDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(UnitTestDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(UnitTestDeclAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(VarDeclAst* ast) const
{
    return loc(ast->name_.get());
}

const SourceLoc& AstLocator::lastLoc(VarDeclAst* ast) const
{
    if (ast->hasInit())
        return lastLoc(ast->init());
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(VarGroupDeclAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(VarGroupDeclAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(ErrorExprAst* ast) const
{
    return ast->errorLoc_;
}

const SourceLoc& AstLocator::lastLoc(ErrorExprAst* ast) const
{
    return ast->errorLoc_;
}

const SourceLoc& AstLocator::loc(AddExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(AddExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(AddrOfExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(AddrOfExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(RecordLitExprAst* ast) const
{
    return loc(ast->exprOrSpec_.get());
}

const SourceLoc& AstLocator::lastLoc(RecordLitExprAst* ast) const
{
    return lastLoc(ast->init_.get());
}

const SourceLoc& AstLocator::loc(RecordInitExprAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(RecordInitExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ArrayInitExprAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(ArrayInitExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(TupleLitExprAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(TupleLitExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ArrayLengthExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ArrayLengthExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(ArraySliceExprAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(ArraySliceExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(ArrayIndexExprAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(ArrayIndexExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(AssertExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(AssertExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(AssignExprAst* ast) const
{
    return loc(ast->exprs1_->front());
}

const SourceLoc& AstLocator::lastLoc(AssignExprAst* ast) const
{
    return lastLoc(ast->exprs2_->back());
}

const SourceLoc& AstLocator::loc(BinExprAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::lastLoc(BinExprAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::loc(BitAndExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(BitAndExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(BitCompExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(BitCompExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(BitOrExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(BitOrExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(BitXorExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(BitXorExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(BoolLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::lastLoc(BoolLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::loc(CallExprAst* ast) const
{
    return loc(ast->base_.get());
}

const SourceLoc& AstLocator::lastLoc(CallExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(CastExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(CastExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(ChanExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(ChanExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(CharLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::lastLoc(CharLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::loc(CommaExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(CommaExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(ConcatExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(ConcatExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(TerExprAst* ast) const
{
    return loc(ast->cond_.get());
}

const SourceLoc& AstLocator::lastLoc(TerExprAst* ast) const
{
    return lastLoc(ast->no_.get());
}

const SourceLoc& AstLocator::loc(DelExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(DelExprAst* ast) const
{
    return lastLoc(ast->exprs_->back());
}

const SourceLoc& AstLocator::loc(DesignateExprAst* ast) const
{
    return loc(ast->id_.get());
}

const SourceLoc& AstLocator::lastLoc(DesignateExprAst* ast) const
{
    return lastLoc(ast->value_.get());
}

const SourceLoc& AstLocator::loc(DivExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(DivExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(EqExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(EqExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(FuncLitExprAst* ast) const
{
    return loc(ast->spec_.get());
}

const SourceLoc& AstLocator::lastLoc(FuncLitExprAst* ast) const
{
    return lastLoc(ast->spec_.get());
}

const SourceLoc& AstLocator::loc(IdentExprAst* ast) const
{
    return loc(ast->name_.get());
}

const SourceLoc& AstLocator::lastLoc(IdentExprAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(InExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(InExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(IncDecExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(IncDecExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(TypeQueryExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(TypeQueryExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(IsExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(IsExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(LogicAndExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(LogicAndExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(LogicNotExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(LogicNotExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(LogicOrExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(LogicOrExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(MakeExprAst* ast) const
{
    return loc(ast->base_.get());
}

const SourceLoc& AstLocator::lastLoc(MakeExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(MemberAccessExprAst* ast) const
{
    return ast->oprLoc_;
    //return loc(ast->exprOrSpec_.get());
}

const SourceLoc& AstLocator::lastLoc(MemberAccessExprAst* ast) const
{
    return lastLoc(ast->name_.get());
}

const SourceLoc& AstLocator::loc(MinusExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(MinusExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(ModExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(ModExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(MulExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(MulExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(NestedNewExprAst* ast) const
{
    return loc(ast->base_.get());
}

const SourceLoc& AstLocator::lastLoc(NestedNewExprAst* ast) const
{
    return lastLoc(ast->nestedNew_.get());
}

const SourceLoc& AstLocator::loc(NewExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(NewExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(NumLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::lastLoc(NumLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::loc(PlusExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(PlusExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(PriExprAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::lastLoc(PriExprAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::loc(PowerExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(PowerExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(PtrDerefExprAst* ast) const
{
    return ast->oprLoc_;
}

const SourceLoc& AstLocator::lastLoc(PtrDerefExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(NullLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::lastLoc(NullLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::loc(RelExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(RelExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(ShiftExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(ShiftExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(StrLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::lastLoc(StrLitExprAst* ast) const
{
    return ast->litLoc_;
}

const SourceLoc& AstLocator::loc(SubExprAst* ast) const
{
    return loc(ast->expr1_.get());
}

const SourceLoc& AstLocator::lastLoc(SubExprAst* ast) const
{
    return lastLoc(ast->expr2_.get());
}

const SourceLoc& AstLocator::loc(SuperExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(SuperExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(ThisExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ThisExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(TypeAssertExprAst* ast) const
{
    return loc(ast->base_.get());
}

const SourceLoc& AstLocator::lastLoc(TypeAssertExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(TypeidExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(TypeidExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(UnaryExprAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::lastLoc(UnaryExprAst* ast) const
{
    UAISO_ASSERT(false, return kEmptyLoc);

    return kEmptyLoc;
}

const SourceLoc& AstLocator::loc(MixinExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(MixinExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(WrappedExprAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(WrappedExprAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(SubrangeExprAst* ast) const
{
    if (ast->low_)
        return loc(ast->low_.get());
    return ast->delim1Loc_;
}

const SourceLoc& AstLocator::lastLoc(SubrangeExprAst* ast) const
{
    if (ast->hi_)
        return lastLoc(ast->hi_.get());
    return ast->delim1Loc_;
}

const SourceLoc& AstLocator::loc(UnpackExprAst* ast) const
{
    return loc(ast->expr_.get());
}

const SourceLoc& AstLocator::lastLoc(UnpackExprAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(PrintExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(PrintExprAst* ast) const
{
    return lastLoc(ast->exprs_->back());
}

const SourceLoc& AstLocator::loc(YieldExprAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(YieldExprAst* ast) const
{
    return lastLoc(ast->exprs_->back());
}

const SourceLoc& AstLocator::loc(ListCompreExprAst* ast) const
{
    return loc(ast->expr_.get());
}

const SourceLoc& AstLocator::lastLoc(ListCompreExprAst* ast) const
{
    if (ast->gens_) {
        if (ast->gens_->back()->filters_)
            return lastLoc(ast->gens_->back()->filters_->back());
        return lastLoc(ast->gens_->back());
    }
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(BlockStmtAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& AstLocator::lastLoc(BlockStmtAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(BodyStmtAst* ast) const
{
    return ast->bodyLoc_;
}

const SourceLoc& AstLocator::lastLoc(BodyStmtAst* ast) const
{
    return lastLoc(ast->block_.get());
}

const SourceLoc& AstLocator::loc(ContractStmtAst* ast) const
{
    return loc(ast->stmt1_.get());
}

const SourceLoc& AstLocator::lastLoc(ContractStmtAst* ast) const
{
    return lastLoc(ast->stmt2_.get());
}

const SourceLoc& AstLocator::loc(ExprStmtAst* ast) const
{
    if (ast->exprs_)
        return loc(ast->exprs_->back());
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::lastLoc(ExprStmtAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(DeclStmtAst* ast) const
{
    return loc(ast->decl_.get());
}

const SourceLoc& AstLocator::lastLoc(DeclStmtAst* ast) const
{
    return lastLoc(ast->decl_.get());
}

const SourceLoc& AstLocator::loc(InStmtAst* ast) const
{
    return ast->inLoc_;
}

const SourceLoc& AstLocator::lastLoc(InStmtAst* ast) const
{
    return lastLoc(ast->block_.get());
}

const SourceLoc& AstLocator::loc(OutStmtAst* ast) const
{
    return ast->outLoc_;
}

const SourceLoc& AstLocator::lastLoc(OutStmtAst* ast) const
{
    return lastLoc(ast->block_.get());
}

const SourceLoc& AstLocator::loc(EmptyStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(EmptyStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::loc(LabeledStmtAst* ast) const
{
    return loc(ast->label_.get());
}

const SourceLoc& AstLocator::lastLoc(LabeledStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(IfStmtAst* ast) const
{
    return ast->ifLoc_;
}

const SourceLoc& AstLocator::lastLoc(IfStmtAst* ast) const
{
    if (ast->notThen_)
        return lastLoc(ast->notThen_.get());
    return lastLoc(ast->then_.get());
}

const SourceLoc& AstLocator::loc(WhileStmtAst* ast) const
{
    return ast->whileLoc_;
}

const SourceLoc& AstLocator::lastLoc(WhileStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(DoWhileStmtAst* ast) const
{
    return ast->doLoc_;
}

const SourceLoc& AstLocator::lastLoc(DoWhileStmtAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(ForStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ForStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(ForeachStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ForeachStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(ContinueStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ContinueStmtAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(BreakStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(BreakStmtAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(ReturnStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ReturnStmtAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(GotoStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(GotoStmtAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(ThrowStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(ThrowStmtAst* ast) const
{
    return ast->terminLoc_;
}

const SourceLoc& AstLocator::loc(SwitchStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(SwitchStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(TypeSwitchStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(TypeSwitchStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(CaseClauseStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(CaseClauseStmtAst* ast) const
{
    return lastLoc(ast->stmts_->back());
}

const SourceLoc& AstLocator::loc(DefaultClauseStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(DefaultClauseStmtAst* ast) const
{
    return lastLoc(ast->stmts_->back());
}

const SourceLoc& AstLocator::loc(SyncedStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(SyncedStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(WithStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(WithStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(TryStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(TryStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(CatchClauseStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(CatchClauseStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(FinallyClauseStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(FinallyClauseStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(DeferredStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(DeferredStmtAst* ast) const
{
    return lastLoc(ast->stmt_.get());
}

const SourceLoc& AstLocator::loc(ErrorStmtAst* ast) const
{
    return ast->errorLoc_;
}

const SourceLoc& AstLocator::lastLoc(ErrorStmtAst* ast) const
{
    return ast->errorLoc_;
}

const SourceLoc& AstLocator::loc(SelectiveStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(SelectiveStmtAst* ast) const
{
    return ast->rDelimLoc_;
}

const SourceLoc& AstLocator::loc(AsyncStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(AsyncStmtAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(YieldStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(YieldStmtAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(EvalStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(EvalStmtAst* ast) const
{
    return lastLoc(ast->expr_.get());
}

const SourceLoc& AstLocator::loc(FallthroughStmtAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& AstLocator::lastLoc(FallthroughStmtAst* ast) const
{
    return ast->keyLoc_;
}
