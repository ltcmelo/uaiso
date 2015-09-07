/******************************************************************************
 * Copyright (c) 2014-2015-2015 Leandro T. C. Melo (ltcmelo@gmail.com)
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

#include "Go/GoIncrementalLexer.h"
#include "Parsing/IncrementalLexer__.h"
#include "Parsing/Phrasing.h"
#include "Parsing/Token.h"

using namespace uaiso;

class GoIncrementalLexer::GoIncrementalLexerTest final : public Test
{
public:
    TEST_RUN(GoIncrementalLexerTest
             , &GoIncrementalLexerTest::testCase1
             , &GoIncrementalLexerTest::testCase2
             , &GoIncrementalLexerTest::testCase3
             , &GoIncrementalLexerTest::testCase4
             , &GoIncrementalLexerTest::testCase5
             , &GoIncrementalLexerTest::testCase6
             , &GoIncrementalLexerTest::testCase7
             , &GoIncrementalLexerTest::testCase8
             , &GoIncrementalLexerTest::testCase9
             , &GoIncrementalLexerTest::testCase10
             )

    GoIncrementalLexer lexer_;

    std::unique_ptr<Phrasing> core(const std::string& code)
    {
        lexer_.tokenize(code);
        return std::unique_ptr<Phrasing>(lexer_.releasePhrasing());
    }

    void testCase1()
    {
        auto phrase = core("var ticks struct {");
        std::vector<Token> expected {
            TK_VAR, TK_IDENTIFIER, TK_STRUCT, TK_LBRACE
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase2()
    {
        auto phrase = core("func makeStringSlice(n int) []string {");
        std::vector<Token> expected {
            TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_INT, TK_RPAREN,
            TK_LBRACKET, TK_RBRACKET, TK_IDENTIFIER, TK_LBRACE
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase3()
    {
        auto phrase = core("var a; // comment\n");
        std::vector<Token> expected {
            TK_VAR, TK_IDENTIFIER, TK_SEMICOLON, TK_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase4()
    {
        auto phrase = core("var a // comment\n");
        std::vector<Token> expected {
            TK_VAR, TK_IDENTIFIER, TK_SEMICOLON, TK_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase5()
    {
        auto phrase = core("var a; /* multiline comment\n");
        std::vector<Token> expected {
            TK_VAR, TK_IDENTIFIER, TK_SEMICOLON, TK_MULTILINE_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InMultilineComment);
    }

    void testCase6()
    {
        auto phrase = core("var a /* multiline comment\n");
        std::vector<Token> expected {
            TK_VAR, TK_IDENTIFIER, TK_SEMICOLON, TK_MULTILINE_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InMultilineComment);
    }

    void testCase7()
    {
        return;
        auto phrase = core("var /* between */ a");
        std::vector<Token> expected {
            TK_VAR, TK_MULTILINE_COMMENT, TK_IDENTIFIER
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase8()
    {
        auto phrase = core(R"raw(
            var a /* multiline comment
                     continues */
            var b
        )raw");
        std::vector<Token> expected {
            TK_VAR, TK_IDENTIFIER, TK_SEMICOLON, TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT,
            TK_VAR, TK_IDENTIFIER, TK_SEMICOLON
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase9()
    {
        auto phrase = core(R"raw(
            var a /* multiline comment
                     continues
                     still */
        )raw");
        std::vector<Token> expected {
            TK_VAR, TK_IDENTIFIER, TK_SEMICOLON, TK_MULTILINE_COMMENT,
            TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase10()
    {
        auto phrase = core(R"raw(
            var /* multiline comment
                   continues
                   still */ a
        )raw");
        std::vector<Token> expected {
            TK_VAR, TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT,
            TK_IDENTIFIER, TK_SEMICOLON
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }
};

MAKE_CLASS_TEST(GoIncrementalLexer)
