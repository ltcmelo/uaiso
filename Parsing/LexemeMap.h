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

#ifndef UAISO_LEXEMEMAP_H__
#define UAISO_LEXEMEMAP_H__

#include "Common/Config.h"
#include "Common/LineCol.h"
#include "Common/Pimpl.h"
#include <string>
#include <tuple>
#include <vector>

namespace uaiso {

class Ident;
class Keyword;
class Lexeme;
class NumLit;
class StrLit;

/*!
 * \brief The LexemeMap class
 */
class UAISO_API LexemeMap final
{
public:
    /*!
     * \brief LexemeMap
     */
    LexemeMap();

    LexemeMap(const LexemeMap&) = delete;
    LexemeMap& operator=(const LexemeMap&) = delete;
    ~LexemeMap();

    /*!
     * \brief insertOrFind
     */
    template <class ValueT>
    const ValueT* insertOrFind(const std::string& s,
                               const std::string& fullFileName,
                               const LineCol& lineCol);

    /*!
     * \brief find
     */
    template <class ValueT>
    const ValueT* find(const std::string& fullFileName,
                       const LineCol& lineCol) const;

    /*!
     * \brief list
     * \warning This method creates copies of the map's underlying storage.
     */
    template <class ValueT>
    std::vector<std::tuple<const ValueT*, LineCol>> list(const std::string& fullFileName) const;

    /*!
     * \brief clear
     */
    void clear();

    /*!
     * \brief clear
     * \param fullFileName
     */
    void clear(const std::string& fullFileName);

    /*!
     * \brief selfIdent
     * \return
     *
     * Return special ientifier `self`.
     */
    const Ident* self() const;

private:
    DECL_PIMPL(LexemeMap)

    void insertPredefined();
};

} // namespace uaiso

#endif
