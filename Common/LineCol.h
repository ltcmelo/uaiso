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

#ifndef UAISO_LINECOL_H__
#define UAISO_LINECOL_H__

#include "Common/Config.h"
#include <functional>
#include <ostream>

namespace uaiso {

/*!
 * \brief The LineCol class
 *
 * Utility for when only line/col information is enough.
 */
class UAISO_API LineCol
{
public:
    LineCol()
        : line_(-1), col_(-1)
    {}

    LineCol(int line, int col)
        : line_(line), col_(col)
    {}

    bool isEmpty() const
    {
        return line_ == -1 && col_ == -1;
    }

    int line_;
    int col_;
};

bool operator==(const LineCol& a, const LineCol& b);
bool operator!=(const LineCol& a, const LineCol& b);
bool operator<(const LineCol& a, const LineCol& b);
LineCol operator+(const LineCol& a, const LineCol& b);

UAISO_API inline std::ostream& operator<<(std::ostream& os, const LineCol& lineCol)
{
    os << lineCol.line_ << ":" << lineCol.col_;
    return os;
}

} // namespace uaiso

namespace std {

template <>
struct hash<uaiso::LineCol>
{
    size_t operator()(const uaiso::LineCol& lineCol) const
    {
        const int a = std::hash<int>()(lineCol.line_);
        const int b = std::hash<int>()(lineCol.col_);
        return a ^ b;
    }
};

} // namespace std

#endif
