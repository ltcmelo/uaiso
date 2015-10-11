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

#include "D/DFlexBison__.h"
#include "Ast/Ast.h"
#include <iostream>

using namespace uaiso;

detail::FullParam::~FullParam()
{}

    //--- Action rules ---//

void detail::actionProgram(ParsingContext* context, DeclAstList* decls)
{
    auto program = make<ProgramAst>()->setDeclsSR(decls);
    context->takeAst(std::unique_ptr<Ast>(program));
    context->notifyProgramMatched();
}

void detail::actionProgram(ParsingContext* context, const SourceLoc& locA,
                           NameAstList* names, const SourceLoc& locB, DeclAstList* decls)
{
    auto name = make<NestedNameAst>();
    name->setNamesSR(names);
    auto module = make<ModuleDeclAst>();
    module->setKeyLoc(locA)->setName(name)->setTerminLoc(locB);
    auto program = make<ProgramAst>();
    program->setModule(module)->setDeclsSR(decls);
    context->takeAst(std::unique_ptr<Ast>(program));
    context->notifyProgramMatched();
}

DeclAst* detail::actionVarGroupDecl(SpecAst* spec, DeclAstList* decls, const SourceLoc& locA)
{
    auto group = make<VarGroupDeclAst>();
    group->setSpec(spec)->setDeclsSR(decls)->setTerminLoc(locA);
    return group;
}

DeclAst* detail::actionVarGroupDecl(AttrAstList* attrs, DeclAstList* decls, const SourceLoc& locA)
{
    auto spec = make<DecoratedSpecAst>();
    auto inferred = make<InferredSpecAst>()->setKeyLoc(locA);
    spec->setSpec(inferred)->setAttrsSR(attrs);
    auto group = make<VarGroupDeclAst>();
    group->setSpec(spec)->setDeclsSR(decls)->setTerminLoc(locA);
    return group;
}
