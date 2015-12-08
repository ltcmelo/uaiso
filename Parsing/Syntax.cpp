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

#include "Parsing/Syntax.h"
#include <cctype>

using namespace uaiso;

Syntax::~Syntax()
{}

bool Syntax::hasBlockLevelScope() const { return true; }

bool Syntax::hasFuncLevelScope() const { return true; }

bool Syntax::hasExecutableRecord() const { return false; }

std::string Syntax::packageSeparator() const
{
    return ".";
}

std::string Syntax::memberAccessOprtr() const
{
    return ".";
}

std::string Syntax::funcCallDelim() const
{
    return "(";
}

bool Syntax::isIdentFirstChar(char ch) const
{
    return std::isalpha(ch) || ch == '_';
}

bool Syntax::isIdentChar(char ch) const
{
    return std::isalnum(ch) || ch == '_';
}

Token Syntax::classifyIdent(const char* spell, size_t leng) const
{
    return TK_IDENTIFIER;
}

bool Syntax::isStrLitQuote(char ch) const
{
    return ch == '"';
}

bool Syntax::isOctalPrefix(char ch) const
{
    return ch == 'o' || ch == 'O';
}

bool Syntax::isHexPrefix(char ch) const
{
    return ch == 'x' || ch == 'X';
}

bool Syntax::isBinPrefix(char ch) const
{
    return ch == 'b' || ch == 'B';
}

bool Syntax::isExponent(char ch) const
{
    return ch == 'e' || ch == 'E';
}
