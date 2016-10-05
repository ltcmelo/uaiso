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
#include "Parsing/ParsingContext.h"
#include "Common/Assert.h"
#include "Common/Trace__.h"

#define TRACE_NAME "Parser"

using namespace uaiso;

namespace uaiso
{
extern std::unordered_map<std::uint16_t, const char*> tokenName;
}

void Parser::setLexer(Lexer *lexer)
{
    lexer_ = lexer;
}

void Parser::setContext(ParsingContext *context)
{
    context_ = context;
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
    return match([tk] (const Token ahead) { return tk == ahead; });
}

bool Parser::match(std::function<bool (const Token)> isExpected)
{
    if (!isExpected(ahead_)) {
        fail();
        return false;
    }
    consumeToken();
    return true;
}

void Parser::matchOrSkipTo(Token tk, const char *rule)
{
    if (!match(tk)) {
        DEBUG_TRACE("%s, skip to %s\n", rule, tokenName[tk]);
        skipTo(tk);
    }
}

void Parser::fail()
{
    if (ahead_ != TK_EOP)
        consumeToken(); // Must move on nevertheless.

    DEBUG_TRACE("error at %d:%d unexpected token (%s)\n",
                prevLoc_.lastLine_, prevLoc_.lastCol_, prevLoc_.fileName_.c_str());
    context_->trackReport(Diagnostic::UnexpectedToken, prevLoc_);
}
