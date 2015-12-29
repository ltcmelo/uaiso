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

#include "Semantic/TypeCheckerTest.h"
#include "Semantic/Binder.h"
#include "Semantic/Manager.h"
#include "Semantic/Program.h"
#include "Semantic/Snapshot.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Common/FileInfo.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit.h"

using namespace uaiso;

std::vector<std::string> readSearchPaths();

std::unique_ptr<Unit>
TypeChecker::TypeCheckerTest::runCore(std::unique_ptr<Factory> factory,
                                      const std::string& code,
                                      const std::string& fullFileName,
                                      const std::vector<Diagnostic::Code>& expectedReports,
                                      const std::pair<std::string, Type::Kind>& expectedBindings)
{
    std::vector<std::string> searchPaths = readSearchPaths();

    TokenMap tokens;
    LexemeMap lexs;
    Snapshot snapshot;
    DiagnosticReports reports;
    Manager manager;
    manager.config(factory.get(), &tokens, &lexs, snapshot);
    for (const auto& path : searchPaths)
        manager.addSearchPath(path);

    std::unique_ptr<Unit> unit = manager.process(code, fullFileName);
    UAISO_EXPECT_TRUE(unit->ast());
    Program* prog = snapshot.find(fullFileName);
    UAISO_EXPECT_TRUE(prog);

    TypeChecker typeChecker(factory.get());
    typeChecker.setLexemes(&lexs);
    typeChecker.setTokens(&tokens);
    typeChecker.collectDiagnostics(&reports);
    typeChecker.check(Program_Cast(unit->ast()));

    UAISO_EXPECT_INT_EQ(expectedReports.size(), reports.size());
    for (const auto& s : expectedReports) {
        UAISO_EXPECT_TRUE(std::find_if(reports.begin(), reports.end(),
                                       [s](const DiagnosticReport& report) {
            return report.diagnostic().code() == s;
        }) != reports.end());
    }

    const std::string& symName = expectedBindings.first;
    if (!symName.empty()) {
        for (auto valSym : prog->env().listValueDecls()) {
            if (valSym->name()->str() == symName) {
                auto ty = valSym->valueType();
                UAISO_EXPECT_INT_EQ(static_cast<int>(expectedBindings.second),
                                    static_cast<int>(ty->kind()));
            }
        }
    }

    return std::move(unit);
}

MAKE_CLASS_TEST(TypeChecker)
