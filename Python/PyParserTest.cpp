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
             , &PyParserTest::testCase71
             , &PyParserTest::testCase72
             , &PyParserTest::testCase73
             , &PyParserTest::testCase74
             , &PyParserTest::testCase75
             , &PyParserTest::testCase76
             , &PyParserTest::testCase77
             , &PyParserTest::testCase78
             , &PyParserTest::testCase79
             , &PyParserTest::testCase80
             , &PyParserTest::testCase81
             , &PyParserTest::testCase82
             , &PyParserTest::testCase83
             , &PyParserTest::testCase84
             , &PyParserTest::testCase85
             , &PyParserTest::testCase86
             , &PyParserTest::testCase87
             , &PyParserTest::testCase88
             , &PyParserTest::testCase89
             , &PyParserTest::testCase90
             , &PyParserTest::testCase91
             , &PyParserTest::testCase92
             , &PyParserTest::testCase93
             , &PyParserTest::testCase94
             , &PyParserTest::testCase95
             , &PyParserTest::testCase96
             , &PyParserTest::testCase97
             , &PyParserTest::testCase98
             , &PyParserTest::testCase99
             , &PyParserTest::testCase100
             , &PyParserTest::testCase101
             , &PyParserTest::testCase102
             , &PyParserTest::testCase103
             , &PyParserTest::testCase104
             , &PyParserTest::testCase105
             , &PyParserTest::testCase106
             , &PyParserTest::testCase107
             , &PyParserTest::testCase108
             , &PyParserTest::testCase109
             , &PyParserTest::testCase110
             , &PyParserTest::testCase111
             , &PyParserTest::testCase112
             , &PyParserTest::testCase113
             , &PyParserTest::testCase114
             , &PyParserTest::testCase115
             , &PyParserTest::testCase116
             , &PyParserTest::testCase117
             , &PyParserTest::testCase118
             , &PyParserTest::testCase119
             , &PyParserTest::testCase120
             , &PyParserTest::testCase121
             , &PyParserTest::testCase122
             , &PyParserTest::testCase123
             , &PyParserTest::testCase124
             , &PyParserTest::testCase125
             , &PyParserTest::testCase126
             , &PyParserTest::testCase127
             , &PyParserTest::testCase128
             , &PyParserTest::testCase129
             , &PyParserTest::testCase130
             , &PyParserTest::testCase131
             , &PyParserTest::testCase132
             , &PyParserTest::testCase133
             , &PyParserTest::testCase134
             , &PyParserTest::testCase135
             , &PyParserTest::testCase136
             , &PyParserTest::testCase137
             , &PyParserTest::testCase138
             , &PyParserTest::testCase139
             , &PyParserTest::testcase140
             , &PyParserTest::testcase141
             , &PyParserTest::testcase142
             , &PyParserTest::testcase143
             , &PyParserTest::testcase144
             , &PyParserTest::testcase145
             , &PyParserTest::testcase146
             , &PyParserTest::testcase147
             , &PyParserTest::testcase148
             , &PyParserTest::testcase149
             , &PyParserTest::testcase150
             , &PyParserTest::testcase151
             , &PyParserTest::testcase152
             , &PyParserTest::testcase153
             , &PyParserTest::testcase154
             , &PyParserTest::testcase155
             , &PyParserTest::testcase156
             , &PyParserTest::testcase157
             , &PyParserTest::testcase158
             , &PyParserTest::testcase159

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
    void testCase71();
    void testCase72();
    void testCase73();
    void testCase74();
    void testCase75();
    void testCase76();
    void testCase77();
    void testCase78();
    void testCase79();
    void testCase80();
    void testCase81();
    void testCase82();
    void testCase83();
    void testCase84();
    void testCase85();
    void testCase86();
    void testCase87();
    void testCase88();
    void testCase89();
    void testCase90();
    void testCase91();
    void testCase92();
    void testCase93();
    void testCase94();
    void testCase95();
    void testCase96();
    void testCase97();
    void testCase98();
    void testCase99();
    void testCase100();
    void testCase101();
    void testCase102();
    void testCase103();
    void testCase104();
    void testCase105();
    void testCase106();
    void testCase107();
    void testCase108();
    void testCase109();
    void testCase110();
    void testCase111();
    void testCase112();
    void testCase113();
    void testCase114();
    void testCase115();
    void testCase116();
    void testCase117();
    void testCase118();
    void testCase119();
    void testCase120();
    void testCase121();
    void testCase122();
    void testCase123();
    void testCase124();
    void testCase125();
    void testCase126();
    void testCase127();
    void testCase128();
    void testCase129();
    void testCase130();
    void testCase131();
    void testCase132();
    void testCase133();
    void testCase134();
    void testCase135();
    void testCase136();
    void testCase137();
    void testCase138();
    void testCase139();
    void testcase140();
    void testcase141();
    void testcase142();
    void testcase143();
    void testcase144();
    void testcase145();
    void testcase146();
    void testcase147();
    void testcase148();
    void testcase149();
    void testcase150();
    void testcase151();
    void testcase152();
    void testcase153();
    void testcase154();
    void testcase155();
    void testcase156();
    void testcase157();
    void testcase158();
    void testcase159();

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

