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

#ifndef UAISO_HSLEXER_H__
#define UAISO_HSLEXER_H__

#include "Common/Config.h"
#include "Common/Test.h"
#include "Parsing/Lexer.h"

namespace uaiso {

class UAISO_API HsLexer final : public Lexer
{
public:
    HsLexer();
    ~HsLexer();

    Token lex() override;

private:
    DECL_CLASS_TEST(HsLexer)

    using Base = Lexer;

    Token lexOprtrTable(char& ch);
    Token lexSpecial(char& ch);
    Token lexAscSymbol(char& ch);
    Token lexAscSymbolMaybe2(char& ch, const char& match, Token reserved);
    Token lexAscSymbolMaybeMore(char& ch, Token tk);

    bool isAscSymbol(const char& ch) const;

    Token classifyKeyword(const char* spell, size_t len) const override;
    Token filterIdent(char& ch) override;
};

} // namespace uaiso

#endif
