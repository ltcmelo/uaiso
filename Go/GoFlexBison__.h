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

/*--------------------------*/
/*--- The UaiSo! Project ---*/
/*--------------------------*/

///////////////////////////////////////////////////////
///                                                 ///
///         This is an INTERNAL header              ///
///                                                 ///
///   Do not include this header from outside the   ///
///   the uaiso lib or from any public API header   ///
///                                                 ///
///////////////////////////////////////////////////////

#ifndef UAISO_GOFLEXBISON__
#define UAISO_GOFLEXBISON__

#include "Ast/Ast.h"
#include "Ast/AstVisitor.h"
#include "Parsing/FlexBison__.h"
#include <iostream>

namespace uaiso {

namespace detail {

/*!
 * \brief The PickupShortVarDecl class
 *
 * If the given expr is actually a short var decl, pick it up and convert
 * it to a proper decl. Otherwise, leave the original expr alone.
 */
class ExprOrShortVarDecl final : public AstVisitor<ExprOrShortVarDecl>
{
public:
    StmtAst* inspect(ExprAst* ast,
                     const SourceLoc& loc = kEmptyLoc)
    {
        traverseExpr(ast);

        if (decl_) {
            delete ast;
            decl_->setTerminLoc(loc);
            return makeAstRaw<DeclStmtAst>()->setDecl(decl_.release());
        }

        return makeAstRaw<ExprStmtAst>()->addExpr(ast)->setTerminLoc(loc);
    }

private:
    friend class AstVisitor<ExprOrShortVarDecl>;

    VisitResult visitAssignExpr(AssignExprAst* ast)
    {
        if (ast->variety() == AssignVariety::Unknow) {
            // The expr is a short var decl if its variety is `Unknown` (and
            // it must be an identifier).
            for (auto expr1 : *ast->exprs1_.get()) {
                if (expr1->kind() == Ast::Kind::IdentExpr) {
                    auto name = IdentExpr_Cast(expr1)->name_.release();
                    auto var = makeAstRaw<VarDeclAst>()->setName(name);
                    if (decl_) {
                        decl_->decls_->pushBack(var);
                    } else {
                        VarGroupDeclAst* group = nullptr;
                        if (ast->exprs2_) {
                            auto groupInit = makeAstRaw<VarGroupDeclAst__<VarGroupInits__>>();
                            groupInit->setInits__(ast->exprs2_.release());
                            group = groupInit;
                        } else {
                            group = makeAstRaw<VarGroupDeclAst>();
                        }
                        group->setSpec(makeAstRaw<InferredSpecAst>());
                        group->decls_.reset(DeclAstList::create(var));
                        decl_.reset(group);
                    }
                }
            }
        }

        return Continue;
    }

    std::unique_ptr<VarGroupDeclAst> decl_;
};

DeclAstList* adjustParamGroupDecl(DeclAstList* paramDeclList,
                                  DeclAstList* paramGroupDeclList);

VarGroupDeclAst* turnExprsIntoVarGroupDecl(ExprAstList* exprs);

void constifyVarGroupDecl(DeclAst* decl, const SourceLoc& loc);

void splitBaseDeclsAndFields(RecordSpecAst* spec, DeclAstList* decls);

SpecAst* extractSpecFromExpr(ExprAst* nameExpr);

    //--- Action rules ---//

void actionProgram(ParsingContext* context, const SourceLoc& locA, NameAst* name,
                   const SourceLoc& locB);
void actionProgram(ParsingContext* context, const SourceLoc& locA, NameAst* name,
                   const SourceLoc& locB, DeclAstList* decls);

} // namespace detail

} // namespace uaiso

#endif
