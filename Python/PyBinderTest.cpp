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

def g():
    i = 10

def h():
    pass

for x in range(10):
    y = x * 2
    print y

class foo:
    k = 1
    def bar():
        self.m = 10
        self.n

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
    const Ident* g = nullptr;
    const Ident* h = nullptr;
    const Ident* i = nullptr;
    const Ident* k = nullptr;
    const Ident* m = nullptr;
    const Ident* n = nullptr;
    const Ident* x = nullptr;
    const Ident* y = nullptr;
    const Ident* w = nullptr;
    const Ident* foo = nullptr;
    const Ident* bar = nullptr;
    const Ident* call = nullptr;
    const Ident* self = nullptr;

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
        else if (ident->str() == "g")
            g = ident;
        else if (ident->str() == "h")
            h = ident;
        else if (ident->str() == "i")
            i = ident;
        else if (ident->str() == "k")
            k = ident;
        else if (ident->str() == "m")
            m = ident;
        else if (ident->str() == "n")
            n = ident;
        else if (ident->str() == "x")
            x = ident;
        else if (ident->str() == "y")
            y = ident;
        else if (ident->str() == "w")
            w = ident;
        else if (ident->str() == "foo")
            foo = ident;
        else if (ident->str() == "bar")
            bar = ident;
        else if (ident->str() == "call")
            call = ident;
        else if (ident->str() == "self")
            self = ident;
    }

    Environment env = program->env();
    UAISO_EXPECT_FALSE(env.isEmpty());

#if 0
    // Vars
    UAISO_EXPECT_TRUE(env.lookUpValue(a));
    UAISO_EXPECT_FALSE(env.lookUpType(a));

    // Functions
    UAISO_EXPECT_TRUE(env.lookUpType(g));
    UAISO_EXPECT_FALSE(env.lookUpValue(g));
#endif
}
