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

#ifndef UAISO_SYNTAX_H__
#define UAISO_SYNTAX_H__

#include "Common/Config.h"
#include "Parsing/Token.h"
#include <string>

namespace uaiso {

class UAISO_API Syntax
{
public:
    virtual ~Syntax();

    virtual std::string sourceFileSuffix() const = 0;

    virtual std::string packageSeparator() const;

    virtual std::string memberAccessOprtr() const;

    virtual std::string funcCallDelim() const;

    virtual bool isIdentFirstChar(char ch) const;

    virtual bool isIdentChar(char ch) const;

    virtual Token classifyIdent(const char* spell, size_t length) const;

    virtual bool isStrLitQuote(char ch) const;

    virtual bool isOctalPrefix(char ch) const;

    virtual bool isHexPrefix(char ch) const;

    virtual bool isBinPrefix(char ch) const;

    virtual bool isExponent(char ch) const;
};

} // namespace uaiso

#endif
