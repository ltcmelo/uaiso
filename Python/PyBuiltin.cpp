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

const char* const kPyBuiltin = "<__pybuiltin__>";
const char* const kPyBuiltinBool = "<__pybuiltin_bool__>";
const char* const kPyBuiltinFloat = "<__pybuiltin_float__>";
const char* const kPyBuiltinInt = "<__pybuiltin_int__>";

// An incremented-per-use line to make sure there are no location collisions
// in the identifiers created for the builtins file.
int line = 0;

const Ident* insertOrFindIdent(LexemeMap* lexemes, const char* name)
{
    auto ident = lexemes->findAnyOf<Ident>(name);
    if (!ident)
        ident = lexemes->insertOrFind<Ident>(name, kPyBuiltin, LineCol(++line, 0));
    return ident;
}

template <class TypeT>
std::unique_ptr<Func> createFunc(LexemeMap* lexemes, const char* name)
{
    std::unique_ptr<FuncType> funcTy(new FuncType);
    funcTy->setReturnType(std::unique_ptr<Type>(new TypeT));
    auto ident = insertOrFindIdent(lexemes, name);
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

std::vector<Builtin::FuncPtr> PyBuiltin::createConstructors(LexemeMap* lexemes) const
{
    std::vector<FuncPtr> ctors;
    ctors.push_back(createFunc<BoolType>(lexemes, tokenSpell(TK_BOOL)));
    ctors.push_back(createFunc<IntType>(lexemes, tokenSpell(TK_INT)));
    ctors.push_back(createFunc<FloatType>(lexemes, tokenSpell(TK_FLOAT32)));
    ctors.push_back(createFunc<FloatType>(lexemes, tokenSpell(TK_COMPLEX_FLOAT32)));

    return ctors;
}

std::vector<Builtin::FuncPtr> PyBuiltin::createGlobalFuncs(LexemeMap* lexemes) const
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

Builtin::TypeDeclPtr PyBuiltin::createRootTypeDecl(LexemeMap* lexemes) const
{
    // The `object` class
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
    std::unique_ptr<RecordType> recTy(new RecordType);
    recTy->setEnv(env);
    auto ident = insertOrFindIdent(lexemes, "object");
    std::unique_ptr<Record> rec(new Record(ident));
    rec->setIsBuiltin(true);
    rec->setType(std::move(recTy));

    return TypeDeclPtr(rec.release());
}

namespace {

template <class NumT>
void addCommonNumOprtrs(Environment env, LexemeMap* lexemes)
{
    // Unary
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__pos__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__neg__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__abs__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__invert__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__round__"));
    env.insertTypeDecl(createFunc<IntType>(lexemes, "__floor_"));
    env.insertTypeDecl(createFunc<IntType>(lexemes, "__ceil__"));
    env.insertTypeDecl(createFunc<IntType>(lexemes, "__trunc__"));

    // Binary
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__add__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__sub__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__mul__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__floordiv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__div__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__truediv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__mod__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__divmod__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__pow__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__lshift__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rshift__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rand__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__ror__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rxor__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__radd__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rsub__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rmul__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rfloordiv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rdiv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rtruediv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rmod__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rdivmod__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rpow__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rlshift__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rrshift__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rand__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__ror__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__rxor__"));

    // Augmented assignment
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__iadd__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__isub__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__imul__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__ifloordiv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__idiv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__itruediv__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__imod__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__idivmod__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__ipow__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__ilshift__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__irshift__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__iand__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__ior__"));
    env.insertTypeDecl(createFunc<NumT>(lexemes, "__ixor__"));
}

} // namespace anonymous

const Ident* PyBuiltin::rootTypeDeclName(LexemeMap *lexemes) const
{
    return insertOrFindIdent(lexemes, "object");
}

Builtin::TypeDeclPtr
PyBuiltin::createBasicTypeDecl(LexemeMap* lexemes, Type::Kind kind) const
{
    switch (kind) {
    case Type::Kind::Int: {
        Environment env;
        addCommonNumOprtrs<IntType>(env, lexemes);
        env.insertTypeDecl(createFunc<IntType>(lexemes, "bit_length"));
        env.insertTypeDecl(createFunc<IntType>(lexemes, "to_bytes"));
        env.insertTypeDecl(createFunc<IntType>(lexemes, "from_bytes"));
        std::unique_ptr<RecordType> recTy(new RecordType);
        recTy->setEnv(env);
        auto ident = insertOrFindIdent(lexemes, kPyBuiltinInt);
        std::unique_ptr<Record> rec(new Record(ident));
        rec->setIsBuiltin(true);
        rec->setIsFake(true);
        rec->setType(std::move(recTy));
        return TypeDeclPtr(rec.release());
    }

    case Type::Kind::Float: {
        Environment env;
        addCommonNumOprtrs<FloatType>(env, lexemes);
        env.insertTypeDecl(createFunc<FloatType>(lexemes, "as_integer_ratio"));
        env.insertTypeDecl(createFunc<FloatType>(lexemes, "is_integer"));
        env.insertTypeDecl(createFunc<FloatType>(lexemes, "hex"));
        env.insertTypeDecl(createFunc<FloatType>(lexemes, "from_hex"));
        std::unique_ptr<RecordType> recTy(new RecordType);
        recTy->setEnv(env);
        auto ident = insertOrFindIdent(lexemes, kPyBuiltinFloat);
        std::unique_ptr<Record> rec(new Record(ident));
        rec->setIsBuiltin(true);
        rec->setIsFake(true);
        rec->setType(std::move(recTy));
        return TypeDeclPtr(rec.release());
    }

    default:
        return TypeDeclPtr();
    }
}

const Ident* PyBuiltin::basicTypeDeclName(LexemeMap* lexemes, Type::Kind kind) const
{
    switch (kind) {
    case Type::Kind::Int:
        return insertOrFindIdent(lexemes, kPyBuiltinInt);

    case Type::Kind::Float:
        return insertOrFindIdent(lexemes, kPyBuiltinFloat);

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
