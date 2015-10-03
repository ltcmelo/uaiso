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

#include "Python/PyLexer.h"
#include "Parsing/Syntax.h"
#include "Parsing/Phrasing.h"
#include <iterator>
#include <vector>

using namespace uaiso;

class PyLexer::PyLexerTest : public Test
{
public:
    TEST_RUN(PyLexerTest
             , &PyLexerTest::testCase1
             , &PyLexerTest::testCase2
             , &PyLexerTest::testCase3
             , &PyLexerTest::testCase4
             , &PyLexerTest::testCase5
             , &PyLexerTest::testCase6
             , &PyLexerTest::testCase7
             , &PyLexerTest::testCase8
             , &PyLexerTest::testCase9
             , &PyLexerTest::testCase10
             , &PyLexerTest::testCase11
             , &PyLexerTest::testCase12
             , &PyLexerTest::testCase13
             , &PyLexerTest::testCase14
             , &PyLexerTest::testCase15
             , &PyLexerTest::testCase16
             , &PyLexerTest::testCase17
             , &PyLexerTest::testCase18
             , &PyLexerTest::testCase19
             , &PyLexerTest::testCase20
             , &PyLexerTest::testCase21
             , &PyLexerTest::testCase22
             , &PyLexerTest::testCase23
             , &PyLexerTest::testCase24
             , &PyLexerTest::testCase25
             , &PyLexerTest::testCase26
             , &PyLexerTest::testCase27
             , &PyLexerTest::testCase28
             , &PyLexerTest::testCase29
             , &PyLexerTest::testCase30
             )

    // Tests cases (with few exceptions) were taken from CPython.

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


    std::vector<Token> core(const std::string& code)
    {
        PyLexer lexer;
        lexer.setBuffer(code.c_str(), code.length());
        std::vector<Token> tks;
        while (true) {
            tks.push_back(lexer.lex());
            if (tks.back() == TK_EOP)
                break;
        }
        return tks;
    }

};

