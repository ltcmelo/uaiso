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

#include "Parsing/ParserLLk.h"
#include "Parsing/Lexer.h"
#include "Parsing/ParsingContext.h"
#include "Common/Assert.h"

using namespace uaiso;

ParserLLk::ParserLLk()
    : cur_(0)
{}

void ParserLLk::prepare(Lexer* lexer, ParsingContext* context)
{
    UAISO_ASSERT(buffer_.empty(), return);

    lexer_ = lexer;
    context_ = context;

    buffer_.reserve(100); // Ad-hoc.
    buffer_.push_back(TK_INVALID); // First valid position is 1.

    Token tk;
    do {
        auto&& loc = lexer_->tokenLoc();
        tk = lexer_->lex();
        buffer_.emplace_back(std::make_tuple(tk, loc));
    } while (tk != TK_EOP);
    last_ = buffer_.size() - 1; // Store size to avoid repeated queries.
}

void ParserLLk::consumeToken()
{
    consumeToken(1);
}

void ParserLLk::consumeToken(size_t k)
{
    auto&& data = std::move(buffer_[++cur_ + k - 1]);

    ahead_ = std::get<0>(data);
    prevLoc_ = std::get<1>(std::move(data));
    prevLoc_.fileName_ = context_->fileName();
}

const Token ParserLLk::peekToken(size_t k)
{
    UAISO_ASSERT(k > 1, return TK_INVALID);

    if (cur_ + k > last_)
        return TK_EOP;
    return std::get<0>(buffer_[cur_ + k]);
}

SourceLoc ParserLLk::currentLoc() const
{
    UAISO_ASSERT(cur_ + 1 < last_, return kEmptyLoc);

    auto loc = std::get<1>(buffer_[cur_ + 1]);
    loc.fileName_ = context_->fileName();
    return loc;
}
