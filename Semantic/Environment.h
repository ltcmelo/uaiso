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

#ifndef UAISO_ENVIRONMENT_H__
#define UAISO_ENVIRONMENT_H__

#include "Ast/AstFwd.h"
#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Common/Test.h"
#include "Semantic/SymbolFwd.h"
#include <iterator>
#include <unordered_map>
#include <vector>

namespace uaiso {

class Ident;
class LexemeMap;

/*!
 * \brief The Environment class
 *
 * An environment for types and values.
 *
 * \note This is an implictly shared type.
 */
class UAISO_API Environment final
{
public:
    Environment();

    bool isEmpty() const;

    /*!
     * \brief takeOver
     * \param env
     *
     * Move type and value symbols from environment \a env into this environment.
     */
    void takeOver(Environment env);

    /*!
     * \brief createSubEnv
     * \return
     *
     * Create a sub environment which has has this environment as its outer
     * environment.
     */
    Environment createSubEnv() const;

    /*!
     * \brief isRootEnv
     * \return
     *
     * Return whether this environment has an outer environment or not (in the
     * former case, this environment is said to be a root environment).
     */
    bool isRootEnv() const;

    /*!
     * \brief detachOuterEnv
     *
     * Detach this environment from its outer environment.
     */
    void detachOuterEnv();

    /*!
     * \brief outerEnv
     * \return
     *
     * Return the outer environment of this environment.
     */
    Environment outerEnv() const;

    /*!
     * \brief nestIntoOuterEnv
     * \param env
     *
     * Nest this environment into its outer environment.
     *
     * \sa nestedEnvs
     */
    void nestIntoOuterEnv();

    /*!
     * \brief nestedEnvs
     * \return
     *
     * Return the environments nested into this environment.
     */
    const std::vector<Environment>& nestedEnvs() const;

    /*!
     * \brief attachNamespace
     * \param sym
     * \param mergeEnv
     *
     * Attach namespace \a sym to this environment. If \a mergeEnv is true,
     * symbols from \a sym will be available in this environment.
     */
    void attachNamespace(std::unique_ptr<Namespace> sym,
                         bool mergeEnv);

    /*!
     * \brief fetchNamespace
     * \param name
     * \return
     */
    const Namespace* fetchNamespace(const Ident* name) const;

    /*!
     * \brief insertType
     * \param symbol
     *
     * Insert symbol \a sym in the type container of this environment. No
     * particular ordering is applied to symbols with duplicate name.
     */
    void insertType(std::unique_ptr<const TypeSymbol> sym);

    /*!
     * \brief insertValue
     * \param symbol
     *
     * Insert symbol \a sym in the value container of this environment. No
     * particular ordering is applied to symbols with duplicate name.
     */
    void insertValue(std::unique_ptr<const ValueSymbol> sym);

    /*!
     * \brief search
     * \param name
     * \return
     *
     * Look up symbol in the type and value environments. If the name is
     * present in both of them, or if there are symbols with duplicate
     * names, it's unspecified which match will be returned.
     */
    const DeclSymbol* lookUp(const Ident* name) const;

    /*!
     * \brief searchType
     * \param name
     * \return
     *
     * Look up symbol in the type environment only. If there are multiple
     * symbols with the same name, it's unspecified which match will be
     * returned.
     */
    const TypeSymbol* lookUpType(const Ident* name) const;

    /*!
     * \brief searchValue
     * \param name
     * \return
     *
     * Look up symbol in the value environment only.
     */
    const ValueSymbol* lookUpValue(const Ident* name) const;


    // DESIGN: This iterator doesn't support the implementation well enough.
    template <class SymbolT>
    class Iterator :
            public std::iterator<std::input_iterator_tag, const SymbolT>
    {
    public:
        Iterator& operator++() { ++it_; return *this; }
        const SymbolT* operator*() { return it_->second.get(); }
        bool operator!=(const Iterator& other) const { return it_ != other.it_; }

    private:
        friend class Environment;
        using BaseIterator =
            typename std::unordered_multimap<const Ident*,
                                             std::unique_ptr<const SymbolT>>::const_iterator;

        Iterator(BaseIterator it) : it_(it) {}
        BaseIterator it_;
    };

    template <class SymbolT> using Range =
        std::pair<Iterator<SymbolT>, Iterator<SymbolT>>;

    /*!
     * \brief searchTypes
     * \param name
     * \return
     *
     * Look up symbol in the type environment only. Lookup into outer
     * environments stops once the first symbol with the given \a name
     * is found (just like C++ lookup rules).
     */
    Range<TypeSymbol> lookUpTypes(const Ident* name) const;

    /*!
     * \brief searchValues
     * \param name
     * \return
     *
     * Look up symbol in the value environment only. Lookup into outer
     * environments stops once the first symbol with the given \a name
     * is found (just like C++ lookup rules).
     */
    Range<ValueSymbol> lookUpValues(const Ident* name) const;

    /*!
     * \brief listValues
     * \return
     *
     * List symbols in the value environment.
     */
    std::vector<const ValueSymbol*> listValues() const;

    /*!
     * \brief listTypes
     * \return
     *
     * List symbols in the type environment.
     */
    std::vector<const TypeSymbol*> listTypes() const;

    /*!
     * \brief list
     * \return
     */
    std::vector<const DeclSymbol*> list() const;

    /*!
     * \brief includeImport
     * \param import
     *
     * Include an import dependence in the environment.
     */
    void includeImport(std::unique_ptr<const Import> import);

    /*!
     * \brief imports
     * \return
     */
    std::vector<const Import*> imports() const;

private:
    DECL_CLASS_TEST(Environment)
    DECL_SHARED_DATA(Environment)
};

const ValueSymbol* lookUpValue(const NameAst* name,
                               Environment env,
                               const LexemeMap* lexemes);

const TypeSymbol* lookUpType(const NameAst* name,
                             Environment env,
                             const LexemeMap* lexemes);

} // namespace uaiso

#endif
