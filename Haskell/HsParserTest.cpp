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

void HsParser::HsParserTest::testCase6(){}
void HsParser::HsParserTest::testCase7(){}
void HsParser::HsParserTest::testCase8(){}
void HsParser::HsParserTest::testCase9(){}
void HsParser::HsParserTest::testCase10(){}
