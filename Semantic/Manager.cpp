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

    std::unique_ptr<Program> bind(Unit* unit, bool isDep)
    {
        using Prog = std::unique_ptr<Program>;

        UAISO_ASSERT(unit->ast(), return Prog());
        UAISO_ASSERT(unit->ast()->kind() == Ast::Kind::Program, return Prog());

        Binder binder(factory_);
        binder.setLexemes(lexs_);
        binder.setTokens(tokens_);
        Manager::BehaviourFlags flags(behaviour_);
        if (isDep || (flags & BehaviourFlag::IgnoreBuiltins))
            binder.ignoreBuiltins();
        if (isDep || (flags & BehaviourFlag::IgnoreAutomaticModules))
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
    std::unique_ptr<Program> prog = P->bind(unit, false);
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
        auto curProgEnv = curProg->env();
        auto imports = curProgEnv.imports();
        for (auto import : imports) {
            DEBUG_TRACE("imported module name: %s\n", import->target().c_str());
            auto fileNames = resolver.resolve(const_cast<Import*>(import), P->searchPaths_);
            for (auto& fileName : fileNames) {
                if (visited.count(fileName))
                    continue;

                DEBUG_TRACE("candidate file: %s\n", fileName.c_str());
                Program* otherProg = P->snapshot_.find(fileName);
                if (!otherProg) {
                    FILE* file = fopen(fileName.c_str(), "r");
                    if (!file)
                        continue;

                    std::unique_ptr<Unit> newUnit = P->parse("", file, fileName);
                    if (!newUnit->ast())
                        continue;

                    std::unique_ptr<Program> newProg = P->bind(newUnit.get(), true);
                    if (!newProg)
                        continue;

                    otherProg = newProg.get();
                    P->snapshot_.insertOrReplace(fileName, std::move(newProg));
                }
                DEBUG_TRACE("import (partially) resolved: %s\n", fileName.c_str());
                progs.push(otherProg);
                visited.insert(fileName);

                if (!import->isSelective()) {
                    std::unique_ptr<Namespace> space;
                    if (import->isEmbedded())
                        space.reset(new Namespace);
                    else
                        space.reset(new Namespace(import->localName()));
                    space->setEnv(otherProg->env());
                    curProgEnv.injectNamespace(std::move(space), import->isEmbedded());
                    continue;
                }

                // When the target of a selective import is a module, the
                // selected items are symbols be inserted into the current
                // program's environment (under an alternate name if that's
                // the case). But when the selective import has a package
                // target, the selected items are modules (files) themselves,
                // which have already been filter during import resolution.
                if (import->targetEntity() == Import::Module) {
                    for (auto actualName : import->selectedItems()) {
                        auto tySym = otherProg->env().searchTypeDecl(actualName);
                        if (!tySym)
                            continue;

                        const TypeDecl* cloned = nullptr;
                        if (auto altName = import->alternateName(actualName))
                            cloned = tySym->clone(altName);
                        else
                            cloned = TypeDecl_Cast(tySym->clone());
                        curProgEnv.insertTypeDecl(std::unique_ptr<const TypeDecl>(cloned));
                    }
                } else {
                    auto baseName = FileInfo(fileName).fileBaseName();
                    for (auto actualName : import->selectedItems()) {
                        if (baseName != actualName->str())
                            continue;

                        std::unique_ptr<Namespace> space(new Namespace(actualName));
                        space->setEnv(otherProg->env());
                        curProgEnv.injectNamespace(std::move(space), import->isEmbedded());
                        break;
                    }
                }
            }
        }
    }
}
