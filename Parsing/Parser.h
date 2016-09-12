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

#ifndef UAISO_PARSER_H__
#define UAISO_PARSER_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Common/Config.h"
#include "Parsing/SourceLoc.h"
#include "Parsing/Token.h"

namespace uaiso {

class Lexer;
class ParsingContext;

class UAISO_API Parser
{
public:
    virtual ~Parser() = default;

    virtual bool parse(Lexer* lexer, ParsingContext* context) = 0;

protected:
    Parser() = default;

    using Decl = std::unique_ptr<DeclAst>;
    using DeclList = std::unique_ptr<DeclAstList>;
    using Expr = std::unique_ptr<ExprAst>;
    using ExprList = std::unique_ptr<ExprAstList>;
    using Name = std::unique_ptr<NameAst>;
    using NameList = std::unique_ptr<NameAstList>;
    using Stmt = std::unique_ptr<StmtAst>;
    using StmtList = std::unique_ptr<StmtAstList>;

    virtual void prepare(Lexer* lexer, ParsingContext* context);

    /*!
     * \brief consumeToken
     */
    virtual void consumeToken() = 0;

    /*!
     * \brief maybeConsume
     * \param tk
     * \return
     */
    bool maybeConsume(Token tk);

    /*!
     * \brief skipTo
     * \param tk
     */
    void skipTo(Token tk);

    /*!
     * \brief match
     * \param tk
     * \return
     */
    bool match(Token tk);

    /*!
     * \brief failMatch
     * \param consume
     */
    void fail();

    /*!
     * \brief loc
     * \return
     *
     * Current source location of the parser.
     */
    virtual SourceLoc currentLoc() const = 0;

    Lexer* lexer_ { nullptr };
    ParsingContext* context_ { nullptr };
    Token ahead_ { TK_INVALID };
    SourceLoc prevLoc_;
};

} // namespace uaiso

#endif
