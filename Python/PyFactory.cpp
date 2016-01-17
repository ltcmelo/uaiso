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

#include "Python/PyFactory.h"
#include "Python/PyAstLocator.h"
#include "Python/PyIncrementalLexer.h"
#include "Python/PySanitizer.h"
#include "Python/PyLang.h"
#include "Python/PyTypeSystem.h"
#include "Python/PyUnit.h"
#include "Python/PyBuiltin.h"

using namespace uaiso;

std::unique_ptr<Unit> PyFactory::makeUnit()
{
    return std::unique_ptr<Unit>(new PyUnit);
}

LangId PyFactory::langName() const
{
    return LangId::Py;
}

std::unique_ptr<AstLocator> PyFactory::makeAstLocator()
{
    return std::unique_ptr<AstLocator>(new PyAstLocator);
}

std::unique_ptr<Builtin> PyFactory::makeBuiltin()
{
    return std::unique_ptr<Builtin>(new PyBuiltin);
}

std::unique_ptr<IncrementalLexer> PyFactory::makeIncrementalLexer()
{
    return std::unique_ptr<IncrementalLexer>(new PyIncrementalLexer);
}

std::unique_ptr<Sanitizer> PyFactory::makeSanitizer()
{
    return std::unique_ptr<Sanitizer>(new PySanitizer);
}

std::unique_ptr<TypeSystem> PyFactory::makeTypeSystem()
{
    return std::unique_ptr<TypeSystem>(new PyTypeSystem);
}

std::unique_ptr<Lang> PyFactory::makeLang()
{
    return std::unique_ptr<Lang>(new PyLang);
}
