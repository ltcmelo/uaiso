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

#include "Parsing/Lexeme.h"

using namespace uaiso;

Lexeme::Lexeme(const std::string& s, Kind kind)
    : s_(s), kind_(kind)
{}

Lexeme::Lexeme(const Lexeme &tv)
    : s_(tv.s_), kind_(tv.kind_)
{}

Lexeme::Lexeme(Lexeme &&tv)
    : s_(std::move(tv.s_)), kind_(tv.kind_)
{}

Lexeme& Lexeme::operator=(const Lexeme& tv)
{
    if (this != &tv) {
        s_ = tv.s_;
        kind_ = tv.kind_;
    }
    return *this;
}

Lexeme& Lexeme::operator=(Lexeme&& tv)
{
    s_ = std::move(tv.s_);
    kind_ = tv.kind_;
    return *this;
}

Lexeme::~Lexeme()
{}
