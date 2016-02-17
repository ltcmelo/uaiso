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

#ifndef UAISO_ASSERT_H__
#define UAISO_ASSERT_H__

#include <iostream>

namespace uaiso {

inline void printAssert(const char* file, int line)
{
    std::cout << "[uaiso] Assert failed at "
              << file << ":" << line << std::endl;
}

inline void printAssert(const char* file, int line, const char* msg)
{
    std::cout << "[uaiso] Assert failed at "
              << file << ":" << line << ", " << msg << std::endl;
}

template <class DataT>
void printAssert(const char* file, int line, const char *msg,
                 const DataT& data)
{
    std::cout << "[uaiso] Assert failed at "
              << file << ":" << line << ", " << msg << " " << data << std::endl;
}

} // namespace uaiso

#define PRINT_ASSERT_A()          printAssert(__FILE__, __LINE__)
#define PRINT_ASSERT_B(MSG)       printAssert(__FILE__, __LINE__, MSG)
#define PRINT_ASSERT_C(MSG, DATA) printAssert(__FILE__, __LINE__, MSG, DATA)

// Variadic arguments supplied to UAISO_ASSERT will be substituted within
// __VA_ARGS in invocation of ALWAYS_4TH_ARG (this substitution might
// "introduce" new arguments). Afterwards, we always get the 4th one.
// Credits: http://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
#define ALWAYS_4TH_ARG(ARG1, ARG2, ARG3, ARG4, ...) ARG4
#define CHOOSE_PRINT_ASSERT(...) \
    ALWAYS_4TH_ARG(__VA_ARGS__, PRINT_ASSERT_C, PRINT_ASSERT_B, PRINT_ASSERT_A)

#define UAISO_ASSERT(COND, CODE, ...) \
    do { \
        if (COND) {} \
        else { \
            /* The `, ##` below causes the comma to be deleted upon an empty
               variable argument (not sure if this works in VC++).*/ \
            CHOOSE_PRINT_ASSERT("dummy", ##__VA_ARGS__)(__VA_ARGS__); \
            CODE; \
        } \
    } while (0)

#endif
