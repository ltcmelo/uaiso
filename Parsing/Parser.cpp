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
#include "Common/Assert.h"
#include "Common/Trace__.h"

#define TRACE_NAME "Parser"

using namespace uaiso;

void Parser::consumeToken()
{
    // Track previous token location.
    lastLoc_ = lexer_->tokenLoc();
    lastLoc_.fileName_ = context_->fileName();

    ahead_ = lexer_->lex();
}

void Parser::consumeTokenCheckEOP()
{
    if (ahead_ != TK_EOP)
        consumeToken();
}

bool Parser::maybeConsume(Token tk)
{
    UAISO_ASSERT(tk != TK_EOP, return false);

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
    UAISO_ASSERT(tk != TK_EOP, return false);

    if (ahead_ != tk) {
        failMatch();
        return false;
    }
    consumeToken();
    return true;
}

void Parser::failMatch()
{
    consumeTokenCheckEOP(); // Move on...

    // Reported location is from last token.
    DEBUG_TRACE("error at %d:%d unexpected token (%s)\n",
                lastLoc_.lastLine_, lastLoc_.lastCol_, lastLoc_.fileName_.c_str());
    context_->trackReport(Diagnostic::UnexpectedToken, lastLoc_);
}
