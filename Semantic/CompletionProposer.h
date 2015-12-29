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

#ifndef UAISO_COMPLETIONPROPOSER_H__
#define UAISO_COMPLETIONPROPOSER_H__

#include "Ast/AstFwd.h"
#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Common/Test.h"
#include "Semantic/Snapshot.h"
#include "Semantic/SymbolFwd.h"
#include <tuple>
#include <vector>

namespace uaiso {

class Factory;
class LexemeMap;

/*!
 * \brief The CompletionProposer class
 */
class UAISO_API CompletionProposer final
{
public:
    CompletionProposer(Factory* factory);
    ~CompletionProposer();

    /*!
     * \brief The ResultCode enum
     */
    enum ResultCode : char
    {
        InternalError,               //!< Internal logic error
        CompletionAstNotFound,       //!< CompletionNameAst not found in ProgramAst
        UnboundCompletionAstContext, //!< Context of CompletionNameAst is not useful
        UnknownSymbol,               //!< Symbol is unknown
        UndefinedType,               //!< Symbol has undefined type
        TypeNotYetInferred,          //!< Type of symbol not yet inferred
        InvalidType,                 //!< Type of symbol has no environment
        UnresolvedElaborateType,     //!< Type of symbol is elaborate and not resolved
        CaseNotImplemented,          //!< Completion case not implemented yet.
        Success                      //!< Success
    };

    using Symbols = std::vector<const Decl*>;
    using Result = std::tuple<Symbols, ResultCode>;

    Result propose(ProgramAst* ast, const LexemeMap* lexemes);

private:
    DECL_PIMPL(CompletionProposer)
    DECL_CLASS_TEST(CompletionProposer)
};

} // namespace uaiso

#endif
