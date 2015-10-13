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

#include "Python/PyParser.h"
#include "Python/PyLexer.h"
#include "Python/PySyntax.h"
#include "Ast/AstDumper.h"
#include "Ast/AstSerializer.h"
#include "Parsing/ParsingContext.h"
#include <iostream>
#include <sstream>

using namespace uaiso;

class PyParser::PyParserTest : public Test
{
public:
    TEST_RUN(PyParserTest
             , &PyParserTest::testCase1
             , &PyParserTest::testCase2
             , &PyParserTest::testCase3
             , &PyParserTest::testCase4
             , &PyParserTest::testCase5
             , &PyParserTest::testCase6
             , &PyParserTest::testCase7
             , &PyParserTest::testCase8
             , &PyParserTest::testCase9
             , &PyParserTest::testCase10
             , &PyParserTest::testCase11
             , &PyParserTest::testCase12
             , &PyParserTest::testCase13
             , &PyParserTest::testCase14
             , &PyParserTest::testCase15
             , &PyParserTest::testCase16
             , &PyParserTest::testCase17
             , &PyParserTest::testCase18
             , &PyParserTest::testCase19
             , &PyParserTest::testCase20
            )

    void testCase1();
    void testCase2();
    void testCase3();
    void testCase4();
    void testCase5();
    void testCase6();
    void testCase7();
    void testCase8();
    void testCase9();
    void testCase10();
    void testCase11();
    void testCase12();
    void testCase13();
    void testCase14();
    void testCase15();
    void testCase16();
    void testCase17();
    void testCase18();
    void testCase19();
    void testCase20();

    void core(const std::string& code, bool expectError = false)
    {
        DiagnosticReports reports;
        ParsingContext context;
        context.setFileName("/test.py");
        context.collectReports(&reports);
        PyLexer lexer;
        lexer.setBuffer(code.c_str(), code.length());
        PyParser parser;
        bool ok = parser.parse(&lexer, &context);
        UAISO_EXPECT_TRUE(ok);

        if (!expectError)
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
        dumpAst_ = false;
    }

    bool dumpAst_ { false };

};

MAKE_CLASS_TEST(PyParser)

void PyParser::PyParserTest::testCase1()
{
    core("print foo\n");
}

void PyParser::PyParserTest::testCase2()
{
    core("print 1\n");
}

void PyParser::PyParserTest::testCase3()
{
    core("print >> f\n");
}

void PyParser::PyParserTest::testCase4()
{
    core("print >> f,\n", true);
}

void PyParser::PyParserTest::testCase5()
{
    core("print f, 'foo'\n");
}

void PyParser::PyParserTest::testCase6()
{
    core("print f(a)\n");
}

void PyParser::PyParserTest::testCase7()
{
    core("print f(a), f(b)\n");
}

void PyParser::PyParserTest::testCase8()
{
    core("print f(a) == a or a == 1\n");
}

void PyParser::PyParserTest::testCase9()
{
    core("print f(a * a ** a)\n");
}

void PyParser::PyParserTest::testCase10()
{
    core("print f(a, b, c)\n");
}

void PyParser::PyParserTest::testCase11()
{
    core("print f(a, b, c,)\n");
}

void PyParser::PyParserTest::testCase12()
{
    core("print f(a, b, c, *d)\n");
}

void PyParser::PyParserTest::testCase13()
{
    core("print f(a, b, c, *d, **e)\n");
}

void PyParser::PyParserTest::testCase14()
{
    core("print f(*a)\n");
}

void PyParser::PyParserTest::testCase15()
{
    core("print f(*a, **b)\n");
}

void PyParser::PyParserTest::testCase16()
{
    core("print f(**a)\n");
}

void PyParser::PyParserTest::testCase17()
{
    core("print f(x**2 for x in range(10))\n");
}

void PyParser::PyParserTest::testCase18()
{
    core("print f(x for x in Y if x % 2 == 0)\n");
}

void PyParser::PyParserTest::testCase19()
{
    core("print f(x for x in Y if x % 2 == 0\n", true);
}

void PyParser::PyParserTest::testCase20()
{
    core("print f(x for x in Y if == 0)\n", true);
}
