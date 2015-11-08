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

///////////////////////////////////////////////////////
///                                                 ///
///         This is an INTERNAL header              ///
///                                                 ///
///   Do not include this header from outside the   ///
///   the uaiso lib or from any public API header   ///
///                                                 ///
///////////////////////////////////////////////////////

#ifndef UAISO_FLEXBISON_H__
#define UAISO_FLEXBISON_H__

#include "Common/LineCol.h"
#include "Parsing/Lexeme.h"

namespace uaiso {

class ParsingContext;

bool shouldStopNow(int line, int col, const ParsingContext* context);

namespace detail {

struct PartialParse {};

} // namespace detail

} // namespace uaiso

#define IGNORE_FOR_NOW(AST) delete AST
#define IGNORE_LIST_FOR_NOW(AST) delete AST->finishSR()

#define DECL_LOC(X, L) \
    SourceLoc loc##X(L.first_line, L.first_column, \
                     L.last_line, L.last_column, \
                     L.filename)
#define DECL_1_LOC(L1) DECL_LOC(A, L1)
#define DECL_2_LOC(L1, L2) DECL_1_LOC(L1); DECL_LOC(B, L2)
#define DECL_3_LOC(L1, L2, L3) DECL_2_LOC(L1, L2); DECL_LOC(C, L3)
#define DECL_4_LOC(L1, L2, L3, L4) DECL_3_LOC(L1, L2, L3); DECL_LOC(D, L4)
#define DECL_5_LOC(L1, L2, L3, L4, L5) DECL_4_LOC(L1, L2, L3, L4); DECL_LOC(E, L5)
#define DECL_6_LOC(L1, L2, L3, L4, L5, L6) DECL_5_LOC(L1, L2, L3, L4, L5); DECL_LOC(F, L6)

#define YY_USER_ACTION \
    CHECK_STOP_REQUEST; \
    ASSIGN_LOC;

#define ASSIGN_LOC \
    /* Would use yy_top_state, but it chrashes. See (3) in 3rdPartyBugs.txt. */ \
    if (!yyextra->hasTokenState()) { \
        yylloc->filename = yyextra->fileName(); \
        yylloc->first_line = yylloc->last_line = yylineno; \
        yylloc->first_column = yycolumn; \
        yylloc->last_column = yycolumn + yyleng; \
        yycolumn += yyleng; \
    } else { \
        yycolumn = yylloc->first_column + yyleng; \
    }

#define ENTER_STATE yyextra->enterTokenState()
#define LEAVE_STATE yyextra->leaveTokenState()

#define CHECK_STOP_REQUEST \
    if (shouldStopNow(yylloc->last_line, yylloc->last_column, yyextra)) { \
        yyextra->clearStopMark(); \
        yyextra->trackPhrase(Token(COMPLETION), \
                             LineCol(yylloc->first_line, yylloc->first_column), \
                             yyleng); \
        return yyextra->interceptRawToken(COMPLETION); \
    }

#define PROCESS_CORE(TOKEN) \
    do { \
        yyextra->trackToken(Token(TOKEN), \
                            LineCol(yylloc->first_line, yylloc->first_column)); \
        return yyextra->interceptRawToken(TOKEN); \
    } while (0)

#define PROCESS_TOKEN(TOKEN) \
    do { \
        yyextra->trackPhrase(Token(TOKEN), \
                             LineCol(yylloc->first_line, yylloc->first_column), \
                             yyleng); \
        PROCESS_CORE(TOKEN); \
    } while (0)

#define PROCESS_LEXEME(LEXEME, TOKEN) \
    do { \
        yyextra->trackLexeme<LEXEME>(yytext, \
                                     LineCol(yylloc->first_line, yylloc->first_column)); \
        PROCESS_TOKEN(TOKEN); \
    } while (0)

#define PROCESS_COMMENT(TOKEN) \
    do { \
        if (yyextra->allowComments()) { \
            PROCESS_TOKEN(TOKEN); \
        } \
    } while (0)

#define PROCESS_UNTERMINATED_COMMENT(TOKEN) \
    do { \
        if (yyextra->allowComments()) { \
            yyextra->trackPhrase(Token(TOKEN), \
                                 LineCol(yylloc->first_line, yylloc->first_column), \
                                 yyleng, true); \
            PROCESS_CORE(TOKEN); \
        } \
    } while (0)

#define PROCESS_IDENTIFIER PROCESS_LEXEME(Ident, IDENTIFIER)
#define PROCESS_STRING_LITERAL PROCESS_LEXEME(StrLit, STRING_LITERAL)
#define PROCESS_INTEGER_LITERAL PROCESS_LEXEME(NumLit, INTEGER_LITERAL)
#define PROCESS_CHAR_LITERAL PROCESS_LEXEME(NumLit, CHAR_LITERAL)
#define PROCESS_FLOAT_LITERAL PROCESS_LEXEME(NumLit, FLOAT_LITERAL)

#define FINISH_OR_POSTPONE \
    do { \
        if (yyextra->postponeEoF()) \
            return EOP; \
        return 0; \
    } while (0)

#endif
