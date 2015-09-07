/******************************************************************************
 * Unified Source Code Modelling Engine
 * Copyright (c) 2014-2015 Leandro T. C. Melo
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

#ifndef UAISO_SOURCELOC_H__
#define UAISO_SOURCELOC_H__

#include "Common/Assert.h"
#include "Common/Config.h"
#include "Common/LineCol.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <utility>

namespace uaiso {

class UAISO_API SourceLoc
{
    friend void swap(SourceLoc &a, SourceLoc &b);

public:
    SourceLoc()
        : line_(0)
        , col_(0)
        , lastLine_(0)
        , lastCol_(0)
        , fileName_("<unspecified file>")
    {}

    SourceLoc(size_t line,
              size_t col,
              size_t lastLine,
              size_t lastCol,
              const std::string& fileName)
        : line_(line)
        , col_(col)
        , lastLine_(lastLine)
        , lastCol_(lastCol)
        , fileName_(fileName)
    {}

    SourceLoc(const SourceLoc& location)
        : line_(location.line_)
        , col_(location.col_)
        , lastLine_(location.lastLine_)
        , lastCol_(location.lastCol_)
        , fileName_(location.fileName_)
    {}

    SourceLoc& operator=(const SourceLoc& location)
    {
        SourceLoc other(location);
        swap(*this, other);
        return *this;
    }

    SourceLoc(SourceLoc&& location)
        : SourceLoc() // Delegating empty construction
    {
        swap(*this, location);
    }

    SourceLoc& operator=(SourceLoc&& location) = default;

    LineCol lineCol() const
    {
        return LineCol(line_, col_);
    }

    bool isEmpty() const
    {
        return line_ == 0
                && col_ == 0
                && lastLine_ == 0
                && lastCol_ == 0;
    }

    size_t line_;
    size_t col_;
    size_t lastLine_;
    size_t lastCol_;
    std::string fileName_;
};

const SourceLoc kEmptyLoc = SourceLoc();

inline void swap(SourceLoc& a, SourceLoc& b)
{
    using std::swap;
    swap(a.line_, b.line_);
    swap(a.col_, b.col_);
    swap(a.lastLine_, b.lastLine_);
    swap(a.lastCol_, b.lastCol_);
    swap(a.fileName_, b.fileName_);
}

UAISO_API inline std::ostream& operator<<(std::ostream& os, const SourceLoc& loc)
{
    os << loc.fileName_ << ":" << loc.line_ << ":" << loc.col_;
    return os;
}

inline SourceLoc joinedLoc(const SourceLoc& a, const SourceLoc& b)
{
    //UAISO_ASSERT(a.fileName_ == b.fileName_, return kEmptyLoc);

    return SourceLoc(a.line_, a.col_, b.lastLine_, b.lastCol_, b.fileName_);
}

} // namespace uaiso

#endif
