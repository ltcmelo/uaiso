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

#ifdef D_YYDEBUG
#undef D_YYDEBUG
#endif
#define D_YYDEBUG 1

#include "D/DUnit.h"
#include "D/DParser.h"
#include "D/DLexer.h"
#include "D/DParsingContext.h"
#include "Ast/Ast.h"
#include "Common/Error__.h"
#include "Common/Trace__.h"
#include "Common/Util__.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/Token.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit__.h"

#define TRACE_NAME "DUnit"

// See Flex bug (1) in 3rdPartyBugs.txt
void D_yyset_column(int column, yyscan_t yyscanner);

using namespace uaiso;

void D_yyerror(D_YYLTYPE* yylocp,
               yyscan_t scanner,
               ParsingContext* context,
               const char *s)
{
    UAISO_UNUSED(scanner);

    DEBUG_TRACE("error at %d:%d %s (%s)\n",
                yylocp->last_line, yylocp->last_column, s, yylocp->filename);

    context->trackReport(Diagnostic::UnexpectedToken,
                         SourceLoc(yylocp->first_line, yylocp->first_column,
                                   yylocp->last_line, yylocp->last_column,
                                   yylocp->filename));
}

void DUnit::parseCore(TokenMap* tokens,
                      LexemeMap* lexemes,
                      DParsingContext* context)
{
    P->ast_.reset(nullptr);
    context->collectLexemes(lexemes);
    context->collectTokens(tokens);
    context->collectReports(P->reports_.get());
    context->setFileName(P->fullFileName_.c_str()); // Filename for Flex actions.

    yyscan_t scanner = 0;
    if (D_yylex_init_extra(context, &scanner)) {
        Error::log("Failed to initializer scanner.\n");
        return;
    }

    YY_BUFFER_STATE buffState = nullptr;
    if (P->bit_.readFromFile_) {
        D_yyset_in(P->file_, scanner);
    } else {
        buffState = D_yy_scan_bytes(P->source_->c_str(), P->source_->size(), scanner);
        D_yyset_lineno(0, scanner); // See Flex bug (2) in 3rdPartyBugs.txt
        D_yyset_column(0, scanner);
    }

    //D_yydebug = 1;
    int success = !D_yyparse(scanner, context);
    if (success)
        P->ast_.reset(context->releaseAst());

    D_yy_delete_buffer(buffState, scanner);
    D_yylex_destroy(scanner);
}

void DUnit::parse(TokenMap* tokens, LexemeMap* lexemes)
{
    DParsingContext context;
    parseCore(tokens, lexemes, &context);
}

void DUnit::parse(TokenMap* tokens,
                  LexemeMap* lexemes,
                  const LineCol& lineCol)
{
    DParsingContext context;
    context.setStopMark(lineCol);
    parseCore(tokens, lexemes, &context);
}
