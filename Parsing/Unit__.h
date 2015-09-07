/******************************************************************************
 * Copyright (c) 2014-2015-2015 Leandro T. C. Melo (ltcmelo@gmail.com)
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

///////////////////////////////////////////////////////
///                                                 ///
///         This is an INTERNAL header              ///
///                                                 ///
///   Do not include this header from outside the   ///
///   the uaiso lib or from any public API header   ///
///                                                 ///
///////////////////////////////////////////////////////

#ifndef UAISO_UNIT_INTERNAL_H__
#define UAISO_UNIT_INTERNAL_H__

#include "Parsing/Unit.h"
#include "Parsing/Token.h"
#include "Ast/Ast.h"

namespace uaiso {

struct Unit::UnitImpl
{
    UnitImpl()
        : bits_(0)
        , reports_(new DiagnosticReports)
        , source_(nullptr)
    {}

    struct BitFields
    {
        uint32_t readFromFile_       : 1;
    };

    union
    {
        BitFields bit_;
        uint32_t  bits_;
    };

    std::string fullFileName_;
    std::unique_ptr<Ast> ast_;
    std::unique_ptr<DiagnosticReports> reports_;

    union
    {
        const std::string* source_;
        FILE* file_;
    };
};

} // namespace uaiso

#endif
