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
#include "Parsing/ParsingContext.h"
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
             , &PyLexerTest::testCase31
             , &PyLexerTest::testCase32
             , &PyLexerTest::testCase33
             , &PyLexerTest::testCase34
             , &PyLexerTest::testCase35
             , &PyLexerTest::testCase36
             , &PyLexerTest::testCase37
             , &PyLexerTest::testCase38
             , &PyLexerTest::testCase39
             , &PyLexerTest::testCase40
             , &PyLexerTest::testCase41
             , &PyLexerTest::testCase42
             , &PyLexerTest::testCase43
             , &PyLexerTest::testCase44
             , &PyLexerTest::testCase45
             , &PyLexerTest::testCase46
             , &PyLexerTest::testCase47
             , &PyLexerTest::testCase48
             , &PyLexerTest::testCase49
             , &PyLexerTest::testCase50
             , &PyLexerTest::testCase51
             , &PyLexerTest::testCase52
             , &PyLexerTest::testCase53
             , &PyLexerTest::testCase54
             , &PyLexerTest::testCase55
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

    std::vector<Token> core(const std::string& code)
    {
        ParsingContext context;
        context.setFileName("/test.py");

        PyLexer lexer;
        lexer.setContext(&context);
        lexer.setBuffer(code.c_str(), code.length());
        std::vector<Token> tks;
        while (true) {
            tks.push_back(lexer.lex());
            if (tks.back() == TK_EOP)
                break;
            if (tks.back() != TK_NEWLINE
                    && tks.back() != TK_INDENT
                    && tks.back() != TK_DEDENT) {
                // Do I care for location of these tokens?
                locs_.push_back(lexer.tokenLoc());
            }
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
        locs_.clear();
    }

    bool dumpTokens_ { false };
    bool dumpLocs_ { false };
    std::vector<SourceLoc> locs_;
};

