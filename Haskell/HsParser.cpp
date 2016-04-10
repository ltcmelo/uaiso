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

#include "Haskell/HsParser.h"
#include "Haskell/HsLexer.h"
#include "Ast/Ast.h"
#include "Parsing/ParsingContext.h"

#define TRACE_NAME "HsParser"

using namespace uaiso;

HsParser::HsParser()
{}

bool HsParser::parse(Lexer* lexer, ParsingContext* context)
{
    UAISO_ASSERT(lexer, return false);
    UAISO_ASSERT(context && context->fileName(), return false);

    lexer_ = lexer;
    context_ = context;

    consumeToken();

    // Temp: Under development.
    if (parseExpr())
        return true;

    return false;
}

Parser::Expr HsParser::parseExpr()
{
    return parseAExpr();
}

/*
 * Rules: aexp
 */
Parser::Expr HsParser::parseAExpr()
{
    switch (ahead_) {
    case TK_INT_LIT:
        consumeToken();
        return Expr(newAst<NumLitExprAst>()->setLitLoc(lastLoc_)
                    ->setVariety(NumLitVariety::IntFormat));

    case TK_FLOAT_LIT:
        consumeToken();
        return Expr(newAst<NumLitExprAst>()->setLitLoc(lastLoc_)
                    ->setVariety(NumLitVariety::FloatFormat));

    case TK_TRUE_VALUE:
    case TK_FALSE_VALUE:
        consumeToken();
        return Expr(newAst<BoolLitExprAst>()->setLitLoc(lastLoc_));

    default:
        failMatch(true);
        return Expr();
    }
}
