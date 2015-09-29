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

#include "Go/GoIncrementalLexer.h"
#include "Go/GoParser.h"
#include "Go/GoLexer.h"
#include "Go/GoParsingContext.h"
#include "Common/Error__.h"
#include "Parsing/IncrementalLexer__.h"
#include "Parsing/Phrasing.h"
#include "Parsing/Token.h"
#include <iostream>

// See Flex bug (1) in 3rdPartyBugs.txt
void GO_yyset_column(int column, yyscan_t yyscanner);

using namespace uaiso;

GoIncrementalLexer::GoIncrementalLexer()
{
    P->context_.reset(new GoParsingContext);
    P->context_->setAllowComments(true);
    P->context_->notifyProgramMatched();
}

GoIncrementalLexer::~GoIncrementalLexer()
{}

void GoIncrementalLexer::lex(const std::string& source)
{
    P->phrasing_.reset(new Phrasing);
    auto goContext = static_cast<GoParsingContext*>(P->context_.get());
    goContext->collectPhrasing(P->phrasing_.get());
    goContext->clearSemicolonInfo();

    yyscan_t scanner = 0;
    if (GO_yylex_init_extra(goContext, &scanner)) {
        Error::log("Failed to initializer scanner.\n");
        return;
    }

    YY_BUFFER_STATE bufferState =
            GO_yy_scan_bytes(source.c_str(), source.size(), scanner);

    YYSTYPE sdummy;
    YYLTYPE ldummy;
    GO_yyset_lineno(0, scanner); // See Flex bug (2) in 3rdPartyBugs.txt
    GO_yyset_column(0, scanner);
    while (GO_yylex(&sdummy, &ldummy, scanner)) {}

    GO_yy_delete_buffer(bufferState, scanner);
    GO_yylex_destroy(scanner);

    decideState();
}
