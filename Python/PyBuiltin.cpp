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

#include "Python/PyBuiltin.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Semantic/Environment.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"

using namespace uaiso;

namespace {

const std::string kPyBuiltin = "<__pybuiltin__>";

// An incremented-per-use line to make sure there are no location collisions
// in the identifiers created for the builtins file.
int line = 0;

template <class TypeT>
std::unique_ptr<Func> createFunc(LexemeMap* lexemes, const char* name)
{
    std::unique_ptr<FuncType> funcTy(new FuncType);
    funcTy->setReturnType(std::unique_ptr<Type>(new TypeT));
    auto ident = lexemes->insertOrFind<Ident>(name, kPyBuiltin, LineCol(++line, 0));
    std::unique_ptr<Func> func(new Func(ident));
    func->setIsBuiltin(true);
    func->setType(std::move(funcTy));
    // TODO: Consider function formals.

    return func;
}

} // namespace uaiso

const char* PyBuiltin::tokenSpell(Token tk) const
{
    switch (tk) {
    case TK_FLOAT32:
        return "float";
    case TK_COMPLEX_FLOAT32:
        return "complex";
    default:
        return Builtin::tokenSpell(tk);
    }
}

std::vector<Builtin::FuncPtr> PyBuiltin::valueConstructors(LexemeMap* lexemes) const
{
    std::vector<FuncPtr> ctors;
    ctors.push_back(createFunc<BoolType>(lexemes, tokenSpell(TK_BOOL)));
    ctors.push_back(createFunc<IntType>(lexemes, tokenSpell(TK_INT)));
    ctors.push_back(createFunc<FloatType>(lexemes, tokenSpell(TK_FLOAT32)));
    ctors.push_back(createFunc<FloatType>(lexemes, tokenSpell(TK_COMPLEX_FLOAT32)));

    return ctors;
}

std::vector<Builtin::FuncPtr> PyBuiltin::globalFuncs(LexemeMap* lexemes) const
{
    std::vector<FuncPtr> funcs;
    funcs.push_back(createFunc<IntType>(lexemes, "abs"));
    funcs.push_back(createFunc<BoolType>(lexemes, "all"));
    funcs.push_back(createFunc<BoolType>(lexemes, "any"));
    funcs.push_back(createFunc<StrType>(lexemes, "ascii"));
    funcs.push_back(createFunc<StrType>(lexemes, "bin"));
    // TODO: Continue...
    return funcs;
}

std::vector<Builtin::TypeDeclPtr> PyBuiltin::typeDecls(LexemeMap* lexemes) const
{
    std::vector<TypeDeclPtr> types;

    // The `object` class
    std::unique_ptr<RecordType> recTy(new RecordType);
    recTy->setVariety(RecordVariety::Class);
    Environment env;
    env.insertTypeDecl(createFunc<RecordType>(lexemes, "__new__"));
    env.insertTypeDecl(createFunc<VoidType>(lexemes, "__init__"));
    env.insertTypeDecl(createFunc<VoidType>(lexemes, "__del__"));
    env.insertTypeDecl(createFunc<StrType>(lexemes, "__repr__"));
    env.insertTypeDecl(createFunc<StrType>(lexemes, "__str__"));
    //env.insertTypeDecl(createFunc<ArrayType>(lexemes, "__bytes__"));
    env.insertTypeDecl(createFunc<StrType>(lexemes, "__format__"));
    env.insertTypeDecl(createFunc<BoolType>(lexemes, "__lt__"));
    env.insertTypeDecl(createFunc<BoolType>(lexemes, "__le__"));
    env.insertTypeDecl(createFunc<BoolType>(lexemes, "__eq__"));
    env.insertTypeDecl(createFunc<BoolType>(lexemes, "__ne__"));
    env.insertTypeDecl(createFunc<BoolType>(lexemes, "__gt__"));
    env.insertTypeDecl(createFunc<BoolType>(lexemes, "__ge__"));
    env.insertTypeDecl(createFunc<IntType>(lexemes, "__hash__"));
    env.insertTypeDecl(createFunc<BoolType>(lexemes, "__bool__"));
    env.insertTypeDecl(createFunc<InferredType>(lexemes, "__getattr__"));
    env.insertTypeDecl(createFunc<InferredType>(lexemes, "__getattribute__"));
    env.insertTypeDecl(createFunc<VoidType>(lexemes, "__setattr__"));
    env.insertTypeDecl(createFunc<VoidType>(lexemes, "__delattr__"));
    //env.insertTypeDecl(createFunc<ArrayType>(lexemes, "__dir__"));
    recTy->setEnv(env);
    auto ident = lexemes->insertOrFind<Ident>("object", kPyBuiltin, LineCol(++line, 0));
    std::unique_ptr<Record> rec(new Record(ident));
    rec->setIsBuiltin(true);
    rec->setType(std::move(recTy));
    types.push_back(std::move(rec));

    return types;
}

Builtin::BaseRecordPtr PyBuiltin::implicitBase(LexemeMap* lexemes) const
{
    auto ident = lexemes->insertOrFind<Ident>("object", kPyBuiltin, LineCol(++line, 0));
    return BaseRecordPtr(new BaseRecord(ident));
}

std::vector<std::string> PyBuiltin::automaticModules() const
{
    static std::vector<std::string> names {
        "numbers"
    };

    return names;
}
