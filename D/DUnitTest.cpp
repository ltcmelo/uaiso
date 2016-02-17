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

#include "D/DUnit.h"
#include "Parsing/UnitTest.h"

using namespace uaiso;

class DUnit::DUnitTest final : public Unit::UnitTest
{
public:
    TEST_RUN(DUnitTest
             , &DUnitTest::testCase1
             , &DUnitTest::testCase2
             , &DUnitTest::testCase3
             , &DUnitTest::testCase4
             , &DUnitTest::testCase5
             , &DUnitTest::testCase6
             , &DUnitTest::testCase7
             , &DUnitTest::testCase8
             , &DUnitTest::testCase9
             , &DUnitTest::testCase10
             , &DUnitTest::testCase11
             , &DUnitTest::testCase12
             , &DUnitTest::testCase13
             , &DUnitTest::testCase14
             , &DUnitTest::testCase15
             , &DUnitTest::testCase16
             , &DUnitTest::testCase17
             )

    const std::string baseCode() const
    {
        return R"raw(
            // Code from http://ddili.org/ders/d.en/
            module teste;
            import std.stdio;
            struct Point
            {
                size_t id;
                int line;
                int column;
                static size_t nextId;
            }                                    // line 10
        )raw";
    }

    void testCase1()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id = Point.nextId;
                ++Point.nextId;
                return Point(id, line, column);
            }

            void main()
            {
                auto top = makePoint(7, 0);
                auto middle = makePoint(8, 0);
                auto bottom =  makePoint(9, 0);

                writeln(top.id);
                writeln(middle.id);
                writeln(bottom.id);
            }
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase2()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id = Point.nextId;
                foo foo foo
            }
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase3()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id = Point.nextId;
                foo foo foo foo foo
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase4()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id = Point.nextId;
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase5()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id =
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase6()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id =
            }
        )raw";

    }

    void testCase7()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id =
                size_t id = 3;
            }
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase8()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id =
                foo foo foo foo
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase9()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id =
                foo foo
            }
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase10()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase11()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line,
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase12()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase13()
    {
        std::string code = R"raw(
            Point makePoint(int line, int column)
            {
                size_t id = Point.nextId;
                size_t id = Point.
            }
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase14()
    {
        std::string code = baseCode() + R"raw(
            Point makePoint(int line, int column)
            {
                size_t id = Point.nextId;
                size_t id = Point.nextId
            }
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase15()
    {
        std::string code = baseCode() + R"raw(
            Point dummy() {}
            int i, j
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase16()
    {
        std::string code = baseCode() + R"raw(
            Point dummy() {}
            int i =
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }

    void testCase17()
    {
        std::string code = baseCode() + R"raw(
            Point dummy() {}
            int
        )raw";

        runCore(FactoryCreator::create(LangId::D), code);
    }
};

MAKE_CLASS_TEST(DUnit)
