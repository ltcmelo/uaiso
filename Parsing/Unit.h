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

#ifndef UAISO_UNIT_H__
#define UAISO_UNIT_H__

#include "Parsing/Diagnostic.h"
#include "Ast/AstFwd.h"
#include "Common/LineCol.h"
#include "Common/Pimpl.h"
#include "Common/Test.h"
#include <memory>
#include <string>

namespace uaiso {

class LexemeMap;
class ParsingContext;
class TokenMap;

/*!
 * \brief The Unit class
 */
class UAISO_API Unit
{
public:
    /*!
     * \brief Unit
     */
    Unit();

    Unit(const Unit&) = delete;
    Unit(Unit&&) = delete;
    Unit& operator=(const Unit&) = delete;
    Unit& operator=(Unit&&) = delete;

    virtual ~Unit();

    /*!
     * \brief assignInput
     * \param source
     */
    virtual void assignInput(const std::string& source);

    /*!
     * \brief assignInput
     * \param file
     */
    virtual void assignInput(FILE* file);

    /*!
     * \brief setFileName
     * \param name
     */
    void setFileName(const std::string& fullFileName);

    /*!
     * \brief fullfileName
     * \return
     */
    const std::string& fileName() const;

    /*!
     * \brief parse
     * \param tokens
     * \param lexs
     *
     * Parse the assigned input.
     */
    virtual void parse(TokenMap* tokens,
                       LexemeMap* lexs) = 0;

    /*!
     * \brief parse
     * \param tokens
     * \param lexs
     * \param lineCol
     *
     * Parse the assigned input with a request for code completion at the
     * given line/col, which must be the position immediately after the
     * last valid token (e.g. `.`).
     */
    virtual void parse(TokenMap* tokens,
                       LexemeMap* lexs,
                       const LineCol& lineCol) = 0;

    /*!
     * \brief ast
     * \return
     *
     * Return the parsed AST.
     */
    Ast* ast() const;

    /*!
     * \brief releaseReports
     * \return
     */
    DiagnosticReports* releaseReports();

protected:
    DECL_CLASS_TEST(Unit)
    DECL_PIMPL(Unit)
};

} // namespace uaiso

#endif
