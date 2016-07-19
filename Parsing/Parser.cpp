/******************************************************************************
 * Coright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)
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
 * You should have received a co of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *****************************************************************************/

/*--------------------------*/
/*--- The UaiSo! Project ---*/
/*--------------------------*/

#include "Parsing/Parser.h"
#include "Parsing/Lexer.h"
#include "Parsing/ParsingContext.h"
#include "Common/Trace__.h"

#define TRACE_NAME "Parser"

using namespace uaiso;

void Parser::consumeToken()
{
    if (ahead_ == TK_EOP)
        return;

    // Track previous token location.
    lastLoc_ = lexer_->tokenLoc();
    lastLoc_.fileName_ = context_->fileName();
    ahead_ = lexer_->lex();
}

bool Parser::maybeConsume(Token tk)
{
    if (ahead_ == tk) {
        consumeToken();
        return true;
    }
    return false;
}

void Parser::skipTo(Token tk)
{
    while (!(ahead_ == tk || ahead_ == TK_EOP))
        consumeToken();
}

bool Parser::match(Token tk)
{
    auto actual = ahead_;
    consumeToken(); // Move on, regardless of a match.
    if (actual != tk) {
        failMatch(false);
        return false;
    }
    return true;
}

void Parser::failMatch(bool consume)
{
    // Location in the report is always from the previously consumed token.
    if (consume)
        consumeToken();

    DEBUG_TRACE("error at %d:%d unexpected token (%s)\n",
                lastLoc_.lastLine_, lastLoc_.lastCol_, lastLoc_.fileName_.c_str());
    context_->trackReport(Diagnostic::UnexpectedToken, lastLoc_);
}
