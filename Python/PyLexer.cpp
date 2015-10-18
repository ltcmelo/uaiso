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

#include "Python/PyLexer.h"
#include "Python/PySyntax.h"
#include "Parsing/ParsingContext.h"
#include "Common/Assert.h"
#include "Common/Trace__.h"
#include <cctype>

#define TRACE_NAME "PyLexer"

using namespace uaiso;

PyLexer::PyLexer()
    : atLineStart_(true)
    , indent_(0)
    , syntax_(new PySyntax)
{
    indentStack_.push(0);
}

namespace {

// The ASCII value of the operator/delimiter character is used to index a
// position corresponding to its token. "Composite" tokens are one adjacent
// to another.
const int oprtrDelimTable[] =
{
    TK_PERCENT, TK_PERCENT_EQUAL, // 0, 1
    TK_AMPER, TK_AMPER_EQUAL, // 2, 3
    TK_STAR, TK_STAR_EQUAL, TK_STAR_STAR, TK_STAR_STAR_EQUAL, // 4...7
    TK_PLUS, TK_PLUS_EQUAL, // 8, 9
    TK_MINUS, TK_MINUS_EQUAL, // 10, 11
    TK_SLASH, TK_SLASH_EQUAL, TK_SLASH_SLASH, TK_SLASH_SLASH_EQUAL, // 12...15
    TK_LESS, TK_LESS_EQUAL, TK_LESS_LESS, TK_LESS_LESS_EQUAL, // 16...19
    TK_EQUAL, TK_EQUAL_EQUAL, // 20, 21
    TK_GREATER, TK_GREATER_EQUAL, TK_GREATER_GREATER, TK_GREATER_GREATER_EQUAL, // 22...25
    TK_CARET, TK_CARET_EQUAL, // 26, 27
    TK_PIPE, TK_PIPE_EQUAL, // 28, 29
    0, 0, 0, 0, 0, 0, 0,
    0,                                                      // 37 (%)
    2,                                                      // 38 (&)
    0, 0, 0,
    4,                                                      // 42 (*)
    8,                                                      // 43 (+)
    0,
    10,                                                     // 45 (-)
    0,
    12,                                                     // 47 (/)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    16,                                                     // 60 (<)
    20,                                                     // 61 (=)
    22,                                                     // 62 (>)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    26,                                                     // 94 (^)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    28                                                      // 124 (|)
};

} // namespace anonymous

