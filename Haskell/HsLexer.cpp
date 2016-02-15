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

#include "Haskell/HsLexer.h"
#include "Haskell/HsKeywords.h"
#include "Haskell/HsLang.h"
#include "Common/Assert.h"
#include "Common/Trace__.h"
#include "Parsing/ParsingContext.h"
#include "parsing/Lexeme.h"
#include <cctype>

#define TRACE_NAME "HsLexer"

using namespace uaiso;

namespace {

const HsLang hsLang;

} // anonymous

HsLexer::HsLexer()
{}

HsLexer::~HsLexer()
{}

Token HsLexer::lex()
{
    Token tk = TK_INVALID;
    updatePos();

LexNextToken:
    mark_ = curr_; // Mark the start of the upcoming token.

    char ch = peekChar();
    switch (ch) {
    case 0:
        return TK_EOP;

    case '\n':
        consumeChar();
        handleNewLine();
        updatePos();
        goto LexNextToken;

    case '\f':
    case '\t':
    case ' ':
        do {
            ch = consumeCharPeekNext();
            ++col_;
            ++mark_;
        } while (ch == '\f' || ch == '\t' || ch == ' ');
        goto LexNextToken;

    case '(':
    case ')':
    case ',':
    case ';':
    case '[':
    case ']':
    case '`':
    case '{':
    case '}':
        tk = lexSpecial(ch);
        break;

    case '!':
    case '#':
    case '$':
    case '%':
    case '&':
    case '*':
    case '+':
    case '-':
    case '.':
    case '/':
    case ':':
    case '<':
    case '=':
    case '>':
    case '?':
    case '@':
    case '\\':
    case '^':
    case '|':
    case '~':
        tk = lexAscSymbol(ch);
        break;

    default:
        if (hsLang.isIdentFirstChar(ch)) {
            tk = lexIdentOrKeyword(ch, &hsLang);
            break;
        }

        if (std::isdigit(ch)) {
            tk = lexNumLit(ch, &hsLang);
            context_->trackLexeme<NumLit>(mark_, curr_ - mark_, LineCol(line_, col_));
            break;
        }

        // Don't know what this is.
        consumeChar();
        PRINT_TRACE("Unknown char %c at %d,%d\n", ch, 0, 0);
        break;
    }

    LineCol lineCol(line_, col_);
    context_->trackToken(tk, lineCol);
    context_->trackPhrase(tk, lineCol, curr_ - mark_);

    return tk;
}

namespace {

// A few tokens indexed by their corresponding ASCII value.
const int oprtrTable[] =
{
    // Category `special`
    TK_LPAREN,                                             // 0
    TK_RPAREN,
    TK_COMMA,
    TK_SEMICOLON,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_BACKTICK,
    TK_LBRACE,
    TK_RBRACE,                                             // 8

    // Category `ascSymbol`
    TK_EXCLAM,                                             // 9
    TK_POUND,
    TK_DOLLAR,
    TK_PERCENT,
    TK_AMPER,
    TK_ASTERISK,
    TK_PLUS,                                               // 15
    TK_MINUS,
    TK_DOT,
    TK_SLASH,
    TK_COLON,
    TK_LESS,
    TK_EQUAL,
    TK_GREATER,
    TK_QUESTION,                                           // 23
    TK_AT_SYMBOL,
    TK_BACKSLASH,
    TK_CARET,
    TK_PIPE,
    TK_TILDE,                                              // 28

    // Indexing
    -1, -1, -1, -1,
    9,                                                     // 33
    -1,
    10, 11, 12, 13,
    -1,                                                    // 39
    0, 1, 14, 15, 2, 16, 17, 18,
    -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1,                            // 51...57
    19, 3, 20, 21, 22, 23, 24,
    -1, -1, -1, -1, -1, -1,                                // 65...70
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 71...80
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 81...90
    4, 25, 5, 26,
    -1,
    6,
    -1, -1, -1, -1,                                        // 97...100
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 101...110
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 111...120
    -1, -1,
    7, 27, 8, 28
};

} // anonymous

Token HsLexer::lexOprtrTable(char& ch)
{
    const char index = oprtrTable[ch];
    ch = consumeCharPeekNext();

    return Token(oprtrTable[index]);
}

Token HsLexer::lexSpecial(char& ch)
{
    return lexOprtrTable(ch);
}

Token HsLexer::lexAscSymbol(char& ch)
{
    return lexOprtrTable(ch);
}

Token HsLexer::classifyKeyword(const char* spell, size_t len) const
{
    return HsKeywords::classify(spell, len);
}

Token HsLexer::filterIdent() const
{
    context_->trackLexeme<Ident>(mark_, curr_ - mark_, LineCol(line_, col_));

    if (mark_[0] < 97)
        return TK_IDENTIFIER_CAPITALIZED;
    return TK_IDENTIFIER;
}
