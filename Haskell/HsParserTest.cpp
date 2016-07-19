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
    // TODO: Fix lexing of `:!`, cannot be custom operator (begins with `:`).
    core("module Foo(bar,(Yup.:!)) where", true);
}

void HsParser::HsParserTest::testCase14()
{
    core("module Foo(bar,(:!)) where");
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