Token PyLexer::lex()
{
    updatePos();

    Token tk = TK_INVALID;

LexNextToken:
    UAISO_ASSERT(!indentStack_.empty(), return tk);

    char ch = peekChar();
    switch (ch) {
    case 0:
        if (indentStack_.top() > 0) {
            indentStack_.pop();
            return TK_DEDENT;
        }
        tk = TK_EOP;
        return tk;

    case '\n':
        indent_ = 0;
        handleNewLine();
        consumeChar();
        if (!atLineStart_) {
            atLineStart_ = true;
            tk = TK_NEWLINE;
            return tk;
        }
        goto LexNextToken;

    case '\t':
    case ' ':
        // Calculate indent level.
        ch = consumeCharPeekNext();
        while (ch == '\t' || ch == ' ') {
            if (atLineStart_)
                ++indent_;
            ch = consumeCharPeekNext();
        }
        // Maybe generate INDENT/DEDENT.
        if (atLineStart_) {
            const size_t largest = indentStack_.top();
            if (indent_ > largest) {
                indentStack_.push(indent_);
                tk = TK_INDENT;
                break;
            }
            if (indent_ < largest) {
                indentStack_.pop();
                tk = TK_DEDENT;
                break;
            }
        }
        goto LexNextToken;

    case '\f':
        ch = consumeCharPeekNext();
        if (atLineStart_) {
            while (ch == '\f')
                ch = consumeCharPeekNext();
        }
        goto LexNextToken;

    case '\\':
        ch = consumeCharPeekNext();
        if (ch == '\n') {
            handleNewLine();
            consumeChar();
            break;
        }
        // TODO: Error
        goto LexNextToken;

    case '"':
    case '\'':
        tk = lexStrLit(ch);
        break;

    case 'r':
    case 'R':
    case 'u':
    case 'U':
    case 'b':
    case 'B': {
        char next = peekChar(1);
        if (next == '"' || next == '\'') {
            consumeChar();
            tk = lexStrLit(next);
            break;
        }
        // Either a string literal or an identifier.
        if (next != ch && (next == 'r' || next == 'R')) {
            char next2 = peekChar(2);
            if (next2 == '"' || next2 == '\'') {
                consumeChar(1);
                tk = lexStrLit(next2);
                break;
            }
            tk = lexIdentOrKeyword(ch, syntax_.get());
            break;
        }
        // Certainly an identifier.
        tk = lexIdentOrKeyword(ch, syntax_.get());
        break;
    }

    case '.':
        if (std::isdigit(peekChar(1))) {
            tk = lexNumLit(ch, syntax_.get());
            break;
        }
        ch = consumeCharPeekNext();
        if (ch == '.' && peekChar(1) == '.') {
            consumeChar(1);
            tk = TK_DOT_DOT_DOT;
            break;
        }
        tk = TK_DOT;
        break;

    case '+':
    case '-':
    case '%':
    case '&':
    case '|':
    case '^':
    case '=':
    case '*':
    case '/':
    case '<':
    case '>':
        tk = lexOprtrOrDelim(ch);
        break;

    case '~':
        consumeChar();
        tk = TK_TILDE;
        break;

    case ',':
        consumeChar();
        tk = TK_COMMA;
        break;

    case ':':
        consumeChar();
        tk = TK_COLON;
        break;

    case ';':
        consumeChar();
        tk = TK_SEMICOLON;
        break;

    case '@':
        consumeChar();
        tk = TK_AT_SYMBOL;
        break;

    case '(':
        consumeChar();
        tk = TK_LPAREN;
        break;

    case ')':
        consumeChar();
        tk = TK_RPAREN;
        break;

    case '[':
        consumeChar();
        tk = TK_LBRACKET;
        break;

    case ']':
        consumeChar();
        tk = TK_RBRACKET;
        break;

    case '{':
        consumeChar();
        tk = TK_LBRACE;
        break;

    case '}':
        consumeChar();
        tk = TK_RBRACE;
        break;

    case '!':
        ch = consumeCharPeekNext();
        if (ch == '=') {
            consumeChar();
            tk = TK_EXCLAM_EQUAL;
            break;
        }
        // erro report
        break;

    case '#':
        ch = consumeCharPeekNext();
        while (ch && ch != '\n') {
            char prev = ch;
            ch = consumeCharPeekNext();
            if (prev == '\\' && ch == '\n') {
                handleNewLine();
                ch = consumeCharPeekNext();
            }
        }
        goto LexNextToken;

    default:
        if (syntax_->isIdentFirstChar(ch)) {
            tk = lexIdentOrKeyword(ch, syntax_.get());
            break;
        }
        if (std::isdigit(ch)) {
            tk = lexNumLit(ch, syntax_.get());
            break;
        }
        // Don't know what this is.
        consumeChar();
        PRINT_TRACE("Unknown char %c at %d,%d\n", ch, 0, 0);
        break;
    }

    atLineStart_ = false;

    return tk;
}

Token PyLexer::lexStrLit(char& ch)
{
    UAISO_ASSERT(ch == '"' || ch == '\'', return TK_INVALID);

    char quote = ch;
    ch = consumeCharPeekNext();

    // Check whether this is a triple-quoted string.
    bool triple = false;
    if (ch == quote) {
        char next = peekChar(1);
        if (next == quote) {
            consumeChar(2);
            triple = true;
        }
    }

    Base::lexStrLit(ch, quote, triple, syntax_.get());

    if (ch) {
        ch = consumeCharPeekNext();
        if (triple) {
            if (ch == quote) {
                ch = consumeCharPeekNext();
                if (ch == quote) {
                    ch = consumeCharPeekNext();
                } else {
                    // TODO: Error, unterminated string.
                }
            } else {
                // TODO: Error, unterminated string.
            }
        }
    } else {
        // TODO: Error, unterminated string.
    }

    return TK_STRING_LITERAL;
}

Token PyLexer::lexOprtrOrDelim(char& ch)
{
    // The "base" token for the operator/delimiter.
    const int base = oprtrDelimTable[ch];

    const char prev = ch;
    ch = consumeCharPeekNext();

    // A token like <=, >=, |=, etc. That means means base + 1.
    if (ch == '=') {
        consumeChar();
        return Token(oprtrDelimTable[base + 1]);
    }
    // Either a token like << or <<=, etc. That means base + 2 or +3.
    if (ch == prev) {
        ch = consumeCharPeekNext();
        if (ch == '=') {
            consumeChar();
            return Token(oprtrDelimTable[base + 3]);
        }
        return Token(oprtrDelimTable[base + 2]);
    }

    return Token(oprtrDelimTable[base]);
}
