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

#include "Go/GoSanitizer.h"
#include "Common/FileInfo.h"
#include "Parsing/Lexeme.h"
#include <iostream>

using namespace uaiso;

bool GoSanitizer::packageMatchesDir(const std::string& fullFileName,
                                    const std::string& packageName) const
{
    if (packageName == "main")
        return true;

    std::string dir = FileInfo(fullFileName).dir();
    if (dir == packageName)
        return true;

    return false;
}

bool GoSanitizer::shouldMergeImport(const Ident* mode) const
{
    return mode && mode->str() == ".";
}
