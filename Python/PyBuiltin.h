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

#ifndef UAISO_PYBUILTIN_H__
#define UAISO_PYBUILTIN_H__

#include "Semantic/Builtin.h"

namespace uaiso {

class UAISO_API PyBuiltin final : public Builtin
{
public:
    const char* tokenSpell(Token tk) const override;

    std::vector<FuncPtr> valueConstructors(LexemeMap* lexemes) const override;

    std::vector<FuncPtr> globalFuncs(LexemeMap* lexemes) const override;

    std::vector<TypeDeclPtr> typeDecls(LexemeMap* lexemes) const override;

    BaseRecordPtr implicitBase(LexemeMap* lexemes) const override;

    std::vector<std::string> automaticModules() const override;
};

} // namespace uaiso

#endif
