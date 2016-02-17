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

#include "D/DAstLocator.h"
#include "Ast/Ast.h"

using namespace uaiso;

const SourceLoc& DAstLocator::loc(FuncSpecAst* ast) const
{
    return loc(ast->param_.get());
}

const SourceLoc& DAstLocator::lastLoc(FuncSpecAst* ast) const
{
    return lastLoc(ast->param_.get());
}

const SourceLoc& DAstLocator::lastLoc(EnumDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    return ast->rDelimLoc_;
}

const SourceLoc& DAstLocator::lastLoc(RecordDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    return lastLoc(ast->spec_.get());
}

