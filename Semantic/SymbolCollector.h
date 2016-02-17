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

#ifndef UAISO_SYMBOLCOLLECTOR_H__
#define UAISO_SYMBOLCOLLECTOR_H__

#include "Ast/AstFwd.h"
#include "Common/Pimpl.h"
#include "Parsing/SourceLoc.h"
#include "Semantic/Snapshot.h"
#include <tuple>
#include <vector>

namespace uaiso {

class Decl;
class Factory;
class LexemeMap;

class UAISO_API SymbolCollector final
{
public:
    SymbolCollector(Factory* factory);
    ~SymbolCollector();

    /*!
     * \brief The Mention enum
     */
    enum class Mention : char
    {
        Def, //! A definition
        Use  //! A use
    };

    using MentionInfo = std::tuple<Mention, const Decl*, SourceLoc>;

    /*!
     * \brief collect
     * \param ast
     * \pre The AST must have already been gone through the Binder.
     * \return
     *
     * Collect symbol definitions.
     */
    std::vector<MentionInfo> collectDefs(ProgramAst* ast);

    /*!
     * \brief collect
     * \param ast
     * \param context
     * \param lexs
     * \pre The AST must have already been gone through the Binder.
     * \return
     *
     * Collect symbol definitions and uses.
     */
    std::vector<MentionInfo> collect(ProgramAst* ast, const LexemeMap* lexs);

private:
    DECL_PIMPL(SymbolCollector)

    template <class VisitorT>
    std::vector<MentionInfo> collectCore(ProgramAst* ast, VisitorT& vis);
};

} // namespace uaiso

#endif
