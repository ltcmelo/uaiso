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

#include "Semantic/Manager.h"
#include "Semantic/Binder.h"
#include "Semantic/ImportResolver.h"
#include "Semantic/Program.h"
#include "Semantic/Sanitizer.h"
#include "Semantic/Snapshot.h"
#include "Semantic/Symbol.h"
#include "Common/Assert.h"
#include "Common/FileInfo.h"
#include "Common/Trace__.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit.h"
#include <iostream>
#include <stack>
#include <unordered_set>

#define TRACE_NAME "Manager"

using namespace uaiso;

#define ENSURE_CONFIG \
    UAISO_ASSERT(P->factory_, return std::unique_ptr<Unit>()); \
    UAISO_ASSERT(P->tokens_, return std::unique_ptr<Unit>()); \
    UAISO_ASSERT(P->lexs_, return std::unique_ptr<Unit>())

struct uaiso::Manager::ManagerImpl
{
    Factory* factory_ { nullptr };
    TokenMap* tokens_ { nullptr };
    LexemeMap* lexs_ { nullptr };
    Snapshot snapshot_;
    std::vector<std::string> searchPaths_;
    std::unique_ptr<Sanitizer> sanitizer_ { nullptr };
    char behaviour_ { 0 };

    std::unique_ptr<Unit> parse(const std::string& code,
                                FILE* file,
                                const std::string& fullFileName,
                                const LineCol& lineCol = LineCol())
    {
        std::unique_ptr<Unit> unit(factory_->makeUnit());
        unit->setFileName(fullFileName);

        if (file)
            unit->assignInput(file);
        else
            unit->assignInput(code);

        if (lineCol.isEmpty())
            unit->parse(tokens_, lexs_);
        else
            unit->parse(tokens_, lexs_, lineCol);

        if (file)
            fclose(file);

        return unit;
    }

    std::unique_ptr<Program> bind(Unit* unit)
    {
        using Prog = std::unique_ptr<Program>;

        UAISO_ASSERT(unit->ast(), return Prog());
        UAISO_ASSERT(unit->ast()->kind() == Ast::Kind::Program, return Prog());

        Binder binder(factory_);
        binder.setLexemes(lexs_);
        binder.setTokens(tokens_);
        Manager::BehaviourFlags flags(behaviour_);
        if (flags & BehaviourFlag::IgnoreBuiltins)
            binder.ignoreBuiltins();
        if (flags & BehaviourFlag::IgnoreAutomaticModules)
            binder.ignoreAutomaticModules();
        return binder.bind(Program_Cast(unit->ast()), unit->fileName());
    }
};

Manager::Manager()
    : P(new ManagerImpl)
{}

Manager::~Manager()
{}

void Manager::config(Factory *factory,
                     TokenMap* tokens,
                     LexemeMap* lexs,
                     Snapshot snapshot)
{
    P->factory_ = factory;
    P->tokens_ = tokens;
    P->lexs_ = lexs;
    P->snapshot_ = snapshot;
    P->sanitizer_ = factory->makeSanitizer();
}

void Manager::addSearchPath(const std::string& searchPath)
{
    P->searchPaths_.push_back(searchPath);
}

void Manager::setBehaviour(BehaviourFlags flags)
{
    P->behaviour_ = flags;
}

Manager::BehaviourFlags Manager::behaviour() const
{
    return BehaviourFlags(P->behaviour_);
}

void Manager::processCore(Unit* unit)
{
    std::unique_ptr<Program> prog = P->bind(unit);
    if (!prog)
        return;

    P->snapshot_.insertOrReplace(unit->fileName(), std::move(prog));

    processDeps(unit->fileName());
}

std::unique_ptr<Unit> Manager::process(const std::string& code,
                                       const std::string& fullFileName)
{
    return process(code, fullFileName, LineCol());
}

std::unique_ptr<Unit> Manager::process(const std::string& code,
                                       const std::string& fullFileName,
                                       const LineCol& lineCol)
{
    ENSURE_CONFIG;

    std::unique_ptr<Unit> unit = P->parse(code, nullptr, fullFileName, lineCol);
    if (!unit->ast())
        return unit;

    processCore(unit.get());

    return unit;
}

std::unique_ptr<Unit> Manager::process(FILE* file,
                                       const std::string& fullFileName)
{
    ENSURE_CONFIG;

    std::unique_ptr<Unit> unit = P->parse("", file, fullFileName);
    if (!unit->ast())
        return unit;

    processCore(unit.get());

    return unit;
}

void Manager::processDeps(const std::string& fullFileName) const
{
    UAISO_ASSERT(P->snapshot_.find(fullFileName), return);

    ImportResolver resolver(P->factory_);

    std::stack<const Program*> progs;
    progs.push(P->snapshot_.find(fullFileName));
    std::unordered_set<std::string> visited;
    visited.insert(fullFileName);
    DEBUG_TRACE("process dependencies of %s\n", fullFileName.c_str());
    while (!progs.empty()) {
        const Program* curProg = progs.top();
        progs.pop();

        // Inspect all imports and, if any of them is not already in the
        // snapshot, parse it, bind it, and start tracking it.
        auto imports = curProg->env().imports();
        for (auto import : imports) {
            auto fileNames = resolver.resolve(import, P->searchPaths_);
            for (auto& fileName : fileNames) {
                if (visited.count(fileName))
                    continue;

                Program* otherProg = P->snapshot_.find(fileName);
                DEBUG_TRACE("found dependency %s\n", fileName.c_str());
                if (!otherProg) {
                    FILE* file = fopen(fileName.c_str(), "r");
                    if (!file)
                        continue;

                    std::unique_ptr<Unit> newUnit = P->parse("", file, fileName);
                    if (!newUnit->ast())
                        continue;

                    std::unique_ptr<Program> newProg = P->bind(newUnit.get());
                    if (!newProg)
                        continue;

                    otherProg = newProg.get();
                    P->snapshot_.insertOrReplace(fileName, std::move(newProg));
                }

                // The namespace is the last symbol to be created.
                std::unique_ptr<Namespace> space(new Namespace(import->localName()));
                space->setEnv(otherProg->env());
                curProg->env().injectNamespace(std::move(space), import->mergeEnv());
                progs.push(otherProg);
                visited.insert(fileName);
            }
        }
    }
}
