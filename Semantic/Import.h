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

#ifndef UAISO_IMPORT_H__
#define UAISO_IMPORT_H__

#include "Common/Config.h"
#include "Common/Pimpl.h"
#include <string>
#include <vector>

namespace uaiso {

class Ident;

/*!
 * \brief The Import class
 */
class UAISO_API Import final
{
public:
    /*!
     * \brief Import
     * \param fromWhere
     * \param targetEntity
     * \param localName
     * \param isEmbedded
     */
    Import(const std::string& fromWhere,
           const std::string& target,
           const Ident* localName,
           bool isEmbedded);

    ~Import();

    const std::string& fromWhere() const;

    const std::string& target() const;

    enum TargetEntity : char
    {
        Module,
        Package
    };

    TargetEntity targetEntity() const;

    const Ident* localName() const;

    bool isEmbedded() const;

    bool isSelective() const;

    void addSelectedItem(const Ident* actualName);
    void addSelectedItem(const Ident* actualName, const Ident* alternateName);

    const std::vector<const Ident*>& selectedItems() const;

    const Ident* alternateName(const Ident* actualName) const;

private:
    DECL_PIMPL(Import)

    friend class ImportResolver;

    void setTargetEntity(TargetEntity kind);
};

} // namespace uaiso

#endif
