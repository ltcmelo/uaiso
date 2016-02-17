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

namespace {

const std::string basicCode = R"raw(
i = 1
s = "string"

def f():
    pass

class c:
    def __init__(self, m):
        self.m = m

o = c()

)raw";

} // namespace anonymous

void Binder::BinderTest::
PyVerifyBasicCode(const Program* prog)
{
    UAISO_EXPECT_TRUE(prog);

    const Ident* i = lexs_.findAnyOf<Ident>("i");
    const Ident* s = lexs_.findAnyOf<Ident>("s");
    const Ident* f = lexs_.findAnyOf<Ident>("f");
    const Ident* c = lexs_.findAnyOf<Ident>("c");
    const Ident* o = lexs_.findAnyOf<Ident>("o");

    Environment env = prog->env();
    UAISO_EXPECT_FALSE(env.isEmpty());
    UAISO_EXPECT_TRUE(env.searchValueDecl(i));
    UAISO_EXPECT_TRUE(env.searchValueDecl(s));
    UAISO_EXPECT_TRUE(env.searchTypeDecl(f));
    UAISO_EXPECT_TRUE(env.searchTypeDecl(c));
    UAISO_EXPECT_TRUE(env.searchValueDecl(o));
}

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

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    UAISO_EXPECT_TRUE(prog);

    const Ident* a = lexs_.findAnyOf<Ident>("a");
    const Ident* b = lexs_.findAnyOf<Ident>("b");
    const Ident* c = lexs_.findAnyOf<Ident>("c");
    const Ident* d = lexs_.findAnyOf<Ident>("d");
    const Ident* e = lexs_.findAnyOf<Ident>("e");
    const Ident* f = lexs_.findAnyOf<Ident>("f");
    const Ident* k = lexs_.findAnyOf<Ident>("k");
    const Ident* w = lexs_.findAnyOf<Ident>("w");
    const Ident* foo = lexs_.findAnyOf<Ident>("foo");
    const Ident* foo_local = lexs_.findAnyOf<Ident>("foo_local");
    const Ident* bar = lexs_.findAnyOf<Ident>("bar");
    const Ident* bar_static = lexs_.findAnyOf<Ident>("bar_static");
    const Ident* bar_method = lexs_.findAnyOf<Ident>("bar_method");
    const Ident* call = lexs_.findAnyOf<Ident>("call");
    const Ident* nothing = lexs_.findAnyOf<Ident>("nothing");

    Environment env = prog->env();
    UAISO_EXPECT_FALSE(env.isEmpty());

    // Global
    UAISO_EXPECT_TRUE(env.searchValueDecl(a));
    UAISO_EXPECT_TRUE(env.searchValueDecl(b));
    UAISO_EXPECT_TRUE(env.searchValueDecl(c));
    UAISO_EXPECT_TRUE(env.searchValueDecl(d));
    UAISO_EXPECT_TRUE(env.searchValueDecl(e));
    UAISO_EXPECT_TRUE(env.searchValueDecl(f));
    UAISO_EXPECT_TRUE(env.searchTypeDecl(foo));
    UAISO_EXPECT_TRUE(env.searchTypeDecl(bar));

    const Func* funcFoo = ConstFunc_Cast(env.searchTypeDecl(foo));
    Environment fooEnv = funcFoo->env();
    UAISO_EXPECT_TRUE(fooEnv.searchValueDecl(foo_local));
    UAISO_EXPECT_FALSE(env.searchValueDecl(foo_local));
    UAISO_EXPECT_FALSE(env.searchTypeDecl(foo_local));
    UAISO_EXPECT_TRUE(fooEnv.searchValueDecl(a));

    const Record* classBar = ConstRecord_Cast(env.searchTypeDecl(bar));
    Environment barEnv = classBar->type()->env();
    UAISO_EXPECT_TRUE(barEnv.searchValueDecl(bar_static));
    UAISO_EXPECT_FALSE(env.searchValueDecl(bar_static));
    UAISO_EXPECT_TRUE(barEnv.searchTypeDecl(bar_method));
    UAISO_EXPECT_FALSE(env.searchTypeDecl(bar_method));
    UAISO_EXPECT_TRUE(barEnv.searchValueDecl(a));
}

