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

#include "Semantic/TypeCheckerTest.h"
#include "Parsing/Factory.h"
#include "Parsing/Unit.h"

using namespace uaiso;

void TypeChecker::TypeCheckerTest::GoTestCase1()
{
    std::string code = R"raw(
        package main
        import "fmt"
        var i, j int = 1, 2
        func main() {
            var c, python, java = true, false, "no!"
            fmt.Println(i, j, c, python, java)
        }
    )raw";

    runCore(FactoryCreator::create(LangId::Go), code,
            "/from/go/tour/code.go", std::vector<Diagnostic::Code>(),
            std::make_pair("", Type::Kind::Void));
}

void TypeChecker::TypeCheckerTest::GoTestCase2()
{
    std::string code = R"raw(
        package main
        import "fmt"
        var i, j int = 1, "hey"
        func main() {
            var c, python, java = true, false, "no!"
            fmt.Println(i, j, c, python, java)
        }
    )raw";

    auto expected = { Diagnostic::IncompatibleAssignment };
    runCore(FactoryCreator::create(LangId::Go), code, "/from/go/tour/code.go",
            expected, std::make_pair("", Type::Kind::Void));
}

void TypeChecker::TypeCheckerTest::GoTestCase3()
{
    std::string code = R"raw(
        package main
        import "fmt"
        var i, j int = "hi", "hey"
        func main() {
            var c, python, java = true, false, "no!"
            fmt.Println(i, j, c, python, java)
        }
    )raw";

    auto expected = { Diagnostic::IncompatibleAssignment,
                      Diagnostic::IncompatibleAssignment };
    runCore(FactoryCreator::create(LangId::Go), code, "/from/go/tour/code.go",
            expected, std::make_pair("", Type::Kind::Void));
}

void TypeChecker::TypeCheckerTest::GoTestCase4()
{
    std::string code = R"raw(
        package main
        import "fmt"
        var inferred = 10
        func main() {
            fmt.Println(inferred)
        }
    )raw";

    runCore(FactoryCreator::create(LangId::Go), code,
            "/from/go/tour/code.go", std::vector<Diagnostic::Code>(),
            std::make_pair("inferred", Type::Kind::Int));
}

void TypeChecker::TypeCheckerTest::GoTestCase5()
{
    std::string code = R"raw(
        package main
        func main() {
            var a [2]int
            a[0]
        }
    )raw";

    runCore(FactoryCreator::create(LangId::Go), code, "/from/go/tour/code.go",
            std::vector<Diagnostic::Code>(),
            std::make_pair("", Type::Kind::Void));
}

void TypeChecker::TypeCheckerTest::GoTestCase6()
{
    std::string code = R"raw(
        package main
        func main() {
            var a [2]int
            var b string
            a[b]
        }
    )raw";

    auto expected = { Diagnostic::IntegerValueExpected };
    runCore(FactoryCreator::create(LangId::Go), code, "/from/go/tour/code.go",
            expected,
            std::make_pair("", Type::Kind::Void));
}

void TypeChecker::TypeCheckerTest::GoTestCase7()
{
    std::string code = R"raw(
        package main
        func main() {
            var a [2]int
            var b int
            a[b]
        }
    )raw";

    runCore(FactoryCreator::create(LangId::Go), code, "/from/go/tour/code.go",
            std::vector<Diagnostic::Code>(),
            std::make_pair("", Type::Kind::Void));
}


void TypeChecker::TypeCheckerTest::GoTestCase8()
{
    std::string code = R"raw(
        package main
        func main() {
            var a map[string]int
            var b string
            a[b]
        }
    )raw";

    runCore(FactoryCreator::create(LangId::Go), code, "/from/go/tour/code.go",
            std::vector<Diagnostic::Code>(),
            std::make_pair("", Type::Kind::Void));
}
