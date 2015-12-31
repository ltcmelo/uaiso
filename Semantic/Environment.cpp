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
#include "Common/Assert.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include <algorithm>
#include <iostream>

using namespace uaiso;

namespace {

template <class SymbolT>
struct SymbolTable
{
    using HashTable =
        std::unordered_multimap<const Ident*,
                                std::unique_ptr<const SymbolT>>;

    using TableRange =
        std::pair<typename HashTable::const_iterator,
                  typename HashTable::const_iterator>;

    const SymbolT* find(const Ident* ident) const
    {
        auto it = table_.find(ident);
        if (it == table_.end())
            return nullptr;
        return it->second.get();
    }

    void insert(std::unique_ptr<const SymbolT> symbol)
    {
        table_.emplace(symbol->name(), std::move(symbol));
    }

    void takeOver(HashTable& hashTable)
    {
        for (auto& p : hashTable)
            table_.emplace(p.first, std::move(p.second));
        hashTable.clear();
    }

    bool isEmpty() const { return table_.empty(); }

    HashTable table_;
};

} // anonymous

struct uaiso::Environment::EnvironmentImpl
{
public:
    bool isEmpty() const
    {
        return types_.isEmpty()
                && values_.isEmpty()
                && namespaces_.isEmpty()
                && mergedEnvs_.empty()
                && (!outer_ || outer_->isEmpty());
    }

    template <class SymbolT>
    const SymbolT*
    recursivelySearch(const Ident* name,
                      SymbolTable<SymbolT> EnvironmentImpl::* symTable) const
    {
        auto sym = (this->*symTable).find(name);
        if (!sym) {
            for (auto env : mergedEnvs_) {
                sym = env.P->recursivelySearch(name, symTable);
                if (sym)
                    break;
            }
        }
        if (!sym && outer_)
            return outer_->recursivelySearch(name, symTable);

        return sym;
    }

    template <class SymbolT>
    typename SymbolTable<SymbolT>::TableRange
    recursivelySearchMultiple(const Ident* name,
                              SymbolTable<SymbolT> EnvironmentImpl::* symTable) const
    {
        auto range = (this->*symTable).table_.equal_range(name);
        if (range.first == range.second && outer_)
            return outer_->recursivelySearchMultiple(name, symTable);
        return range;
    }

    template <class SymbolT>
    std::vector<const SymbolT*>
    list(SymbolTable<SymbolT> EnvironmentImpl::* symTable) const
    {
        std::vector<const SymbolT*> syms;
        for (auto& sym : (this->*symTable).table_) {
            if (!sym.second->isFake())
                syms.push_back(sym.second.get());
        }

        for (auto env : mergedEnvs_) {
            auto other = env.P->list(symTable);
            std::copy(other.begin(), other.end(), std::back_inserter(syms));
        }

        return syms;
    }

    std::shared_ptr<EnvironmentImpl> outer_;
    std::vector<Environment> nested_;
    SymbolTable<TypeDecl> types_;
    SymbolTable<ValueDecl> values_;
    SymbolTable<Namespace> namespaces_;
    std::vector<Environment> mergedEnvs_;
    std::vector<std::unique_ptr<const Import>> imports_;
};

Environment::Environment()
    : P(new EnvironmentImpl)
{}

Environment Environment::createSubEnv() const
{
    Environment env;
    env.P->outer_ = P;
    return env;
}

Environment Environment::outerEnv() const
{
    UAISO_ASSERT(!isRootEnv(), return Environment());

    Environment env;
    env.P = P->outer_;
    return env;
}

bool Environment::isRootEnv() const
{
    return !P->outer_.get();
}

bool Environment::isEmpty() const
{
    return P->isEmpty();
}

const Decl* Environment::searchDecl(const Ident* name) const
{
    UAISO_ASSERT(name, return nullptr);

    const Decl* sym = P->recursivelySearch<ValueDecl>(name, &EnvironmentImpl::values_);
    if (!sym)
        sym = P->recursivelySearch<TypeDecl>(name, &EnvironmentImpl::types_);
    return sym;
}

const TypeDecl* Environment::searchTypeDecl(const Ident* name) const
{
    UAISO_ASSERT(name, return nullptr);

    return P->recursivelySearch<TypeDecl>(name, &EnvironmentImpl::types_);
}

const ValueDecl* Environment::searchValueDecl(const Ident* name) const
{
    UAISO_ASSERT(name, return nullptr);

    return P->recursivelySearch<ValueDecl>(name, &EnvironmentImpl::values_);
}

void Environment::insertTypeDecl(std::unique_ptr<const TypeDecl> symbol)
{
    UAISO_ASSERT(symbol, return);

    P->types_.insert(std::move(symbol));
}

void Environment::insertValueDecl(std::unique_ptr<const ValueDecl> symbol)
{
    UAISO_ASSERT(symbol, return);

    P->values_.insert(std::move(symbol));
}

