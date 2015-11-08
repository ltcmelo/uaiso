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

    std::unique_ptr<Program> program(core(FactoryCreator::create(LangName::Go),
                                          code,
                                          "/from/go/source/runtime/code.go"));
    UAISO_EXPECT_TRUE(program);
    UAISO_EXPECT_STR_EQ(program->moduleName(), "code");
    UAISO_EXPECT_STR_EQ(program->packageName(), "runtime");

    const Ident* runtime = nullptr;
    const Ident* ticks = nullptr;
    const Ident* lock = nullptr;
    const Ident* val = nullptr;
    const Ident* tls0 = nullptr;
    const Ident* tickspersecond = nullptr;
    const Ident* r = nullptr;
    const Ident* t0 = nullptr;
    const Ident* c0 = nullptr;
    const Ident* t1 = nullptr;
    const Ident* c1 = nullptr;
    const Ident* makeStringSlice = nullptr;
    const Ident* parforalloc = nullptr;
    const Ident* parfor = nullptr;
    const Ident* thr = nullptr;
    const Ident* nthrmax = nullptr;
    const Ident* envs = nullptr;
    const Ident* argslice = nullptr;
    const Ident* runtime_envs = nullptr;
    const Ident* runtime_args = nullptr;
    const Ident* vertex = nullptr;
    const Ident* x = nullptr;
    const Ident* y = nullptr;
    const Ident* main = nullptr;
    const Ident* v = nullptr;

    auto tuples = lexemes_.list<Ident>("/from/go/source/runtime/code.go");
    for (auto tuple : tuples) {
        const Ident* ident = std::get<0>(tuple);
        if (ident->str() == "runtime")
            runtime = ident;
        else if (ident->str() == "ticks")
            ticks = ident;
        else if (ident->str() == "lock")
            lock = ident;
        else if (ident->str() == "val")
            val = ident;
        else if (ident->str() == "tls0")
            tls0 = ident;
        else if (ident->str() == "tickspersecond")
            tickspersecond = ident;
        else if (ident->str() == "r")
            r = ident;
        else if (ident->str() == "t0")
            t0 = ident;
        else if (ident->str() == "c0")
            c0 = ident;
        else if (ident->str() == "t1")
            t1 = ident;
        else if (ident->str() == "c1")
            c1 = ident;
        else if (ident->str() == "makeStringSlice")
            makeStringSlice = ident;
        else if (ident->str() == "parforalloc")
            parforalloc = ident;
        else if (ident->str() == "parfor")
            parfor = ident;
        else if (ident->str() == "thr")
            thr = ident;
        else if (ident->str() == "nthrmax")
            nthrmax = ident;
        else if (ident->str() == "envs")
            envs = ident;
        else if (ident->str() == "argslice")
            argslice = ident;
        else if (ident->str() == "runtime_envs")
            runtime_envs = ident;
        else if (ident->str() == "runtime_args")
            runtime_args = ident;
        else if (ident->str() == "vertex") {
            vertex = ident;
        } else if (ident->str() == "x")
            x = ident;
        else if (ident->str() == "y")
            y = ident;
        else if (ident->str() == "main")
            main = ident;
        else if (ident->str() == "v")
            v = ident;
    }

    Environment moduleEnv = program->env();
    UAISO_EXPECT_TRUE(moduleEnv.lookUpType(vertex));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(vertex));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpValue(ticks));
    UAISO_EXPECT_FALSE(moduleEnv.lookUp(lock));
    UAISO_EXPECT_FALSE(moduleEnv.lookUp(val));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpValue(tls0));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpType(tickspersecond));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(tickspersecond));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(r));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpType(makeStringSlice));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(makeStringSlice));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpType(parforalloc));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(parforalloc));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpValue(envs));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpValue(argslice));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpType(runtime_envs));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(runtime_envs));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpType(runtime_args));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(runtime_args));
    UAISO_EXPECT_TRUE(moduleEnv.lookUpType(main));
    UAISO_EXPECT_FALSE(moduleEnv.lookUpValue(main));

    Environment vertexEnv =
            ConstRecordType_Cast(moduleEnv.lookUpType(vertex)->type())->env();
    UAISO_EXPECT_TRUE(vertexEnv.lookUpValue(x));
    UAISO_EXPECT_TRUE(vertexEnv.lookUpValue(y));
    UAISO_EXPECT_TRUE(vertexEnv.lookUpValue(vertex));
    UAISO_EXPECT_TRUE(vertexEnv.lookUpType(vertex));

    const Var* varTicks = ConstVar_Cast(moduleEnv.lookUpValue(ticks));
    Environment ticksEnv =
            ConstRecordType_Cast(varTicks->valueType())->env();
    UAISO_EXPECT_TRUE(ticksEnv.lookUpValue(lock));
    UAISO_EXPECT_TRUE(ticksEnv.lookUpValue(val));

    const Func* funcTicksPerSec =
            ConstFunc_Cast(moduleEnv.lookUpType(tickspersecond));
    Environment ticksPerSecEnv = funcTicksPerSec->env();
    UAISO_EXPECT_TRUE(ticksPerSecEnv.lookUpValue(t0));
    UAISO_EXPECT_TRUE(ticksPerSecEnv.lookUpValue(r));
    const std::vector<Environment>& ticksPerSecEnvNested =
            ticksPerSecEnv.nestedEnvs();
    UAISO_EXPECT_INT_EQ(2, ticksPerSecEnvNested.size());
    UAISO_EXPECT_TRUE(ticksPerSecEnvNested[1].lookUpValue(t1));
    UAISO_EXPECT_TRUE(ticksPerSecEnvNested[1].lookUpValue(c0));
    UAISO_EXPECT_TRUE(ticksPerSecEnvNested[1].lookUpValue(c1));

    const Func* funcStringSlice =
            ConstFunc_Cast(moduleEnv.lookUpType(makeStringSlice));
    Environment stringSliceEnv = funcStringSlice->env();
    stringSliceEnv.detachOuterEnv();
    UAISO_EXPECT_TRUE(stringSliceEnv.isEmpty());

    const Func* funcParallelFor =
            ConstFunc_Cast(moduleEnv.lookUpType(parforalloc));
    Environment parallelForEnv = funcParallelFor->env();
    parallelForEnv.detachOuterEnv();
    UAISO_EXPECT_FALSE(parallelForEnv.isEmpty()); // Param is part of env.
}
