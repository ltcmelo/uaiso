/******************************************************************************
 * Copyright (c) 2014-2015-2015 Leandro T. C. Melo (ltcmelo@gmail.com)
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

///////////////////////////////////////////////////////
///                                                 ///
///         This is an INTERNAL header              ///
///                                                 ///
///   Do not include this header from outside the   ///
///   the uaiso lib or from any public API header   ///
///                                                 ///
///////////////////////////////////////////////////////

#ifndef UAISO_TRACE_H__
#define UAISO_TRACE_H__

#include <string>

namespace uaiso {

// In order to use the following macros, a TRACE_NAME must be #defined.
#define TRACE_CORE(M, ...) \
    do { \
        Trace(TRACE_NAME).M(__VA_ARGS__); \
    } while (0)

#define PRINT_TRACE(...) TRACE_CORE(print, __VA_ARGS__)
#define DEBUG_TRACE(...) TRACE_CORE(debug, __VA_ARGS__)

/*!
 * \brief The Trace class
 */
class Trace final
{
public:
    Trace(const std::string& name);

    /*!
     * \brief debug
     * \param msg
     *
     * Conditionally print \a msg, depending on whether or not this trace
     * is currently active.
     */
    void debug(const char* msg, ...);

    /*!
     * \brief print
     * \param msg
     *
     * Allways print \a msg, despite whether or not this trace is
     * currently active
     */
    void print(const char* msg, ...);

private:
    bool isActive() const;

    std::string name_;
};

} // namespace uaiso

#endif