void PyLexer::PyLexerTest::testCase1()
{
    auto tks = core(R"raw(
the_world_is_flat = 1
if the_world_is_flat:
    print "Be careful not to fall off!"
)raw");

    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_NEWLINE,
        TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE, TK_INDENT,
        TK_PRINT, TK_STRING_LITERAL, TK_NEWLINE, TK_DEDENT, TK_EOP
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
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON,
        TK_NEWLINE, TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON,
        TK_NEWLINE, TK_INDENT, TK_PASS, TK_NEWLINE, TK_DEDENT,
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
        TK_IMPORT, TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER, TK_NEWLINE,
        TK_IDENTIFIER, TK_EQUAL, TK_IDENTIFIER, TK_DOT, TK_IDENTIFIER,
        TK_LBRACKET, TK_STRING_LITERAL, TK_RBRACKET, TK_DOT, TK_IDENTIFIER,
        TK_LPAREN, TK_RPAREN, TK_NEWLINE, TK_EOP
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
        TK_AT_SYMBOL, TK_IDENTIFIER, TK_NEWLINE, TK_FUNC,
        TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER,
        TK_RPAREN, TK_COLON, TK_PASS, TK_NEWLINE, TK_EOP
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
        TK_IDENTIFIER, TK_FUNC, TK_IDENTIFIER,
        TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE, TK_INDENT, TK_FUNC,
        TK_IDENTIFIER, TK_LPAREN, TK_IDENTIFIER, TK_RPAREN, TK_COLON,
        TK_NEWLINE, TK_INDENT, TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL,
        TK_NEWLINE, TK_DEDENT, TK_IF, TK_INTEGER_LITERAL, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_IDENTIFIER,
        TK_PLUS_EQUAL, TK_INTEGER_LITERAL, TK_NEWLINE, TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase31()
{
    auto tks = core(R"raw(
a = [1, 2,
     3, 4]
)raw");

    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_LBRACKET, TK_INTEGER_LITERAL, TK_COMMA,
        TK_INTEGER_LITERAL, TK_COMMA, TK_INTEGER_LITERAL, TK_COMMA,
        TK_INTEGER_LITERAL, TK_RBRACKET, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase32()
{
    auto tks = core(R"raw(
if a:
    print [1, 2,
3, 4]
)raw");

    std::vector<Token> expected {
        TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE, TK_INDENT, TK_PRINT,
        TK_LBRACKET, TK_INTEGER_LITERAL, TK_COMMA, TK_INTEGER_LITERAL,
        TK_COMMA, TK_INTEGER_LITERAL, TK_COMMA, TK_INTEGER_LITERAL,
        TK_RBRACKET, TK_NEWLINE, TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase33()
{
    auto tks = core(R"raw(
if a:
    print [1, 2,
           3, 4]
)raw");

    std::vector<Token> expected {
        TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE, TK_INDENT, TK_PRINT,
        TK_LBRACKET, TK_INTEGER_LITERAL, TK_COMMA, TK_INTEGER_LITERAL,
        TK_COMMA, TK_INTEGER_LITERAL, TK_COMMA, TK_INTEGER_LITERAL,
        TK_RBRACKET, TK_NEWLINE, TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase34()
{
    auto tks = core(R"raw(
if a:
    print 1
else:
    print 2
)raw");

    std::vector<Token> expected {
        TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE, TK_INDENT,
        TK_PRINT, TK_INTEGER_LITERAL, TK_NEWLINE, TK_DEDENT,
        TK_ELSE, TK_COLON, TK_NEWLINE, TK_INDENT, TK_PRINT,
        TK_INTEGER_LITERAL, TK_NEWLINE, TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase35()
{
    auto tks = core(R"raw(
""" triple-quoted string """
)raw");

    std::vector<Token> expected {
        TK_STRING_LITERAL, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase36()
{
    auto tks = core(R"raw(
""" triple-quoted "a" string """
)raw");

    std::vector<Token> expected {
        TK_STRING_LITERAL, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase37()
{
    auto tks = core(R"raw(
" regular \"a\" string "
)raw");

    std::vector<Token> expected {
        TK_STRING_LITERAL, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase38()
{
    auto tks = core(R"raw(
" regular string "
)raw");

    std::vector<Token> expected {
        TK_STRING_LITERAL, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase39()
{
    auto tks = core(R"raw(
if a:
    # Comment
    print a
)raw");

    std::vector<Token> expected {
        TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE, TK_INDENT,
        TK_PRINT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase40()
{
    auto tks = core(R"raw(
def f():
    a = b

def g():
    a = b
)raw");

    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_EQUAL, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT,
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_EQUAL, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT,
        TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase41()
{
    auto tks = core(R"raw(
try:
    print 1
except A, a:
    print 2
except B, b:
    print 3
    print 4
    if (c):
         print 5
finally:
    print 6
)raw");

    std::vector<Token> expected {
        TK_TRY, TK_COLON, TK_NEWLINE, TK_INDENT, TK_PRINT, TK_INTEGER_LITERAL,
        TK_NEWLINE, TK_DEDENT, TK_CATCH, TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER,
        TK_COLON, TK_NEWLINE, TK_INDENT, TK_PRINT, TK_INTEGER_LITERAL,
        TK_NEWLINE, TK_DEDENT, TK_CATCH, TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER,
        TK_COLON, TK_NEWLINE, TK_INDENT, TK_PRINT, TK_INTEGER_LITERAL, TK_NEWLINE,
        TK_PRINT, TK_INTEGER_LITERAL, TK_NEWLINE, TK_IF, TK_LPAREN,
        TK_IDENTIFIER, TK_RPAREN, TK_COLON, TK_NEWLINE, TK_INDENT, TK_PRINT,
        TK_INTEGER_LITERAL, TK_NEWLINE, TK_DEDENT, TK_DEDENT, TK_FINALLY, TK_COLON,
        TK_NEWLINE, TK_INDENT, TK_PRINT, TK_INTEGER_LITERAL, TK_NEWLINE, TK_DEDENT,
        TK_EOP
    };
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
}

void PyLexer::PyLexerTest::testCase42()
{
    auto tks = core(R"raw(
def f():
    if a:
        if b:
            x
    if a:
        x
)raw");

    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_DEDENT,
        TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_DEDENT,
        TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase43()
{
    auto tks = core(R"raw(
def f():
    if a:
        if b:
            x


    if a:
        x
)raw");

    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_DEDENT,
        TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_DEDENT,
        TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase44()
{
    auto tks = core(R"raw(
def f():
    if a:
        if b:
            x
def g():
    pass
)raw");

    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_DEDENT, TK_DEDENT,
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_PASS, TK_NEWLINE, TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase45()
{
    auto tks = core(R"raw(
def f():
    if a:
        if b:
            x


def g():
    pass
)raw");

    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IF, TK_IDENTIFIER, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_NEWLINE, TK_DEDENT, TK_DEDENT, TK_DEDENT,
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_PASS, TK_NEWLINE, TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase46()
{
    auto tks = core(R"raw(
def f():
    a = 1

    b = 1
)raw");

    std::vector<Token> expected {
        TK_FUNC, TK_IDENTIFIER, TK_LPAREN, TK_RPAREN, TK_COLON, TK_NEWLINE,
        TK_INDENT, TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_NEWLINE,
        TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_NEWLINE,
        TK_DEDENT, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase47()
{
    auto tks = core("x = 0xffffffffL");

    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase48()
{
    auto tks = core("x = 1L");

    std::vector<Token> expected {
        TK_IDENTIFIER, TK_EQUAL, TK_INTEGER_LITERAL, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase49()
{
    auto tks = core(R"raw(
from a import \
     b, \
     c, \
     d
)raw");

    std::vector<Token> expected {
        TK_FROM, TK_IDENTIFIER, TK_IMPORT, TK_IDENTIFIER, TK_COMMA,
        TK_IDENTIFIER, TK_COMMA, TK_IDENTIFIER, TK_NEWLINE, TK_EOP
    };
    UAISO_EXPECT_INT_EQ(expected.size(), tks.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, tks);
}

void PyLexer::PyLexerTest::testCase50()
{
    core(R"raw(
import abc
import xyz.mnop


import fancy   .     stuff

def foobar( ):
    cool_name = "foo bar"
    id        = 1
    if alright:
        print 1234

if __name__ == '__main__'
    run(  arg1   ,arg2   )
)raw");

    std::vector<SourceLoc> expected {
        SourceLoc(1, 0, 1, 6, ""),                    // import
        SourceLoc(1, 7, 1, 10, ""),                   // abc
        SourceLoc(2, 0, 2, 6, ""),
        SourceLoc(2, 7, 2, 10, ""),
        SourceLoc(2, 10, 2, 11, ""),
        SourceLoc(2, 11, 2, 15, ""),
        SourceLoc(5, 0, 5, 6, ""),
        SourceLoc(5, 7, 5, 12, ""),
        SourceLoc(5, 15, 5, 16, ""),
        SourceLoc(5, 21, 5, 26, ""),                  // stuff
        SourceLoc(7, 0, 7, 3, ""),                    // def
        SourceLoc(7, 4, 7, 10, ""),
        SourceLoc(7, 10, 7, 11, ""),
        SourceLoc(7, 12, 7, 13, ""),
        SourceLoc(7, 13, 7, 14, ""),
        SourceLoc(8, 4, 8, 13, ""),                   // cool_name
        SourceLoc(8, 14, 8, 15, ""),
        SourceLoc(8, 16, 8, 25, ""),
        SourceLoc(9, 4, 9, 6, ""),
        SourceLoc(9, 14, 9, 15, ""),
        SourceLoc(9, 16, 9, 17, ""),
        SourceLoc(10, 4, 10, 6, ""),
        SourceLoc(10, 7, 10, 14, ""),
        SourceLoc(10, 14, 10, 15, ""),
        SourceLoc(11, 8, 11, 13, ""),                 // print
        SourceLoc(11, 14, 11, 18, ""),
        SourceLoc(13, 0, 13, 2, ""),                  // if
        SourceLoc(13, 3, 13, 11, ""),
        SourceLoc(13, 12, 13, 14, ""),
        SourceLoc(13, 15, 13, 25, ""),
        SourceLoc(14, 4, 14, 7, ""),
        SourceLoc(14, 7, 14, 8, ""),
        SourceLoc(14, 10, 14, 14, ""),
        SourceLoc(14, 17, 14, 18, ""),
        SourceLoc(14, 18, 14, 22, ""),
        SourceLoc(14, 25, 14, 26, "")
    };
    UAISO_EXPECT_INT_EQ(expected.size(), locs_.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, locs_);
}

void PyLexer::PyLexerTest::testCase51()
{
    core(R"raw(
import abc

# a comment
# and another


def foobar( ):
    if alright:
        print 1234
    print 1234
)raw");

    std::vector<SourceLoc> expected {
        SourceLoc(1, 0, 1, 6, ""),                    // import
        SourceLoc(1, 7, 1, 10, ""),                   // abc
        SourceLoc(7, 0, 7, 3, ""),                    // def
        SourceLoc(7, 4, 7, 10, ""),
        SourceLoc(7, 10, 7, 11, ""),
        SourceLoc(7, 12, 7, 13, ""),
        SourceLoc(7, 13, 7, 14, ""),
        SourceLoc(8, 4, 8, 6, ""),
        SourceLoc(8, 7, 8, 14, ""),
        SourceLoc(8, 14, 8, 15, ""),
        SourceLoc(9, 8, 9, 13, ""),                   // print
        SourceLoc(9, 14, 9, 18, ""),
        SourceLoc(10, 4, 10, 9, ""),
        SourceLoc(10, 10, 10, 14, ""),
    };
    UAISO_EXPECT_INT_EQ(expected.size(), locs_.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, locs_);
}

void PyLexer::PyLexerTest::testCase52()
{
    core(R"raw(
""" a string that goes accross
a few
lines and eventually ends """

if abc:
    if xyz:
        print 1234
)raw");

    std::vector<SourceLoc> expected {
        SourceLoc(1, 0, 3, 29, ""),                   // string literal
        SourceLoc(5, 0, 5, 2, ""),                    // if
        SourceLoc(5, 3, 5, 6, ""),
        SourceLoc(5, 6, 5, 7, ""),
        SourceLoc(6, 4, 6, 6, ""),                    // if
        SourceLoc(6, 7, 6, 10, ""),
        SourceLoc(6, 10, 6, 11, ""),
        SourceLoc(7, 8, 7, 13, ""),
        SourceLoc(7, 14, 7, 18, "")
    };
    UAISO_EXPECT_INT_EQ(expected.size(), locs_.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, locs_);
}

void PyLexer::PyLexerTest::testCase53()
{
    core(R"raw(
print 1

from a import \
     b, \
     c, \
     d

print 2
)raw");

    std::vector<SourceLoc> expected {
        SourceLoc(1, 0, 1, 5, ""),                    // print
        SourceLoc(1, 6, 1, 7, ""),
        SourceLoc(3, 0, 3, 4, ""),                    // from
        SourceLoc(3, 5, 3, 6, ""),
        SourceLoc(3, 7, 3, 13, ""),
        SourceLoc(4, 5, 4, 6, ""),
        SourceLoc(4, 6, 4, 7, ""),
        SourceLoc(5, 5, 5, 6, ""),
        SourceLoc(5, 6, 5, 7, ""),
        SourceLoc(6, 5, 6, 6, ""),
        SourceLoc(8, 0, 8, 5, ""),                    // print
        SourceLoc(8, 6, 8, 7, "")
    };
    UAISO_EXPECT_INT_EQ(expected.size(), locs_.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, locs_);
}

void PyLexer::PyLexerTest::testCase54()
{
    auto tks = core(R"raw(
__abc__ = """ triple-quoted "a" string """
)raw");

    std::vector<SourceLoc> expected {
        SourceLoc(1, 0, 1, 7, ""),
        SourceLoc(1, 8, 1, 9, ""),
        SourceLoc(1, 10, 1, 42, "")
    };
    UAISO_EXPECT_INT_EQ(expected.size(), locs_.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, locs_);
}

void PyLexer::PyLexerTest::testCase55()
{
    auto tks = core(R"raw(
__abc__ = """ triple-quoted "a" string

bla bla bla
bla bla bla
"""
)raw");

    std::vector<SourceLoc> expected {
        SourceLoc(1, 0, 1, 7, ""),
        SourceLoc(1, 8, 1, 9, ""),
        SourceLoc(1, 10, 5, 3, "")
    };
    UAISO_EXPECT_INT_EQ(expected.size(), locs_.size());
    UAISO_EXPECT_CONTAINER_EQ(expected, locs_);
}

MAKE_CLASS_TEST(PyLexer)
