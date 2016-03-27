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

#include "Parsing/Lexer.h"
#include "Parsing/ParsingContext.h"
#include "Parsing/Lang.h"
#include "Common/Assert.h"
#include <cctype>
#include <iostream>

using namespace uaiso;

Lexer::Lexer()
{}

Lexer::~Lexer()
{}

void Lexer::setBuffer(const char* buff, size_t len)
{
    buff_ = buff;
    mark_ = buff;
    curr_ = buff;
    eof_ = buff + len;
}

void Lexer::setContext(ParsingContext *context)
{
    context_ = context;
}

SourceLoc Lexer::tokenLoc() const
{
    SourceLoc loc(line_, col_, line_ + breaks_, col_ + (curr_ - mark_), "");
    if (breaks_)
        loc.lastCol_ = rearLeng_ - 1; // Not a past-the-end value.
    return loc;
}

void Lexer::updatePos()
{
    if (breaks_) {
        line_ += breaks_;
        breaks_ = 0;
        col_ += rearLeng_;
    } else {
        col_ += curr_ - mark_;
    }
}

void Lexer::handleNewLine()
{
    handleNewLineNoColReset();
    col_ = 0;
}

void Lexer::handleNewLineNoColReset()
{
    ++breaks_;
    rearLeng_ = 0;
}

char Lexer::peekChar(size_t dist) const
{
    if (curr_ + dist >= eof_)
        return 0;
    return *(curr_ + dist);
}

void Lexer::consumeChar(size_t dist)
{
    UAISO_ASSERT(peekChar(dist), return);

    curr_ += dist + 1;
    rearLeng_ += dist + 1;
}

char Lexer::consumeCharPeekNext(size_t dist)
{
    consumeChar(dist);
    return peekChar();
}

Token Lexer::lexStrLit(char& ch, const bool mayBreak, const Lang* lang)
{
    UAISO_ASSERT(ch == '"' || ch == '\'', return TK_INVALID);

    char quote = ch;
    ch = consumeCharPeekNext();

    return lexStrLitEnd(ch, quote, mayBreak, lang);
}

Token Lexer::lexStrLitEnd(char& ch, const char quote, const bool mayBreak, const Lang* lang)
{
    UAISO_ASSERT(ch != '"' || ch != '\'', return TK_INVALID);

    while (ch && ch != quote) {
        if (ch == '\\') {
            ch = consumeCharPeekNext();
            // Current character must be a valid escape. This includes a line
            // split string literal join, if the language supports it, such as
            // C++ or Python. In addition, a "matcher" for the escape may be
            // required, as it's the case in Haskell.
            if (ch == '\n' && lang->hasStrLitJoinEscape()) {
                while (ch && std::isspace(ch))
                    ch = consumeCharPeekNext();
                bool hasMatcher;
                char matcher;
                std::tie(hasMatcher, matcher) = lang->strLitJoinEscapeMatcher();
                if (hasMatcher && ch != matcher)
                    context_->trackReport(Diagnostic::UnmatchedStringJoining, tokenLoc());
            } else if (!lang->isStrLitEscape(ch)) {
                context_->trackReport(Diagnostic::UnknownEscapeSequence, tokenLoc());
            }
        } else if (ch == '\n') {
            handleNewLineNoColReset();
            if (!mayBreak)
                context_->trackReport(Diagnostic::UnterminatedString, tokenLoc());
        }
        ch = consumeCharPeekNext();
    }

    if (ch)
        ch = consumeCharPeekNext();
    else
        context_->trackReport(Diagnostic::UnterminatedString, tokenLoc());

    return TK_STR_LIT;
}

Token Lexer::lexIdentOrKeyword(char& ch, const Lang* lang)
{
    UAISO_ASSERT(lang->isIdentFirstChar(ch), return TK_INVALID);

    ch = consumeCharPeekNext();
    while (lang->isIdentChar(ch))
        ch = consumeCharPeekNext();

    const Token& tk = filterKeyword(mark_, curr_ - mark_);
    if (tk != TK_INVALID)
        return tk;

    return classifyIdent(ch);
}

Token Lexer::classifyIdent(char&)
{
    return TK_IDENT;
}

Token Lexer::lexNumLit(char& ch, const Lang* lang)
{
    UAISO_ASSERT(std::isdigit(ch)
                     || (ch == '.' && !lang->hasStrictDecimalPoint()),
                 return TK_INVALID);

    if (ch == '0') {
        ch = consumeCharPeekNext();

        // Octal format
        auto isOctalDigit = [] (char ch) -> bool {
            return ch >= '0' && ch < '8';
        };
        if (lang->isOctalPrefix(ch) && isOctalDigit(peekChar(1))) {
            ch = consumeCharPeekNext(1);
            while (ch && isOctalDigit(ch))
                ch = consumeCharPeekNext();
        }

        // Hex format
        if (lang->isHexPrefix(ch) && std::isxdigit(peekChar(1))) {
            ch = consumeCharPeekNext(1);
            while (ch && std::isxdigit(ch))
                ch = consumeCharPeekNext();
        }

        // Bin format
        auto isBinDigit = [] (char ch) -> bool {
            return ch == '0' || ch == '1';
        };
        if (lang->isBinPrefix(ch) && isBinDigit(peekChar(1))) {
            ch = consumeCharPeekNext(1);
            while (ch && isBinDigit(ch))
                ch = consumeCharPeekNext();
        }
    }

    while (ch && std::isdigit(ch))
        ch = consumeCharPeekNext();

    // Long integer
    if (ch && lang->isLongSuffix(ch)) {
        ch = consumeCharPeekNext();
        return TK_INT_LIT;
    }

    if (ch && (lang->isExponent(ch)
               || (ch == '.' && (!lang->hasStrictDecimalPoint()
                                 || std::isdigit(peekChar(1)))))) {
        return lexFloatLit(ch, lang);
    }

    return TK_INT_LIT;
}

Token Lexer::lexFloatLit(char& ch, const Lang* lang)
{
    UAISO_ASSERT(lang->isExponent(ch)
                    || (ch == '.' && (!lang->hasStrictDecimalPoint()
                                      || std::isdigit(peekChar(1)))),
                  return TK_INVALID);

    if (ch == '.')
        ch = consumeCharPeekNext();

    while (ch && std::isdigit(ch))
        ch = consumeCharPeekNext();

    if (lang->isExponent(ch)) {
        ch = consumeCharPeekNext();
        if (ch == '+' || ch == '-')
            ch = consumeCharPeekNext();
        while (ch && std::isdigit(ch))
            ch = consumeCharPeekNext();
    }

    return TK_FLOAT_LIT;
}

bool Lexer::inCompletionArea() const
{
    if (!context_->hasStopMark())
        return false;

    const auto& lineCol = context_->stopMark();
    if (lineCol.line_ <= line_ && lineCol.col_ <= col_)
        return true;

    return false;
}

bool Lexer::maybeRealizeCompletion()
{
    if (inCompletionArea()) {
        context_->clearStopMark();
        return true;
    }

    return false;
}
