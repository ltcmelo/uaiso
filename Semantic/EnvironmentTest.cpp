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

#include "Semantic/Environment.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Ast/Ast.h"
#include "Parsing/Lexeme.h"

using namespace uaiso;

class Environment::EnvironmentTest : public Test
{
public:
    TEST_RUN(EnvironmentTest
             , &EnvironmentTest::testCase1
             , &EnvironmentTest::testCase2
             , &EnvironmentTest::testCase3
             , &EnvironmentTest::testCase4
             , &EnvironmentTest::testCase5
             )

    void testCase1()
    {
        std::unique_ptr<Ident> a(new Ident("a"));
        TypeSymbol* s = new Record(a.get());
        Environment env;
        env.insertType(std::unique_ptr<const TypeSymbol>(s));

        UAISO_EXPECT_TRUE(env.lookUpType(a.get()));
        UAISO_EXPECT_FALSE(env.lookUpValue(a.get()));
        UAISO_EXPECT_TRUE(env.lookUp(a.get()));
    }

    void testCase2()
    {
        std::unique_ptr<Ident> a(new Ident("a"));
        ValueSymbol* s = new Var(a.get());
        Environment env;
        env.insertValue(std::unique_ptr<const ValueSymbol>(s));

        UAISO_EXPECT_FALSE(env.lookUpType(a.get()));
        UAISO_EXPECT_TRUE(env.lookUpValue(a.get()));
        UAISO_EXPECT_TRUE(env.lookUp(a.get()));
    }

    void testCase3()
    {
        std::unique_ptr<Ident> a(new Ident("a"));
        Record* s = new Record(a.get());
        std::unique_ptr<Ident> b(new Ident("b"));
        Record* t = new Record(b.get());
        RecordType* m = new RecordType;
        m->env().insertType(std::unique_ptr<const TypeSymbol>(t));
        s->setType(std::unique_ptr<RecordType>(m));
        Environment env;
        env.insertType(std::unique_ptr<const TypeSymbol>(s));

        UAISO_EXPECT_TRUE(env.lookUpType(a.get()));
        UAISO_EXPECT_FALSE(env.lookUpValue(a.get()));
        UAISO_EXPECT_TRUE(env.lookUp(a.get()));
        const Record* s_env = ConstRecord_Cast(env.lookUpType(a.get()));
        UAISO_EXPECT_TRUE(s_env->type()->env().lookUpType(b.get()));
        UAISO_EXPECT_TRUE(s_env->type()->env().lookUp(b.get()));
        UAISO_EXPECT_FALSE(s_env->type()->env().lookUpValue(b.get()));
    }

    void testCase4()
    {
        std::unique_ptr<Ident> a(new Ident("a"));
        Var* s = new Var(a.get());
        Environment env;
        env.insertValue(std::unique_ptr<const Var>(s));
        std::unique_ptr<Ident> b(new Ident("b"));
        Func* t = new Func(b.get());
        t->setEnv(env.createSubEnv());
        t->env().insertType(std::unique_ptr<const TypeSymbol>(t));
        env.insertType(std::unique_ptr<const TypeSymbol>(t));

        UAISO_EXPECT_TRUE(env.lookUpValue(a.get()));
        UAISO_EXPECT_TRUE(env.lookUpType(b.get()));
        const Func* t_env = ConstFunc_Cast(env.lookUpType(b.get()));
        UAISO_EXPECT_TRUE(t_env->env().lookUpValue(a.get()));
    }

    void testCase5()
    {
        // Duplicates
        std::unique_ptr<Ident> a(new Ident("a"));
        TypeSymbol* s1 = new Record(a.get());
        TypeSymbol* s2 = new Record(a.get());
        TypeSymbol* s3 = new Record(a.get());
        Environment env;
        env.insertType(std::unique_ptr<const TypeSymbol>(s1));
        env.insertType(std::unique_ptr<const TypeSymbol>(s2));
        env.insertType(std::unique_ptr<const TypeSymbol>(s3));

        UAISO_EXPECT_TRUE(env.lookUpType(a.get()));
        UAISO_EXPECT_FALSE(env.lookUpValue(a.get()));
        UAISO_EXPECT_TRUE(env.lookUp(a.get()));

        auto p = env.lookUpTypes(a.get());
        size_t total = std::distance(p.first, p.second);
        UAISO_EXPECT_INT_EQ(3, total);
    }

};

MAKE_CLASS_TEST(Environment)
