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

#ifndef UAISO_FACTORY_H__
#define UAISO_FACTORY_H__

#include "Common/Config.h"
#include "Parsing/Language.h"
#include <memory>
#include <string>

namespace uaiso {

class AstLocator;
class IncrementalLexer;
class Sanitizer;
class Syntax;
class TypeSystem;
class Unit;

class UAISO_API Factory
{
public:
    virtual ~Factory();

    virtual std::unique_ptr<AstLocator> makeAstLocator() = 0;

    virtual std::unique_ptr<IncrementalLexer> makeIncrementalLexer() = 0;

    virtual std::unique_ptr<Sanitizer> makeSanitizer() = 0;

    virtual std::unique_ptr<TypeSystem> makeTypeSystem() = 0;

    virtual std::unique_ptr<Syntax> makeSyntax() = 0;

    virtual std::unique_ptr<Unit> makeUnit() = 0;
};

class UAISO_API FactoryCreator final
{
public:
    static std::unique_ptr<Factory> create(LangName langName);
};

} // namespace uaiso

#endif
