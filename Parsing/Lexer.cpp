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

#include "Parsing/Lexer.h"
#include "Parsing/ParsingContext.h"
#include "Parsing/Syntax.h"
#include "Common/Assert.h"
#include <cctype>
#include <iostream>

using namespace uaiso;

Lexer::Lexer()
{}

Lexer::~Lexer()
{}

void Lexer::setBuffer(const char* buff, size_t leng)
{
    buff_ = buff;
    mark_ = buff;
    curr_ = buff;
    eof_ = buff + leng;
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

Token Lexer::lexStrLit(char& ch, const char quote, const bool mayBreak,
                       const Syntax* syntax)
{
    while (ch && ch != quote) {
        if (ch == '\\') {
            ch = consumeCharPeekNext();
            if (!std::iscntrl(ch) && !std::isprint(ch))
                context_->trackReport(Diagnostic::UnknownEscapeSequence, tokenLoc());
        } else if (ch == '\n') {
            handleNewLineNoColReset();
            if (!mayBreak)
                context_->trackReport(Diagnostic::UnterminatedString, tokenLoc());
        }
        ch = consumeCharPeekNext();
    }

    return TK_STRING_LITERAL;
}

Token Lexer::lexIdentOrKeyword(char& ch, const Syntax* syntax)
{
    UAISO_ASSERT(syntax->isIdentFirstChar(ch), return TK_INVALID);

    ch = consumeCharPeekNext();
    while (syntax->isIdentChar(ch))
        ch = consumeCharPeekNext();

    return syntax->classifyIdent(mark_, curr_ - mark_);
}

Token Lexer::lexNumLit(char& ch, const Syntax* syntax)
{
    UAISO_ASSERT(std::isdigit(ch) || ch == '.', return TK_INVALID);

    if (ch == '0') {
        ch = consumeCharPeekNext();
        // Octal
        if (syntax->isOctalPrefix(ch)) {
            ch = consumeCharPeekNext();
            if (!(ch >= '0' && ch < '8'))
                context_->trackReport(Diagnostic::InvalidOctalDigit, tokenLoc());
            while (ch && ch >= '0' && ch < '8')
                ch = consumeCharPeekNext();
        }
        // Hex
        if (syntax->isHexPrefix(ch)) {
            ch = consumeCharPeekNext();
            if (!std::isxdigit(ch))
                context_->trackReport(Diagnostic::InvalidHexDigit, tokenLoc());
            while (ch && std::isxdigit(ch))
                ch = consumeCharPeekNext();
        }
        // Bin
        if (syntax->isBinPrefix(ch)) {
            ch = consumeCharPeekNext();
            if (!(ch == '0' || ch == '1'))
                context_->trackReport(Diagnostic::InvalidBinaryDigit, tokenLoc());
            while (ch && (ch == '0' || ch == '1'))
                ch = consumeCharPeekNext();
        }
    }

    Token tk = TK_INTEGER_LITERAL;

    while (ch && (std::isdigit(ch)
                  || ch == '.'
                  || syntax->isExponent(ch))) {
        if (!std::isdigit(ch))
            tk = TK_FLOAT_LITERAL;
        const char prev = ch;
        ch = consumeCharPeekNext();
        if (syntax->isExponent(prev) && (ch == '+' || ch == '-'))
            ch = consumeCharPeekNext();
    }

    if (ch && (ch == 'L' || ch == 'l')) {
        ch = consumeCharPeekNext();
        if (tk == TK_FLOAT_LITERAL)
            context_->trackReport(Diagnostic::InvalidFloatSuffix, tokenLoc());
    }

    return tk;
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
