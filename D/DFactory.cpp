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

#include "D/DFactory.h"
#include "D/DAstLocator.h"
#include "D/DIncrementalLexer.h"
#include "D/DSanitizer.h"
#include "D/DSyntax.h"
#include "D/DTypeSystem.h"
#include "D/DUnit.h"

using namespace uaiso;

std::unique_ptr<Unit> DFactory::makeUnit()
{
    return std::unique_ptr<Unit>(new DUnit);
}

std::unique_ptr<AstLocator> DFactory::makeAstLocator()
{
    return std::unique_ptr<AstLocator>(new DAstLocator);
}

std::unique_ptr<IncrementalLexer> DFactory::makeIncrementalLexer()
{
    return std::unique_ptr<IncrementalLexer>(new DIncrementalLexer);
}

std::unique_ptr<Sanitizer> DFactory::makeSanitizer()
{
    return std::unique_ptr<Sanitizer>(new DSanitizer);
}

std::unique_ptr<TypeSystem> DFactory::makeTypeSystem()
{
    return std::unique_ptr<TypeSystem>(new DTypeSystem);
}

std::unique_ptr<Syntax> DFactory::makeSyntax()
{
    return std::unique_ptr<Syntax>(new DSyntax);
}
