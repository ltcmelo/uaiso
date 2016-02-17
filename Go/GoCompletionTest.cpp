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

#include "Semantic/CompletionTest.h"
#include "Parsing/Factory.h"
#include "Parsing/Unit.h"

using namespace uaiso;

/*
    Note: Request for completion must be immediately after the triggering
    token, so for consistency we have to consider where an "auto" semi-colon
    would be inserted (if it's the case) and request the parser to stop at
    the following column.
*/

void CompletionProposer::CompletionProposerTest::GoTestCase1()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) {
            v := Point{1, 2}                     // line 13
            w :=
            //  ^
            //  |
            //  complete at up-arrow
    )raw";

    lineCol_ = { 14, 16 };
    auto expected = { "toBe", "max", "zero", "Point", "calc", "makePoint",
                      "paramLine", "paramColumn", "w", "v" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase2()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) {
            v := Point{1, 2};
            //               ^
            //               |
            //  complete at up-arrow
        }
    )raw";

    lineCol_ = { 13, 29 };
    auto expected = { "toBe", "max", "zero", "Point", "calc", "makePoint",
                      "paramLine", "paramColumn", "v" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase3()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) {
            v := Point{1, 2}
            //               ^
            //               |
            //               complete at up-arrow (see auto-semicolon note)
        }
    )raw";

    lineCol_ = { 13, 29 };
    auto expected = { "toBe", "max", "zero", "Point", "calc", "makePoint",
                      "paramLine", "paramColumn", "v" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase4()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) {
            v := Point{1, 2};
            //               ^
            //               |
            //               complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 13, 29 };
    auto expected = { "toBe", "max", "zero", "Point", "calc", "makePoint",
                      "paramLine", "paramColumn", "v" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase5()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) {
            v := Point{1, 2}
            //               ^
            //               |
            //               complete at up-arrow (see auto-semicollon note)

            // No closing `}`
    )raw";

    lineCol_ = { 13, 29 };
    auto expected = { "toBe", "max", "zero", "Point", "calc", "makePoint",
                      "paramLine", "paramColumn", "v" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase6()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
            id, nextId int
        }                                        // line 11
        func calc(total int) {}                  // line 12
        func makePoint(paramLine, paramColumn int) {
            var point Point
            point.
            //    ^
            //    |
            //    complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "line", "column", "id", "nextId" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase7()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
            id, nextId int
        }                                        // line 11
        func calc(total int) {}                  // line 12
        func makePoint(paramLine, paramColumn int) {
            var point Point = Point{1, 2, 3, 4}
            point.
            //    ^
            //    |
            //    complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "line", "column", "id", "nextId" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase8()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
            id, nextId int
        }                                        // line 11
        func calc(total int) {}                  // line 12
        func makePoint(paramLine, paramColumn int) {
            var point = Point{1, 2, 3, 4}
            point.
            //    ^
            //    |
            //    complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "line", "column", "id", "nextId" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase9()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
            id, nextId int
        }                                        // line 11
        func calc(total int) {}                  // line 12
        func makePoint(paramLine, paramColumn int) {
            point := Point{1, 2, 3, 4}
            point.
            //    ^
            //    |
            //    complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 15, 18 };
    auto expected = { "line", "column", "id", "nextId" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase10()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
            id, nextId int
        }                                        // line 11
        func calc(total int) {}                  // line 12
        var point = Point{1, 2, 3, 4}
        func makePoint(paramLine, paramColumn int) {
            otherPoint := Point{5, 6, 7, 8}      // line 15
            point.
            //    ^
            //    |
            //    complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 16, 18 };
    auto expected = { "line", "column", "id", "nextId" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase11()
{
    UAISO_SKIP_TEST; // TODO: Take a look...

    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
            id, nextId int
        }                                        // line 11
        func calc(total int) {}                  // line 12
        var point = Point{1, 2, 3, 4}
        func makePoint(paramLine, paramColumn int) {
            calc(
            //   ^
            //   |
            //   complete at up-arrow

            // No closing `}`
    )raw";

    lineCol_ = { 15, 17 };
    auto expected = { "line", "column", "id", "nextId" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase12()
{
    std::string code = R"raw(                    // line 0
        package main
        import . "pack"
        func main() {
            v := Point{1, 2}
            //               ^
            //               |
            //               complete at up-arrow
        }
    )raw";

    lineCol_ = { 4, 29 };
    auto expected = { "v", "main",
                      // From pack/file.go
                      "abcXyz", "fooBar", "barFoo", "here", "there", "State",
                      "Formatter", "Stringer", "Write", "WriteString", "pool",
                      "Printf", "Sprintf", "Errorf"
                    };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase13()
{
    std::string code = R"raw(
        package main
        import "pack"
        func main() {
            pack.
        //       ^
        //       |
        //       complete at up-arrow
        }
    )raw";

    lineCol_ = { 4, 17 };
    auto expected = { "abcXyz", "fooBar", "barFoo", "here", "there", "State",
                      "Formatter", "Stringer", "Write", "WriteString", "pool",
                      "Printf", "Sprintf", "Errorf"
                    };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase14()
{
    std::string code = R"raw(
        package main
        import cool "pack"
        func main() {
            cool.
        //       ^
        //       |
        //       complete at up-arrow
        }
    )raw";

    lineCol_ = { 4, 17 };
    auto expected = { "abcXyz", "fooBar", "barFoo", "here", "there", "State",
                      "Formatter", "Stringer", "Write", "WriteString", "pool",
                      "Printf", "Sprintf", "Errorf"
                    };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase15()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) {
            Println(identifier)
            //                  ^
            //                  |
            //                  complete at up-arrow (see auto-semicollon note)

            // No closing `}`
    )raw";

    lineCol_ = { 13, 32 };
    auto expected = { "toBe", "max", "zero", "Point", "calc", "makePoint",
                      "paramLine", "paramColumn", };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase16()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) {
            Println("literal")
            //                 ^
            //                 |
            //                 complete at up-arrow (see auto-semicollon note)

            // No closing `}`
    )raw";

    lineCol_ = { 13, 31 };
    auto expected = { "toBe", "max", "zero", "Point", "calc", "makePoint",
                      "paramLine", "paramColumn", };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}

void CompletionProposer::CompletionProposerTest::GoTestCase17()
{
    std::string code = R"raw(                    // line 0
        package main                             // line 1
        var toBe bool
        var (
            max uint64 = 1 << 64 - 1
            zero = 0
        )
        type Point struct {
            line int
            column int
        }                                        // line 10
        func calc(total int) {}                  // line 11
        func makePoint(paramLine, paramColumn int) Point {
            return Point{paramLine, paramColumn}
        }
        func main() {                            // line 15
            p := makePoint(6, 7)
            p.
        //    ^
        //    |
        //    complete at up-arrow
        }
    )raw";

    lineCol_ = { 17, 14 };
    auto expected = { "column", "line" };
    runCore(FactoryCreator::create(LangId::Go),
            code, "/path/to/file/go.d", expected);
}
