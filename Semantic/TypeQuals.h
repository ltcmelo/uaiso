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

#ifndef UAISO_TYPEQUALIFIERS_H__
#define UAISO_TYPEQUALIFIERS_H__

#include "Common/Flag.h"

namespace uaiso {

enum class TypeQualFlag : char
{
    Const       = 0x1,
    Immutable   = 0x1 << 1,
    Restrict    = 0x1 << 2,
    Shared      = 0x1 << 3,
    Volatile    = 0x1 << 4
};

UAISO_FLAGGED_ENUM(TypeQualFlag);

} // namespace uaiso

#endif
