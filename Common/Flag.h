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

#ifndef UAISO_FLAG_H__
#define UAISO_FLAG_H__

#include <type_traits>

namespace uaiso {

template <class EnumT>
class Flags
{
private:
    using UnderType = typename std::underlying_type<EnumT>::type;

    UnderType flags_; // Combined enum values.

public:
    Flags(EnumT e)
        : flags_(static_cast<UnderType>(e))
    {}

    Flags(UnderType underlyingType)
        : flags_(underlyingType)
    {}

    operator bool() { return static_cast<UnderType>(flags_) != 0; }

    Flags operator|(EnumT e) const
    {
        return Flags(flags_ | static_cast<UnderType>(e));
    }

    Flags operator&(EnumT e) const
    {
        return Flags(flags_ & static_cast<UnderType>(e));
    }

    Flags& operator|=(EnumT e)
    {
        flags_ |= static_cast<UnderType>(e);
        return *this;
    }

    Flags& operator&=(EnumT e)
    {
        flags_ &= static_cast<UnderType>(e);
        return *this;
    }
};

} // namespace uaiso

#define UAISO_FLAGGED_ENUM(NAME) \
    using NAME##s = Flags<NAME>;

#endif
