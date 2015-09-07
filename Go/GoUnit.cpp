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

#ifdef GO_YYDEBUG
#undef GO_YYDEBUG
#endif
#define GO_YYDEBUG 1

#include "Go/GoUnit.h"
#include "Go/GoParser.h"
#include "Go/GoLexer.h"
#include "Go/GoParsingContext.h"
#include "Ast/Ast.h"
#include "Common/Error__.h"
#include "Common/Trace__.h"
#include "Common/Util__.h"
#include "Parsing/Diagnostic.h"
#include "Parsing/Token.h"
#include "Parsing/Unit__.h"

#define TRACE_NAME "GoUnit"

// See Flex bug (1) in 3rdPartyBugs.txt
void GO_yyset_column(int column, yyscan_t yyscanner);

using namespace uaiso;

void GO_yyerror(const GO_YYLTYPE* yylocp,
                yyscan_t,
                ParsingContext* context,
                const char *s)
{
    DEBUG_TRACE("error at %d:%d %s (%s)\n",
                yylocp->last_line, yylocp->last_column, s, yylocp->filename);

    context->trackReport(Diagnostic::UnexpectedToken,
                         SourceLoc(yylocp->first_line, yylocp->first_column,
                                   yylocp->last_line, yylocp->last_column,
                                   yylocp->filename));
}

void GoUnit::parseCore(TokenMap* tokens,
                       LexemeMap* lexemes,
                       GoParsingContext* context)
{
    P->ast_.reset(nullptr);
    context->collectLexemes(lexemes);
    context->collectTokens(tokens);
    context->collectReports(P->reports_.get());
    context->setFileName(P->fullFileName_.c_str()); // Filename for Flex actions.

    yyscan_t scanner = 0;
    if (GO_yylex_init_extra(context, &scanner)) {
        Error::log("Failed to initializer scanner.\n");
        return;
    }

    YY_BUFFER_STATE buffState = nullptr;
    if (P->bit_.readFromFile_) {
        GO_yyset_in(P->file_, scanner);
    } else {
        buffState = GO_yy_scan_bytes(P->source_->c_str(), P->source_->size(), scanner);
        GO_yyset_lineno(0, scanner); // See Flex bug (2) in 3rdPartyBugs.txt
        GO_yyset_column(0, scanner);
    }

    //GO_yydebug = 1;
    int success = !GO_yyparse(scanner, context);
    if (success)
        P->ast_.reset(context->releaseAst());

    GO_yy_delete_buffer(buffState, scanner);
    GO_yylex_destroy(scanner);
}

void GoUnit::parse(TokenMap* tokens, LexemeMap* lexemes)
{
    GoParsingContext context;
    parseCore(tokens, lexemes, &context);
}

void GoUnit::parse(TokenMap* tokens,
                   LexemeMap* lexemes,
                   const LineCol& lineCol)
{
    GoParsingContext context;
    context.setStopMark(lineCol);
    parseCore(tokens, lexemes, &context);
}
