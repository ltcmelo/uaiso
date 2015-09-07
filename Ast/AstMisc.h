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

#ifndef UAISO_ASTMISC_H__
#define UAISO_ASTMISC_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"

namespace uaiso {

class Program;

class UAISO_API ProgramAst final : public Ast
{
public:
    using Self = ProgramAst;

    ProgramAst()
        : Ast(Kind::Program)
        , program_(nullptr)
    {}

    NAMED_AST_PARAM(setModule, module, DeclAst)
    NAMED_AST_PARAM(setPackage, package, DeclAst)
    NAMED_AST_LIST_PARAM(setDecls, decls, DeclAst)

    std::unique_ptr<DeclAst> module_;
    std::unique_ptr<DeclAst> package_;
    std::unique_ptr<DeclAstList> decls_;

    Program* program_;
};

/*!
 * \brief The TemplateArgVariety enum
 */
enum class TemplateArgVariety : char
{
    Unknown,
    NameToLookup, //!< Either the name of a type or an identifier expr.
    Type,
    Expr,
    Template
};

/*!
 * \brief The TemplateArgAst class
 *
 * A template argument, which can be a type, expression, or a template.
 *
 * \note This is not a "first-class" AST node and should always be used
 * within an actual AST note that knows how to interpret it.
 */
class UAISO_API TemplateArgAst final : public Ast
{
public:
    using Self = TemplateArgAst;

    TemplateArgAst()
    {
        bit_.variety = static_cast<char>(TemplateArgVariety::Unknown);
    }

    NAMED_AST_PARAM(setArg, arg, Ast)

    void setVariety(TemplateArgVariety v)
    {
        bit_.variety = static_cast<char>(v);
    }

    TemplateArgVariety variety() const { return TemplateArgVariety(bit_.variety); }

    std::unique_ptr<Ast> arg_;
};

} // namespace uaiso

#define Program_Cast(AST) static_cast<uaiso::ProgramAst*>(AST)

#endif
