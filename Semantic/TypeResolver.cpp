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

#include "Semantic/TypeResolver.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Semantic/TypeCast.h"
#include "Common/Trace__.h"

#define TRACE_NAME "TypeResolver"

using namespace uaiso;

struct uaiso::TypeResolver::TypeResolverImpl
{
    TypeResolverImpl(Factory*)
    {}
};

TypeResolver::TypeResolver(Factory* factory)
    : P(new TypeResolverImpl(factory))
{}

TypeResolver::~TypeResolver()
{}

TypeResolver::Result TypeResolver::resolve(ElaborateType* elabTy,
                                           Environment env) const
{
    UAISO_ASSERT(elabTy, return Result(nullptr, InternalError));

    if (elabTy->isResolved())
        return Result(elabTy->canonicalType(), Success);

    auto tySym = env.lookUpType(elabTy->name());
    if (!tySym) {
        DEBUG_TRACE("type decl symbol lookup failed");
        return Result(nullptr, TypeSymbolLookupFailed);
    }

    UAISO_ASSERT(tySym->type(), return Result(nullptr, InternalError));
    auto ty = tySym->type();
    elabTy->resolveType(std::unique_ptr<Type>(ty->clone()));

    return Result(ty, Success);
}
