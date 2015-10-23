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
             , &PyParserTest::testCase21
             , &PyParserTest::testCase22
             , &PyParserTest::testCase23
             , &PyParserTest::testCase24
             , &PyParserTest::testCase25
             , &PyParserTest::testCase26
             , &PyParserTest::testCase27
             , &PyParserTest::testCase28
             , &PyParserTest::testCase29
             , &PyParserTest::testCase30
             , &PyParserTest::testCase31
             , &PyParserTest::testCase32
             , &PyParserTest::testCase33
             , &PyParserTest::testCase34
             , &PyParserTest::testCase35
             , &PyParserTest::testCase36
             , &PyParserTest::testCase37
             , &PyParserTest::testCase38
             , &PyParserTest::testCase39
             , &PyParserTest::testCase40
             , &PyParserTest::testCase41
             , &PyParserTest::testCase42
             , &PyParserTest::testCase43
             , &PyParserTest::testCase44
             , &PyParserTest::testCase45
             , &PyParserTest::testCase46
             , &PyParserTest::testCase47
             , &PyParserTest::testCase48
             , &PyParserTest::testCase49
             , &PyParserTest::testCase50
             , &PyParserTest::testCase51
             , &PyParserTest::testCase52
             , &PyParserTest::testCase53
             , &PyParserTest::testCase54
             , &PyParserTest::testCase55
             , &PyParserTest::testCase56
             , &PyParserTest::testCase57
             , &PyParserTest::testCase58
             , &PyParserTest::testCase59
             , &PyParserTest::testCase60
             , &PyParserTest::testCase61
             , &PyParserTest::testCase62
             , &PyParserTest::testCase63
             , &PyParserTest::testCase64
             , &PyParserTest::testCase65
             , &PyParserTest::testCase66
             , &PyParserTest::testCase67
             , &PyParserTest::testCase68
             , &PyParserTest::testCase69
             , &PyParserTest::testCase70
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
    void testCase21();
    void testCase22();
    void testCase23();
    void testCase24();
    void testCase25();
    void testCase26();
    void testCase27();
    void testCase28();
    void testCase29();
    void testCase30();
    void testCase31();
    void testCase32();
    void testCase33();
    void testCase34();
    void testCase35();
    void testCase36();
    void testCase37();
    void testCase38();
    void testCase39();
    void testCase40();
    void testCase41();
    void testCase42();
    void testCase43();
    void testCase44();
    void testCase45();
    void testCase46();
    void testCase47();
    void testCase48();
    void testCase49();
    void testCase50();
    void testCase51();
    void testCase52();
    void testCase53();
    void testCase54();
    void testCase55();
    void testCase56();
    void testCase57();
    void testCase58();
    void testCase59();
    void testCase60();
    void testCase61();
    void testCase62();
    void testCase63();
    void testCase64();
    void testCase65();
    void testCase66();
    void testCase67();
    void testCase68();
    void testCase69();
    void testCase70();

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

void PyParser::PyParserTest::testCase21()
{
    core("print x and y\n");
}

void PyParser::PyParserTest::testCase22()
{
    core("print x and ==\n", true);
}

void PyParser::PyParserTest::testCase23()
{
    core("print x and\n", true);
}

void PyParser::PyParserTest::testCase24()
{
    core("print 1 or y\n");
}

void PyParser::PyParserTest::testCase25()
{
    core("print x or ==\n", true);
}

void PyParser::PyParserTest::testCase26()
{
    core("print x or\n", true);
}

void PyParser::PyParserTest::testCase27()
{
    core("print x < y\n");
}

void PyParser::PyParserTest::testCase28()
{
    core("print x < ==\n", true);
}

void PyParser::PyParserTest::testCase29()
{
    core("print x <\n", true);
}

void PyParser::PyParserTest::testCase30()
{
    core("print 1 if a else 2\n");
}

void PyParser::PyParserTest::testCase31()
{
    core("print x.f()\n");
}

void PyParser::PyParserTest::testCase32()
{
    core("print x.f(]\n", true);
}

