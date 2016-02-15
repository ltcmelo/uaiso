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
             )

    void testCase1();
    void testCase2();
    void testCase3();
    void testCase4();
    void testCase5();

    std::vector<Token> core(const std::string& code)
    {
        ParsingContext context;
        context.setFileName("/test.hs");
        context.setAllowComments(keepComments_);

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
        locs_.clear();
    }

    bool dumpTokens_ { false };
    bool dumpLocs_ { false };
    bool keepComments_ { false };
    std::vector<SourceLoc> locs_;
};

MAKE_CLASS_TEST(HsLexer)

void HsLexer::HsLexerTest::testCase1()
{
    auto tks = core("5 + 10");

    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_PLUS, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase2()
{
    auto tks = core("max 1 2");

    std::vector<Token> expected {
        TK_IDENTIFIER, TK_INTEGER_LITERAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase3()
{
    auto tks = core("if x then 1 else 2");

    std::vector<Token> expected {
        TK_IF, TK_IDENTIFIER, TK_THEN, TK_INTEGER_LITERAL, TK_ELSE,
        TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase4()
{
    auto tks = core("let area r = pi * r ^ 2");

    std::vector<Token> expected {
        TK_LET, TK_IDENTIFIER, TK_IDENTIFIER, TK_EQUAL, TK_IDENTIFIER,
        TK_ASTERISK, TK_IDENTIFIER, TK_CARET, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void HsLexer::HsLexerTest::testCase5()
{
    auto tks = core("data Color = Red | Green");

    std::vector<Token> expected {
        TK_DATA, TK_IDENTIFIER_CAPITALIZED, TK_EQUAL, TK_IDENTIFIER_CAPITALIZED,
        TK_PIPE, TK_IDENTIFIER_CAPITALIZED, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);

}
