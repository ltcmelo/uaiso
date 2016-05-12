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
#include "Parsing/Unit.h"

using namespace uaiso;

void Binder::BinderTest::GoTestCase1()
{
    std::string code = R"raw(
        // Copyright 2009 The Go Authors.  All rights reserved.
        // Use of this source code is governed by a BSD-style
        // license that can be found in the LICENSE file.

        package runtime

        var ticks struct {
                lock mutex
                val  uint64
        }

        var tls0 [8]uintptr // available storage for m0's TLS; not necessarily used; opaque to GC

        // Note: Called by runtime/pprof in addition to runtime code.
        func tickspersecond() int64 {
                var t0 int
                r := int64(atomicload64(&ticks.val))
                if r != 0 {
                        return r
                }
                lock(&ticks.lock)
                r = int64(ticks.val)
                if r == 0 {
                        c0 := cputicks()
                        usleep(100 * 1000)
                        t1 := nanotime()
                        c1 := cputicks()
                        if t1 == t0 {
                                t1++
                        }
                        r = (c1 - c0) * 1000 * 1000 * 1000 / (t1 - t0)
                        if r == 0 {
                                r++
                        }
                        atomicstore64(&ticks.val, uint64(r))
                }
                unlock(&ticks.lock)
                return r
        }

        func makeStringSlice() []string {
                return make([]string, 5)
        }


        func parforalloc(nthrmax uint32) *parfor {
                return &parfor{
                        thr:     &make([]parforthread, nthrmax)[0],
                        nthrmax: nthrmax,
                }
        }

        var envs []string
        var argslice []string

        // called from syscall
        func runtime_envs() []string { return envs }

        // called from os
        func runtime_args() []string { return argslice }

        // Snippets from Go Tour.

        type vertex struct {
                x int
                y int
                vertex string
        }

        func main() {
                v := vertex{1, 2, "foo"}
                v.vertex = "tst"
                fmt.Println(v)
                p := []int{2, 3, 5, 7, 11, 13}
        }
    )raw";

    std::unique_ptr<Program> program(core(FactoryCreator::create(LangId::Go),
                                          code,
                                          "/from/go/source/runtime/code.go"));
    UAISO_EXPECT_TRUE(program);
    UAISO_EXPECT_STR_EQ(program->moduleName(), "code");
    UAISO_EXPECT_STR_EQ(program->packageName(), "runtime");

    const Ident* runtime = lexs_.findAnyOfIdent("runtime");
    const Ident* ticks = lexs_.findAnyOfIdent("ticks");
    const Ident* lock = lexs_.findAnyOfIdent("lock");
    const Ident* val = lexs_.findAnyOfIdent("val");
    const Ident* tls0 = lexs_.findAnyOfIdent("tls0");
    const Ident* tickspersecond = lexs_.findAnyOfIdent("tickspersecond");
    const Ident* r = lexs_.findAnyOfIdent("r");
    const Ident* t0 = lexs_.findAnyOfIdent("t0");
    const Ident* c0 = lexs_.findAnyOfIdent("c0");
    const Ident* t1 = lexs_.findAnyOfIdent("t1");
    const Ident* c1 = lexs_.findAnyOfIdent("c1");
    const Ident* makeStringSlice = lexs_.findAnyOfIdent("makeStringSlice");
    const Ident* parforalloc = lexs_.findAnyOfIdent("parforalloc");
    const Ident* parfor = lexs_.findAnyOfIdent("parfor");
    const Ident* thr = lexs_.findAnyOfIdent("thr");
    const Ident* nthrmax = lexs_.findAnyOfIdent("nthrmax");
    const Ident* envs = lexs_.findAnyOfIdent("envs");
    const Ident* argslice = lexs_.findAnyOfIdent("argslice");
    const Ident* runtime_envs = lexs_.findAnyOfIdent("runtime_envs");
    const Ident* runtime_args = lexs_.findAnyOfIdent("runtime_args");
    const Ident* vertex = lexs_.findAnyOfIdent("vertex");
    const Ident* x = lexs_.findAnyOfIdent("x");
    const Ident* y = lexs_.findAnyOfIdent("y");
    const Ident* main = lexs_.findAnyOfIdent("main");
    const Ident* v = lexs_.findAnyOfIdent("v");

    Environment moduleEnv = program->env();
    UAISO_EXPECT_TRUE(moduleEnv.searchTypeDecl(vertex));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(vertex));
    UAISO_EXPECT_TRUE(moduleEnv.searchValueDecl(ticks));
    UAISO_EXPECT_FALSE(moduleEnv.searchDecl(lock));
    UAISO_EXPECT_FALSE(moduleEnv.searchDecl(val));
    UAISO_EXPECT_TRUE(moduleEnv.searchValueDecl(tls0));
    UAISO_EXPECT_TRUE(moduleEnv.searchTypeDecl(tickspersecond));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(tickspersecond));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(r));
    UAISO_EXPECT_TRUE(moduleEnv.searchTypeDecl(makeStringSlice));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(makeStringSlice));
    UAISO_EXPECT_TRUE(moduleEnv.searchTypeDecl(parforalloc));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(parforalloc));
    UAISO_EXPECT_TRUE(moduleEnv.searchValueDecl(envs));
    UAISO_EXPECT_TRUE(moduleEnv.searchValueDecl(argslice));
    UAISO_EXPECT_TRUE(moduleEnv.searchTypeDecl(runtime_envs));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(runtime_envs));
    UAISO_EXPECT_TRUE(moduleEnv.searchTypeDecl(runtime_args));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(runtime_args));
    UAISO_EXPECT_TRUE(moduleEnv.searchTypeDecl(main));
    UAISO_EXPECT_FALSE(moduleEnv.searchValueDecl(main));

    Environment vertexEnv =
            ConstRecordType_Cast(moduleEnv.searchTypeDecl(vertex)->type())->env();
    UAISO_EXPECT_TRUE(vertexEnv.searchValueDecl(x));
    UAISO_EXPECT_TRUE(vertexEnv.searchValueDecl(y));
    UAISO_EXPECT_TRUE(vertexEnv.searchValueDecl(vertex));
    UAISO_EXPECT_TRUE(vertexEnv.searchTypeDecl(vertex));

    const Var* varTicks = ConstVar_Cast(moduleEnv.searchValueDecl(ticks));
    Environment ticksEnv =
            ConstRecordType_Cast(varTicks->valueType())->env();
    UAISO_EXPECT_TRUE(ticksEnv.searchValueDecl(lock));
    UAISO_EXPECT_TRUE(ticksEnv.searchValueDecl(val));

    const Func* funcTicksPerSec =
            ConstFunc_Cast(moduleEnv.searchTypeDecl(tickspersecond));
    Environment ticksPerSecEnv = funcTicksPerSec->env();
    UAISO_EXPECT_TRUE(ticksPerSecEnv.searchValueDecl(t0));
    UAISO_EXPECT_TRUE(ticksPerSecEnv.searchValueDecl(r));
    const std::vector<Environment>& ticksPerSecEnvNested =
            ticksPerSecEnv.nestedEnvs();
    UAISO_EXPECT_INT_EQ(2, ticksPerSecEnvNested.size());
    UAISO_EXPECT_TRUE(ticksPerSecEnvNested[1].searchValueDecl(t1));
    UAISO_EXPECT_TRUE(ticksPerSecEnvNested[1].searchValueDecl(c0));
    UAISO_EXPECT_TRUE(ticksPerSecEnvNested[1].searchValueDecl(c1));

    const Func* funcStringSlice =
            ConstFunc_Cast(moduleEnv.searchTypeDecl(makeStringSlice));
    Environment stringSliceEnv = funcStringSlice->env();
    stringSliceEnv.detachOuterEnv();
    UAISO_EXPECT_TRUE(stringSliceEnv.isEmpty());

    const Func* funcParallelFor =
            ConstFunc_Cast(moduleEnv.searchTypeDecl(parforalloc));
    Environment parallelForEnv = funcParallelFor->env();
    parallelForEnv.detachOuterEnv();
    UAISO_EXPECT_FALSE(parallelForEnv.isEmpty()); // Param is part of env.
}
