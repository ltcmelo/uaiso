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

#include "Semantic/Builtins.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Common/Util__.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include <unordered_map>

using namespace uaiso;

namespace uaiso {

extern std::unordered_map<std::uint16_t, const char*> tokenName; // Generated.

} // namespace uaiso

Builtins::~Builtins()
{}

const char *Builtins::tokenSpell(Token tk) const
{
    return tokenName[tk];
}

std::vector<Builtins::FuncPtr> Builtins::valueConstructors(LexemeMap*) const
{
    return std::vector<FuncPtr>();
}

std::vector<std::string> Builtins::moduleNames() const
{
    return std::vector<std::string>();
}