void PyParser::PyParserTest::testCase71()
{
    core("()\n"); // Empty tuple
}

void PyParser::PyParserTest::testCase72()
{
    core("(1)\n"); // Not a tuple, a wrapped expr.
}

void PyParser::PyParserTest::testCase73()
{
    core("(1,)\n"); // A tuple
}

void PyParser::PyParserTest::testCase74()
{
    core("(1, 2)\n");
}

void PyParser::PyParserTest::testCase75()
{
    core("(1, 2, 3)\n");
}

void PyParser::PyParserTest::testCase76()
{
    core(R"raw(
try:
    print 1
)raw", true);
}

void PyParser::PyParserTest::testCase77()
{
    core(R"raw(
try:
    print 1
except:
    print 2
)raw");
}

void PyParser::PyParserTest::testCase78()
{
    core(R"raw(
try:
    print 1
except A:
    print 2
)raw");
}

void PyParser::PyParserTest::testCase79()
{
    core(R"raw(
try:
    print 1
except A as a:
    print 2
)raw");
}

void PyParser::PyParserTest::testCase80()
{
    core(R"raw(
try:
    print 1
finally A:
    print 2
)raw", true);
}

void PyParser::PyParserTest::testCase81()
{
    core(R"raw(
try:
    print 1
finally:
    print 2
)raw");
}

void PyParser::PyParserTest::testCase82()
{
    core(R"raw(
try:
    print 1
except A, a:
    print 2
finally:
    print 3
)raw");
}

void PyParser::PyParserTest::testCase83()
{
    core(R"raw(
try:
    print 1
except A, a:
    print 2
except B, b:
    print 3
finally:
    print 4
)raw");
}

void PyParser::PyParserTest::testCase84()
{
    core(R"raw(
try:
    print 1
except A, a:
    print 2
except B, b, nope:
    print 3
finally:
    print 4
)raw", true);
}

void PyParser::PyParserTest::testCase85()
{
    core(R"raw(
try:
    print 1
except A, a:
    print 2
except B, b:
    print 3
    print 4
    if (c):
         print 5
finally:
    print 6
)raw");
}

void PyParser::PyParserTest::testCase86()
{
    core(R"raw(
try:
    print 1
else:
    print 2
)raw", true);
}

void PyParser::PyParserTest::testCase87()
{
    core(R"raw(
try:
    print 1
except:
    print 2
else:
    print 3
)raw");
}

void PyParser::PyParserTest::testCase88()
{
    core(R"raw(
try:
    print 1
except:
    print 2
else:
    print 3
finally:
    print 4
)raw");
}

void PyParser::PyParserTest::testCase89()
{
    core(R"raw(
try:
    print 1
except:
    print 2
else:
    print 3
else:
    print 4
finally:
    print 5
)raw", true);
}

void PyParser::PyParserTest::testCase90()
{
    core(R"raw(
with A() as a:
    suite
)raw");
}

void PyParser::PyParserTest::testCase91()
{
    core(R"raw(
with A() as a, B() as b:
    suite
)raw");
}

void PyParser::PyParserTest::testCase92()
{
    core(R"raw(
with A():
    suite
)raw");
}

void PyParser::PyParserTest::testCase93()
{
    core(R"raw(
with:
    suite
)raw", true);
}

void PyParser::PyParserTest::testCase94()
{
    core("import a\n");
}

void PyParser::PyParserTest::testCase95()
{
    core("import a as la\n");
}

void PyParser::PyParserTest::testCase96()
{
    core("import a, b\n");
}

void PyParser::PyParserTest::testCase97()
{
    core("import a as la, b\n");
}

void PyParser::PyParserTest::testCase98()
{
    core("import a as la, b as lb\n");
}

void PyParser::PyParserTest::testCase99()
{
    core("import a, b as lb\n");
}

void PyParser::PyParserTest::testCase100()
{
    core("import a.b.c\n");
}

void PyParser::PyParserTest::testCase101()
{
    core("import a.b.c as abc\n");
}

void PyParser::PyParserTest::testCase102()
{
    core("from .moduleY import spam\n");
}

void PyParser::PyParserTest::testCase103()
{
    core("from .moduleY import spam as ham\n");
}

void PyParser::PyParserTest::testCase104()
{
    core("from . import moduleY\n");
}

void PyParser::PyParserTest::testCase105()
{
    core("from ..subpackage1 import moduleY\n");
}

void PyParser::PyParserTest::testCase106()
{
    core("from ..subpackage2.moduleZ import eggs\n");
}

void PyParser::PyParserTest::testCase107()
{
    core("from ..moduleA import foo\n");
}

void PyParser::PyParserTest::testCase108()
{
    core("from ...package import bar\n");
}

void PyParser::PyParserTest::testCase109()
{
    core("from ...sys import path\n");
}

void PyParser::PyParserTest::testCase110()
{
    core("from a import c\n");
}

void PyParser::PyParserTest::testCase111()
{
    core("from os import path as p\n");
}

