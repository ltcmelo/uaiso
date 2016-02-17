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

#include "Go/GoUnit.h"
#include "Parsing/UnitTest.h"

using namespace uaiso;

class GoUnit::GoUnitTest final : public Unit::UnitTest
{
public:
    TEST_RUN(GoUnitTest
             , &GoUnitTest::testCase1
             , &GoUnitTest::testCase2
             , &GoUnitTest::testCase3
             , &GoUnitTest::testCase4
             , &GoUnitTest::testCase5
             , &GoUnitTest::testCase6
             , &GoUnitTest::testCase7
             , &GoUnitTest::testCase8
             , &GoUnitTest::testCase9
             , &GoUnitTest::testCase10
             , &GoUnitTest::testCase11
             , &GoUnitTest::testCase12
             , &GoUnitTest::testCase13
             , &GoUnitTest::testCase14
             , &GoUnitTest::testCase15
             , &GoUnitTest::testCase16
             , &GoUnitTest::testCase17
             , &GoUnitTest::testCase18
             , &GoUnitTest::testCase19
             , &GoUnitTest::testCase20
             , &GoUnitTest::testCase21
             , &GoUnitTest::testCase22
             , &GoUnitTest::testCase23
             , &GoUnitTest::testCase24
             , &GoUnitTest::testCase25
             , &GoUnitTest::testCase26
             , &GoUnitTest::testCase27
             , &GoUnitTest::testCase28
             , &GoUnitTest::testCase29
             , &GoUnitTest::testCase30
             , &GoUnitTest::testCase31
             )

    const std::string baseCode() const
    {
        return R"raw(
            // Code from gotour.
            package main
            import "fmt"
            type Vertex struct {
                Lat, Long float64
                X int
                Y int
            }

            var m = map[string]Vertex{
                "Bell Labs": Vertex{
                        40.68433, -74.39967,
                },
                "Google": Vertex{
                        37.42202, -122.08408,
                },
            }
        )raw";
    }

    void testCase1()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{1, 2}
                v.X = 4
                fmt.Println(v.X)
                m = make(map[string]Vertex)
                m["Bell Labs"] = Vertex{
                        40.68433, -74.39967,
                }
                fmt.Println(m["Bell Labs"])
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase2()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{1, 2}
                foo foo foo
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase3()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{1, 2}
                foo foo foo foo foo
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase4()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{1, 2}
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase5()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v :=
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase6()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{1,
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase7()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase8()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase9()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Ver
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase10()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v :=
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase11()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                q :=
                v := Vertex{1, 2}
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase12()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{1,
                foo foo foo foo foo
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase13()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                v := Vertex{1,
                foo foo foo foo foo
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase14()
    {
        std::string code = baseCode() + R"raw(
            func other(abc int) {
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase15()
    {
        std::string code = baseCode() + R"raw(
            func other(abc int)
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase16()
    {
        std::string code = baseCode() + R"raw(
            func other(abc int
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase17()
    {
        std::string code = baseCode() + R"raw(
            func other(abc
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase18()
    {
        std::string code = baseCode() + R"raw(
            func other(
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase19()
    {
        std::string code = baseCode() + R"raw(
            func other
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase20()
    {
        std::string code = baseCode() + R"raw(
            func other(abc int,
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase21()
    {
        std::string code = baseCode() + R"raw(
            func other(abc int, xyz
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase22()
    {
        std::string code = baseCode() + R"raw(
            func other(abc int, xyz int
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase23()
    {
        std::string code = baseCode() + R"raw(
            func dummy() {}
            var i, j
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase24()
    {
        std::string code = baseCode() + R"raw(
            func dummy() {}
            var i, j int =
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase25()
    {
        std::string code = baseCode() + R"raw(
            func dummy() {}
            var i, j int =
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase26()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                sum := 1
                for ; sum < 1000
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase27()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                sum := 1
                for ; sum < 1000;
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase28()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                sum := 1
                for ; sum < 1000; {
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase29()
    {
        std::string code = baseCode() + R"raw(
            func main() {
                sum := 1
                for ; sum < 1000; {
                    sum
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase30()
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
            }
            func calc(total int) {}
            func makePoint(paramLine, paramColumn int) {
                v := Point{1, 2}                     // line 13
                w :=
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }

    void testCase31()
    {
        std::string code = R"raw(                    // line 0
            package main                             // line 1
            var toBe bool
            var (
                max uint64 = 1 << 64 - 1
                zero = 0
            )

                TRASH HERE

            type Point struct {
                line int
                column int
            }
            func calc(total int) {}
            func makePoint(paramLine, paramColumn int) {
                v := Point{1, 2}
            }
        )raw";

        runCore(FactoryCreator::create(LangId::Go), code);
    }
};

MAKE_CLASS_TEST(GoUnit)
