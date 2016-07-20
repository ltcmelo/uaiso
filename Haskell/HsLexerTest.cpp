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

#include "Haskell/HsLexer.h"
#include "Parsing/Diagnostic.h"
#include "Parsing/Lang.h"
#include "Parsing/ParsingContext.h"
#include "Parsing/Phrasing.h"
#include <iterator>
#include <vector>

using namespace uaiso;

class HsLexer::HsLexerTest : public Test
{
public:
    TEST_RUN(HsLexerTest
             , &HsLexerTest::testCase1
             , &HsLexerTest::testCase2
             , &HsLexerTest::testCase3
             , &HsLexerTest::testCase4
             , &HsLexerTest::testCase5
             , &HsLexerTest::testCase6
             , &HsLexerTest::testCase7
             , &HsLexerTest::testCase8
             , &HsLexerTest::testCase9
             , &HsLexerTest::testCase10
             , &HsLexerTest::testCase11
             , &HsLexerTest::testCase12
             , &HsLexerTest::testCase13
             , &HsLexerTest::testCase14
             , &HsLexerTest::testCase15
             , &HsLexerTest::testCase16
             , &HsLexerTest::testCase17
             , &HsLexerTest::testCase18
             , &HsLexerTest::testCase19
             , &HsLexerTest::testCase20
             , &HsLexerTest::testCase21
             , &HsLexerTest::testCase22
             , &HsLexerTest::testCase23
             , &HsLexerTest::testCase24
             , &HsLexerTest::testCase25
             , &HsLexerTest::testCase26
             , &HsLexerTest::testCase27
             , &HsLexerTest::testCase28
             , &HsLexerTest::testCase29
             , &HsLexerTest::testCase30
             , &HsLexerTest::testCase31
             , &HsLexerTest::testCase32
             , &HsLexerTest::testCase33
             , &HsLexerTest::testCase34
             , &HsLexerTest::testCase35
             , &HsLexerTest::testCase36
             , &HsLexerTest::testCase37
             , &HsLexerTest::testCase38
             , &HsLexerTest::testCase39
             , &HsLexerTest::testCase40
             , &HsLexerTest::testCase41
             , &HsLexerTest::testCase42
             , &HsLexerTest::testCase43
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

    std::vector<Token> core(const std::string& code)
    {
        ParsingContext context;
        context.setFileName("/test.hs");
        context.setAllowComments(keepComments_);
        DiagnosticReports reports;
        context.collectReports(&reports);

        HsLexer lexer;
        lexer.setContext(&context);
        lexer.setBuffer(code.c_str(), code.length());
        std::vector<Token> tks;
        while (true) {
            tks.push_back(lexer.lex());
            if (tks.back() == TK_EOP)
                break;
            locs_.push_back(lexer.tokenLoc());
        }

        UAISO_EXPECT_TRUE((expectError_ && reports.size() > 0)
                          || (!expectError_ && reports.size() == 0));

        if (dumpTokens_) {
            std::copy(tks.begin(), tks.end(),
                      std::ostream_iterator<Token>(std::cout, " "));
        }

        if (dumpLocs_) {
            std::copy(locs_.begin(), locs_.end(),
                      std::ostream_iterator<SourceLoc>(std::cout, " "));
        }

        return tks;
    }

    void reset() override
    {
        dumpTokens_ = false;
        dumpLocs_ = false;
        keepComments_ = false;
        expectError_ = false;
        locs_.clear();
    }

    bool dumpTokens_ { false };
    bool dumpLocs_ { false };
    bool keepComments_ { false };
    bool expectError_ { false };
    std::vector<SourceLoc> locs_;
};

MAKE_CLASS_TEST(HsLexer)

void HsLexer::HsLexerTest::testCase1()
{
    auto tks = core("5 + 10");

    std::vector<Token> expected {
        TK_INT_LIT, TK_PLUS, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase2()
{
    auto tks = core("max 1 2");

    std::vector<Token> expected {
        TK_IDENT, TK_INT_LIT, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase3()
{
    auto tks = core("if x then 1 else 2");

    std::vector<Token> expected {
        TK_IF, TK_IDENT, TK_THEN, TK_INT_LIT, TK_ELSE,
        TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase4()
{
    auto tks = core("area r = pi * r ^ 2");

    std::vector<Token> expected {
        TK_IDENT, TK_IDENT, TK_EQ, TK_IDENT,
        TK_ASTERISK, TK_IDENT, TK_CARET, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase5()
{
    auto tks = core("data Color = Red | Green");

    std::vector<Token> expected {
        TK_DATA, TK_PROPER_IDENT, TK_EQ, TK_PROPER_IDENT,
        TK_PIPE, TK_PROPER_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase6()
{
    auto tks = core("cool :: Int -> Bool");

    std::vector<Token> expected {
        TK_IDENT, TK_COLON_COLON, TK_PROPER_IDENT, TK_DASH_ARROW,
        TK_PROPER_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase7()
{
    auto tks = core("import A.B");

    std::vector<Token> expected {
        TK_IMPORT, TK_PROPER_IDENT, TK_JOKER, TK_PROPER_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase8()
{
    auto tks = core("(<=>) :: Int -> Int -> Int");

    std::vector<Token> expected {
        TK_LPAREN, TK_SYMBOL_IDENT, TK_RPAREN, TK_COLON_COLON,
        TK_PROPER_IDENT, TK_DASH_ARROW, TK_PROPER_IDENT,
        TK_DASH_ARROW, TK_PROPER_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase9()
{
    auto tks = core("x <^> y = x + y * 2");

    std::vector<Token> expected {
        TK_IDENT, TK_SYMBOL_IDENT, TK_IDENT, TK_EQ,
        TK_IDENT, TK_PLUS, TK_IDENT, TK_STAR, TK_INT_LIT,
        TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase10()
{
    auto tks = core("(@@) :: Int -> Int -> Int");

    std::vector<Token> expected {
        TK_LPAREN, TK_SYMBOL_IDENT, TK_RPAREN, TK_COLON_COLON,
        TK_PROPER_IDENT, TK_DASH_ARROW, TK_PROPER_IDENT,
        TK_DASH_ARROW, TK_PROPER_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase11()
{
    auto tks = core("f.g");

    std::vector<Token> expected {
        TK_IDENT, TK_DOT, TK_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase12()
{
    auto tks = core("F.g");

    std::vector<Token> expected {
        TK_PROPER_IDENT, TK_JOKER, TK_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase13()
{
    auto tks = core("f..");

    std::vector<Token> expected {
        TK_IDENT, TK_DOT_DOT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase14()
{
    auto tks = core("F..");

    std::vector<Token> expected {
        TK_PROPER_IDENT, TK_JOKER, TK_DOT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase15()
{
    auto tks = core("F.");

    std::vector<Token> expected {
        TK_PROPER_IDENT, TK_DOT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase16()
{
    auto tks = core("a = 0o123");

    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase17()
{
    auto tks = core("a = 0O77");

    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase18()
{
    auto tks = core("a = 0xab01ef79");

    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase19()
{
    auto tks = core("a = 0123");

    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase20()
{
    auto tks = core("a = 0.1");

    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_FLOAT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase21()
{
    auto tks = core("a = .1");

    // A floating literal must contain digits before and after a decimal point.
    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_DOT, TK_INT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase22()
{
    auto tks = core("a = 1.");

    // A floating literal must contain digits before and after a decimal point.
    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_DOT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase23()
{
    auto tks = core("a = 123.456e22");

    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_FLOAT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase24()
{
    auto tks = core("a = 123.456e-2");

    std::vector<Token> expected {
        TK_IDENT, TK_EQ, TK_FLOAT_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase25()
{
    auto tks = core(R"raw(
"haskell"
)raw");

    std::vector<Token> expected {
        TK_STR_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase26()
{
    auto tks = core(R"raw(
"a'a"
)raw");

    std::vector<Token> expected {
        TK_STR_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase27()
{
    auto tks = core(R"raw(
"aa\"aa\"aa"
)raw");

    std::vector<Token> expected {
        TK_STR_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase28()
{
    auto tks = core(R"raw(
"aaa\
        \aaa"
)raw");

    std::vector<Token> expected {
        TK_STR_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase29()
{
    expectError_ = true; // String join is not properly matched.
    auto tks = core(R"raw(
"aaa\
        aaa"
)raw");

    std::vector<Token> expected {
        TK_STR_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase30()
{
    auto tks = core(R"raw(
"Here is a backslant \\ as well as \137, \
    \a numeric escape character, and \^X, a control character."
)raw");

    std::vector<Token> expected {
        TK_STR_LIT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase31()
{
    auto tks = core(R"raw(
module Foo where
data Bar a = Empty
)raw");

    std::vector<Token> expected {
        TK_MODULE, TK_PROPER_IDENT, TK_WHERE, TK_LBRACE, TK_DATA,
        TK_PROPER_IDENT, TK_IDENT, TK_EQ, TK_PROPER_IDENT, TK_RBRACE,
        TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase32()
{
    auto tks = core(R"raw(
let a = 1
)raw");

    std::vector<Token> expected {
        TK_LET, TK_LBRACE, TK_IDENT, TK_EQ, TK_INT_LIT, TK_RBRACE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase33()
{
    auto tks = core(R"raw(
let a = 1
    b = 2
)raw");

    std::vector<Token> expected {
        TK_LET, TK_LBRACE, TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_RBRACE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase34()
{
    auto tks = core(R"raw(
let a = 1; b = 1
    c d = e
)raw");

    std::vector<Token> expected {
        TK_LET, TK_LBRACE, TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_IDENT, TK_EQ, TK_IDENT, TK_RBRACE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase35()
{
    auto tks = core(R"raw(
f x = let a = 1; b = 2
          g y = exp2
       in exp1
)raw");

    std::vector<Token> expected {
        TK_IDENT, TK_IDENT, TK_EQ, TK_LET, TK_LBRACE,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_IDENT, TK_EQ, TK_IDENT, TK_RBRACE,
        TK_IN, TK_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase36()
{
    auto tks = core(R"raw(
let { a = 1 }
)raw");

    std::vector<Token> expected {
        TK_LET, TK_LBRACE, TK_IDENT, TK_EQ, TK_INT_LIT, TK_RBRACE, TK_EOP
    };
//    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);

}

void HsLexer::HsLexerTest::testCase37()
{
    auto tks = core(R"raw(
f x = let { a = 1; b = 2;
            g y = exp2 }
       in exp1
)raw");

    std::vector<Token> expected {
        TK_IDENT, TK_IDENT, TK_EQ, TK_LET, TK_LBRACE,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_IDENT, TK_EQ, TK_IDENT, TK_RBRACE,
        TK_IN, TK_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase38()
{
    auto tks = core(R"raw(
f x = let { a = 1; b = 2; g y = exp2 }
       in exp1
)raw");

    std::vector<Token> expected {
        TK_IDENT, TK_IDENT, TK_EQ, TK_LET, TK_LBRACE,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_EQ, TK_INT_LIT, TK_SEMICOLON,
        TK_IDENT, TK_IDENT, TK_EQ, TK_IDENT, TK_RBRACE,
        TK_IN, TK_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase39()
{
    auto tks = core("Aa.Bb");

    std::vector<Token> expected {
        TK_PROPER_IDENT, TK_JOKER, TK_PROPER_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase40()
{
    auto tks = core("Aa.Bb.+");

    std::vector<Token> expected {
        TK_PROPER_IDENT, TK_JOKER, TK_PROPER_IDENT, TK_JOKER, TK_PLUS, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase41()
{
    auto tks = core("Aa.Bb.<=>");

    std::vector<Token> expected {
        TK_PROPER_IDENT, TK_JOKER, TK_PROPER_IDENT, TK_JOKER, TK_SYMBOL_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase42()
{
    auto tks = core("Aa.Bb.:<=>");

    std::vector<Token> expected {
        TK_PROPER_IDENT, TK_JOKER, TK_PROPER_IDENT, TK_JOKER, TK_SPECIAL_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase43()
{
    auto tks = core("(:@@) :: Int");

    std::vector<Token> expected {
        TK_LPAREN, TK_SPECIAL_IDENT, TK_RPAREN, TK_COLON_COLON,
        TK_PROPER_IDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}
