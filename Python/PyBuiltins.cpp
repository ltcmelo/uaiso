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

#include "Python/PyBuiltins.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"

using namespace uaiso;

namespace {

const std::string kPyBuiltinsFile = "<__pybuiltins__>";

template <class TypeT>
std::unique_ptr<Func> createFunc(LexemeMap* lexemes, const char* name)
{
    static int line = 0; // Incrementing line (no location collision).

    std::unique_ptr<FuncType> funcTy(new FuncType);
    funcTy->setReturnType(std::unique_ptr<Type>(new TypeT));
    auto ident = lexemes->insertOrFind<Ident>(name, kPyBuiltinsFile, LineCol(++line, 0));
    std::unique_ptr<Func> func(new Func(ident));
    func->setIsBuiltin(true);
    func->setType(std::move(funcTy));

    return func;
}

} // namespace uaiso

const char* PyBuiltins::tokenSpell(Token tk) const
{
    switch (tk) {
    case TK_FLOAT32:
        return "float";
    case TK_COMPLEX_FLOAT32:
        return "complex";
    default:
        return Builtins::tokenSpell(tk);
    }
}

std::vector<Builtins::FuncPtr> PyBuiltins::valueConstructors(LexemeMap* lexemes) const
{
    std::vector<FuncPtr> ctors;
    ctors.push_back(createFunc<BoolType>(lexemes, tokenSpell(TK_BOOL)));
    ctors.push_back(createFunc<IntType>(lexemes, tokenSpell(TK_INT)));
    ctors.push_back(createFunc<FloatType>(lexemes, tokenSpell(TK_FLOAT32)));
    ctors.push_back(createFunc<FloatType>(lexemes, tokenSpell(TK_COMPLEX_FLOAT32)));

    return ctors;
}

std::vector<std::string> PyBuiltins::moduleNames() const
{
    static std::vector<std::string> names {
        "numbers"
    };

    return names;
}
