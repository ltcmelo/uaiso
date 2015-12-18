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

#ifndef UAISO_PHRASING_H__
#define UAISO_PHRASING_H__

#include "Common/Flag.h"
#include "Common/Config.h"
#include "Common/LineCol.h"
#include "Common/Pimpl.h"
#include "Parsing/Token.h"
#include <iterator>
#include <iostream>
#include <vector>

namespace uaiso {

/*!
 * \brief The Phrasing class
 */
class UAISO_API Phrasing
{
public:
    Phrasing();
    virtual ~Phrasing();

    enum class TokenFlag : uint8_t
    {
        None              = 0,
        Unterminated      = 0x1,
        Joined            = 0x1 << 1
    };
    UAISO_FLAGGED_ENUM(TokenFlag)

    void append(Token tk, const LineCol& lineCol, int len);

    void append(Token tk, const LineCol& lineCol, int len, TokenFlags flags);

    size_t size() const;

    Token token(size_t index) const;

    LineCol lineCol(size_t index) const;

    int length(size_t index) const;

    TokenFlags flags(size_t index) const;

    class Iterator
            : public std::iterator<std::input_iterator_tag, Token>
    {
    public:
        Iterator& operator++() { ++it_; return *this; }
        Token operator*() { return *it_; }
        bool operator!=(const Iterator& other) { return it_ != other.it_; }

    private:
        friend class Phrasing;
        using BaseIterator = std::vector<Token>::const_iterator;
        Iterator(BaseIterator it) : it_(it) {}
        BaseIterator it_;
    };

    Iterator begin() const;

    Iterator end() const;

    void clear();

    bool isEmpty() const;

private:
    DECL_PIMPL(Phrasing)
};

inline std::ostream& operator<<(std::ostream& os, const Phrasing& phrasing)
{
    std::copy(phrasing.begin(), phrasing.end(),
              std::ostream_iterator<Token>(os));
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const std::vector<Token>& v)
{
    std::copy(v.begin(), v.end(), std::ostream_iterator<Token>(os));
    return os;
}

} // namespace uaiso

#endif
