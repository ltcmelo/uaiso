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

#include "Semantic/CompletionTest.h"
#include "Semantic/Binder.h"
#include "Semantic/CompletionProposer.h"
#include "Semantic/Manager.h"
#include "Semantic/Program.h"
#include "Semantic/TypeChecker.h"
#include "Semantic/Snapshot.h"
#include "Semantic/Symbol.h"
#include "Ast/Ast.h"
#include "Ast/AstDumper.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit.h"
#include <sstream>
#include <algorithm>

using namespace uaiso;

std::vector<std::string> readSearchPaths();

std::unique_ptr<Unit>
CompletionProposer::CompletionProposerTest::runCore(
        std::unique_ptr<Factory> factory,
        const std::string& code,
        const std::string& fullFileName,
        const std::vector<const char*>& expected)
{
    std::vector<std::string> searchPaths = readSearchPaths();

    TokenMap tokens;
    LexemeMap lexemes;
    Snapshot snapshot;
    Manager manager;
    manager.config(factory.get(), &tokens, &lexemes, snapshot);
    Manager::BehaviourFlags flags = 0;
    if (disableBuiltins_)
        flags |= Manager::BehaviourFlag::IgnoreBuiltins;
    if (disableAutoModules_)
        flags |= Manager::BehaviourFlag::IgnoreAutomaticModules;
    manager.setBehaviour(flags);
    for (const auto& path : searchPaths)
        manager.addSearchPath(path);

    std::unique_ptr<Unit> unit = manager.process(code, fullFileName, lineCol_);
    UAISO_EXPECT_TRUE(unit->ast());
    ProgramAst* progAst = Program_Cast(unit->ast());
    if (dumpAst_) {
        std::ostringstream oss;
        oss << "AST dump\n";
        AstDumper().dumpProgram(progAst, oss);
        std::cout << oss.str();
    }

    Program* prog = snapshot.find(fullFileName);
    UAISO_EXPECT_TRUE(prog);
    UAISO_EXPECT_FALSE(prog->env().isEmpty());

    TypeChecker checker(factory.get());
    checker.setLexemes(&lexemes);
    checker.setTokens(&tokens);
    checker.check(progAst);

    CompletionProposer completer(factory.get());
    auto syms = std::get<0>(completer.propose(progAst, &lexemes));
    if (dumpCompletions_) {
        std::ostringstream oss;
        oss << "Produced completions\n";
        std::for_each(syms.begin(), syms.end(), [&oss] (const Decl* decl) {
            oss << decl->name()->str() << " ";
        });
        oss << std::endl;
        std::cout << oss.str().c_str();
    }

    UAISO_EXPECT_INT_EQ(expected.size(), syms.size());
    for (const auto& s : expected) {
        UAISO_EXPECT_TRUE(std::find_if(syms.begin(), syms.end(),
                                       [s](const Decl* decl) {
            return decl->name()->str() == s;
        }) != syms.end());
    }

    return std::move(unit);
}

MAKE_CLASS_TEST(CompletionProposer)
