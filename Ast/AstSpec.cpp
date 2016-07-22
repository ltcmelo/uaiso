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

#include "Ast/Ast.h"

using namespace uaiso;

FuncSpecAst* FuncSpecAst::setResult(SpecAst* spec)
{
    auto clause = newAst<ParamClauseDeclAst>();
    auto group = newAst<ParamGroupDeclAst>()->setSpec(spec);
    clause->decls_ = DeclAstList::create(std::unique_ptr<ParamGroupDeclAst>(group));
    result_.reset(clause);
    return this;
}

ArraySpecAst* ArraySpecAst::setSpec(SpecAst* spec)
{
    exprOrSpec_.reset(spec);
    return this;
}

ArraySpecAst* ArraySpecAst::setExpr(ExprAst* expr)
{
    exprOrSpec_.reset(expr);
    return this;
}