void PyParser::PyParserTest::testCase33()
{
    core("print x.f()[]\n", true);
}

void PyParser::PyParserTest::testCase34()
{
    core("print x.f()[:]\n");
}

void PyParser::PyParserTest::testCase35()
{
    core("print x.f()[1:]\n");
}

void PyParser::PyParserTest::testCase36()
{
    core("print x.f()[1:1]\n");
}

void PyParser::PyParserTest::testCase37()
{
    core("print x.f()[:1]\n");
}

void PyParser::PyParserTest::testCase38()
{
    core("print x.f()[1:a:]\n");
}

void PyParser::PyParserTest::testCase39()
{
    core("print x.f()[1:a:1]\n");
}

void PyParser::PyParserTest::testCase40()
{
    core("print x.f()[::]\n");
}

void PyParser::PyParserTest::testCase41()
{
    core("print x.f()[::1]\n");
}

void PyParser::PyParserTest::testCase42()
{
    core("print x.f()[::1 a]\n", true);
}

void PyParser::PyParserTest::testCase43()
{
    core("a\n");
}

void PyParser::PyParserTest::testCase44()
{
    core("a, b, c\n");
}

void PyParser::PyParserTest::testCase45()
{
    core("yield a\n");
}

void PyParser::PyParserTest::testCase46()
{
    core("yield a, b\n");
}

void PyParser::PyParserTest::testCase47()
{
    core("yield a b\n", true);
}

void PyParser::PyParserTest::testCase48()
{
    core("global a\n");
}

void PyParser::PyParserTest::testCase49()
{
    core("global a, b\n");
}

void PyParser::PyParserTest::testCase50()
{
    core("global a, 1\n", true);
}

void PyParser::PyParserTest::testCase51()
{
    core("if a: pass\n");
}

void PyParser::PyParserTest::testCase52()
{
    core(R"raw(

if a:
    pass

)raw");
}

void PyParser::PyParserTest::testCase53()
{
    core(R"raw(

if a:
    print 1
else:
    print 2

)raw");
}

void PyParser::PyParserTest::testCase54()
{
    core(R"raw(

if a:
    print 1
elif b:
    print 2
elif c:
    print 3
else:
    print 4

)raw");
}

void PyParser::PyParserTest::testCase55()
{
    core("if a pass\n", true);
}

void PyParser::PyParserTest::testCase56()
{
    core(R"raw(

while a:
    print 1

)raw");
}

void PyParser::PyParserTest::testCase57()
{
    core(R"raw(

while a:
    print 1
else:
    print 2

)raw");
}

void PyParser::PyParserTest::testCase58()
{
    core(R"raw(

while a
    print 1

)raw", true);
}

void PyParser::PyParserTest::testCase59()
{
    core(R"raw(

for x in range(1, 10):
    print 1

)raw");
}

void PyParser::PyParserTest::testCase60()
{
    core(R"raw(

for x, y in a, b:
    print 1

)raw");
}

void PyParser::PyParserTest::testCase61()
{
    core("a = []\n");
}

void PyParser::PyParserTest::testCase62()
{
    core("a = [1, 2]\n");
}

void PyParser::PyParserTest::testCase63()
{
    core("a = [x*2 for x in [1, 2]]\n");
}

void PyParser::PyParserTest::testCase64()
{
    core("a = {1 : 10}\n");
}

void PyParser::PyParserTest::testCase65()
{
    core("tel = {'jack': 4098, 'sape': 4139}\n");
}

void PyParser::PyParserTest::testCase66()
{
    core("{x: x**2 for x in (2, 4, 6)}\n");
}

void PyParser::PyParserTest::testCase67()
{
    core("{1, 2, 3}\n");
}

void PyParser::PyParserTest::testCase68()
{
    core("{1}\n"); // Singleton set.
}

void PyParser::PyParserTest::testCase69()
{
    core("{}\n");
}

void PyParser::PyParserTest::testCase70()
{
    core("[1]\n"); // Singleton list.
}
