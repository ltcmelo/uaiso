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

#include "Go/GoFlexBison__.h"
#include "Parsing/ParsingContext.h"
#include <algorithm>

namespace uaiso {

namespace detail {

DeclAstList* adjustParamGroupDecl(DeclAstList* paramDeclList,
                                  DeclAstList* paramGroupDeclList)
{
    // TODO: Delim will be lost in this conversion.
    for (auto decl : *paramDeclList) {
        auto name = static_cast<ParamDeclAst*>(decl)->name_.release();
        auto spec = (new NamedSpecAst)->setName(name);
        auto paramGroupDecl = (new ParamGroupDeclAst)->setSpec(spec);
        if (paramGroupDeclList)
            paramGroupDeclList = paramGroupDeclList->handleSR(paramGroupDecl);
        else
            paramGroupDeclList = DeclAstList::createSR(paramGroupDecl);
    }
    return paramGroupDeclList;
}

VarGroupDeclAst* turnExprsIntoVarGroupDecl(ExprAstList* exprs)
{
    auto group = newAst<VarGroupDeclAst>()->setSpec(newAst<InferredSpecAst>());
    for (auto expr : *exprs) {
        if (expr->kind() != Ast::Kind::IdentExpr) {
            // Don't care if there's a decl nested into deeper levels,
            // would be wrong anyway. Let it be deleted.
            continue;
        }
        auto name = IdentExpr_Cast(expr)->name_.release();
        auto var = VarDeclAst::create();
        var->setName(name);
        group->decls_ ? group->decls_->append(std::move(var)) :
                        (void)(group->decls_ = DeclAstList::create(std::move(var)));
    }
    delete exprs;
    return group;
}

void constifyVarGroupDecl(DeclAst* decl, const SourceLoc& loc)
{
    auto group = static_cast<VarGroupDeclAst*>(decl);
    auto qual = (new TypeQualAttrAst)->setKeyLoc(loc);
    auto spec = (new DecoratedSpecAst)->addAttr(qual)->setSpec(group->spec_.release());
    group->spec_.reset(spec);
    group->setAllocScheme(AllocScheme::CompileTime);
}

void splitBaseDeclsAndFields(RecordSpecAst* spec, DeclAstList* decls)
{
    auto declsP = std::unique_ptr<DeclAstList>(decls);
    while (declsP) {
        auto p = std::move(declsP->detachHead());
        spec->bases_ = std::unique_ptr<DeclAstList>();
        if (p.first->kind() == Ast::Kind::BaseDecl) {
            spec->bases_ ? spec->bases_->append(std::move(p.first)) :
                           (void)(spec->bases_ =  DeclAstList::create(std::move(p.first)));
        } else {
            spec->decls_ ? spec->decls_->append(std::move(p.first)) :
                           (void)(spec->decls_ = DeclAstList::create(std::move(p.first)));
        }
        declsP = std::move(p.second);
    }
}

SpecAst* extractSpecFromExpr(ExprAst* nameExpr) {
    NameAst* name = nullptr;
    if (nameExpr->kind() == Ast::Kind::IdentExpr) {
        name = IdentExpr_Cast(nameExpr)->name_.release();
    } else if (nameExpr->kind() == Ast::Kind::MemberAccessExpr) {
        auto memberExpr = MemberAccessExpr_Cast(nameExpr);
        if (memberExpr->exprOrSpec_->kind() == Ast::Kind::IdentExpr) {
            auto names = NameAstList::create(std::move(IdentExpr_Cast(memberExpr->exprOrSpec_.get())
                                             ->name_));
            names->append(std::move(memberExpr->name_));
            name = newAst<NestedNameAst>()->setNamesSR(names.release());
        }
    }
    SpecAst* spec = nullptr;
    if (name)
        spec = newAst<NamedSpecAst>()->setName(name);
    delete nameExpr;

    return spec;
}

    //--- Action rules ---//

void actionProgram(ParsingContext* context, const SourceLoc& locA, NameAst* name,
                   const SourceLoc& locB)
{
    auto package = newAst<PackageDeclAst>();
    package->setKeyLoc(locA)->setName(name)->setTerminLoc(locB);
    auto program = newAst<ProgramAst>()->setPackage(package);
    context->takeAst(std::unique_ptr<Ast>(program));
    context->notifyProgramMatched();
}

void actionProgram(ParsingContext* context, const SourceLoc& locA, NameAst* name,
                   const SourceLoc& locB, DeclAstList* decls)
{
    auto package = newAst<PackageDeclAst>();
    package->setKeyLoc(locA)->setName(name)->setTerminLoc(locB);
    auto program = newAst<ProgramAst>()->setPackage(package)->setDeclsSR(decls);
    context->takeAst(std::unique_ptr<Ast>(program));
    context->notifyProgramMatched();
}

} // namespace detail

} // namespace uaiso
