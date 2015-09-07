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

#include "Parsing/IncrementalLexer__.h"

using namespace uaiso;

IncrementalLexer::IncrementalLexer()
    : P(new IncrementalLexerImpl)
{}

IncrementalLexer::~IncrementalLexer()
{}

void IncrementalLexer::tokenize(const std::string& source, State state)
{
    P->state_ = state;
    tokenize(source);
}

Phrasing* IncrementalLexer::releasePhrasing()
{
    return P->phrasing_.release();
}

IncrementalLexer::State IncrementalLexer::state() const
{
    return P->state_;
}

void IncrementalLexer::decideState()
{
    if (!P->phrasing_->isEmpty()) {
        size_t index = P->phrasing_->size() - 1;
        Token token = P->phrasing_->token(index);
        if (token == TK_MULTILINE_COMMENT) {
            Phrasing::TokenFlags flags = P->phrasing_->flags(index);
            if (flags & Phrasing::TokenFlag::Unterminated) {
                P->state_ = IncrementalLexer::InMultilineComment;
                return;
            }
        }
    }

    P->state_ = IncrementalLexer::InCode;
}
