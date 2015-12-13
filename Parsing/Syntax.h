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

#ifndef UAISO_SYNTAX_H__
#define UAISO_SYNTAX_H__

#include "Common/Config.h"
#include "Parsing/Token.h"
#include <string>

namespace uaiso {

/*!
 * \brief The Syntax class
 *
 * General characteristics of a language and of its syntax.
 */
class UAISO_API Syntax
{
public:
    virtual ~Syntax();

    /*!
     * \brief hasBlockLevelScope
     * \return
     *
     * Return whether a block defines a scope, typical in the C-family
     * languages.
     */
    virtual bool hasBlockLevelScope() const;

    /*!
     * \brief hasFuncLevelScope
     * \return
     *
     * Return whether a function defines a scope, most languages do.
     */
    virtual bool hasFuncLevelScope() const;

    /*!
     * \brief hasExecutableRecord
     * \return false
     *
     * Return whether the class-specifying syntax is formed by statement(s)
     * that must be executed at runtime in order to bind names. Python and
     * Ruby are example of such languages, as opposed to C-family languages
     * in which a class is a declaration.
     */
    virtual bool hasExecutableRecord() const;

    enum Structure
    {
        ExprBased,
        DeclBased,
        StmtBased,
    };

    /*!
     * \brief structure
     * \return
     */
    virtual Structure structure() const = 0;

    virtual std::string sourceFileSuffix() const = 0;

    virtual std::string packageSeparator() const;

    virtual std::string memberAccessOprtr() const;

    virtual std::string funcCallDelim() const;

    virtual bool isIdentFirstChar(char ch) const;

    virtual bool isIdentChar(char ch) const;

    virtual Token classifyIdent(const char* spell, size_t leng) const;

    virtual bool isStrLitQuote(char ch) const;

    virtual bool isOctalPrefix(char ch) const;

    virtual bool isHexPrefix(char ch) const;

    virtual bool isBinPrefix(char ch) const;

    virtual bool isExponent(char ch) const;
};

} // namespace uaiso

#endif
