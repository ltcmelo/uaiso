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

#include "D/DIncrementalLexer.h"
#include "D/DParser.h"
#include "D/DLexer.h"
#include "D/DParsingContext.h"
#include "Common/Error__.h"
#include "Parsing/IncrementalLexer__.h"
#include "Parsing/Phrasing.h"

// See Flex bug (1) in 3rdPartyBugs.txt
void D_yyset_column(int column, yyscan_t yyscanner);

using namespace uaiso;

DIncrementalLexer::DIncrementalLexer()
{
    P->context_.reset(new DParsingContext);
    P->context_->setAllowComments(true);
    P->context_->notifyProgramMatched();
}

DIncrementalLexer::~DIncrementalLexer()
{}

void DIncrementalLexer::tokenize(const std::string& source)
{
    P->phrasing_.reset(new Phrasing);
    P->context_->collectPhrasing(P->phrasing_.get());

    yyscan_t scanner = 0;
    if (D_yylex_init_extra(static_cast<DParsingContext*>(P->context_.get()),
                           &scanner)) {
        Error::log("Failed to initializer scanner.\n");
        return;
    }

    YY_BUFFER_STATE bufferState =
            D_yy_scan_bytes(source.c_str(), source.size(), scanner);

    YYSTYPE sdummy;
    YYLTYPE ldummy;
    D_yyset_lineno(0, scanner); // See Flex bug (2) in 3rdPartyBugs.txt
    D_yyset_column(0, scanner);
    while (D_yylex(&sdummy, &ldummy, scanner)) {}

    D_yy_delete_buffer(bufferState, scanner);
    D_yylex_destroy(scanner);

    decideState();
}