void Binder::BinderTest::PyTestCase2()
{
    std::string code = R"raw(
for g in range(10):
    h = g * 2
    print h
)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    UAISO_EXPECT_TRUE(prog);

    const Ident* g = lexs_.findAnyOf<Ident>("g");
    const Ident* h = lexs_.findAnyOf<Ident>("h");

    UAISO_SKIP_TEST;

    Environment env = prog->env();
    UAISO_EXPECT_FALSE(env.isEmpty());

    // Global
    UAISO_EXPECT_TRUE(env.searchValueDecl(g));
    UAISO_EXPECT_TRUE(env.searchValueDecl(h));
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

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    UAISO_EXPECT_TRUE(prog);

    const Ident* bar = lexs_.findAnyOf<Ident>("bar");
    const Ident* bar_static = lexs_.findAnyOf<Ident>("bar_static");
    const Ident* bar_method = lexs_.findAnyOf<Ident>("bar_method");
    const Ident* bar_inst1 = lexs_.findAnyOf<Ident>("bar_inst1");
    const Ident* bar_inst2 = lexs_.findAnyOf<Ident>("bar_inst2");
    const Ident* self = lexs_.findAnyOf<Ident>("self");

    Environment env = prog->env();
    UAISO_EXPECT_FALSE(env.isEmpty());

    // Global
    UAISO_EXPECT_TRUE(env.searchTypeDecl(bar));

    const Record* classBar = ConstRecord_Cast(env.searchTypeDecl(bar));
    Environment barEnv = classBar->type()->env();
    UAISO_EXPECT_TRUE(barEnv.searchValueDecl(bar_static));
    UAISO_EXPECT_FALSE(env.searchValueDecl(bar_static));
    UAISO_EXPECT_TRUE(barEnv.searchTypeDecl(bar_method));
    UAISO_EXPECT_FALSE(env.searchTypeDecl(bar_method));
    UAISO_EXPECT_TRUE(barEnv.searchValueDecl(bar_inst1));
    UAISO_EXPECT_FALSE(env.searchValueDecl(bar_inst1));
    UAISO_EXPECT_TRUE(barEnv.searchValueDecl(bar_inst2));
    UAISO_EXPECT_FALSE(env.searchValueDecl(bar_inst2));
}

void Binder::BinderTest::PyTestCase4()
{
    std::string code = basicCode + R"raw(
def g():

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    PyVerifyBasicCode(prog.get());
}

void Binder::BinderTest::PyTestCase5()
{
    std::string code = basicCode + R"raw(
def g()

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    PyVerifyBasicCode(prog.get());
}

void Binder::BinderTest::PyTestCase6()
{
    std::string code = basicCode + R"raw(
def g(

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    PyVerifyBasicCode(prog.get());
}

void Binder::BinderTest::PyTestCase7()
{
    std::string code = basicCode + R"raw(
def g

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    PyVerifyBasicCode(prog.get());
}

void Binder::BinderTest::PyTestCase8()
{
    std::string code = basicCode + R"raw(
def

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));
    PyVerifyBasicCode(prog.get());
}

void Binder::BinderTest::PyTestCase9()
{
    std::string code = basicCode + R"raw(
def g():
    l = 1
    l2

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));

    PyVerifyBasicCode(prog.get());

    const Ident* g = lexs_.findAnyOf<Ident>("g");
    const Ident* l = lexs_.findAnyOf<Ident>("l");

    Environment env = prog->env();
    UAISO_EXPECT_TRUE(env.searchTypeDecl(g));
    const Func* func = ConstFunc_Cast(env.searchTypeDecl(g));
    Environment funcEnv = func->env();
    UAISO_EXPECT_TRUE(funcEnv.searchValueDecl(l));
}

void Binder::BinderTest::PyTestCase10()
{
    std::string code = basicCode + R"raw(
def g():
    l = 1
    l2.

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));

    PyVerifyBasicCode(prog.get());

    const Ident* g = lexs_.findAnyOf<Ident>("g");
    const Ident* l = lexs_.findAnyOf<Ident>("l");

    Environment env = prog->env();
    UAISO_EXPECT_TRUE(env.searchTypeDecl(g));
    const Func* func = ConstFunc_Cast(env.searchTypeDecl(g));
    Environment funcEnv = func->env();
    UAISO_EXPECT_TRUE(funcEnv.searchValueDecl(l));
}

void Binder::BinderTest::PyTestCase11()
{
    std::string code = basicCode + R"raw(
def g():
    l = 1
    h(

)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));

    PyVerifyBasicCode(prog.get());

    const Ident* g = lexs_.findAnyOf<Ident>("g");
    const Ident* l = lexs_.findAnyOf<Ident>("l");

    Environment env = prog->env();
    UAISO_EXPECT_TRUE(env.searchTypeDecl(g));
    const Func* func = ConstFunc_Cast(env.searchTypeDecl(g));
    Environment funcEnv = func->env();
    UAISO_EXPECT_TRUE(funcEnv.searchValueDecl(l));
}

void Binder::BinderTest::PyTestCase12()
{
    std::string code = basicCode + R"raw(
class c2:
    sm = "static"
    def __init__(self, m):
        self.m = m
    foo foo foo
)raw";

    std::unique_ptr<Program> prog(core(FactoryCreator::create(LangId::Py),
                                       code, "/test.py"));

    PyVerifyBasicCode(prog.get());

    const Ident* c2 = lexs_.findAnyOf<Ident>("c2");
    const Ident* m = lexs_.findAnyOf<Ident>("m");
    const Ident* sm = lexs_.findAnyOf<Ident>("sm");

    Environment env = prog->env();
    UAISO_EXPECT_TRUE(env.searchTypeDecl(c2));
    const Record* clazz = ConstRecord_Cast(env.searchTypeDecl(c2));
    Environment clazzEnv = clazz->type()->env();
    UAISO_EXPECT_TRUE(clazzEnv.searchValueDecl(sm));
    UAISO_EXPECT_TRUE(clazzEnv.searchValueDecl(m));
}