void Environment::takeOver(Environment env)
{
    P->values_.takeOver(env.P->values_.table_);
    P->types_.takeOver(env.P->types_.table_);
    P->namespaces_.takeOver(env.P->namespaces_.table_);
}

void Environment::nestIntoOuterEnv()
{
    UAISO_ASSERT(!isRootEnv(), return);

    P->outer_->nested_.push_back(*this);
}

const std::vector<Environment>& Environment::nestedEnvs() const
{
    return P->nested_;
}

void Environment::detachOuterEnv()
{
    P->outer_.reset();
}

void Environment::includeImport(std::unique_ptr<const Import> import)
{
    P->imports_.push_back(std::move(import));
}

std::vector<const Import*> Environment::imports() const
{
    std::vector<const Import*> imports;
    for (const auto& import : P->imports_)
        imports.push_back(import.get());
    return imports; // RVO
}

Environment::Range<TypeDecl> Environment::searchTypeDecls(const Ident* name) const
{
    UAISO_ASSERT(name,
                 return std::make_pair(Iterator<TypeDecl>(P->types_.table_.end()),
                                       Iterator<TypeDecl>(P->types_.table_.end())));

    auto mapRange =
            P->recursivelySearchMultiple<TypeDecl>(name, &EnvironmentImpl::types_);

    return std::make_pair(Iterator<TypeDecl>(mapRange.first),
                          Iterator<TypeDecl>(mapRange.second));
}

Environment::Range<ValueDecl> Environment::searchValueDecls(const Ident* name) const
{
    UAISO_ASSERT(name,
                 return std::make_pair(Iterator<ValueDecl>(P->values_.table_.end()),
                                       Iterator<ValueDecl>(P->values_.table_.end())));

    auto mapRange =
            P->recursivelySearchMultiple<ValueDecl>(name, &EnvironmentImpl::values_);

    return std::make_pair(Iterator<ValueDecl>(mapRange.first),
                          Iterator<ValueDecl>(mapRange.second));
}

std::vector<const ValueDecl*> Environment::listValueDecls() const
{
    return P->list<ValueDecl>(&EnvironmentImpl::values_);
}

std::vector<const TypeDecl*> Environment::listTypeDecls() const
{
    return P->list<TypeDecl>(&EnvironmentImpl::types_);
}

std::vector<const Decl*> Environment::listDecls() const
{
    std::vector<const Decl*> syms;
    auto valSyms = P->list<ValueDecl>(&EnvironmentImpl::values_);
    std::copy(valSyms.begin(), valSyms.end(), std::back_inserter(syms));
    auto tySyms = P->list<TypeDecl>(&EnvironmentImpl::types_);
    std::copy(tySyms.begin(), tySyms.end(), std::back_inserter(syms));

    return syms;
}

void Environment::injectNamespace(std::unique_ptr<Namespace> sym, bool mergeEnv)
{
    if (mergeEnv)
        P->mergedEnvs_.push_back(sym->env());

    if (!sym->isAnonymous())
        P->namespaces_.insert(std::move(sym));
}

const Namespace* Environment::fetchNamespace(const Ident* name) const
{
    return P->recursivelySearch<Namespace>(name, &EnvironmentImpl::namespaces_);
}

std::vector<const Namespace *> Environment::listNamespaces() const
{
    return P->list<Namespace>(&EnvironmentImpl::namespaces_);
}

namespace uaiso {

bool operator==(const Environment& env1, const Environment& env2)
{
    return env1.P == env2.P;
}

bool operator!=(const Environment& env1, const Environment& env2)
{
    return !(env1 == env2);
}

const Ident* nameToIdent(const SimpleNameAst* name, const LexemeMap* lexs)
{
    return lexs->findAt<Ident>(name->nameLoc_.fileName_, name->nameLoc_.lineCol());
}

const ValueDecl* searchValueDecl(const NameAst* name,
                                 Environment env,
                                 const LexemeMap* lexs)
{
    UAISO_ASSERT(name, return nullptr);
    UAISO_ASSERT(lexs, return nullptr);

    if (name->kind() == Ast::Kind::SimpleName)
        return env.searchValueDecl(nameToIdent(ConstSimpleName_Cast(name), lexs));

    // TODO: Templates, namespaces...

    return nullptr;
}

const TypeDecl* searchTypeDecl(const NameAst* name,
                               Environment env,
                               const LexemeMap* lexs)
{
    UAISO_ASSERT(name, return nullptr);
    UAISO_ASSERT(lexs, return nullptr);

    if (name->kind() == Ast::Kind::SimpleName)
        return env.searchTypeDecl(nameToIdent(ConstSimpleName_Cast(name), lexs));

    // TODO: Namespace...

    return nullptr;
}

} // namespace uaiso
