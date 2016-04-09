/******************************************************************************
 * Copyright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)
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

#include "Haskell/HsFactory.h"
#include "Haskell/HsLang.h"
#include "Haskell/HsLexer.h"
#include "Haskell/HsParser.h"
#include "Ast/AstLocator.h"
#include "Parsing/Unit.h"
#include "Parsing/IncrementalLexer.h"
#include "Semantic/Sanitizer.h"
#include "Semantic/Builtin.h"
#include "Semantic/TypeSystem.h"

using namespace uaiso;

std::unique_ptr<Unit> HsFactory::makeUnit()
{
    return std::unique_ptr<Unit>();
}

LangId HsFactory::langName() const
{
    return LangId::Hs;
}

std::unique_ptr<AstLocator> HsFactory::makeAstLocator()
{
    return std::unique_ptr<AstLocator>();
}

std::unique_ptr<Builtin> HsFactory::makeBuiltin()
{
    return std::unique_ptr<Builtin>();
}

std::unique_ptr<IncrementalLexer> HsFactory::makeIncrementalLexer()
{
    return std::unique_ptr<IncrementalLexer>();
}

std::unique_ptr<Sanitizer> HsFactory::makeSanitizer()
{
    return std::unique_ptr<Sanitizer>();
}

std::unique_ptr<TypeSystem> HsFactory::makeTypeSystem()
{
    return std::unique_ptr<TypeSystem>();
}

std::unique_ptr<Lang> HsFactory::makeLang()
{
    return std::unique_ptr<Lang>(new HsLang);
}

std::unique_ptr<Lexer> HsFactory::makeLexer()
{
    return std::unique_ptr<Lexer>(new HsLexer);
}

std::unique_ptr<Parser> HsFactory::makeParser()
{
    return std::unique_ptr<Parser>(new HsParser);
}
