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

#include "Parsing/Phrasing.h"
#include "Common/Assert.h"
#include <vector>

using namespace uaiso;

#define ENSURE_CONSISTENCY \
    UAISO_ASSERT(P->tokens_.size() == P->lineCol_.size(), return 0); \
    UAISO_ASSERT(P->tokens_.size() == P->lengs_.size(), return 0); \
    UAISO_ASSERT(P->tokens_.size() == P->flags_.size(), return 0);

struct uaiso::Phrasing::PhrasingImpl
{
    std::vector<Token> tokens_;
    std::vector<LineCol> lineCol_;
    std::vector<int> lengs_;
    std::vector<Phrasing::TokenFlags> flags_;
};

Phrasing::Phrasing()
    : P(new PhrasingImpl)
{}

Phrasing::~Phrasing()
{}

void Phrasing::append(Token tk, const LineCol& lineCol, int leng)
{
    append(tk, lineCol, leng, TokenFlag::None);
}

void Phrasing::append(Token tk, const LineCol& lineCol, int leng, TokenFlags flags)
{
    P->tokens_.push_back(tk);
    P->lineCol_.push_back(lineCol);
    P->lengs_.push_back(leng);
    P->flags_.push_back(flags);
}

size_t Phrasing::size() const
{
    ENSURE_CONSISTENCY;

    return P->tokens_.size();
}

Token Phrasing::token(size_t index) const
{
    UAISO_ASSERT(P->tokens_.size() > index, return Token::TK_INVALID);

    return P->tokens_[index];
}

LineCol Phrasing::lineCol(size_t index) const
{
    UAISO_ASSERT(P->lineCol_.size() > index, return LineCol());

    return P->lineCol_[index];
}

int Phrasing::length(size_t index) const
{
    UAISO_ASSERT(P->lengs_.size() > index, return 0);

    return P->lengs_[index];
}

Phrasing::TokenFlags Phrasing::flags(size_t index) const
{
    UAISO_ASSERT(P->flags_.size() > index, return TokenFlag::None);

    return P->flags_[index];
}

bool Phrasing::isEmpty() const
{
    ENSURE_CONSISTENCY;

    return P->tokens_.empty();
}

void Phrasing::clear()
{
    P->tokens_.clear();
    P->lineCol_.clear();
    P->lengs_.clear();
    P->flags_.clear();
}

Phrasing::Iterator Phrasing::begin() const
{
    return Iterator(P->tokens_.begin());
}

Phrasing::Iterator Phrasing::end() const
{
    return Iterator(P->tokens_.end());
}
