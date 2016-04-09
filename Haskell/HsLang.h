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

#ifndef UAISO_HSLANG_H__
#define UAISO_HSLANG_H__

#include "Parsing/Lang.h"

namespace uaiso {

class UAISO_API HsLang final : public Lang
{
public:
    HsLang();

    Structure structure() const override;

    ImportMechanism importMechanism() const override;

    std::string sourceFileSuffix() const override;

    bool isBinPrefix(char ch) const override { return false; }

    bool hasStrictDecimalPoint() const override { return true; }

    bool hasStrLitJoinEscape() const override { return true; }

    std::pair<bool, char> strLitJoinEscapeMatcher() const override;
};

} // namespace uaiso

#endif
