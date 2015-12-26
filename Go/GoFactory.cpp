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

#include "Go/GoFactory.h"
#include "Go/GoAstLocator.h"
#include "Go/GoIncrementalLexer.h"
#include "Go/GoSanitizer.h"
#include "Go/GoLang.h"
#include "Go/GoTypeSystem.h"
#include "Go/GoUnit.h"
#include "Semantic/Builtins.h"

using namespace uaiso;

std::unique_ptr<Unit> GoFactory::makeUnit()
{
    return std::unique_ptr<Unit>(new GoUnit);
}

std::unique_ptr<Builtins> GoFactory::makeBuiltins()
{
    return std::unique_ptr<Builtins>(new Builtins);
}

std::unique_ptr<AstLocator> GoFactory::makeAstLocator()
{
    return std::unique_ptr<AstLocator>(new GoAstLocator);
}

std::unique_ptr<IncrementalLexer> GoFactory::makeIncrementalLexer()
{
    return std::unique_ptr<IncrementalLexer>(new GoIncrementalLexer);
}

std::unique_ptr<Sanitizer> GoFactory::makeSanitizer()
{
    return std::unique_ptr<Sanitizer>(new GoSanitizer);
}

std::unique_ptr<TypeSystem> GoFactory::makeTypeSystem()
{
    return std::unique_ptr<TypeSystem>(new GoTypeSystem);
}

std::unique_ptr<Lang> GoFactory::makeLang()
{
    return std::unique_ptr<Lang>(new GoLang);
}
