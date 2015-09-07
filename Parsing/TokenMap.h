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

#ifndef UAISO_TOKENMAP_H__
#define UAISO_TOKENMAP_H__

#include "Common/LineCol.h"
#include "Common/Pimpl.h"
#include "Parsing/Token.h"

namespace uaiso {

/*!
 * \brief The TokenMap class
 */
class UAISO_API TokenMap final
{
public:
    TokenMap();
    ~TokenMap();

    Token insertOrFind(int tk,
                       const std::string& fullFileName,
                       const LineCol& lineCol);

    Token find(const std::string& fullFileName,
               const LineCol& lineCol) const;

    void clear();
    void clear(const std::string& fullFileName);

private:
    DECL_PIMPL(TokenMap)
};

} // namespace uaiso

#endif
