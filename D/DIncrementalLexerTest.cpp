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

#include "D/DIncrementalLexer.h"
#include "Parsing/IncrementalLexer__.h"
#include "Parsing/Phrasing.h"
#include "Parsing/Token.h"

using namespace uaiso;

class DIncrementalLexer::DIncrementalLexerTest final : public Test
{
public:
    TEST_RUN(DIncrementalLexerTest
             , &DIncrementalLexerTest::testCase1
             , &DIncrementalLexerTest::testCase2
             , &DIncrementalLexerTest::testCase3
             , &DIncrementalLexerTest::testCase4
             , &DIncrementalLexerTest::testCase5
             , &DIncrementalLexerTest::testCase6
             , &DIncrementalLexerTest::testCase7
             , &DIncrementalLexerTest::testCase8
             , &DIncrementalLexerTest::testCase9
             , &DIncrementalLexerTest::testCase10
             , &DIncrementalLexerTest::testCase11
             )

    DIncrementalLexer lexer_;

    std::unique_ptr<Phrasing> core(const std::string& code)
    {
        lexer_.tokenize(code);
        return std::unique_ptr<Phrasing>(lexer_.releasePhrasing());
    }

    void testCase1()
    {
        auto phrase = core("struct ticks {");
        std::vector<Token> expected {
            TK_STRUCT, TK_IDENTIFIER, TK_LBRACE
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase2()
    {
        auto phrase = core("int makeStringSlice(int n) {");
        std::vector<Token> expected {
            TK_INT, TK_IDENTIFIER, TK_LPAREN, TK_INT, TK_IDENTIFIER,
            TK_RPAREN, TK_LBRACE
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase3()
    {
        auto phrase = core("int a; // comment\n");
        std::vector<Token> expected {
            TK_INT, TK_IDENTIFIER, TK_SEMICOLON, TK_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase4()
    {
        auto phrase = core("int a; /* multiline comment\n");
        std::vector<Token> expected {
            TK_INT, TK_IDENTIFIER, TK_SEMICOLON, TK_MULTILINE_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InMultilineComment);
    }

    void testCase5()
    {
        return;
        auto phrase = core("int /* between */ a");
        std::vector<Token> expected {
            TK_INT, TK_MULTILINE_COMMENT, TK_IDENTIFIER
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase6()
    {
        auto phrase = core(R"raw(
            int a; /* multiline comment
                      continues */
            int b;
        )raw");
        std::vector<Token> expected {
            TK_INT, TK_IDENTIFIER, TK_SEMICOLON, TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT,
            TK_INT, TK_IDENTIFIER, TK_SEMICOLON
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase7()
    {
        auto phrase = core(R"raw(
            int a; /* multiline comment
                      continues
                      still */
        )raw");
        std::vector<Token> expected {
            TK_INT, TK_IDENTIFIER, TK_SEMICOLON, TK_MULTILINE_COMMENT,
            TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase8()
    {
        auto phrase = core(R"raw(
            int /* multiline comment
                   continues
                   still */ a;
        )raw");
        std::vector<Token> expected {
            TK_INT, TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT, TK_MULTILINE_COMMENT,
            TK_IDENTIFIER, TK_SEMICOLON
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase9()
    {
        lexer_.P->context_->setStopMark(LineCol(0, 7));
        auto phrase = core("object. ");
        lexer_.P->context_->clearStopMark();

        std::vector<Token> expected {
            TK_IDENTIFIER, TK_DOT, TK_COMPLETION
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase10()
    {
        lexer_.P->context_->setStopMark(LineCol(0, 7));
        auto phrase = core("object.\n");
        lexer_.P->context_->clearStopMark();

        std::vector<Token> expected {
            TK_IDENTIFIER, TK_DOT, TK_COMPLETION
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }

    void testCase11()
    {
        lexer_.P->context_->setStopMark(LineCol(0, 7));
        auto phrase = core("object. foo");
        lexer_.P->context_->clearStopMark();

        std::vector<Token> expected {
            TK_IDENTIFIER, TK_DOT, TK_COMPLETION, TK_IDENTIFIER
        };
        UAISO_EXPECT_INT_EQ(expected.size(), phrase->size());
        UAISO_EXPECT_CONTAINER_EQ(expected, (*phrase));
        UAISO_EXPECT_TRUE(lexer_.state() == IncrementalLexer::InCode);
    }
};

MAKE_CLASS_TEST(DIncrementalLexer)
