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

#include "Semantic/BinderTest.h"
#include "Semantic/Program.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Factory.h"
#include "Parsing/Unit.h"

using namespace uaiso;

std::unique_ptr<Program> Binder::BinderTest::core(std::unique_ptr<Factory> factory,
                                                  const std::string& code,
                                                  const std::string& fullFileName)
{
    TokenMap tokens;
    std::unique_ptr<Unit> unit(factory->makeUnit());
    unit->assignInput(code);
    unit->setFileName(fullFileName);
    unit->parse(&tokens, &lexs_);
    ProgramAst* ast = Program_Cast(unit->ast());
    UAISO_EXPECT_TRUE(ast);

    Binder binder(factory.get());
    binder.setLexemes(&lexs_);
    binder.setTokens(&tokens);
    return binder.bind(ast, unit->fileName());
}


MAKE_CLASS_TEST(Binder)
