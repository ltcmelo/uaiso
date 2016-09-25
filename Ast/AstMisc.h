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

#ifndef UAISO_ASTMISC_H__
#define UAISO_ASTMISC_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"
#include "Common/Assert.h"
#include "Parsing/Lang.h"

namespace uaiso {

class Program;

/*!
 * \brief The ProgramAst class
 */
class UAISO_API ProgramAst final : public Ast
{
public:
    using Self = ProgramAst;

    ProgramAst()
        : Ast(Kind::Program)
        , program_(nullptr)
    {}

    NAMED_AST_PARAM(Module, module, DeclAst)
    NAMED_AST_PARAM(Package, package, DeclAst)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_AST_LIST_PARAM(Stmt, stmts, StmtAst)

    std::unique_ptr<DeclAst> module_;
    std::unique_ptr<DeclAst> package_;
    std::unique_ptr<DeclAstList> decls_;
    std::unique_ptr<StmtAstList> stmts_;

    Program* program_;
};

/*!
 * \brief The GeneratorAst class
 *
 * A generator in a list comprehension.
 */
class UAISO_API GeneratorAst final : public Ast
{
public:
    using Self = GeneratorAst;

    static std::unique_ptr<Self> create()
    {
        return std::unique_ptr<Self>(newAst<Self>());
    }

    GeneratorAst()
        : Ast(Kind::Generator)
    {}

    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_LIST_PARAM(Pattern, patterns, ExprAst)
    NAMED_AST_PARAM(Range, range, ExprAst)
    NAMED_AST_LIST_PARAM(Filter, filters, ExprAst)

    SourceLoc oprLoc_;
    std::unique_ptr<ExprAstList> patterns_;
    std::unique_ptr<ExprAst> range_;
    std::unique_ptr<ExprAstList> filters_;
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

    NAMED_AST_PARAM(Arg, arg, Ast)

    void setVariety(TemplateArgVariety v)
    {
        bit_.variety = static_cast<char>(v);
    }

    TemplateArgVariety variety() const { return TemplateArgVariety(bit_.variety); }

    std::unique_ptr<Ast> arg_;
};

    //--- Utility ---//

template <class AstVisitorT>
typename AstVisitorT::VisitResult traverseProgram(const ProgramAst* progAst,
                                                  AstVisitorT* visitor,
                                                  const Lang* lang)
{
    typename AstVisitorT::VisitResult result = AstVisitorT::Continue;
    switch (lang->structure()) {
    case Lang::DeclBased:
        for (auto decl : *progAst->decls()) {
            result = visitor->traverseDecl(decl);
            if (result != AstVisitorT::Continue)
                return result;
        }
        break;
    case Lang::StmtBased:
        for (auto stmt : *progAst->stmts()) {
            result = visitor->traverseStmt(stmt);
            if (result != AstVisitorT::Continue)
                return result;
        }
        break;
    case Lang::ExprBased:
        break;
    }
    return result;
}

template <class AstVisitorT>
typename AstVisitorT::VisitResult traverseProgram(const ProgramAst* progAst,
                                                  AstVisitorT* visitor,
                                                  const std::unique_ptr<const Lang>& lang)
{
    return traverseProgram(progAst, visitor, lang.get());
}

SpecAst* undecorateSpec(SpecAst* spec);

} // namespace uaiso

#define Program_Cast(AST) static_cast<uaiso::ProgramAst*>(AST)

#endif
