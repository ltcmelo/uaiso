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

#include "Semantic/BinderTest.h"
#include "Semantic/Program.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit.h"
#include <iostream>

using namespace uaiso;

void Binder::BinderTest::PyTestCase1()
{
    std::string code = R"raw(
a = 1
b = "val"
c, d = (10, "str")
e = call()
f = w
print nothing

def foo():
    foo_local = 10

class bar:
    bar_static = 1
    def bar_method():
        print 1
)raw";

    std::unique_ptr<Program> program(core(FactoryCreator::create(LangName::Py),
                                          code,
                                          "/test.py"));
    UAISO_EXPECT_TRUE(program);

    const Ident* a = nullptr;
    const Ident* b = nullptr;
    const Ident* c = nullptr;
    const Ident* d = nullptr;
    const Ident* e = nullptr;
    const Ident* f = nullptr;
    const Ident* k = nullptr;
    const Ident* w = nullptr;
    const Ident* foo = nullptr;
    const Ident* foo_local = nullptr;
    const Ident* bar = nullptr;
    const Ident* bar_static = nullptr;
    const Ident* bar_method = nullptr;
    const Ident* call = nullptr;
    const Ident* nothing = nullptr;

    auto tuples = lexemes_.list<Ident>("/test.py");
    for (auto tuple : tuples) {
        const Ident* ident = std::get<0>(tuple);
        if (ident->str() == "a")
            a = ident;
        else if (ident->str() == "b")
            b = ident;
        else if (ident->str() == "c")
            c = ident;
        else if (ident->str() == "d")
            d = ident;
        else if (ident->str() == "e")
            e = ident;
        else if (ident->str() == "f")
            f = ident;
        else if (ident->str() == "k")
            k = ident;
        else if (ident->str() == "w")
            w = ident;
        else if (ident->str() == "foo")
            foo = ident;
        else if (ident->str() == "foo_local")
            foo_local = ident;
        else if (ident->str() == "bar")
            bar = ident;
        else if (ident->str() == "bar_static")
            bar_static = ident;
        else if (ident->str() == "bar_method")
            bar_method = ident;
        else if (ident->str() == "call")
            call = ident;
        else if (ident->str() == "nothing")
            nothing = ident;
    }

    Environment env = program->env();
    UAISO_EXPECT_FALSE(env.isEmpty());

    // Global
    UAISO_EXPECT_TRUE(env.lookUpValue(a));
    UAISO_EXPECT_TRUE(env.lookUpValue(b));
    UAISO_EXPECT_TRUE(env.lookUpValue(c));
    UAISO_EXPECT_TRUE(env.lookUpValue(d));
    UAISO_EXPECT_TRUE(env.lookUpValue(e));
    UAISO_EXPECT_TRUE(env.lookUpValue(f));
    UAISO_EXPECT_TRUE(env.lookUpType(foo));
    UAISO_EXPECT_TRUE(env.lookUpType(bar));

    const Func* funcFoo = ConstFunc_Cast(env.lookUpType(foo));
    Environment fooEnv = funcFoo->env();
    UAISO_EXPECT_TRUE(fooEnv.lookUpValue(foo_local));
    UAISO_EXPECT_FALSE(env.lookUpValue(foo_local));
    UAISO_EXPECT_FALSE(env.lookUpType(foo_local));
    UAISO_EXPECT_TRUE(fooEnv.lookUpValue(a));

    const Record* classBar = ConstRecord_Cast(env.lookUpType(bar));
    Environment barEnv = classBar->type()->env();
    UAISO_EXPECT_TRUE(barEnv.lookUpValue(bar_static));
    UAISO_EXPECT_FALSE(env.lookUpValue(bar_static));
    UAISO_EXPECT_TRUE(barEnv.lookUpType(bar_method));
    UAISO_EXPECT_FALSE(env.lookUpType(bar_method));
    UAISO_EXPECT_TRUE(barEnv.lookUpValue(a));
}

void Binder::BinderTest::PyTestCase2()
{
    std::string code = R"raw(
for g in range(10):
    h = g * 2
    print h
)raw";

    std::unique_ptr<Program> program(core(FactoryCreator::create(LangName::Py),
                                          code,
                                          "/test.py"));
    UAISO_EXPECT_TRUE(program);

    const Ident* g = nullptr;
    const Ident* h = nullptr;

    auto tuples = lexemes_.list<Ident>("/test.py");
    for (auto tuple : tuples) {
        const Ident* ident = std::get<0>(tuple);
        if (ident->str() == "g")
            g = ident;
        else if (ident->str() == "h")
            h = ident;
    }

    UAISO_SKIP_TEST;

    Environment env = program->env();
    UAISO_EXPECT_FALSE(env.isEmpty());

    // Global
    UAISO_EXPECT_TRUE(env.lookUpValue(g));
    UAISO_EXPECT_TRUE(env.lookUpValue(h));
}

void Binder::BinderTest::PyTestCase3()
{
    std::string code = R"raw(
class bar:
    bar_static = 1
    def bar_method():
        self.bar_inst1 = 10
        self.bar_inst2 = "bar"
)raw";

    std::unique_ptr<Program> program(core(FactoryCreator::create(LangName::Py),
                                          code,
                                          "/test.py"));
    UAISO_EXPECT_TRUE(program);

    const Ident* bar = nullptr;
    const Ident* bar_static = nullptr;
    const Ident* bar_method = nullptr;
    const Ident* bar_inst1 = nullptr;
    const Ident* bar_inst2 = nullptr;
    const Ident* self = nullptr;

    auto tuples = lexemes_.list<Ident>("/test.py");
    for (auto tuple : tuples) {
        const Ident* ident = std::get<0>(tuple);
        if (ident->str() == "bar")
            bar = ident;
        else if (ident->str() == "bar_static")
            bar_static = ident;
        else if (ident->str() == "bar_method")
            bar_method = ident;
        else if (ident->str() == "bar_inst1")
            bar_inst1 = ident;
        else if (ident->str() == "bar_inst2")
            bar_inst2 = ident;
        else if (ident->str() == "self")
            self = ident;
    }


    Environment env = program->env();
    UAISO_EXPECT_FALSE(env.isEmpty());

    // Global
    UAISO_EXPECT_TRUE(env.lookUpType(bar));

    const Record* classBar = ConstRecord_Cast(env.lookUpType(bar));
    Environment barEnv = classBar->type()->env();
    UAISO_EXPECT_TRUE(barEnv.lookUpValue(bar_static));
    UAISO_EXPECT_FALSE(env.lookUpValue(bar_static));
    UAISO_EXPECT_TRUE(barEnv.lookUpType(bar_method));
    UAISO_EXPECT_FALSE(env.lookUpType(bar_method));
    UAISO_EXPECT_TRUE(barEnv.lookUpValue(bar_inst1));
    UAISO_EXPECT_FALSE(env.lookUpValue(bar_inst1));
    UAISO_EXPECT_TRUE(barEnv.lookUpValue(bar_inst2));
    UAISO_EXPECT_FALSE(env.lookUpValue(bar_inst2));
}
