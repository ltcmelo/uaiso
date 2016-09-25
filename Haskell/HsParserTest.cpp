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
x, y, z :: Abc Mno
        )raw",
        true);
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

void HsParser::HsParserTest::testCase55() {}
void HsParser::HsParserTest::testCase56() {}
void HsParser::HsParserTest::testCase57() {}
void HsParser::HsParserTest::testCase58() {}
void HsParser::HsParserTest::testCase59() {}
void HsParser::HsParserTest::testCase60() {}
void HsParser::HsParserTest::testCase61() {}
void HsParser::HsParserTest::testCase62() {}
void HsParser::HsParserTest::testCase63() {}
void HsParser::HsParserTest::testCase64() {}
void HsParser::HsParserTest::testCase65() {}
void HsParser::HsParserTest::testCase66() {}
void HsParser::HsParserTest::testCase67() {}
void HsParser::HsParserTest::testCase68() {}
void HsParser::HsParserTest::testCase69() {}
