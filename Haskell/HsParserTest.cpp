/******************************************************************************
 * Copyright (c) 2014-201 Leandro T. C. Melo (ltcmelo@gmail.com)
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

#include "Haskell/HsParser.h"
#include "Parsing/LangId.h"
#include "Parsing/ParserTest.h"

using namespace uaiso;

class HsParser::HsParserTest final : public ParserTest
{
public:
    HsParserTest() : ParserTest(LangId::Hs)
    {}

    TEST_RUN(HsParserTest
             , &HsParserTest::testCase1
             , &HsParserTest::testCase2
             , &HsParserTest::testCase3
             , &HsParserTest::testCase4
             , &HsParserTest::testCase5
             , &HsParserTest::testCase6
             , &HsParserTest::testCase7
             , &HsParserTest::testCase8
             , &HsParserTest::testCase9
             , &HsParserTest::testCase10
             , &HsParserTest::testCase11
             , &HsParserTest::testCase12
             , &HsParserTest::testCase13
             , &HsParserTest::testCase14
             , &HsParserTest::testCase15
             , &HsParserTest::testCase16
             , &HsParserTest::testCase17
             , &HsParserTest::testCase18
             , &HsParserTest::testCase19
             , &HsParserTest::testCase20
             , &HsParserTest::testCase21
             , &HsParserTest::testCase22
             , &HsParserTest::testCase23
             , &HsParserTest::testCase24
             , &HsParserTest::testCase25
             , &HsParserTest::testCase26
             , &HsParserTest::testCase27
             , &HsParserTest::testCase28
             , &HsParserTest::testCase29
             , &HsParserTest::testCase30
             , &HsParserTest::testCase31
             , &HsParserTest::testCase33
             , &HsParserTest::testCase32
             , &HsParserTest::testCase34
             , &HsParserTest::testCase35
             , &HsParserTest::testCase36
             , &HsParserTest::testCase37
             , &HsParserTest::testCase38
             , &HsParserTest::testCase39
             , &HsParserTest::testCase40
             , &HsParserTest::testCase41
             , &HsParserTest::testCase42
             , &HsParserTest::testCase43
             , &HsParserTest::testCase44
             , &HsParserTest::testCase45
             , &HsParserTest::testCase46
             , &HsParserTest::testCase47
             , &HsParserTest::testCase48
             , &HsParserTest::testCase49
             , &HsParserTest::testCase50
             , &HsParserTest::testCase51
             , &HsParserTest::testCase52
             , &HsParserTest::testCase53
             , &HsParserTest::testCase54
             , &HsParserTest::testCase55
             , &HsParserTest::testCase56
             , &HsParserTest::testCase57
             , &HsParserTest::testCase58
             , &HsParserTest::testCase59
             , &HsParserTest::testCase60
             , &HsParserTest::testCase61
             , &HsParserTest::testCase62
             , &HsParserTest::testCase63
             , &HsParserTest::testCase64
             , &HsParserTest::testCase65
             , &HsParserTest::testCase66
             , &HsParserTest::testCase67
             , &HsParserTest::testCase68
             , &HsParserTest::testCase69
             , &HsParserTest::testCase70
             , &HsParserTest::testCase71
             , &HsParserTest::testCase72
             , &HsParserTest::testCase73
             , &HsParserTest::testCase74
             , &HsParserTest::testCase75
             , &HsParserTest::testCase76
             , &HsParserTest::testCase77
             , &HsParserTest::testCase78
             , &HsParserTest::testCase79
             , &HsParserTest::testCase80
             , &HsParserTest::testCase81
             , &HsParserTest::testCase82
             , &HsParserTest::testCase83
             , &HsParserTest::testCase84
             , &HsParserTest::testCase85
             , &HsParserTest::testCase86
             , &HsParserTest::testCase87
             , &HsParserTest::testCase88
             , &HsParserTest::testCase89
             , &HsParserTest::testCase90
             , &HsParserTest::testCase91
             , &HsParserTest::testCase92
             , &HsParserTest::testCase93
             , &HsParserTest::testCase94
             , &HsParserTest::testCase95
             , &HsParserTest::testCase96
             , &HsParserTest::testCase97
             , &HsParserTest::testCase98
             , &HsParserTest::testCase99
             , &HsParserTest::testCase100
             , &HsParserTest::testCase101
             , &HsParserTest::testCase102
             , &HsParserTest::testCase103
             , &HsParserTest::testCase104
             , &HsParserTest::testCase105
             , &HsParserTest::testCase106
             , &HsParserTest::testCase107
             , &HsParserTest::testCase108
             , &HsParserTest::testCase109
             , &HsParserTest::testCase110
             , &HsParserTest::testCase111
             , &HsParserTest::testCase112
             , &HsParserTest::testCase113
             , &HsParserTest::testCase114
             , &HsParserTest::testCase115
             , &HsParserTest::testCase116
             , &HsParserTest::testCase117
             , &HsParserTest::testCase118
             , &HsParserTest::testCase119
             , &HsParserTest::testCase120
             , &HsParserTest::testCase121
             , &HsParserTest::testCase122
             , &HsParserTest::testCase123
             , &HsParserTest::testCase124
             , &HsParserTest::testCase125
             , &HsParserTest::testCase126
             , &HsParserTest::testCase127
             , &HsParserTest::testCase128
             , &HsParserTest::testCase129
             , &HsParserTest::testCase130
             , &HsParserTest::testCase131
             , &HsParserTest::testCase133
             , &HsParserTest::testCase132
             , &HsParserTest::testCase134
             , &HsParserTest::testCase135
             , &HsParserTest::testCase136
             , &HsParserTest::testCase137
             , &HsParserTest::testCase138
             , &HsParserTest::testCase139
             , &HsParserTest::testCase140
             , &HsParserTest::testCase141
             , &HsParserTest::testCase143
             , &HsParserTest::testCase142
             , &HsParserTest::testCase144
             , &HsParserTest::testCase145
             , &HsParserTest::testCase146
             , &HsParserTest::testCase147
             , &HsParserTest::testCase148
             , &HsParserTest::testCase149
             , &HsParserTest::testCase150
             , &HsParserTest::testCase151
             , &HsParserTest::testCase153
             , &HsParserTest::testCase152
             , &HsParserTest::testCase154
             , &HsParserTest::testCase155
             , &HsParserTest::testCase156
             , &HsParserTest::testCase157
             , &HsParserTest::testCase158
             , &HsParserTest::testCase159
             , &HsParserTest::testCase160
             , &HsParserTest::testCase161
             , &HsParserTest::testCase162
             , &HsParserTest::testCase163
             , &HsParserTest::testCase164
             , &HsParserTest::testCase165
             , &HsParserTest::testCase166
             , &HsParserTest::testCase167
             , &HsParserTest::testCase168
             , &HsParserTest::testCase169
             , &HsParserTest::testCase170
             , &HsParserTest::testCase171
             , &HsParserTest::testCase172
             , &HsParserTest::testCase173
             , &HsParserTest::testCase174
             , &HsParserTest::testCase175
             , &HsParserTest::testCase176
             , &HsParserTest::testCase177
             , &HsParserTest::testCase178
             , &HsParserTest::testCase179
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
     void testCase140();
     void testCase141();
     void testCase142();
     void testCase143();
     void testCase144();
     void testCase145();
     void testCase146();
     void testCase147();
     void testCase148();
     void testCase149();
     void testCase150();
     void testCase151();
     void testCase152();
     void testCase153();
     void testCase154();
     void testCase155();
     void testCase156();
     void testCase157();
     void testCase158();
     void testCase159();
     void testCase160();
     void testCase161();
     void testCase162();
     void testCase163();
     void testCase164();
     void testCase165();
     void testCase166();
     void testCase167();
     void testCase168();
     void testCase169();
     void testCase170();
     void testCase171();
     void testCase172();
     void testCase173();
     void testCase174();
     void testCase175();
     void testCase176();
     void testCase177();
     void testCase178();
     void testCase179();
};

MAKE_CLASS_TEST(HsParser)

void HsParser::HsParserTest::testCase1()
{
    core("module Main where\n");
}

void HsParser::HsParserTest::testCase2()
{
    core("module A.B where\n");
}

void HsParser::HsParserTest::testCase3()
{
    core("module A .B where\n", true);
}

void HsParser::HsParserTest::testCase4()
{
    core("module A. B where\n", true);
}

void HsParser::HsParserTest::testCase5()
{
    core("module \"abc\" where\n", true);
}

void HsParser::HsParserTest::testCase6()
{
    core("module Control.Monad.ST where\n");
}

void HsParser::HsParserTest::testCase7()
{
    core("module Foo(bar,zem) where");
}

void HsParser::HsParserTest::testCase8()
{
    core("module Foo((!!!)) where");
}

void HsParser::HsParserTest::testCase9()
{
    core("module Foo(!!!) where", true);
}

void HsParser::HsParserTest::testCase10()
{
    core("module Foo((!)) where");
}

void HsParser::HsParserTest::testCase11()
{
    core("module Foo(bar,(Yup.!)) where");
}

void HsParser::HsParserTest::testCase12()
{
    core("module Foo(bar,(Yup.!!!)) where");
}

void HsParser::HsParserTest::testCase13()
{
    core("module Foo(bar,(Yup.:!)) where", true);
}

void HsParser::HsParserTest::testCase14()
{
    core("module Foo(bar,(:!)) where", true);
}

void HsParser::HsParserTest::testCase15()
{
    core("module Foo(module Yup) where");
}

void HsParser::HsParserTest::testCase16()
{
    core("module Foo() where");
}

void HsParser::HsParserTest::testCase17()
{
    core("module A( Maybe( Nothing, Just ) ) where");
}

void HsParser::HsParserTest::testCase18()
{
    core("module A( Mb.Maybe( Nothing, Just ) ) where");
}

void HsParser::HsParserTest::testCase19()
{
    core("module Foo((Yup.!!!)) where");
}

void HsParser::HsParserTest::testCase20()
{
    core("module Foo(foo,bar,) where");
}

void HsParser::HsParserTest::testCase21()
{
    core("module Foo(A.B(x, y, z)) where");
}

void HsParser::HsParserTest::testCase22()
{
    core("module Foo(A.B(x, y, (!!))) where");
}

void HsParser::HsParserTest::testCase23()
{
    core("module Foo(A.B(X, Y, Z)) where");
}

void HsParser::HsParserTest::testCase24()
{
    core("module Foo(A.B(X, Y, (:!!))) where");
}

void HsParser::HsParserTest::testCase25()
{
    core("module Foo(F.f) where");
}

void HsParser::HsParserTest::testCase26()
{
    core(R"raw(
module Foo where
import A
    )raw");
}

void HsParser::HsParserTest::testCase27()
{
    core(R"raw(
module Foo where
import qualified A
    )raw");
}

void HsParser::HsParserTest::testCase28()
{
    core(R"raw(
module Foo where
import qualified A as ModA
    )raw");
}

void HsParser::HsParserTest::testCase29()
{
    core(R"raw(
module Foo where
import qualified A as ModA(foo, bar, zem)
    )raw");
}

void HsParser::HsParserTest::testCase30()
{
    core(R"raw(
module Foo where
import qualified A as ModA hiding(foo, bar, zem)
    )raw");
}

void HsParser::HsParserTest::testCase31()
{
    core(R"raw(
module Foo where
import qualified A (foo, bar, zem)
    )raw");
}

void HsParser::HsParserTest::testCase32()
{
    core(R"raw(
module Foo where
import A
import B
import C
    )raw");
}

void HsParser::HsParserTest::testCase33()
{
    core(R"raw(
module Foo where;
    )raw");
}

void HsParser::HsParserTest::testCase34()
{
    core(R"raw(
module Foo where;;
    )raw");
}

void HsParser::HsParserTest::testCase35()
{
    core(R"raw(
module Foo where
;
    )raw");
}

void HsParser::HsParserTest::testCase36()
{
    core(R"raw(
module Foo where
x :: Abc
    )raw");
}

void HsParser::HsParserTest::testCase37()
{
    core(R"raw(
module Foo where
x, y :: Abc
    )raw");
}

void HsParser::HsParserTest::testCase38()
{
    core(R"raw(
module Foo where
x, y, z :: Abc
    )raw");
}

void HsParser::HsParserTest::testCase39()
{
    core(R"raw(
module Foo where
x, y, z d :: Abc
        )raw",
        true);
}

void HsParser::HsParserTest::testCase40()
{
    core(R"raw(
module Foo where
x, y, z = Abc
        )raw",
        true);
}

void HsParser::HsParserTest::testCase41()
{
    core(R"raw(
module Foo where
x, y, z :: Template Ty
        )raw");
}

void HsParser::HsParserTest::testCase42()
{
    core(R"raw(
module Foo where
(<@>) :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase43()
{
    core(R"raw(
module Foo where
(<@>), (<@>) :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase44()
{
    core(R"raw(
module Foo where
(<@>), (<@@@>) :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase45()
{
    core(R"raw(
module Foo where
($<>$), (%%%%) :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase46()
{
    core(R"raw(
module Foo where
($<>$), xyz, (%%%%) :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase47()
{
    core(R"raw(
module Foo where
mno, ($<>$), xyz, (%%%%) :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase48()
{
    core(R"raw(
module Foo where
mno, xzy, ($<>$), ikj, (%%%%) :: Abc
         )raw");
}

void HsParser::HsParserTest::testCase49()
{
    core(R"raw(
module Foo where
mno, xzy, ($<>$), (%%%%), ijk :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase50()
{
    core(R"raw(
module Foo where
($<>$), (%%%%), ijk, mno, xzy :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase51()
{
    core(R"raw(
module Foo where
($<>$), (%%%%), ijk, mno, xzy, :: Abc
        )raw",
        true);
}

void HsParser::HsParserTest::testCase52()
{
    core(R"raw(
module Foo where
($<>$), xzy :: Abc
        )raw");
}

void HsParser::HsParserTest::testCase53()
{
    core(R"raw(
module Foo where
($<>$ :: Abc
        )raw",
        true);
}

void HsParser::HsParserTest::testCase54()
{
    core(R"raw(
module Foo where
($<>$) xzy :: Abc
        )raw",
        true);
}

void HsParser::HsParserTest::testCase55()
{
    core(R"raw(
module Foo where
(:<>:) :: Abc
        )raw",
        true);
}

void HsParser::HsParserTest::testCase56()
{
    core(R"raw(
module Foo where
No :: Abc
        )raw",
        true);
}

void HsParser::HsParserTest::testCase57()
{
    // Pattern binding.
    core(R"raw(
module Foo where
a = 10
        )raw");
}

void HsParser::HsParserTest::testCase58()
{
    // Pattern binding.
    core(R"raw(
module Foo where
a@b = 10
        )raw");
}

void HsParser::HsParserTest::testCase59()
{
    // Pattern binding.
    core(R"raw(
module Foo where
a@(b) = 10
        )raw");
}

void HsParser::HsParserTest::testCase60()
{
    // Pattern binding.
    core(R"raw(
module Foo where
a@(b:c) = 10
        )raw");
}

void HsParser::HsParserTest::testCase61()
{
    // Pattern binding.
    core(R"raw(
module Foo where
(a:b) = 10
        )raw");
}

void HsParser::HsParserTest::testCase62()
{
    // Pattern binding.
    core(R"raw(
module Foo where
(a@b) = 10
        )raw");
}

void HsParser::HsParserTest::testCase63()
{
    // Pattern binding.
    core(R"raw(
module Foo where
(a@b:c) = 10
        )raw");
}

void HsParser::HsParserTest::testCase64()
{
    // Pattern binding.
    core(R"raw(
module Foo where
(((a@b:c))) = 10
        )raw");
}

void HsParser::HsParserTest::testCase65()
{
    // Almost a pattern binding, missing a closing paren.
    core(R"raw(
module Foo where
(((a@b:c)) = 10
        )raw",
        true);
}

void HsParser::HsParserTest::testCase66()
{
    // Almost a pattern binding, extra closing paren.
    core(R"raw(
module Foo where
(a@b:c)) = 10
        )raw",
        true);
}

void HsParser::HsParserTest::testCase67()
{
    // Neither a pattern binding nor a function.
    core(R"raw(
module Foo where
(a@b:c) d = 10
        )raw",
        true);
}

void HsParser::HsParserTest::testCase68()
{
    // "Chained" function.
    core(R"raw(
module Foo where
(f x y) z = 1
        )raw");
}

void HsParser::HsParserTest::testCase69()
{
    // "Chained" function.
    core(R"raw(
module Foo where
(((f x y))) z = 1
        )raw");
}

void HsParser::HsParserTest::testCase70()
{
    // Almost a "chained" function, missing closing paren.
    core(R"raw(
module Foo where
((f x y) z = 1
        )raw",
        true);
}

void HsParser::HsParserTest::testCase71()
{
    // "Chained" function.
    core(R"raw(
module Foo where
(f <|> g) x = 10
        )raw");
}

void HsParser::HsParserTest::testCase72()
{
    // Regular function.
    core(R"raw(
module Foo where
(<|>) x y = 10
        )raw");
}

void HsParser::HsParserTest::testCase73()
{
    // Infix function.
    core(R"raw(
module Foo where
x <|> y = 10
        )raw");
}

void HsParser::HsParserTest::testCase74()
{
    // "Chained" function.
    core(R"raw(
module Foo where
((<|>) x y) z = 10
        )raw");
}

void HsParser::HsParserTest::testCase75()
{
    // "Chained" function.
    core(R"raw(
module Foo where
(x <|> y) z = 10
        )raw");
}

void HsParser::HsParserTest::testCase76()
{
    // Infix function.
    core(R"raw(
module Foo where
x `f` y = 10
        )raw");
}

void HsParser::HsParserTest::testCase77()
{
    // "Chained" function.
    core(R"raw(
module Foo where
(x `f` y) z = 10
        )raw");
}

void HsParser::HsParserTest::testCase78()
{
    // "Chained" function.
    core(R"raw(
module Foo where
(f . g) x = 10
        )raw");
}

void HsParser::HsParserTest::testCase79()
{
    core(R"raw(
module Foo where
x `f` y z = 10
        )raw",
        true);
}

void HsParser::HsParserTest::testCase80()
{
    core(R"raw(
module Foo where
(x `f` y z) w = 10
        )raw",
        true);
}

void HsParser::HsParserTest::testCase81()
{
    core(R"raw(
module Foo where
a:b = 10
        )raw");
}

void HsParser::HsParserTest::testCase82()
{
    core(R"raw(
module Foo where
[a:b] = 10
        )raw");
}

void HsParser::HsParserTest::testCase83()
{
    core(R"raw(
module Foo where
[a:b] <||> c = 10
        )raw");
}

void HsParser::HsParserTest::testCase84()
{
    core(R"raw(
module Foo where
([a:b]) = 10
        )raw");
}

void HsParser::HsParserTest::testCase85()
{
    core(R"raw(
module Foo where
((([a:b]))) = 10
        )raw");
}

void HsParser::HsParserTest::testCase86()
{
    // "Chained" function.
    core(R"raw(
module Foo where
([a:b] <|> [c:d]) e = 10
        )raw");
}

void HsParser::HsParserTest::testCase87()
{
    core(R"raw(
module Foo where
[a:b] c d = 10
        )raw",
        true);
}

void HsParser::HsParserTest::testCase88()
{
    // Pattern binding.
    core(R"raw(
module Foo where
(a) = 10
        )raw");
}

void HsParser::HsParserTest::testCase89()
{
    // Pattern binding.
    core(R"raw(
module Foo where
((a)) = 10
        )raw");
}

void HsParser::HsParserTest::testCase90()
{
    core(R"raw(
module Foo where
a@b <*> c@d = 10
        )raw");
}

void HsParser::HsParserTest::testCase91()
{
    core(R"raw(
module Foo where
a:b <*> c = 10
        )raw");
}

void HsParser::HsParserTest::testCase92()
{
    core(R"raw(
module Foo where
a@b c d = 10
        )raw",
        true);
}

void HsParser::HsParserTest::testCase93()
{
    core(R"raw(
module Foo where
f :: A -> B
        )raw");
}

void HsParser::HsParserTest::testCase94()
{
    // Not a type signature for `f', because `a' and `b' are dangling.
    core(R"raw(
module Foo where
f a b :: A -> B
        )raw",
        true);
}

void HsParser::HsParserTest::testCase95()
{
    // Not a type signature for `f', because a `::' is expected, not a `='.
    core(R"raw(
module Foo where
f a b = A -> B
        )raw",
        true);
}

void HsParser::HsParserTest::testCase96()
{
    core(R"raw(
module Foo where
f :: A -> B -> C -> D
        )raw");
}

void HsParser::HsParserTest::testCase97()
{
    core(R"raw(
module Foo where
f :: A -> b -> B
        )raw");
}

void HsParser::HsParserTest::testCase98()
{
    core(R"raw(
module Foo where
f :: [] Int
        )raw");
}

void HsParser::HsParserTest::testCase99()
{
    core(R"raw(
module Foo where
f :: [] ([] Int)
        )raw");
}

void HsParser::HsParserTest::testCase100()
{
    UAISO_SKIP_TEST;

    core(R"raw(
module Foo where
f :: [] [] Int
        )raw", true);
}

void HsParser::HsParserTest::testCase101()
{
    core(R"raw(
module Foo where
f :: (A -> B -> C) -> D
        )raw");
}

void HsParser::HsParserTest::testCase102()
{
    core(R"raw(
module Foo where
f :: A -> B -> (C -> D)
        )raw");
}

void HsParser::HsParserTest::testCase103()
{
    core(R"raw(
module Foo where
f :: A -> B (-> C) -> D
        )raw", true);
}

void HsParser::HsParserTest::testCase104()
{
    core(R"raw(
module Foo where
f :: (A, B, C) -> D
        )raw");
}

void HsParser::HsParserTest::testCase105()
{
    core(R"raw(
module Foo where
a :: ()
        )raw");
}

void HsParser::HsParserTest::testCase106()
{
    core(R"raw(
module Foo where
data Val;
        )raw");
}

void HsParser::HsParserTest::testCase107()
{
    core(R"raw(
module Foo where
data Val Val
        )raw", true);
}

void HsParser::HsParserTest::testCase108()
{
    core(R"raw(
module Foo where
data Val = One
        )raw");
}

void HsParser::HsParserTest::testCase109()
{
    core(R"raw(
module Foo where
data Val = One | Two
        )raw");
}

void HsParser::HsParserTest::testCase110()
{
    core(R"raw(
module Foo where
data Val = One
         | Two
         | Three
        )raw");
}

void HsParser::HsParserTest::testCase111()
{
    core(R"raw(
module Foo where
data Val = One
         | Two
         | Three
    deriving Eq
        )raw");
}

void HsParser::HsParserTest::testCase112()
{
    core(R"raw(
module Foo where
data Val = One
         | Two
         | Three
    deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase113()
{
    core(R"raw(
module Foo where
data Val = One
         | Two
         | Three
    deriving Eq, Ord)
        )raw", true);
}

void HsParser::HsParserTest::testCase114()
{
    core(R"raw(
module Foo where
data Val = One |
        )raw", true);
}

void HsParser::HsParserTest::testCase115()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
        )raw");
}

void HsParser::HsParserTest::testCase116()
{
    core(R"raw(
module Foo where
data Val = One deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase117()
{
    core(R"raw(
module Foo where
data Val = One
    deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase118()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
         | Eta
         | Tad Dem
        )raw");
}

void HsParser::HsParserTest::testCase119()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
         | Eta
         | Tad Dem
    deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase120()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
         | Eta :<-:
         | Tad Dem
        )raw", true);
}

void HsParser::HsParserTest::testCase121()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
         | Eta
         |
        )raw", true);
}

void HsParser::HsParserTest::testCase122()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
         | Eta
    deriving (Eq, Ord)
         | Tad
        )raw", true);
}

void HsParser::HsParserTest::testCase123()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
    deriving (Eq, Ord)
         | Tad
        )raw", true);
}

void HsParser::HsParserTest::testCase124()
{
    core(R"raw(
module Foo where
data Val = Bar Zem Qeu
         | Eta :<-: Eta
    deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase125()
{
    core(R"raw(
module Foo where
data Val = Bar []Int
    deriving (Eq, Ord)
        )raw");

}

void HsParser::HsParserTest::testCase126()
{
    core(R"raw(
module Foo where
data Val = Bar :<-: []Int
         | Eta
    deriving (Eq, Ord)
        )raw");

}

void HsParser::HsParserTest::testCase127()
{
    core(R"raw(
module Foo where
data Val = Bar :<: (Int -> Int)
        )raw");
}

void HsParser::HsParserTest::testCase128()
{
    core(R"raw(
module Foo where
data Val = Bar :<: (Int -> Int)
    deriving Eq
        )raw");
}

void HsParser::HsParserTest::testCase129()
{
    core(R"raw(
module Foo where
data Val = (Int -> Int) :<: Zaf
        )raw");
}

void HsParser::HsParserTest::testCase130()
{
    core(R"raw(
module Foo where
data Val = (Int -> Int) :<: (Int -> Int)
        )raw");
}

void HsParser::HsParserTest::testCase131()
{
    core(R"raw(
module Foo where
data Val = Bar :<: (Int -> Int)
         | Bie
    deriving Eq
        )raw");
}

void HsParser::HsParserTest::testCase132()
{
    core(R"raw(
module Foo where
data Val = | Bar
        )raw", true);
}

void HsParser::HsParserTest::testCase133()
{
    core(R"raw(
module Foo where
data Val = deriving Eq
        )raw", true);
}

void HsParser::HsParserTest::testCase134()
{
    core(R"raw(
module Foo where
data Val = Bar :<: (Int -> Int)
         |
    deriving (Eq, Ord)
        )raw", true);
}

void HsParser::HsParserTest::testCase135()
{
    core(R"raw(
module Foo where
data Val = []([] Int) :<- Bar
    deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase136()
{
    core(R"raw(
module Foo where
data Val = [] | :<- Bar
    deriving (Eq, Ord)
        )raw", true);
}

void HsParser::HsParserTest::testCase137()
{
    core(R"raw(
module Foo where
data Bar a = Zem a
        )raw");
}

void HsParser::HsParserTest::testCase138()
{
    core(R"raw(
module Foo where
data Bar a = Zem a | Nope
        )raw");
}

void HsParser::HsParserTest::testCase139()
{
    core(R"raw(
module Foo where
data Bar a = Nope | Zem a deriving Eq
        )raw");
}

void HsParser::HsParserTest::testCase140()
{
    core(R"raw(
module Foo where
data Bar a b c = Zem a
               | Qui a b c
               | Yum a b
        )raw");
}

void HsParser::HsParserTest::testCase141()
{
    core(R"raw(
module Foo where
data Bar = Zem !Raw
        )raw");
}

void HsParser::HsParserTest::testCase142()
{
    core(R"raw(
module Foo where
type String = [Char]
        )raw");
}

void HsParser::HsParserTest::testCase143()
{
    core(R"raw(
module Foo where
type Item = (Foo, Bar, Que)
        )raw");
}

void HsParser::HsParserTest::testCase144()
{
    core(R"raw(
module Foo where
type Rec = Shape
        )raw");
}

void HsParser::HsParserTest::testCase145()
{
    core(R"raw(
module Foo where
type Rec a = [Circ a]
        )raw");
}

void HsParser::HsParserTest::testCase146()
{
    core(R"raw(
module Foo where
type Rec Shape
        )raw", true);
}

void HsParser::HsParserTest::testCase147()
{
    core(R"raw(
module Foo where
type a
        )raw", true);
}

void HsParser::HsParserTest::testCase148()
{
    core(R"raw(
module Foo where
type [] Abc = Mnp
        )raw", true);
}

void HsParser::HsParserTest::testCase149()
{
    core(R"raw(
module Foo where
data Bar = (:<>:)
        )raw");
}

void HsParser::HsParserTest::testCase150()
{
    core(R"raw(
module Foo where
data Bar = (:<>:) Int
        )raw");
}

void HsParser::HsParserTest::testCase151()
{
    core(R"raw(
module Foo where
data Bar = (:<>:) | Bar | (:<<>>:) Abc Zex
        )raw");
}

void HsParser::HsParserTest::testCase152()
{
    core(R"raw(
module Foo where
data Bar = (:<>:) | Bar
         | (:<<>>:) Dex Moi
    deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase153()
{
    core(R"raw(
module Foo where
newtype N = N Int
        )raw");
}

void HsParser::HsParserTest::testCase154()
{
    core(R"raw(
module Foo where
newtype Age = Age { unAge :: Int }
        )raw");
}

void HsParser::HsParserTest::testCase155()
{
    core(R"raw(
module Foo where
newtype N = N Int Int
        )raw", true);
}

void HsParser::HsParserTest::testCase156()
{
    core(R"raw(
module Foo where
newtype N = N
        )raw", true);
}

void HsParser::HsParserTest::testCase157()
{
    core(R"raw(
module Foo where
newtype N = N Int deriving (Eq, Ord)
        )raw");
}

void HsParser::HsParserTest::testCase158()
{
    core(R"raw(
module Foo where
a = b
        )raw");
}

void HsParser::HsParserTest::testCase159()
{
    core(R"raw(
module Foo where
a = B.b
        )raw");
}

void HsParser::HsParserTest::testCase160()
{
    core(R"raw(
module Foo where
a = x y z
        )raw");
}

void HsParser::HsParserTest::testCase161()
{
    core(R"raw(
module Foo where
a = W.x y z
        )raw");
}

void HsParser::HsParserTest::testCase162()
{
    core(R"raw(
module Foo where
a = W. y z
        )raw", true);
}

void HsParser::HsParserTest::testCase163()
{
    core(R"raw(
module Foo where
a = W.x 1 'c'
        )raw");
}

void HsParser::HsParserTest::testCase164()
{
    core(R"raw(
module Foo where
a = W.Y 1 x
        )raw");
}

void HsParser::HsParserTest::testCase165()
{
    core(R"raw(
module Foo where
a = 1 x y
        )raw", true);
}

void HsParser::HsParserTest::testCase166()
{
    core(R"raw(
module Foo where
a = []
        )raw");
}

void HsParser::HsParserTest::testCase167()
{
    core(R"raw(
module Foo where
a = [] []
        )raw", true);
}

void HsParser::HsParserTest::testCase168()
{
    core(R"raw(
module Foo where
a = [1]
        )raw");
}

void HsParser::HsParserTest::testCase169()
{
    core(R"raw(
module Foo where
a = [1, 2]
        )raw");
}

void HsParser::HsParserTest::testCase170()
{
    core(R"raw(
module Foo where
a = [1, 2 3]
        )raw", true);
}

void HsParser::HsParserTest::testCase171()
{
    core(R"raw(
module Foo where
a = [1 .. 10]
        )raw");
}

void HsParser::HsParserTest::testCase172()
{
    core(R"raw(
module Foo where
a = [0, 1 .. 10]
        )raw");
}

void HsParser::HsParserTest::testCase173()
{
    core(R"raw(
module Foo where
a = [1 ..]
        )raw");
}

void HsParser::HsParserTest::testCase174()
{
    core(R"raw(
module Foo where
a = [1, 2 ..]
        )raw");
}

void HsParser::HsParserTest::testCase175()
{
    core(R"raw(
module Foo where
a = [1 .. 2 .. 3]
        )raw", true);
}

void HsParser::HsParserTest::testCase176() {}
void HsParser::HsParserTest::testCase177() {}
void HsParser::HsParserTest::testCase178() {}
void HsParser::HsParserTest::testCase179() {}
