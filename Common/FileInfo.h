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

#ifndef UAISO_FILEINFO_H__
#define UAISO_FILEINFO_H__

#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Common/Test.h"
#include <string>

namespace uaiso {

/*!
 * \brief The FileInfo class
 *
 * This class needs lots of complements. Not only in the API, but also regarding
 * cross-platform details, and corner-cases.
 */
class UAISO_API FileInfo final
{
public:
    ~FileInfo();

    /*!
     * \brief FileInfo
     * \param fullFileName - file name with complete path
     *
     * Within the docs, suppose an instance of this class is created as:
     *
     * \code
     * FileInfo fileInfo("/home/user/project/code.cpp");
     * \endcode
     */
    FileInfo(const std::string& fullFileName);

    /*!
     * \brief fileName
     * \return
     *
     * According to constructor's assumption, return is:
     * "code.cpp"
     */
    std::string fileName() const;

    /*!
     * \brief fileBaseName
     * \return
     *
     * According to constructor's assumption, return is:
     * "code"
     */
    std::string fileBaseName() const;

    /*!
     * \brief fullFileName
     * \return
     *
     * According to constructor's assumption, return is:
     * "/home/user/project/code.cpp"
     */
    std::string fullFileName() const;

    /*!
     * \brief dir
     * \return
     *
     * According to constructor's assumption, return is:
     * "project"
     */
    std::string dir() const;

    /*!
     * \brief fullDir
     * \return
     *
     * According to constructor's assumption, return is:
     * "/home/user/project"
     */
    std::string fullDir() const;

    /*!
     * \brief dirSeparator
     * \return
     */
    static char dirSeparator();

private:
    DECL_CLASS_TEST(FileInfo)
    DECL_SHARED_DATA(FileInfo)
};

bool operator==(const FileInfo& a, const FileInfo& b);
bool operator!=(const FileInfo& a, const FileInfo& b);

} // namespace uaiso

#endif
