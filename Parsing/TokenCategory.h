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

#ifndef UAISO_TOKENCATEGORY_H__
#define UAISO_TOKENCATEGORY_H__

#include "Common/Config.h"
#include "Parsing/Token.h"

namespace uaiso {

UAISO_API inline bool isBuiltin(Token tk)
{
    return tk > BEGIN_BUILTIN_TYPES && tk < END_BUILTIN_TYPES;
}

UAISO_API inline bool isKeyword(Token tk)
{
    return tk > BEGIN_KEYWORD && tk < END_KEYWORD;
}

UAISO_API inline bool isOprtr(Token tk)
{
    return (tk > BEGIN_ASCII && tk < END_ASCII)
            || (tk > BEGIN_MULTICHAR_OPERATOR && tk < END_MULTICHAR_OPERATOR);
}

UAISO_API inline bool isNumLit(Token tk)
{
    return tk > BEGIN_NUMERIC_LITERAL && tk < END_NUMERIC_LITERAL;
}

UAISO_API inline bool isStrLit(Token tk)
{
    return tk > BEGIN_STRING_LITERAL && tk < END_STRING_LITERAL;
}

UAISO_API inline bool isComment(Token tk)
{
    return tk > BEGIN_COMMENT && tk < END_COMMENT;
}

} // namespace uaiso

#endif
