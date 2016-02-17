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

#include "Go/GoAstLocator.h"
#include "Ast/Ast.h"

using namespace uaiso;

const SourceLoc& GoAstLocator::loc(ArraySpecAst* ast) const
{
    return ast->lDelimLoc_;
}

const SourceLoc& GoAstLocator::lastLoc(ArraySpecAst* ast) const
{
    return lastLoc(ast->baseSpec_.get());
}

const SourceLoc& GoAstLocator::loc(FuncSpecAst* ast) const
{
    return ast->keyLoc_;
}

const SourceLoc& GoAstLocator::lastLoc(VersionDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    return lastLoc(ast->expr_.get());
}

const SourceLoc& GoAstLocator::lastLoc(EnumDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    return ast->rDelimLoc_;
}

const SourceLoc& GoAstLocator::lastLoc(PackageDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    return lastLoc(ast->name_.get());
}

const SourceLoc& GoAstLocator::lastLoc(SectionDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    return ast->rDelimLoc_;
}

const SourceLoc& GoAstLocator::lastLoc(RecordDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    return lastLoc(ast->spec_.get());
}

const SourceLoc& GoAstLocator::lastLoc(VarGroupDeclAst* ast) const
{
    if (!ast->terminLoc_.isEmpty())
        return ast->terminLoc_;
    if (ast->hasInits())
        return lastLoc(ast->inits()->back());
    if (ast->spec_)
        return lastLoc(ast->spec_.get());
    return lastLoc(ast->decls_->back());
}

const SourceLoc& GoAstLocator::loc(CastExprAst* ast) const
{
    return loc(ast->spec_.get());
}

const SourceLoc& GoAstLocator::lastLoc(CastExprAst* ast) const
{
    return ast->rDelimLoc_;
}
