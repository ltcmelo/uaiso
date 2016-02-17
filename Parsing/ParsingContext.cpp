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

#include "Parsing/ParsingContext.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/Phrasing.h"
#include "Parsing/Token.h"
#include "Parsing/TokenMap.h"
#include "Ast/Ast.h"
#include "Common/Util__.h"
#include <iostream>

using namespace uaiso;

ParsingContext::ParsingContext()
    : bits_(0)
{}

ParsingContext::~ParsingContext()
{}

void ParsingContext::setFileName(const char* fullFileName)
{
    fileName_ = fullFileName;
}

const char *ParsingContext::fileName() const
{
    return fileName_;
}

void ParsingContext::setAllowComments(bool enable)
{
    bit_.comments_ = enable;
}

bool ParsingContext::allowComments() const
{
    return bit_.comments_;
}

void ParsingContext::setStopMark(const LineCol& lineCol)
{
    stopMark_ = lineCol;
}

const LineCol& ParsingContext::stopMark() const
{
    return stopMark_;
}

bool ParsingContext::hasStopMark() const
{
    return !stopMark_.isEmpty();
}

void ParsingContext::clearStopMark()
{
    stopMark_ = { -1, -1 };
}

void ParsingContext::enterTokenState()
{
    bit_.hasState_ = 1;
}

bool ParsingContext::hasTokenState() const
{
    return bit_.hasState_;
}

void ParsingContext::leaveTokenState()
{
    bit_.hasState_ = 0;
}

void ParsingContext::notifyProgramMatched()
{
    bit_.acceptEOF_ = true;
}

bool ParsingContext::postponeEoF()
{
    if (--toleranceCounter_ == 0)
        bit_.acceptEOF_ = true;
    return !bit_.acceptEOF_;
}

int ParsingContext::interceptRawToken(int token)
{
    return token;
}

void ParsingContext::trackPhrase(Token tk,
                                 const LineCol& lineCol,
                                 int len, bool unterminated)
{
    if (phrasing_) {
        Phrasing::TokenFlags flags = Phrasing::TokenFlag::None;
        if (!bit_.prevTkTerminated_) {
            flags |= Phrasing::TokenFlag::Joined;
            bit_.prevTkTerminated_ = true;
        }
        if (unterminated) {
            flags |= Phrasing::TokenFlag::Unterminated;
            bit_.prevTkTerminated_ = false;
        }
        phrasing_->append(tk, lineCol, len, flags);
    }
}

void ParsingContext::trackToken(Token tk, const LineCol& lineCol)
{
    if (tokens_)
        tokens_->insertOrFind(tk, fileName_, lineCol);
}

template <class LexemeT>
void ParsingContext::trackLexeme(const char* lex, const LineCol& lineCol)
{
    if (lexs_)
        lexs_->insertOrFind<LexemeT>(lex, fileName_, lineCol);
}

template <class LexemeT>
void ParsingContext::trackLexeme(const char* lex,
                                 int count,
                                 const LineCol& lineCol)
{
    if (lexs_)
        lexs_->insertOrFind<LexemeT>(std::string(lex, count),
                                        fileName_,
                                        lineCol);
}

// Explicit instantiations for the known lexemes.
template void
ParsingContext::trackLexeme<Ident>(const char* lex, const LineCol& lineCol);
template void
ParsingContext::trackLexeme<Ident>(const char* lex, int count, const LineCol& lineCol);
template void
ParsingContext::trackLexeme<StrLit>(const char* lex, const LineCol& lineCol);
template void
ParsingContext::trackLexeme<StrLit>(const char* lex, int count, const LineCol& lineCol);
template void
ParsingContext::trackLexeme<NumLit>(const char* lex, const LineCol& lineCol);
template void
ParsingContext::trackLexeme<NumLit>(const char* lex, int count, const LineCol& lineCol);

void ParsingContext::takeAst(std::unique_ptr<Ast> ast)
{
    ast_ = std::move(ast);
}

Ast* ParsingContext::releaseAst()
{
    return ast_.release();
}
