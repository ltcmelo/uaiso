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

#ifndef UAISO_TYPERESOLVER_H__
#define UAISO_TYPERESOLVER_H__

#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Semantic/Environment.h"
#include "Semantic/TypeFwd.h"
#include <tuple>

namespace uaiso {

class Factory;

class UAISO_API TypeResolver final
{
public:
    TypeResolver(Factory* factory);
    ~TypeResolver();

    enum ResultCode : char
    {
        InternalError,
        TypeDeclLookupFailed,
        Success
    };

    using Result = std::tuple<const Type*, ResultCode>;

    Result resolve(ElaborateType* elabTy, Environment env) const;

private:
    DECL_PIMPL(TypeResolver)
};

} // namespace uaiso

#endif
