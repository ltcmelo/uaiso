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

#ifndef UAISO_LEXER_H__
#define UAISO_LEXER_H__

#include "Common/Config.h"
#include "Parsing/Token.h"
#include <cstddef>
#include <memory>

namespace uaiso {

class Syntax;

class UAISO_API Lexer
{
public:
    virtual ~Lexer();

    void setBuffer(const char* buff, size_t length);

    virtual Token lex() = 0;

protected:
    Lexer();

    char peekChar(size_t dist = 0) const;
    void consumeChar(size_t dist = 0);
    char consumeCharPeekNext(size_t dist = 0);

    Token lexStrLit(char& ch, const char quote, const bool mayBreak, const Syntax* syntax);
    Token lexIdentOrKeyword(char& ch, const Syntax* syntax);
    Token lexNumLit(char& ch, const Syntax* syntax);

    const char* buff_ { nullptr };
    const char* curr_ { nullptr };
    const char* eof_ { nullptr };

    size_t line_ { 0 };
    size_t col_ { 0 };
};

} // namespace uaiso

#endif
