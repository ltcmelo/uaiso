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

#ifndef UAISO_ASTVISITOR_H__
#define UAISO_ASTVISITOR_H__

#include "Ast/Ast.h"
#include "Ast/AstDefs.h"
#include "Common/Assert.h"
#include <iostream>

namespace uaiso {

#define DECL_RESULT VisitResult result

#define EVAL_RESULT_CORE(INVOCATION) \
    do { \
        if ((result = actualVisitor().INVOCATION) == Abort) \
            return result; \
    } while (0) \

#define EVAL_RESULT_LIST_CORE(INVOCATION) \
    do { \
        if ((result = INVOCATION) == Abort) \
            return result; \
    } while (0) \

#define EVAL_RESULT_0(INVOCATION) \
    DECL_RESULT; \
    EVAL_RESULT_CORE(INVOCATION);

#define EVAL_RESULT_N(INVOCATION) \
    do { \
        if (result == Continue) \
            EVAL_RESULT_CORE(INVOCATION); \
    } while (0) \

#define EVAL_RESULT_LIST_0(INVOCATION) \
    DECL_RESULT; \
    EVAL_RESULT_LIST_CORE(INVOCATION);

#define EVAL_RESULT_LIST_N(INVOCATION) \
    do { \
        if (result == Continue) \
            EVAL_RESULT_LIST_CORE(INVOCATION); \
    } while (0) \

/*!
 * \brief The AstVisitor class
 *
 * This visitor follows the design of Clang's RecursiveASTVisitor. It has
 * a few additions and slight modifications.
 */
template <class DerivedT>
class AstVisitor
{
    typedef AstVisitor<DerivedT> Self;

public:
    //! Result of a visit.
    enum VisitResult : char
    {
        Continue,
        Skip,
        Abort
    };

    DerivedT& actualVisitor() { return *static_cast<DerivedT*>(this); }

