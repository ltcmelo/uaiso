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

#include "Python/PyBuiltin.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Semantic/Environment.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"

using namespace uaiso;

namespace {

const char* const kPyBuiltin = "<__pybuiltin__>";
const char* const kPyBuiltinBool = "<__pybuiltin_bool__>";
const char* const kPyBuiltinFloat = "<__pybuiltin_float__>";
const char* const kPyBuiltinInt = "<__pybuiltin_int__>";

// An incremented-per-use line to make sure there are no location collisions
// in the identifiers created for the builtins file.
int line = 0;

const Ident* insertOrFindIdent(LexemeMap* lexs, const char* name)
{
    auto ident = lexs->findAnyOf<Ident>(name);
    if (!ident)
        ident = lexs->insertOrFind<Ident>(name, kPyBuiltin, LineCol(++line, 0));
    return ident;
}

template <class TypeT>
std::unique_ptr<Func> createFunc(LexemeMap* lexs, const char* name)
{
    std::unique_ptr<FuncType> funcTy(new FuncType);
    funcTy->setReturnType(std::unique_ptr<Type>(new TypeT));
    auto ident = insertOrFindIdent(lexs, name);
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

std::vector<Builtin::FuncPtr> PyBuiltin::createConstructors(LexemeMap* lexs) const
{
    std::vector<FuncPtr> ctors;
    ctors.push_back(createFunc<BoolType>(lexs, tokenSpell(TK_BOOL)));
    ctors.push_back(createFunc<IntType>(lexs, tokenSpell(TK_INT)));
    ctors.push_back(createFunc<FloatType>(lexs, tokenSpell(TK_FLOAT32)));
    ctors.push_back(createFunc<FloatType>(lexs, tokenSpell(TK_COMPLEX_FLOAT32)));
    return ctors;
}

std::vector<Builtin::FuncPtr> PyBuiltin::createGlobalFuncs(LexemeMap* lexs) const
{
    std::vector<FuncPtr> funcs;
    funcs.push_back(createFunc<IntType>(lexs, "abs"));
    funcs.push_back(createFunc<BoolType>(lexs, "all"));
    funcs.push_back(createFunc<BoolType>(lexs, "any"));
    funcs.push_back(createFunc<StrType>(lexs, "ascii"));
    funcs.push_back(createFunc<StrType>(lexs, "bin"));
    funcs.push_back(createFunc<StrType>(lexs, "bytearray"));
    funcs.push_back(createFunc<StrType>(lexs, "bytes"));
    funcs.push_back(createFunc<StrType>(lexs, "callable"));
    funcs.push_back(createFunc<StrType>(lexs, "chr"));
    funcs.push_back(createFunc<StrType>(lexs, "classmethod"));
    funcs.push_back(createFunc<StrType>(lexs, "compile"));
    funcs.push_back(createFunc<StrType>(lexs, "delattr"));
    funcs.push_back(createFunc<StrType>(lexs, "dir"));
    funcs.push_back(createFunc<StrType>(lexs, "divmod"));
    funcs.push_back(createFunc<StrType>(lexs, "enumerate"));
    funcs.push_back(createFunc<StrType>(lexs, "eval"));
    funcs.push_back(createFunc<StrType>(lexs, "exec"));
    funcs.push_back(createFunc<StrType>(lexs, "filter"));
    funcs.push_back(createFunc<StrType>(lexs, "format"));
    funcs.push_back(createFunc<StrType>(lexs, "getattr"));
    funcs.push_back(createFunc<StrType>(lexs, "globals"));
    funcs.push_back(createFunc<StrType>(lexs, "hasattr"));
    funcs.push_back(createFunc<StrType>(lexs, "hash"));
    funcs.push_back(createFunc<StrType>(lexs, "help"));
    funcs.push_back(createFunc<StrType>(lexs, "id"));
    funcs.push_back(createFunc<StrType>(lexs, "input"));
    funcs.push_back(createFunc<StrType>(lexs, "isinstance"));
    funcs.push_back(createFunc<StrType>(lexs, "issubclass"));
    funcs.push_back(createFunc<StrType>(lexs, "iter"));
    funcs.push_back(createFunc<StrType>(lexs, "len"));
    funcs.push_back(createFunc<StrType>(lexs, "locals"));
    funcs.push_back(createFunc<StrType>(lexs, "map"));
    funcs.push_back(createFunc<StrType>(lexs, "max"));
    funcs.push_back(createFunc<StrType>(lexs, "min"));
    funcs.push_back(createFunc<StrType>(lexs, "next"));
    funcs.push_back(createFunc<StrType>(lexs, "oct"));
    funcs.push_back(createFunc<StrType>(lexs, "open"));
    funcs.push_back(createFunc<StrType>(lexs, "ord"));
    funcs.push_back(createFunc<StrType>(lexs, "pow"));
    funcs.push_back(createFunc<StrType>(lexs, "property"));
    funcs.push_back(createFunc<StrType>(lexs, "range"));
    funcs.push_back(createFunc<StrType>(lexs, "repr"));
    funcs.push_back(createFunc<StrType>(lexs, "reversed"));
    funcs.push_back(createFunc<StrType>(lexs, "round"));
    funcs.push_back(createFunc<StrType>(lexs, "setattr"));
    funcs.push_back(createFunc<StrType>(lexs, "slice"));
    funcs.push_back(createFunc<StrType>(lexs, "sorted"));
    funcs.push_back(createFunc<StrType>(lexs, "staticmethod"));
    funcs.push_back(createFunc<StrType>(lexs, "sum"));
    funcs.push_back(createFunc<StrType>(lexs, "super"));
    funcs.push_back(createFunc<StrType>(lexs, "type"));
    funcs.push_back(createFunc<StrType>(lexs, "vars"));
    funcs.push_back(createFunc<StrType>(lexs, "zip"));
    funcs.push_back(createFunc<StrType>(lexs, "__import__"));
    return funcs;
}

Builtin::TypeDeclPtr PyBuiltin::createRootTypeDecl(LexemeMap* lexs) const
{
    // The `object` class
    Environment env;
    env.insertValueDecl(createFunc<RecordType>(lexs, "__new__"));
    env.insertValueDecl(createFunc<EmptyType>(lexs, "__init__"));
    env.insertValueDecl(createFunc<EmptyType>(lexs, "__del__"));
    env.insertValueDecl(createFunc<StrType>(lexs, "__repr__"));
    env.insertValueDecl(createFunc<StrType>(lexs, "__str__"));
    //env.insertValueDecl(createFunc<ArrayType>(lexs, "__bytes__"));
    env.insertValueDecl(createFunc<StrType>(lexs, "__format__"));
    env.insertValueDecl(createFunc<BoolType>(lexs, "__lt__"));
    env.insertValueDecl(createFunc<BoolType>(lexs, "__le__"));
    env.insertValueDecl(createFunc<BoolType>(lexs, "__eq__"));
    env.insertValueDecl(createFunc<BoolType>(lexs, "__ne__"));
    env.insertValueDecl(createFunc<BoolType>(lexs, "__gt__"));
    env.insertValueDecl(createFunc<BoolType>(lexs, "__ge__"));
    env.insertValueDecl(createFunc<IntType>(lexs, "__hash__"));
    env.insertValueDecl(createFunc<BoolType>(lexs, "__bool__"));
    env.insertValueDecl(createFunc<InferredType>(lexs, "__getattr__"));
    env.insertValueDecl(createFunc<InferredType>(lexs, "__getattribute__"));
    env.insertValueDecl(createFunc<EmptyType>(lexs, "__setattr__"));
    env.insertValueDecl(createFunc<EmptyType>(lexs, "__delattr__"));
    //env.insertValueDecl(createFunc<ArrayType>(lexs, "__dir__"));
    std::unique_ptr<RecordType> recTy(new RecordType);
    recTy->setEnv(env);
    auto ident = insertOrFindIdent(lexs, "object");
    std::unique_ptr<Record> rec(new Record(ident));
    rec->setIsBuiltin(true);
    rec->setType(std::move(recTy));

    return TypeDeclPtr(rec.release());
}

namespace {

template <class NumT>
void addCommonNumOprtrs(Environment env, LexemeMap* lexs)
{
    // Unary
    env.insertValueDecl(createFunc<NumT>(lexs, "__pos__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__neg__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__abs__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__invert__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__round__"));
    env.insertValueDecl(createFunc<IntType>(lexs, "__floor_"));
    env.insertValueDecl(createFunc<IntType>(lexs, "__ceil__"));
    env.insertValueDecl(createFunc<IntType>(lexs, "__trunc__"));

    // Binary
    env.insertValueDecl(createFunc<NumT>(lexs, "__add__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__sub__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__mul__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__floordiv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__div__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__truediv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__mod__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__divmod__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__pow__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__lshift__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rshift__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rand__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__ror__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rxor__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__radd__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rsub__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rmul__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rfloordiv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rdiv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rtruediv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rmod__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rdivmod__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rpow__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rlshift__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rrshift__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rand__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__ror__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__rxor__"));

    // Augmented assignment
    env.insertValueDecl(createFunc<NumT>(lexs, "__iadd__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__isub__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__imul__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__ifloordiv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__idiv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__itruediv__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__imod__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__idivmod__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__ipow__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__ilshift__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__irshift__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__iand__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__ior__"));
    env.insertValueDecl(createFunc<NumT>(lexs, "__ixor__"));
}

std::unique_ptr<Record> createBasicRecord(LexemeMap* lexs,
                                          Environment env,
                                          const char* name)
{
    std::unique_ptr<RecordType> recTy(new RecordType);
    recTy->setEnv(env);
    auto ident = insertOrFindIdent(lexs, name);
    std::unique_ptr<Record> rec(new Record(ident));
    rec->setIsBuiltin(true);
    rec->setIsFake(true);
    rec->setType(std::move(recTy));
    return rec;
}

} // namespace anonymous

const Ident* PyBuiltin::rootTypeDeclName(LexemeMap *lexs) const
{
    return insertOrFindIdent(lexs, "object");
}

Builtin::TypeDeclPtr
PyBuiltin::createBasicTypeDecl(LexemeMap* lexs, Type::Kind kind) const
{
    switch (kind) {
    case Type::Kind::Int: {
        Environment env;
        addCommonNumOprtrs<IntType>(env, lexs);
        env.insertValueDecl(createFunc<IntType>(lexs, "bit_length"));
        env.insertValueDecl(createFunc<IntType>(lexs, "to_bytes"));
        env.insertValueDecl(createFunc<IntType>(lexs, "from_bytes"));
        return TypeDeclPtr(createBasicRecord(lexs, env, kPyBuiltinInt));
    }

    case Type::Kind::Float: {
        Environment env;
        addCommonNumOprtrs<FloatType>(env, lexs);
        env.insertValueDecl(createFunc<IntType>(lexs, "as_integer_ratio"));
        env.insertValueDecl(createFunc<BoolType>(lexs, "is_integer"));
        env.insertValueDecl(createFunc<StrType>(lexs, "hex"));
        env.insertValueDecl(createFunc<FloatType>(lexs, "from_hex"));
        return TypeDeclPtr(createBasicRecord(lexs, env, kPyBuiltinFloat));
    }

    default:
        return TypeDeclPtr();
    }
}

const Ident* PyBuiltin::basicTypeDeclName(LexemeMap* lexs, Type::Kind kind) const
{
    switch (kind) {
    case Type::Kind::Int:
        return insertOrFindIdent(lexs, kPyBuiltinInt);

    case Type::Kind::Float:
        return insertOrFindIdent(lexs, kPyBuiltinFloat);

    default:
        return nullptr;
    }
}

std::vector<std::string> PyBuiltin::automaticModules() const
{
    static std::vector<std::string> names {
        "numbers"
    };

    return names;
}
