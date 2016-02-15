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

#ifndef UAISO_PYLANG_H__
#define UAISO_PYLANG_H__

#include "Parsing/Lang.h"

namespace uaiso {

class UAISO_API PyLang final : public Lang
{
public:
    PyLang();

    bool hasBlockLevelScope() const override;

    bool hasExecutableRecord() const override;

    bool hasNewlineAsTerminator() const override;

    bool requiresReturnTypeInference() const override;

    Structure structure() const override;

    ImportMechanism importMechanism() const override;

    bool isPurelyOO() const override;

    std::string sourceFileSuffix() const override;

    bool isStrLitQuote(char ch) const override;
};

} // namespace uaiso

#endif
