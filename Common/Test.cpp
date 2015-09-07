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

#include "Common/Test.h"
#include <iterator>

using namespace uaiso;

size_t Test::passed_ = 0;
size_t Test::skipped_ = 0;
size_t Test::failed_ = 0;
std::vector<std::string> Test::failedNames_;

void Test::printStats()
{
    std::cout << "[uaiso] Test stats" <<  "\n"
              << "        Passed:  " << passed_ << "\n"
              << "        Failed:  " << failed_ << "\n"
              << "        Skipped: " << skipped_ << std::endl;

    if (!failed_)
        return;

    std::cout << "        (";
    std::copy(failedNames_.begin(), failedNames_.end(),
              std::ostream_iterator<std::string>(std::cout, " "));
    std::cout << ")" << std::endl;
}