void PyLexer::PyLexerTest::testCase1()
{
    auto tks = core(R"raw(

the_world_is_flat = 1
if the_world_is_flat:
    print "Be careful not to fall off!"

)raw");

    std::vector<Token> expected {
        TK_NEWLINE, TK_NEWLINE, TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL,
        TK_NEWLINE, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE, TK_INDENT,
        TK_PRINT, TK_STRING_LITERAL, TK_NEWLINE, TK_NEWLINE, TK_DEDENT,
        TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase2()
{
    auto tks = core(R"raw(

def check():
    if let_it:
        pass

)raw");

    std::vector<Token> expected {
        TK_NEWLINE, TK_NEWLINE, TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN,
        TK_COLON, TK_NEWLINE, TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON,
        TK_NEWLINE, TK_INDENT, TK_PASS, TK_NEWLINE, TK_NEWLINE, TK_DEDENT,
        TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase3()
{
    auto tks = core("0xff <= 255");
    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_LESS_EQUAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase4()
{
    auto tks = core("0o123 <= 0O123");
    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_LESS_EQUAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase5()
{
    auto tks = core("1234567 > ~0x15");
    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_GREATER, TK_TILDE, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase6()
{
    auto tks = core("2134568 != 1231515");
    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_EXCLAM_EQUAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase7()
{
    auto tks = core("(-124561-1) & 200000000");
    std::vector<Token> expected {
        TK_LPAREN, TK_MINUS, TK_INTEGER_LITERAL, TK_MINUS, TK_INTEGER_LITERAL,
        TK_RPAREN, TK_AMPER, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase8()
{
    auto tks = core("0xdeadbeef != -1");
    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_EXCLAM_EQUAL, TK_MINUS, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase9()
{
    auto tks = core("0xdeadc0de & 12345");
    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_AMPER, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase10()
{
    auto tks = core("0xFF & 0x15 | 1234");
    std::vector<Token> expected {
        TK_INTEGER_LITERAL, TK_AMPER, TK_INTEGER_LITERAL, TK_PIPE,
        TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase11()
{
    auto tks = core("x = 0");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase12()
{
    auto tks = core("x = 0xfffffffffff");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase13()
{
    auto tks = core("x = 3.14159");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_FLOAT_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase14()
{
    auto tks = core("x = 314159.");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_FLOAT_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase15()
{
    auto tks = core("x = .314159");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_FLOAT_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase16()
{
    auto tks = core("x = 3e14159");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_FLOAT_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase17()
{
    auto tks = core("x+y = 3e-1230");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_PLUS, TK_IDENTIFIER, TK_EQUAL, TK_FLOAT_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase18()
{
    auto tks = core("x = r'abc' + r'ABC' + R'ABC' + R'ABC'");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_STRING_LITERAL, TK_PLUS,
        TK_STRING_LITERAL, TK_PLUS, TK_STRING_LITERAL, TK_PLUS,
        TK_STRING_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase19()
{
    auto tks = core(R"raw(y = r"abc" + r"ABC" + R"ABC" + R"ABC")raw");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_STRING_LITERAL, TK_PLUS,
        TK_STRING_LITERAL, TK_PLUS, TK_STRING_LITERAL, TK_PLUS,
        TK_STRING_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase20()
{
    auto tks = core("u'abc' + U'abc'");
    std::vector<Token> expected {
        TK_STRING_LITERAL, TK_PLUS, TK_STRING_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase21()
{
    auto tks = core(R"raw(u"abc" + U"abc")raw");
    std::vector<Token> expected {
        TK_STRING_LITERAL, TK_PLUS, TK_STRING_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase22()
{
    auto tks = core("br'abc' + bR'abc' + Br'abc' + BR'abc'");
    std::vector<Token> expected {
        TK_STRING_LITERAL, TK_PLUS, TK_STRING_LITERAL, TK_PLUS,
        TK_STRING_LITERAL, TK_PLUS, TK_STRING_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase23()
{
    auto tks = core("def d22(a, b, c=2, d=2, *k): pass");
    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_COMMA,
        TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL,
        TK_COMMA, TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_COMMA,
        TK_STAR, TK_IDENTIFIER, TK_RPAREN, TK_COLON, TK_PASS, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase24()
{
    auto tks = core("def d01v_(a=1, *k, **w): pass");
    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL,
        TK_COMMA, TK_STAR, TK_IDENTIFIER, TK_COMMA, TK_STAR_STAR, TK_IDENTIFIER,
        TK_RPAREN, TK_COLON, TK_PASS, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase25()
{
    auto tks = core("x = 1 - y + 15 - 1 + 0x124 + z + a[5]");
    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_MINUS, TK_IDENTIFIER,
        TK_PLUS, TK_INTEGER_LITERAL, TK_MINUS, TK_INTEGER_LITERAL, TK_PLUS,
        TK_INTEGER_LITERAL, TK_PLUS, TK_IDENTIFIER, TK_PLUS, TK_IDENTIFIER, TK_LBRACKET,
        TK_INTEGER_LITERAL, TK_RBRACKET, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase26()
{
    auto tks = core("~1 ^ 1 & 1 |1 ^ -1");
    std::vector<Token> expected {
        TK_TILDE, TK_INTEGER_LITERAL, TK_CIRCUMFLEX, TK_INTEGER_LITERAL,
        TK_AMPER, TK_INTEGER_LITERAL, TK_PIPE, TK_INTEGER_LITERAL, TK_CIRCUMFLEX,
        TK_MINUS, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase27()
{
    auto tks = core(R"raw(

import sys, time
x = sys.modules['time'].time()

)raw");

    std::vector<Token> expected {
        TK_NEWLINE, TK_NEWLINE, TK_IMPORT, TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER,
        TK_NEWLINE, TK_IDENTIFIER, TK_EQUAL, TK_IDENTIFIER, TK_DOT, TK_IDENTIFIER,
        TK_LBRACKET, TK_STRING_LITERAL, TK_RBRACKET, TK_DOT, TK_IDENTIFIER,
        TK_LPAREN, TK_RPAREN, TK_NEWLINE, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase28()
{
    auto tks = core(R"raw(

@staticmethod
def foo(x,y): pass

)raw");

    std::vector<Token> expected {
        TK_NEWLINE, TK_NEWLINE, TK_AT_SYMBOL, TK_IDENTIFIER, TK_NEWLINE, TK_FUNC,
        TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER, TK_RPAREN,
        TK_COLON, TK_PASS, TK_NEWLINE, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase29()
{
    auto tks = core("class async(Bar):pass");
    std::vector<Token> expected {
        TK_CLASS, TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_RPAREN,
        TK_COLON, TK_PASS, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase30()
{
    auto tks = core(R"raw(

async def foo():
    def foo(await):
        await = 1
    if 1:
        await
    async += 1

)raw");

    std::vector<Token> expected {
        TK_NEWLINE, TK_NEWLINE, TK_IDENTIFIER, TK_FUNC, TK_IDENTIFIER,
        TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE, TK_INDENT, TK_FUNC,
        TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_RPAREN, TK_COLON,
        TK_NEWLINE, TK_INDENT, TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL,
        TK_NEWLINE, TK_DEDENT, TK_IF, TK_INTEGER_LITERAL, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_IDENTIFIER,
        TK_PLUS_EQUAL, TK_INTEGER_LITERAL, TK_NEWLINE, TK_NEWLINE, TK_DEDENT,
        TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

MAKE_CLASS_TEST(PyLexer)