    void enterList() {}
    void leaveList() {}

#define KIND_VISIT(AST_KIND) \
    VisitResult traverse##AST_KIND(AST_KIND##Ast*); \
    VisitResult recursivelyVisit##AST_KIND(AST_KIND##Ast* ast) { return actualVisitor().visit##AST_KIND(ast); } \
    VisitResult visit##AST_KIND(AST_KIND##Ast*) { return Continue; }\

KIND_VISIT(Name)
KIND_VISIT(Spec)
KIND_VISIT(Attr)
KIND_VISIT(Decl)
KIND_VISIT(Expr)
KIND_VISIT(Stmt)
#undef KIND_VISIT

#define NODE_VISIT(AST_NODE, AST_NODE_BASE, AST_KIND) \
    VisitResult traverse##AST_NODE##AST_KIND(AST_NODE##AST_KIND##Ast* ast); \
    VisitResult recursivelyVisit##AST_NODE##AST_KIND(AST_NODE##AST_KIND##Ast* ast) \
    { \
        EVAL_RESULT_0(recursivelyVisit##AST_NODE_BASE(ast)); \
        EVAL_RESULT_N(visit##AST_NODE##AST_KIND(ast)); \
        return result; \
    } \
    VisitResult visit##AST_NODE##AST_KIND(AST_NODE##AST_KIND##Ast*) { return Continue; }

#define NAME_NODES(AST_NODE, AST_NODE_BASE) NODE_VISIT(AST_NODE, AST_NODE_BASE, Name)
#define SPEC_NODES(AST_NODE, AST_NODE_BASE) NODE_VISIT(AST_NODE, AST_NODE_BASE, Spec)
#define ATTR_NODES(AST_NODE, AST_NODE_BASE) NODE_VISIT(AST_NODE, AST_NODE_BASE, Attr)
#define DECL_NODES(AST_NODE, AST_NODE_BASE) NODE_VISIT(AST_NODE, AST_NODE_BASE, Decl)
#define EXPR_NODES(AST_NODE, AST_NODE_BASE) NODE_VISIT(AST_NODE, AST_NODE_BASE, Expr)
#define STMT_NODES(AST_NODE, AST_NODE_BASE) NODE_VISIT(AST_NODE, AST_NODE_BASE, Stmt)

NAME_AST_MIXIN(NAME_NODES)
SPEC_AST_MIXIN(SPEC_NODES)
ATTR_AST_MIXIN(ATTR_NODES)
DECL_AST_MIXIN(DECL_NODES)
EXPR_AST_MIXIN(EXPR_NODES)
STMT_AST_MIXIN(STMT_NODES)

#undef NODE_VISIT
#undef NAME_NODES
#undef SPEC_NODES
#undef ATTR_NODES
#undef DECL_NODES
#undef EXPR_NODES
#undef STMT_NODES

protected:
    template <class AstT, class AstListT>
    VisitResult traverseList(AstListT* list, VisitResult (DerivedT::*traverse)(AstT*));
};

template <class DerivedT> template <class AstT, class AstListT>
typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseList(AstListT* list,
                                   VisitResult (DerivedT::*traverse) (AstT*))
{
    actualVisitor().enterList();
    VisitResult result;
    for (auto ast : *list) {
        result = ((actualVisitor()).*(traverse))(ast);
        if (result != Continue) {
            actualVisitor().leaveList();
            return result;
        }
    }
    actualVisitor().leaveList();
    return Continue;
}

#define DEFINE_KIND_TRAVERSAL(AST_KIND, CASE_MAKER) \
    template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult \
    AstVisitor<DerivedT>::traverse##AST_KIND(AST_KIND##Ast* ast) \
    { \
        if (!ast) \
            return Continue; \
        VisitResult result; \
        switch (ast->kind()) { \
        CASE_MAKER \
        default: \
            UAISO_ASSERT(false, return Abort); \
            break; \
        } \
        return Continue; \
    }

#define MAKE_CASE(AST_NODE, AST_KIND) \
    case Ast::Kind::AST_NODE##AST_KIND: \
        result = actualVisitor().traverse##AST_NODE##AST_KIND(static_cast<AST_NODE##AST_KIND##Ast*>(ast)); \
        if (result != Continue) \
            return result; \
        break;

DEFINE_KIND_TRAVERSAL(Name, NAME_AST_MIXIN(MAKE_NAME_CASE))
DEFINE_KIND_TRAVERSAL(Spec, SPEC_AST_MIXIN(MAKE_SPEC_CASE))
DEFINE_KIND_TRAVERSAL(Attr, ATTR_AST_MIXIN(MAKE_ATTR_CASE))
DEFINE_KIND_TRAVERSAL(Decl, DECL_AST_MIXIN(MAKE_DECL_CASE))
DEFINE_KIND_TRAVERSAL(Expr, EXPR_AST_MIXIN(MAKE_EXPR_CASE))
DEFINE_KIND_TRAVERSAL(Stmt, STMT_AST_MIXIN(MAKE_STMT_CASE))

#undef MAKE_CASE
#undef DEFINE_KIND_TRAVERSAL

#define TRIVIAL_VISIT(AST_NODE) \
    template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult \
    AstVisitor<DerivedT>::traverse##AST_NODE(AST_NODE##Ast *ast) \
    { \
        EVAL_RESULT_0(recursivelyVisit##AST_NODE(ast)); \
        return Continue; \
    }

    /*--- Names traversal ---*/

TRIVIAL_VISIT(CompletionName)
TRIVIAL_VISIT(GenName)
TRIVIAL_VISIT(ErrorName)
TRIVIAL_VISIT(PuncName)
TRIVIAL_VISIT(SimpleName)
TRIVIAL_VISIT(SpecialName)

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseNestedName(NestedNameAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitNestedName(ast));
    EVAL_RESULT_LIST_N(traverseList<NameAst>(ast->names_.get(), &DerivedT::traverseName));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTemplateInstName(TemplateInstNameAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitTemplateInstName(ast));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

    /*--- Specifiers traversal ---*/

TRIVIAL_VISIT(VoidSpec)
TRIVIAL_VISIT(BuiltinSpec)
TRIVIAL_VISIT(PtrSpec)
TRIVIAL_VISIT(ChanSpec)
TRIVIAL_VISIT(InferredSpec)

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseNamedSpec(NamedSpecAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitNamedSpec(ast));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseFuncSpec(FuncSpecAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitFuncSpec(ast));
    EVAL_RESULT_N(traverseDecl(ast->param_.get()));
    EVAL_RESULT_N(traverseDecl(ast->result_.get()));
    EVAL_RESULT_N(traverseDecl(ast->templateParam()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTypeofSpec(TypeofSpecAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitTypeofSpec(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseRecordSpec(RecordSpecAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitRecordSpec(ast));
    EVAL_RESULT_N(traverseDecl(ast->templ_.get()));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(ast->bases_.get(), &DerivedT::traverseDecl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(ast->decls_.get(), &DerivedT::traverseDecl));
    EVAL_RESULT_N(traverseStmt(ast->proto_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseArraySpec(ArraySpecAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitArraySpec(ast));
    if (!ast->exprOrSpec_)
        return Continue;
    if (ast->exprOrSpec_->isExpr())
        EVAL_RESULT_N(traverseExpr(Expr_Cast(ast->exprOrSpec_.get())));
    else
        EVAL_RESULT_N(traverseSpec(Spec_Cast(ast->exprOrSpec_.get())));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDecoratedSpec(DecoratedSpecAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitDecoratedSpec(ast));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_LIST_N(traverseList<AttrAst>(ast->attrs_.get(), &DerivedT::traverseAttr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseOpaqueSpec(OpaqueSpecAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitOpaqueSpec(ast));
    EVAL_RESULT_N(traverseSpec(ast->baseSpec_.get()));
    return Continue;
}

    /*--- Attributes traversal ---*/

TRIVIAL_VISIT(StorageClassAttr)
TRIVIAL_VISIT(TypeQualAttr)
TRIVIAL_VISIT(LinkageAttr)
TRIVIAL_VISIT(ParamDirAttr)
TRIVIAL_VISIT(EvalStrategyAttr)
TRIVIAL_VISIT(VisibilityAttr)
TRIVIAL_VISIT(AnnotAttr)
TRIVIAL_VISIT(DeclAttr)
TRIVIAL_VISIT(AutoAttr)

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseCodegenAttr(CodegenAttrAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitCodegenAttr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

    /*--- Declarations traversal ---*/

TRIVIAL_VISIT(EmptyDecl)
TRIVIAL_VISIT(ErrorDecl)
TRIVIAL_VISIT(PatDecl)
TRIVIAL_VISIT(AsPatDecl)
TRIVIAL_VISIT(WildCardPatDecl)
TRIVIAL_VISIT(IrrefutPatDecl)
TRIVIAL_VISIT(PatBindDecl)

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseRecordDecl(RecordDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitRecordDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    EVAL_RESULT_N(traverseSpec(decl->spec_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBaseDecl(BaseDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitBaseDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBlockDecl(BlockDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitBlockDecl(decl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->decls_.get(), &DerivedT::traverseDecl));
    EVAL_RESULT_LIST_N(traverseList<AttrAst>(decl->attrs_.get(), &DerivedT::traverseAttr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseSelectiveDecl(SelectiveDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitSelectiveDecl(decl));
    EVAL_RESULT_N(traverseExpr(decl->expr_.get()));
    EVAL_RESULT_N(traverseDecl(decl->ifDecl_.get()));
    EVAL_RESULT_N(traverseDecl(decl->elseDecl_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseConstraintDecl(ConstraintDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitConstraintDecl(decl));
    EVAL_RESULT_N(traverseExpr(decl->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseVersionDecl(VersionDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitVersionDecl(decl));
    EVAL_RESULT_N(traverseExpr(decl->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDebugDecl(DebugDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitDebugDecl(decl));
    EVAL_RESULT_N(traverseExpr(decl->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseEnumDecl(EnumDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitEnumDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->decls_.get(), &DerivedT::traverseDecl));
    EVAL_RESULT_N(traverseSpec(decl->spec_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseEnumMemberDecl(EnumMemberDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitEnumMemberDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseFuncDecl(FuncDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitFuncDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    EVAL_RESULT_N(traverseSpec(decl->spec_.get()));
    EVAL_RESULT_N(traverseStmt(decl->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseForwardDecl(ForwardDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitForwardDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseImportGroupDecl(ImportGroupDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitImportGroupDecl(decl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->modules_.get(), &DerivedT::traverseDecl));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseImportDecl(ImportDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitImportDecl(decl));
    EVAL_RESULT_N(traverseExpr(decl->target_.get()));
    EVAL_RESULT_N(traverseName(decl->localName_.get()));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->selections_.get(), &DerivedT::traverseDecl));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseImportSelectionDecl(ImportSelectionDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitImportSelectionDecl(decl));
    EVAL_RESULT_N(traverseName(decl->actualName_.get()));
    EVAL_RESULT_N(traverseName(decl->alternateName_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseInvariantDecl(InvariantDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitInvariantDecl(decl));
    EVAL_RESULT_N(traverseStmt(decl->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseParamClauseDecl(ParamClauseDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitParamClauseDecl(decl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->decls_.get(), &DerivedT::traverseDecl));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseParamGroupDecl(ParamGroupDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitParamGroupDecl(decl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->decls_.get(), &DerivedT::traverseDecl));
    EVAL_RESULT_N(traverseSpec(decl->spec_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseParamDecl(ParamDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitParamDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    EVAL_RESULT_N(traverseExpr(decl->defaultArg()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseSectionDecl(SectionDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitSectionDecl(decl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->decls_.get(), &DerivedT::traverseDecl));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseVarGroupDecl(VarGroupDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitVarGroupDecl(decl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->decls_.get(), &DerivedT::traverseDecl));
    EVAL_RESULT_N(traverseSpec(decl->spec_.get()));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(decl->inits(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseVarDecl(VarDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitVarDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    EVAL_RESULT_N(traverseExpr(decl->init()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseStaticAssertDecl(StaticAssertDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitStaticAssertDecl(decl));
    EVAL_RESULT_N(traverseExpr(decl->expr_.get()));
    EVAL_RESULT_N(traverseExpr(decl->message_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTemplateDecl(TemplateDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitTemplateDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTemplateParamDecl(TemplateParamDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitTemplateParamDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    EVAL_RESULT_N(traverseSpec(decl->spec_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTemplateParamClauseDecl(TemplateParamClauseDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitTemplateParamClauseDecl(decl));
    EVAL_RESULT_LIST_N(traverseList<DeclAst>(decl->decls_.get(), &DerivedT::traverseDecl));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseAliasDecl(AliasDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitAliasDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    EVAL_RESULT_N(traverseSpec(decl->spec_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traversePackageDecl(PackageDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitPackageDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseModuleDecl(ModuleDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitModuleDecl(decl));
    EVAL_RESULT_N(traverseName(decl->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseUnitTestDecl(UnitTestDeclAst* decl)
{
    EVAL_RESULT_0(recursivelyVisitUnitTestDecl(decl));
    EVAL_RESULT_N(traverseStmt(decl->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseExportDecl(ExportDeclAst* decl)
{
    EVAL_RESULT_LIST_0(traverseList<DeclAst>(decl->selections_.get(), &DerivedT::traverseDecl));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseExportSelectionDecl(ExportSelectionDeclAst* decl)
{
    EVAL_RESULT_0(traverseName(decl->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseWrappedPatDecl(WrappedPatDeclAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitWrappedPatDecl(ast));
    EVAL_RESULT_N(traverseDecl(ast->decl_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTuplePatDecl(TuplePatDeclAst* decl)
{
    EVAL_RESULT_LIST_0(traverseList<DeclAst>(decl->pats_.get(), &DerivedT::traverseDecl));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseListPatDecl(ListPatDeclAst* decl)
{
    EVAL_RESULT_LIST_0(traverseList<DeclAst>(decl->pats_.get(), &DerivedT::traverseDecl));
    return Continue;
}

    /*--- Expressions traversal ---*/

TRIVIAL_VISIT(ArrayLengthExpr)
TRIVIAL_VISIT(BoolLitExpr)
TRIVIAL_VISIT(CharLitExpr)
TRIVIAL_VISIT(ErrorExpr)
TRIVIAL_VISIT(NullLitExpr)
TRIVIAL_VISIT(NumLitExpr)
TRIVIAL_VISIT(PriExpr)
TRIVIAL_VISIT(StrLitExpr)
TRIVIAL_VISIT(SuperExpr)
TRIVIAL_VISIT(ThisExpr)
TRIVIAL_VISIT(MixinExpr)

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseUnaryExpr(UnaryExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitUnaryExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBinExpr(BinExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitBinExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseArrayIndexExpr(ArrayIndexExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitArrayIndexExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->base_.get()));
    EVAL_RESULT_N(traverseExpr(ast->index_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseArraySliceExpr(ArraySliceExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitArraySliceExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->base_.get()));
    EVAL_RESULT_N(traverseExpr(ast->range_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseArrayInitExpr(ArrayInitExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitArrayInitExpr(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->inits_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTupleLitExpr(TupleLitExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitTupleLitExpr(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->inits_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseAddExpr(AddExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitAddExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseAddrOfExpr(AddrOfExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitAddrOfExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTerExpr(TerExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitTerExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->cond_.get()));
    EVAL_RESULT_N(traverseExpr(ast->yes_.get()));
    EVAL_RESULT_N(traverseExpr(ast->no_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseFuncLitExpr(FuncLitExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitFuncLitExpr(ast));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseRecordLitExpr(RecordLitExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitRecordLitExpr(ast));
    if (ast->exprOrSpec_->isExpr())
        EVAL_RESULT_N(traverseExpr(Expr_Cast(ast->exprOrSpec_.get())));
    else
        EVAL_RESULT_N(traverseSpec(Spec_Cast(ast->exprOrSpec_.get())));
    EVAL_RESULT_N(traverseExpr(ast->init_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseRecordInitExpr(RecordInitExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitRecordInitExpr(ast));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->inits_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseIdentExpr(IdentExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitIdentExpr(ast));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traversePtrDerefExpr(PtrDerefExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitPtrDerefExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDelExpr(DelExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitDelExpr(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traversePlusExpr(PlusExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitPlusExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseMinusExpr(MinusExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitMinusExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseLogicNotExpr(LogicNotExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitLogicNotExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseLogicAndExpr(LogicAndExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitLogicAndExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseLogicOrExpr(LogicOrExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitLogicOrExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseIncDecExpr(IncDecExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitIncDecExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBitCompExpr(BitCompExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitBitCompExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseChanExpr(ChanExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitChanExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseCommaExpr(CommaExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitCommaExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseAssignExpr(AssignExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitAssignExpr(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs1_.get(), &DerivedT::traverseExpr));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs2_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseEqExpr(EqExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitEqExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseRelExpr(RelExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitRelExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseShiftExpr(ShiftExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitShiftExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseConcatExpr(ConcatExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitConcatExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseIsExpr(IsExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitIsExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseInExpr(InExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitInExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBitXorExpr(BitXorExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitBitXorExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBitAndExpr(BitAndExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitBitAndExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBitOrExpr(BitOrExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitBitOrExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseSubExpr(SubExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitSubExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseMulExpr(MulExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitMulExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDivExpr(DivExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitDivExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseModExpr(ModExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitModExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traversePowerExpr(PowerExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitPowerExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr1_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr2_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTypeQueryExpr(TypeQueryExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitTypeQueryExpr(ast));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTypeAssertExpr(TypeAssertExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitTypeAssertExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->base_.get()));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTypeidExpr(TypeidExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitTypeidExpr(ast));
    if (ast->exprOrSpec_->isExpr())
        EVAL_RESULT_N(traverseExpr(Expr_Cast(ast->exprOrSpec_.get())));
    else
        EVAL_RESULT_N(traverseSpec(Spec_Cast(ast->exprOrSpec_.get())));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseMemberAccessExpr(MemberAccessExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitMemberAccessExpr(ast));
    if (ast->exprOrSpec_->isExpr())
        EVAL_RESULT_N(traverseExpr(Expr_Cast(ast->exprOrSpec_.get())));
    else
        EVAL_RESULT_N(traverseSpec(Spec_Cast(ast->exprOrSpec_.get())));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseWrappedExpr(WrappedExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitWrappedExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseAssertExpr(AssertExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitAssertExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    EVAL_RESULT_N(traverseExpr(ast->message_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseMakeExpr(MakeExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitMakeExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->base_.get()));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->args_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseNewExpr(NewExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitNewExpr(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->allocArgs_.get(), &DerivedT::traverseExpr));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->args_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseNestedNewExpr(NestedNewExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitNestedNewExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->base_.get()));
    EVAL_RESULT_N(traverseExpr(ast->nestedNew_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseCastExpr(CastExprAst *ast)
{
    EVAL_RESULT_0(recursivelyVisitCastExpr(ast));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDesignateExpr(DesignateExprAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitDesignateExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->id_.get()));
    EVAL_RESULT_N(traverseExpr(ast->value_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseCallExpr(CallExprAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitCallExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->base_.get()));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->args_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseSubrangeExpr(SubrangeExprAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitSubrangeExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->low_.get()));
    EVAL_RESULT_N(traverseExpr(ast->hi_.get()));
    EVAL_RESULT_N(traverseExpr(ast->max_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseUnpackExpr(UnpackExprAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitUnpackExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traversePrintExpr(PrintExprAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitPrintExpr(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseYieldExpr(YieldExprAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitYieldExpr(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseListCompreExpr(ListCompreExprAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitListCompreExpr(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    // TODO
    return Continue;
}

    /*--- Statements traversal ---*/

TRIVIAL_VISIT(EmptyStmt)
TRIVIAL_VISIT(ErrorStmt)
TRIVIAL_VISIT(FallthroughStmt)

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBlockStmt(BlockStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitBlockStmt(ast));
    EVAL_RESULT_LIST_N(traverseList<StmtAst>(ast->stmts_.get(), &DerivedT::traverseStmt));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseExprStmt(ExprStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitExprStmt(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDeclStmt(DeclStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitDeclStmt(ast));
    EVAL_RESULT_N(traverseDecl(ast->decl_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseLabeledStmt(LabeledStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitLabeledStmt(ast));
    EVAL_RESULT_N(traverseName(ast->label_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseIfStmt(IfStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitIfStmt(ast));
    EVAL_RESULT_N(traverseStmt(ast->preamble_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    EVAL_RESULT_N(traverseStmt(ast->then_.get()));
    EVAL_RESULT_N(traverseStmt(ast->notThen_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseWhileStmt(WhileStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitWhileStmt(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDoWhileStmt(DoWhileStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitDoWhileStmt(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseForStmt(ForStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitForStmt(ast));
    EVAL_RESULT_N(traverseStmt(ast->preamble_.get()));
    EVAL_RESULT_N(traverseExpr(ast->cond_.get()));
    EVAL_RESULT_N(traverseExpr(ast->post_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseForeachStmt(ForeachStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitForeachStmt(ast));
    EVAL_RESULT_N(traverseDecl(ast->decl_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseContinueStmt(ContinueStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitContinueStmt(ast));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseBreakStmt(BreakStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitBreakStmt(ast));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseReturnStmt(ReturnStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitReturnStmt(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs_.get(), &DerivedT::traverseExpr));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseGotoStmt(GotoStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitGotoStmt(ast));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseThrowStmt(ThrowStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitThrowStmt(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseSwitchStmt(SwitchStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitSwitchStmt(ast));
    EVAL_RESULT_N(traverseStmt(ast->preamble_.get()));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTypeSwitchStmt(TypeSwitchStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitTypeSwitchStmt(ast));
    EVAL_RESULT_N(traverseSpec(ast->spec_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseCaseClauseStmt(CaseClauseStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitCaseClauseStmt(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs_.get(), &DerivedT::traverseExpr));
    EVAL_RESULT_LIST_N(traverseList<StmtAst>(ast->stmts_.get(), &DerivedT::traverseStmt));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDefaultClauseStmt(DefaultClauseStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitDefaultClauseStmt(ast));
    EVAL_RESULT_LIST_N(traverseList<StmtAst>(ast->stmts_.get(), &DerivedT::traverseStmt));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseSyncedStmt(SyncedStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitSyncedStmt(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseWithStmt(WithStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitWithStmt(ast));
    EVAL_RESULT_LIST_N(traverseList<ExprAst>(ast->exprs_.get(), &DerivedT::traverseExpr));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseTryStmt(TryStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitTryStmt(ast));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    EVAL_RESULT_LIST_N(traverseList<StmtAst>(ast->catchs_.get(), &DerivedT::traverseStmt));
    EVAL_RESULT_N(traverseStmt(ast->final_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseCatchClauseStmt(CatchClauseStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitCatchClauseStmt(ast));
    EVAL_RESULT_N(traverseDecl(ast->decl_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseFinallyClauseStmt(FinallyClauseStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitFinallyClauseStmt(ast));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseDeferredStmt(DeferredStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitDeferredStmt(ast));
    EVAL_RESULT_N(traverseName(ast->name_.get()));
    EVAL_RESULT_N(traverseStmt(ast->stmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseSelectiveStmt(SelectiveStmtAst* Stmt)
{
    EVAL_RESULT_0(recursivelyVisitSelectiveStmt(Stmt));
    EVAL_RESULT_N(traverseExpr(Stmt->expr_.get()));
    EVAL_RESULT_N(traverseStmt(Stmt->ifStmt_.get()));
    EVAL_RESULT_N(traverseStmt(Stmt->elseStmt_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseAsyncStmt(AsyncStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitAsyncStmt(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseYieldStmt(YieldStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitYieldStmt(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

template <class DerivedT> typename AstVisitor<DerivedT>::VisitResult
AstVisitor<DerivedT>::traverseEvalStmt(EvalStmtAst* ast)
{
    EVAL_RESULT_0(recursivelyVisitEvalStmt(ast));
    EVAL_RESULT_N(traverseExpr(ast->expr_.get()));
    return Continue;
}

// TODO: Later...
TRIVIAL_VISIT(InStmt)
TRIVIAL_VISIT(OutStmt)
TRIVIAL_VISIT(BodyStmt)
TRIVIAL_VISIT(ContractStmt)

#undef TRIVIAL_VISIT
#undef DECL_RESULT
#undef EVAL_RESULT_0
#undef EVAL_RESULT_N
#undef EVAL_RESULT_LIST_0
#undef EVAL_RESULT_LIST_N
#undef EVAL_RESULT_CORE
#undef EVAL_RESULT_LIST_CORE

} // namespace uaiso

#define VIS_CALL(INVOCATION) \
    do { \
        VisitResult result = INVOCATION; \
        if (result == Abort) \
            return Abort; \
        if (result == Skip) \
            return Continue; \
    } while (0)

#endif
