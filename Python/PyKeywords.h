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

#ifndef UAISO_PYKEYWORDS_H__
#define UAISO_PYKEYWORDS_H__

#include "Common/Config.h"
#include "Parsing/Token.h"
#include <cstddef>

namespace uaiso {

class UAISO_API PyKeywords final
{
public:
    PyKeywords() = delete;

    static Token classify(const char* spell, size_t len);

private:
    static Token classify2(const char* spell);
    static Token classify3(const char* spell);
    static Token classify4(const char* spell);
    static Token classify5(const char* spell);
    static Token classify6(const char* spell);
    static Token classify7(const char* spell);
    static Token classify8(const char* spell);
};

} // namespace uaiso

#endif
