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

#ifndef UAISO_PARSERLL1_H__
#define UAISO_PARSERLL1_H__

#include "Parsing/Parser.h"
#include "Parsing/Lexer.h"

namespace uaiso {

class ParserLL1 : public Parser
{
public:
    virtual ~ParserLL1() = default;

private:
    // Hide the lexer to avoid conflicting use with the parser.
    using Parser::lexer_;

protected:
    ParserLL1() = default;

    void consumeToken() override;

    SourceLoc currentLoc() const override { return lexer_->tokenLoc(); }
};

} // namespace uaiso

#endif
