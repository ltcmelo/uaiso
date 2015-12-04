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

#ifndef UAISO_TYPESYSTEM_H__
#define UAISO_TYPESYSTEM_H__

#include "Common/Config.h"
#include "Semantic/TypeFwd.h"

namespace uaiso {

class UAISO_API TypeSystem
{
public:
    virtual ~TypeSystem();

    /*!
     * \brief isStructural
     * \return
     *
     * Return whether this is a structural type system.
     */
    virtual bool isStructural() const;

    /*!
     * \brief isDynamic
     * \return
     *
     * Return whether this is a dynamic type system.
     */
    virtual bool isDynamic() const;

    /*!
     * \brief isArrayFirstClass
     * \return
     *
     * Return whether an array is a first-class type.
     */
    virtual bool isArrayFirstClass() const;

    /*!
     * \brief isStringFirstClass
     * \return
     *
     * Return whether an string is a first-class type.
     */
    virtual bool isStringFirstClass() const;

    /*!
     * \brief isBoolConvertible
     * \param ty
     * \return
     *
     * Not required to check the type bool itself.
     */
    virtual bool isBoolConvertible(const Type* ty) const;

    /*!
     * \brief isIntConvertible
     * \param ty
     * \return
     *
     * Not required to check int itself.
     */
    virtual bool isIntConvertible(const Type* ty) const;
};

} // namespace uaiso

#endif
