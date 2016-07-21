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

void HsParser::HsParserTest::testCase32() {}
void HsParser::HsParserTest::testCase33() {}
void HsParser::HsParserTest::testCase34() {}
void HsParser::HsParserTest::testCase35() {}
void HsParser::HsParserTest::testCase36() {}
void HsParser::HsParserTest::testCase37() {}
void HsParser::HsParserTest::testCase38() {}
void HsParser::HsParserTest::testCase39() {}
void HsParser::HsParserTest::testCase40() {}
