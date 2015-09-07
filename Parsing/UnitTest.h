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

#ifndef UAISO_UNITTEST_H__
#define UAISO_UNITTEST_H__

#include "Parsing/Unit.h"
#include "Ast/Ast.h"
#include "Ast/AstDumper.h"
#include "Ast/AstSerializer.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/Token.h"
#include "Parsing/TokenMap.h"
#include <sstream>
#include <utility>

namespace uaiso {

class Unit::UnitTest : public Test
{
protected:
    std::unique_ptr<Unit> runCore(std::unique_ptr<Factory> factory,
                                  const std::string& code)
    {
        LexemeMap lexemes;
        TokenMap tokens;
        std::unique_ptr<Unit> unit(factory->makeUnit());
        unit->assignInput(code);
        if (lineCol_.line_ != -1 && lineCol_.col_ != -1)
            unit->parse(&tokens, &lexemes, lineCol_);
        else
            unit->parse(&tokens, &lexemes);
        ProgramAst* ast = Program_Cast(unit->ast());
        UAISO_EXPECT_TRUE(ast);

        if (dumpAst_) {
            std::ostringstream dumpOut;
            AstDumper().dumpProgram(ast, dumpOut);
            std::cout << dumpOut.str();
        }

        std::ostringstream serialOut;
        if (dumpSerialAst_) {
            AstSerializer().serializeProgram(ast, serialOut);
            std::cout << serialOut.str();
        }

        if (!expectedAst_.empty()) {
            if (serialOut.str().empty())
                AstSerializer().serializeProgram(ast, serialOut);
            UAISO_EXPECT_STR_EQ(expectedAst_, serialOut.str());
        }

        return std::move(unit);
    }

    void reset() override
    {
        lineCol_ = { -1, -1 };
        expectedAst_.clear();
        dumpSerialAst_ = false;
        dumpAst_ = false;
    }

    LineCol lineCol_;
    std::string expectedAst_;
    bool dumpSerialAst_ { false } ;
    bool dumpAst_ { false };
};

} // namespace uaiso

#endif
