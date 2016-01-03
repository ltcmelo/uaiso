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

#ifndef UAISO_IMPORTRESOLVER_H__
#define UAISO_IMPORTRESOLVER_H__

#include "Common/Config.h"
#include "Common/Pimpl.h"
#include <cstdio>
#include <string>
#include <vector>

namespace uaiso {

class Factory;
class Import;

/*!
 * \brief The ImportResolver class
 */
class UAISO_API ImportResolver final
{
public:
    ImportResolver(Factory* factory);
    ~ImportResolver();

    /*!
     * \brief resolve
     * \param import
     * \return
     *
     * Attempt to resolve the import locally from the original directory.
     */
    std::vector<std::string> resolve(Import* import) const;

    /*!
     * \brief resolve
     * \param import
     * \param paths
     * \return
     *
     * Attempt to resolve the import locally from the original directory and
     * if a match if not found make another attempt with the given paths
     * (processed in order).
     */
    std::vector<std::string> resolve(Import* import,
                                     const std::vector<std::string>& searchPaths) const;

private:
    DECL_PIMPL(ImportResolver)
};

} // namespace uaiso

#endif
