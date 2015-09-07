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

#ifndef UAISO_PIMPL_H__
#define UAISO_PIMPL_H__

#include <memory>

#define DECL_PIMPL(CLASS) \
    struct CLASS##Impl; \
    std::unique_ptr<CLASS##Impl> impl_;

#define DECL_PIMPL_BASE(CLASS) \
    CLASS(CLASS##Impl*);

#define DECL_PIMPL_CAST(CLASS) \
    struct CLASS##Impl; \
    CLASS##Impl* implCast() const; \

#define DEF_PIMPL_CAST(CLASS) \
    CLASS::CLASS##Impl* CLASS::implCast() const \
    { \
        return static_cast<CLASS##Impl*>(impl_.get()); \
    }

#define DECL_SHARED_DATA(CLASS) \
    struct CLASS##Impl; \
    std::shared_ptr<CLASS##Impl> impl_;

#define P impl_
#define P_CAST implCast()

#endif
