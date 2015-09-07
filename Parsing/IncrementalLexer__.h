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

#ifndef UAISO_INCREMENTALLEXER_INTERNAL_H__
#define UAISO_INCREMENTALLEXER_INTERNAL_H__

#include "Parsing/IncrementalLexer.h"
#include "Parsing/ParsingContext.h"
#include "Parsing/Phrasing.h"
#include "Common/Assert.h"

namespace uaiso {

struct IncrementalLexer::IncrementalLexerImpl
{
    IncrementalLexerImpl()
        : state_(IncrementalLexer::InCode)
        , phrasing_(new Phrasing)
    {}

    IncrementalLexer::State state_;
    std::unique_ptr<Phrasing> phrasing_;
    std::unique_ptr<ParsingContext> context_;
};

} // namespace uaiso

#endif
