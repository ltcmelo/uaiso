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

#ifndef UAISO_INCREMENTALLEXER_H__
#define UAISO_INCREMENTALLEXER_H__

#include "Common/Config.h"
#include "Common/Pimpl.h"
#include <cstdint>
#include <memory>
#include <string>

namespace uaiso {

class Phrasing;

/*!
 * \brief The IncrementalLexer class
 */
class UAISO_API IncrementalLexer
{
public:
    virtual ~IncrementalLexer();

    // TODO: For nested comments we need to be able to count.
    enum State : uint8_t
    {
        InCode,
        InMultilineComment
    };

    virtual void lex(const std::string& source) = 0;

    void lex(const std::string& source, State state);

    Phrasing* releasePhrasing();

    State state() const;

protected:
    IncrementalLexer();

    void decideState();

    DECL_PIMPL(IncrementalLexer)
};

} // namespace uaiso

#endif
