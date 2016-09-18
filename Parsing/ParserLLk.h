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

#ifndef UAISO_PARSERLLk_H__
#define UAISO_PARSERLLk_H__

#include "Parsing/Parser.h"
#include <cstddef>
#include <tuple>
#include <vector>

namespace uaiso {

class ParserLLk : public Parser
{
public:
    virtual ~ParserLLk() = default;

protected:
    ParserLLk();

    void init() override;

    void consumeToken() override;
    void consumeToken(size_t k);

    /*!
     * \brief peekToken
     * \param k
     * \return
     *
     * Peek token beyond the one ahead.
     */
    const Token peekToken(size_t k);

    SourceLoc currentLoc() const override;

private:
    // Hide the lexer to avoid conflicting use with the parser.
    using Parser::lexer_;

    using Buffer = std::vector<std::tuple<Token, SourceLoc>>;
    using Pos = Buffer::size_type;

    Buffer buffer_;
    Pos cur_;
    Pos last_;
};

} // namespace uaiso

#endif