void PyParser::PyParserTest::testCase112()
{
    core("from os import path as p, path2 as p2\n");
}

void PyParser::PyParserTest::testCase113()
{
    core("import 1\n", true);
}

void PyParser::PyParserTest::testCase114()
{
    core("import a b\n", true);
}

void PyParser::PyParserTest::testCase115()
{
    core("from import\n", true);
}

void PyParser::PyParserTest::testCase116()
{
    core("from a b import c\n", true);
}

void PyParser::PyParserTest::testCase117()
{
    core("from a import c d\n", true);
}

void PyParser::PyParserTest::testCase118()
{
    core("from a import *\n");
}

void PyParser::PyParserTest::testCase119()
{
    core(R"raw(
class A:
    i = 1
)raw");
}

void PyParser::PyParserTest::testCase120()
{
    core(R"raw(
class A(B, C):
    i = 1
    print 'class'
)raw");
}

void PyParser::PyParserTest::testCase121()
{
    core(R"raw(
class A(B C):
    i = 1
    print 'class'
)raw", true);
}

void PyParser::PyParserTest::testCase122()
{
    core(R"raw(
class :
    i = 1
    print 'class'
)raw", true);
}

void PyParser::PyParserTest::testCase123()
{
    core("def f(): pass\n");
}

void PyParser::PyParserTest::testCase124()
{
    core(R"raw(
def f():
    pass
)raw");
}

void PyParser::PyParserTest::testCase125()
{
    core(R"raw(
def f(a):
    pass
)raw");
}

void PyParser::PyParserTest::testCase126()
{
    core(R"raw(
def f(a=1):
    pass
)raw");
}

void PyParser::PyParserTest::testCase127()
{
    core(R"raw(
def f(a, b, c):
    pass
)raw");
}

void PyParser::PyParserTest::testCase128()
{
    core(R"raw(
def f(a, *b):
    pass
)raw");
}

void PyParser::PyParserTest::testCase129()
{
    core(R"raw(
def f(a, **b):
    pass
)raw");
}

void PyParser::PyParserTest::testCase130()
{
    core(R"raw(
def f(a, *b, **c):
    pass
)raw");
}

void PyParser::PyParserTest::testCase131()
{
    core(R"raw(
def f(a,):
    pass
)raw");
}

void PyParser::PyParserTest::testCase132()
{
    core(R"raw(
def f(a, b,):
    pass
)raw");
}

void PyParser::PyParserTest::testCase133()
{
    core(R"raw(
def f(*a):
    pass
)raw");
}

void PyParser::PyParserTest::testCase134()
{
    core(R"raw(
def f(*a, **b):
    pass
)raw");
}

void PyParser::PyParserTest::testCase135()
{
    core(R"raw(
def f(**a):
    pass
)raw");
}

void PyParser::PyParserTest::testCase136()
{
    core(R"raw(
def f(*a, b):
    pass
)raw", true);
}

void PyParser::PyParserTest::testCase137()
{
    core(R"raw(
def f(a b):
    pass
)raw", true);
}

void PyParser::PyParserTest::testCase138()
{
    core(R"raw(
def f(a, *b, c):
    pass
)raw", true);
}

void PyParser::PyParserTest::testCase139()
{
    core(R"raw(
@decor
def f(a):
    pass
)raw");
}

void PyParser::PyParserTest::testcase140()
{
    core(R"raw(
@decor
@other
def f(a):
    pass
)raw");
}

void PyParser::PyParserTest::testcase141()
{
    core(R"raw(
@decor("p")
def f(a):
    pass
)raw");
}

void PyParser::PyParserTest::testcase142()
{
    core(R"raw(
@decor def f(a):
    pass
)raw", true);
}

void PyParser::PyParserTest::testcase143()
{
    core(R"raw(
@decor x
def f(a):
    pass
)raw", true);
}

void PyParser::PyParserTest::testcase144()
{
    core("lambda x: x**2\n");
}

void PyParser::PyParserTest::testcase145()
{
    core("lambda x, y: x + y\n");
}

void PyParser::PyParserTest::testcase146()
{
    core("lambda: self.doit()\n");
}

void PyParser::PyParserTest::testcase147()
{
    core("lambda a\n", true);
}

void PyParser::PyParserTest::testcase148()
{
    core("lambda: a b\n", true);
}

void PyParser::PyParserTest::testcase149()
{
    core("lamb x: x\n", true);
}

void PyParser::PyParserTest::testcase150()
{
    core(R"raw(
a = ("abc"
     "def"
     "ghi")
)raw");
}

void PyParser::PyParserTest::testcase151()
{
}

void PyParser::PyParserTest::testcase152()
{
}

void PyParser::PyParserTest::testcase153()
{
}

void PyParser::PyParserTest::testcase154()
{
}

void PyParser::PyParserTest::testcase155()
{
}

void PyParser::PyParserTest::testcase156()
{
}

void PyParser::PyParserTest::testcase157()
{
}

void PyParser::PyParserTest::testcase158()
{
}

void PyParser::PyParserTest::testcase159()
{
}
