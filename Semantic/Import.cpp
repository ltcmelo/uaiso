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

#include "Semantic/Import.h"
#include <unordered_map>

using namespace uaiso;

struct Import::ImportImpl
{
    ImportImpl(const std::string& fromWhere,
               const std::string& target,
               const Ident* localName,
               bool isEmbedded)
        : fromWhere_(fromWhere)
        , target_(target)
        , localName_(localName)
        , isEmbedded_(isEmbedded)
    {}

    const std::string fromWhere_;
    const std::string target_;
    const Ident* localName_;
    std::vector<const Ident*> items_;
    std::unordered_map<const Ident*, const Ident*> nicks_;
    bool isEmbedded_;
    TargetEntity entity_;
};

Import::Import(const std::string& fromWhere,
               const std::string& target,
               const Ident* localName,
               bool isEmbedded)
    : P(new ImportImpl(fromWhere, target, localName, isEmbedded))
{}

Import::~Import()
{}

const std::string& Import::target() const
{
    return P->target_;
}

const std::string& Import::fromWhere() const
{
    return P->fromWhere_;
}

const Ident* Import::localName() const
{
    return P->localName_;
}

bool Import::isEmbedded() const
{
    return P->isEmbedded_;
}

bool Import::isSelective() const
{
    return !P->items_.empty();
}

void Import::addSelectedItem(const Ident* actualName)
{
    P->items_.push_back(actualName);
}

void Import::addSelectedItem(const Ident* actualName, const Ident* alternateName)
{
    addSelectedItem(actualName);
    if (alternateName)
        P->nicks_.insert(std::make_pair(actualName, alternateName));
}

const std::vector<const Ident*>& Import::selectedItems() const
{
    return P->items_;
}

const Ident* Import::alternateName(const Ident* actualName) const
{
    auto it = P->nicks_.find(actualName);
    if (it != P->nicks_.end())
        return it->second;
    return nullptr;
}

Import::TargetEntity Import::targetEntity() const
{
    return P->entity_;
}

void Import::setTargetEntity(TargetEntity entity)
{
    P->entity_ = entity;
}
