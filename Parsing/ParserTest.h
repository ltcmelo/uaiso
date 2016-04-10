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

#include "Ast/AstDumper.h"
#include "Ast/AstSerializer.h"
#include "Common/Test.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexer.h"
#include "Parsing/Parser.h"
#include "Parsing/ParsingContext.h"
#include <iostream>
#include <sstream>

namespace uaiso {

class UAISO_API ParserTest : public Test
{
protected:
    ParserTest(LangId langId)
        : dumpAst_(false)
        , langId_(langId)
    {}

    void core(const std::string& code, bool expectError = false)
    {
        DiagnosticReports reports;
        ParsingContext context;
        context.setFileName("/test.py");
        context.collectReports(&reports);

        lexer_->setContext(&context);
        lexer_->setBuffer(code.c_str(), code.length());
        bool ok = parser_->parse(lexer_.get(), &context);
        UAISO_EXPECT_TRUE(ok);

        if (expectError)
            UAISO_EXPECT_TRUE(reports.size() > 0);
        else
            UAISO_EXPECT_INT_EQ(0, reports.size());

        if (dumpAst_) {
            std::ostringstream oss;
            oss << "AST dump\n";
            AstDumper().dumpProgram(Program_Cast(context.releaseAst()), oss);
            std::cout << oss.str();
        }
    }

    void reset() override
    {
        lexer_ = FactoryCreator::create(langId_)->makeLexer();
        parser_ = FactoryCreator::create(langId_)->makeParser();
        dumpAst_ = false;
    }

    bool dumpAst_;
    LangId langId_;
    std::unique_ptr<Lexer> lexer_;
    std::unique_ptr<Parser> parser_;
};

} // namespace uaiso
