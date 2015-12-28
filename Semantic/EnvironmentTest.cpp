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
        TypeDecl* s = new Record(a.get());
        Environment env;
        env.insertTypeDecl(std::unique_ptr<const TypeDecl>(s));

        UAISO_EXPECT_TRUE(env.searchTypeDecl(a.get()));
        UAISO_EXPECT_FALSE(env.searchValueDecl(a.get()));
        UAISO_EXPECT_TRUE(env.searchDecl(a.get()));
    }

    void testCase2()
    {
        std::unique_ptr<Ident> a(new Ident("a"));
        ValueDecl* s = new Var(a.get());
        Environment env;
        env.insertValueDecl(std::unique_ptr<const ValueDecl>(s));

        UAISO_EXPECT_FALSE(env.searchTypeDecl(a.get()));
        UAISO_EXPECT_TRUE(env.searchValueDecl(a.get()));
        UAISO_EXPECT_TRUE(env.searchDecl(a.get()));
    }

    void testCase3()
    {
        std::unique_ptr<Ident> a(new Ident("a"));
        Record* s = new Record(a.get());
        std::unique_ptr<Ident> b(new Ident("b"));
        Record* t = new Record(b.get());
        RecordType* m = new RecordType;
        m->env().insertTypeDecl(std::unique_ptr<const TypeDecl>(t));
        s->setType(std::unique_ptr<RecordType>(m));
        Environment env;
        env.insertTypeDecl(std::unique_ptr<const TypeDecl>(s));

        UAISO_EXPECT_TRUE(env.searchTypeDecl(a.get()));
        UAISO_EXPECT_FALSE(env.searchValueDecl(a.get()));
        UAISO_EXPECT_TRUE(env.searchDecl(a.get()));
        const Record* s_env = ConstRecord_Cast(env.searchTypeDecl(a.get()));
        UAISO_EXPECT_TRUE(s_env->type()->env().searchTypeDecl(b.get()));
        UAISO_EXPECT_TRUE(s_env->type()->env().searchDecl(b.get()));
        UAISO_EXPECT_FALSE(s_env->type()->env().searchValueDecl(b.get()));
    }

    void testCase4()
    {
        std::unique_ptr<Ident> a(new Ident("a"));
        Var* s = new Var(a.get());
        Environment env;
        env.insertValueDecl(std::unique_ptr<const Var>(s));
        std::unique_ptr<Ident> b(new Ident("b"));
        Func* t = new Func(b.get());
        t->setEnv(env.createSubEnv());
        t->env().insertTypeDecl(std::unique_ptr<const TypeDecl>(t));
        env.insertTypeDecl(std::unique_ptr<const TypeDecl>(t));

        UAISO_EXPECT_TRUE(env.searchValueDecl(a.get()));
        UAISO_EXPECT_TRUE(env.searchTypeDecl(b.get()));
        const Func* t_env = ConstFunc_Cast(env.searchTypeDecl(b.get()));
        UAISO_EXPECT_TRUE(t_env->env().searchValueDecl(a.get()));
    }

    void testCase5()
    {
        // Duplicates
        std::unique_ptr<Ident> a(new Ident("a"));
        TypeDecl* s1 = new Record(a.get());
        TypeDecl* s2 = new Record(a.get());
        TypeDecl* s3 = new Record(a.get());
        Environment env;
        env.insertTypeDecl(std::unique_ptr<const TypeDecl>(s1));
        env.insertTypeDecl(std::unique_ptr<const TypeDecl>(s2));
        env.insertTypeDecl(std::unique_ptr<const TypeDecl>(s3));

        UAISO_EXPECT_TRUE(env.searchTypeDecl(a.get()));
        UAISO_EXPECT_FALSE(env.searchValueDecl(a.get()));
        UAISO_EXPECT_TRUE(env.searchDecl(a.get()));

        auto p = env.searchTypeDecls(a.get());
        size_t total = std::distance(p.first, p.second);
        UAISO_EXPECT_INT_EQ(3, total);
    }

};

MAKE_CLASS_TEST(Environment)
