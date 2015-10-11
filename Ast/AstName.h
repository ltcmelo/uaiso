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

#ifndef UAISO_ASTNAME_H__
#define UAISO_ASTNAME_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"
#include <string>

namespace uaiso {

class UAISO_API NameAst : public Ast
{
public:
    using Ast::Ast;
};

class UAISO_API SimpleNameAst final : public NameAst
{
public:
    using Self = SimpleNameAst;

    SimpleNameAst()
        : NameAst(Kind::SimpleName)
    {}

    NAMED_LOC_PARAM(Name, name)

    SourceLoc nameLoc_;
};

class UAISO_API NestedNameAst final : public NameAst
{
public:
    using Self = NestedNameAst;

    NestedNameAst()
        : NameAst(Kind::NestedName)
    {}

    NAMED_AST_LIST_PARAM(Name, names, NameAst)

    std::unique_ptr<NameAstList> names_;
};

class UAISO_API TemplateInstNameAst final : public NameAst
{
public:
    using Self = TemplateInstNameAst;

    TemplateInstNameAst()
        : NameAst(Kind::TemplateInstName)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Mark, mark)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Arg, args, TemplateArgAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<NameAst> name_;
    SourceLoc markLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<TemplateArgAstList> args_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The GenNameAst class
 *
 * A generated name, one that is not in the source.
 */
class UAISO_API GenNameAst final : public NameAst
{
public:
    using Self = GenNameAst;

    GenNameAst()
        : NameAst(Kind::GenName)
    {}

    GenNameAst(const std::string& name)
        : NameAst(Kind::GenName)
        , str_(name)
    {}

    NAMED_LOC_PARAM(Gen, gen)

    SourceLoc genLoc_;
    std::string str_;
};

class UAISO_API CompletionNameAst final : public NameAst
{
public:
    using Self = CompletionNameAst;

    CompletionNameAst()
        : NameAst(Kind::CompletionName)
    {}

    NAMED_LOC_PARAM(Name, name)

    SourceLoc nameLoc_;
};

} // namespace uaiso

#endif
