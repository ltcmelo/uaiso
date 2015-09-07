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

#include "Common/Trace__.h"
#include "Common/FileInfo.h"
#include "Common/Assert.h"

#define TRACE_NAME "FileInfo"

using namespace uaiso;

struct uaiso::FileInfo::FileInfoImpl
{
    FileInfoImpl(const std::string& fullFileName)
        : fullFileName_(fullFileName)
    {}

    std::string fullFileName_;
};

FileInfo::FileInfo(const std::string& fullFileName)
    : impl_(new FileInfoImpl(fullFileName))
{
    UAISO_ASSERT(!fullFileName.empty(), {}, "non-empty name required");
    UAISO_ASSERT(fullFileName.find('/') != std::string::npos
                     || fullFileName.find('\\') != std::string::npos,
                 impl_->fullFileName_.clear(),
                 "full path required");
}

FileInfo::~FileInfo()
{}

std::string FileInfo::fileName() const
{
    UAISO_ASSERT(!impl_->fullFileName_.empty(), return "");

    size_t pos = impl_->fullFileName_.rfind('/');
    return impl_->fullFileName_.substr(pos + 1);
}

std::string FileInfo::fileBaseName() const
{
    UAISO_ASSERT(!impl_->fullFileName_.empty(), return "");

    size_t pos2 = impl_->fullFileName_.rfind('.');
    size_t pos1 = impl_->fullFileName_.rfind('/', pos2);
    return impl_->fullFileName_.substr(pos1 + 1, pos2 - (pos1 + 1));
}

std::string FileInfo::fullFileName() const
{
    UAISO_ASSERT(!impl_->fullFileName_.empty(), return "");

    return impl_->fullFileName_;
}

std::string FileInfo::dir() const
{
    UAISO_ASSERT(!impl_->fullFileName_.empty(), return "");

    size_t pos1 = std::string::npos;
    size_t pos2 = impl_->fullFileName_.rfind('/');
    if (pos2 == 0)
        return "";
    pos1 = impl_->fullFileName_.rfind('/', pos2 - 1);
    if (pos1 == std::string::npos)
        pos1 = 0;
    return impl_->fullFileName_.substr(pos1 + 1, pos2 - (pos1 + 1));
}

std::string FileInfo::fullDir() const
{
    UAISO_ASSERT(!impl_->fullFileName_.empty(), return "");

    size_t pos = impl_->fullFileName_.rfind('/');
    return impl_->fullFileName_.substr(0, pos + 1);
}

char FileInfo::dirSeparator()
{
    return '/';
}
