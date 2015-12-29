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

#ifndef UAISO_BUILTIN_H__
#define UAISO_BUILTIN_H__

#include "Common/Config.h"
#include "Parsing/Token.h"
#include "Semantic/Type.h"
#include <memory>
#include <vector>

namespace uaiso {

class Func;
class Ident;
class LexemeMap;
class TypeDecl;

/*!
 * \brief The Builtins class
 */
class UAISO_API Builtin
{
public:
    virtual ~Builtin();

    using FuncPtr = std::unique_ptr<Func>;
    using TypeDeclPtr = std::unique_ptr<TypeDecl>;

    /*!
     * \brief tokenSpell
     * \param tk
     * \return
     *
     * Return the actual spelling of a token in a given language.
     *
     * \remarks Tokens are unified, based on their typical meaning (see
     * Tokens.def). This is a hook to circumvent this issue.
     */
    virtual const char* tokenSpell(Token tk) const;

    /*!
     * \brief createConstructors
     * \param lexemes
     * \return
     *
     * Return a list of all builtin value constructors.
     */
    virtual std::vector<FuncPtr> createConstructors(LexemeMap* lexemes) const;

    /*!
     * \brief globalFuncs
     * \param lexemes
     * \return
     *
     * Return a list of all builtin (non-constructor) global functions.
     *
     * \sa createConstructors
     */
    virtual std::vector<FuncPtr> createGlobalFuncs(LexemeMap* lexemes) const;

    /*!
     * \brief createBasicTypeDecl
     * \param lexemes
     * \param kind
     * \return
     */
    virtual TypeDeclPtr createBasicTypeDecl(LexemeMap* lexemes, Type::Kind kind) const;

    /*!
     * \brief basicTypeDeclName
     * \param lexemes
     * \param kind
     * \return
     */
    virtual const Ident* basicTypeDeclName(LexemeMap* lexemes, Type::Kind kind) const;

    /*!
     * \brief createRootTypeDecl
     * \param lexemes
     * \return
     *
     * Return the root type decl from which every type inhertis from in a
     * purely OO language.
     *
     * \sa Lang::isPurelyOO
     */
    virtual TypeDeclPtr createRootTypeDecl(LexemeMap* lexemes) const;

    /*!
     * \brief rootTypeDeclName
     * \return
     *
     * Return the root type decl's name.
     *
     * \sa Builtin::createRootTypeDecl
     */
    virtual const Ident* rootTypeDeclName(LexemeMap* lexemes) const;

    /*!
     * \brief automaticModules
     * \return
     *
     * Return a list of all modules automatically imported.
     */
    virtual std::vector<std::string> automaticModules() const;
};

} // namespace uaiso

#endif
