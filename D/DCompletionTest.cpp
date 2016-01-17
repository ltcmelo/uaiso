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

#include "Semantic/CompletionTest.h"
#include "Parsing/Factory.h"
#include "Parsing/Unit.h"

using namespace uaiso;

void CompletionProposer::CompletionProposerTest::DTestCase1()
{
    std::string code = R"raw(                    // line 0
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 10
            size_t a = Point.nextId;
            size_t b =
            //        ^
            //        |
            //        complete at up-arrow
        }
    )raw";

    lineCol_ = { 12, 22 };
    auto expected = { "paramLine", "paramColumn", "a", "b", "global", "makePoint",
                      "Point" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase2()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            size_t a = Point.nextId;
        //                          ^
        //                          |
        //                          complete at up-arrow
        }
    )raw";

    lineCol_ = { 13, 36 };
    auto expected = { "paramLine", "paramColumn", "a", "global", "makePoint", "Point" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase3()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }

        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            size_t id = Point.nextId;
            size_t id = Point.
            //                ^
            //                |
            //                complete at up-arrow
        }
    )raw";

    lineCol_ = { 14, 30 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase4()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }

        Point foo() {}
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 13
            otherFunc(
            //        ^
            //        |
            //        complete at up-arrow
        }
    )raw";

    lineCol_ = { 14, 22 };
    auto expected = { "paramLine", "paramColumn", "Point", "makePoint", "foo" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase5()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }

        Unknown foo() {}
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 13
            otherFunc(paramLine,
            //                  ^
            //                  |
            //                  complete at up-arrow
        }
    )raw";

    lineCol_ = { 14, 32 };
    auto expected = { "paramLine", "paramColumn", "Point", "makePoint", "foo" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}


void CompletionProposer::CompletionProposerTest::DTestCase6()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }

        Unknown foo() {}
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 13
            otherFunc(paramLine, paramColumn);
            //                                ^
            //                                |
            //                                complete at up-arrow
        }
    )raw";

    lineCol_ = { 14, 46 };
    auto expected = { "paramLine", "paramColumn", "Point", "makePoint", "foo" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase7()
{
    std::string code = R"raw(                    // line 0
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 10
            size_t a = Point.nextId;
            size_t b =
            //        ^
            //        |
            //        complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 12, 22 };
    auto expected = { "paramLine", "paramColumn", "a", "b", "global", "makePoint",
                      "Point" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase8()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            size_t a = Point.nextId;
            //                      ^
            //                      |
            //                      complete at up-arrow

        // No closing `}`
    )raw";

    lineCol_ = { 13, 36 };
    auto expected = { "paramLine", "paramColumn", "a", "global", "makePoint", "Point" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase9()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            Point point;
            point.id;
            point.
            //    ^
            //    |
            //    complete at up-arrow

        // No closing `}`
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase10()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            Point.
            //    ^
            //    |
            //    complete at up-arrow

        // No closing `}`
    )raw";

    lineCol_ = { 13, 18 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase11()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            Point point;
            point.id;
            point.
            //    ^
            //    |
            //    complete at up-arrow
        }
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase12()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            Point.
            //    ^
            //    |
            //    complete at up-arrow
        }
    )raw";

    lineCol_ = { 13, 18 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase13()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            Point point = new Point();
            point.id;
            point.
            //    ^
            //    |
            //    complete at up-arrow

        // No closing `}`
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase14()
{
    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            Point point = new Point();
            point.id;
            point.
            //    ^
            //    |
            //    complete at up-arrow
            foo foo foo foo foo foo

        // No closing `}`
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}


void CompletionProposer::CompletionProposerTest::DTestCase15()
{
    UAISO_SKIP_TEST; // TODO: Case not implemented yet.

    std::string code = R"raw(                    // line 0
        module teste;                            // line 1
        import std.stdio;
        struct Point
        {
            size_t id;
            int line;
            int column;
            static size_t nextId;
        }
        int global;
        Point makePoint(int paramLine, int paramColumn)
        {                                        // line 12
            Point point1 = new Point();
            Point point2 = new
            //                ^
            //                |
            //                complete at up-arrow

        // No closing `}`
    )raw";

    lineCol_ = { 14, 30 };
    auto expected = { "id", "line", "column", "nextId" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase16()
{
    std::string code = R"raw(                    // line 0
        class Widget                             // line 1
        {
            string name;
            uint width, height;
            void changeName(string another)
            {
                name = another;
            }                                    // line 8
            final void quadrupleSize() {
            //                          ^
            //                          |
            //                          complete at up-arrow
            }
        }
    )raw";

    lineCol_ = { 9, 40 };
    auto expected = { "width", "height", "name", "quadrupleSize",
                      "changeName", "Widget" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase17()
{
    std::string code = R"raw(                    // line 0
        class Widget                             // line 1
        {
            string name;
            uint width, height;
            void changeName(string another)
            {
                name = another;
            }                                    // line 8
            final void quadrupleSize() {
                int abc;
            //          ^
            //          |
            //          complete at up-arrow
            }
        }
    )raw";

    lineCol_ = { 10, 24 };
    auto expected = { "width", "height", "name", "quadrupleSize",
                      "changeName", "Widget", "abc" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase18()
{
    std::string code = R"raw(                    // line 0
        class Widget                             // line 1
        {
            string name;
            uint width, height;
            void changeName(string another)
            {
                name = another;
            }                                    // line 8
            final void quadrupleSize() {
                int abc;
            //          ^
            //          |
            //          complete at up-arrow

            // No closing `}`
        }
    )raw";

    lineCol_ = { 10, 24 };
    auto expected = { "width", "height", "name", "quadrupleSize",
                      "changeName", "Widget", "abc" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}

void CompletionProposer::CompletionProposerTest::DTestCase19()
{
    std::string code = R"raw(                    // line 0
        class Widget                             // line 1
        {
            string name;
            uint width, height;
            void changeName(string another)
            {
                name = another;
            }                                    // line 8
            final void quadrupleSize() {
                int abc;
            //          ^
            //          |
            //          complete at up-arrow

            // No closing `}` for function

        // No closing `}` for class
    )raw";

    lineCol_ = { 10, 24 };
    auto expected = { "width", "height", "name", "quadrupleSize",
                      "changeName", "Widget", "abc" };
    runCore(FactoryCreator::create(LangId::D),
            code, "/path/to/file/dummy.d", expected);
}
