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

#ifndef UAISO_LANGID_H__
#define UAISO_LANGID_H__

#include "Common/Config.h"
#include <string>
#include <vector>

namespace uaiso {

/*!
 * \brief The LangName enum
 *
 * An enumeration with supported language's name
 */
enum class LangId : char
{
    D,
    Go,
    Hs,
    Py
};

/*!
 * Return a vector with the available languages.
 */
UAISO_API std::vector<LangId> availableLangs();

/*!
 * Return a string representation of the given language name.
 */
UAISO_API std::string langName(LangId langId);

} // namespace uaiso

#endif
