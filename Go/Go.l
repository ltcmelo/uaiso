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

%option yylineno noyywrap nodefault stack
%option outfile="GoLexer.cpp" header-file="GoLexer.h"
%option reentrant bison-bridge bison-locations
%option prefix="GO_yy"
%option extra-type="uaiso::GoParsingContext*"

%{
#include "Ast/Ast.h"
#include "Common/Trace__.h"
#include "Go/GoParser.h"
#include "Go/GoParsingContext.h"
#include "Parsing/FlexBison__.h"
#include <cstdlib>

#define TRACE_NAME "Go.l"

using namespace uaiso;

int go_yyxprevstate; /* Previous state (valid under certain conditions) */

/* In addition to the standard location info, we also need to track the
   previous last column so that completion works correctly in the presence
   of auto-inserted semicolons (see HANDLE_AUTO_SEMICOLON). */
#undef  ASSIGN_LOC
#define ASSIGN_LOC \
    if (!yyextra->hasTokenState()) { \
        yylloc->filename = yyextra->fileName(); \
        yylloc->first_line = yylloc->last_line = yylineno; \
        yylloc->prev_last_column = yylloc->last_column; \
        yylloc->first_column = yycolumn; \
        yylloc->last_column = yycolumn + yyleng; \
        yycolumn += yyleng; \
    } else { \
        yycolumn = yylloc->first_column + yyleng; \
    }

/* Handling of lexical part of Go's automatic semicolon insertion
   rules. Statements and declarations optionally terminated with a
   semicolon are handled in the grammar directly - this is the
   same approach from Go's official compiler. */
#define HANDLE_AUTO_SEMICOLON \
    do { \
        if (yyextra->mayAddSemicolon()) { \
            yyextra->clearSemicolonInfo(); \
            yyless(0); \
            /* yylineno is decremented back in `yyless`, but column must be \
               handled manually through prev_last_column. */ \
            yylloc->first_line = yylloc->last_line = yylineno; \
            yycolumn = yylloc->prev_last_column; \
            yyleng = 1; \
            yylloc->first_column = yycolumn; \
            yylloc->last_column = yycolumn + yyleng; \
            PROCESS_TOKEN(';'); \
        } \
        yyextra->clearSemicolonInfo(); \
        yycolumn = 0; \
    } while(0)
%}

%x BCOMMENT DQSTRING RAWSTRING QCHAR ESCSEQ WAITING

%%
"<" |
">" |
"=" |
"/" |
"." |
"&" |
"|" |
"-" |
"+" |
"!" |
"(" |
")" |
"[" |
"]" |
"{" |
"}" |
"," |
";" |
":" |
"%" |
"*" |
"^" { PROCESS_TOKEN(yytext[0]); }

"==" { PROCESS_TOKEN(EQUAL_EQUAL); }
"!=" { PROCESS_TOKEN(EXCLAM_EQUAL); }
"<=" { PROCESS_TOKEN(LESS_EQUAL); }
">=" { PROCESS_TOKEN(GREATER_EQUAL); }

"+=" { PROCESS_TOKEN(PLUS_EQUAL); }
"-=" { PROCESS_TOKEN(MINUS_EQUAL); }
"*=" { PROCESS_TOKEN(STAR_EQUAL); }
"/=" { PROCESS_TOKEN(SLASH_EQUAL); }
"%=" { PROCESS_TOKEN(PERCENT_EQUAL); }
"^=" { PROCESS_TOKEN(CARET_EQUAL); }
"&=" { PROCESS_TOKEN(AMPER_EQUAL); }
"|=" { PROCESS_TOKEN(PIPE_EQUAL); }
"<<=" { PROCESS_TOKEN(LESS_LESS_EQUAL); }
">>=" { PROCESS_TOKEN(GREATER_GREATER_EQUAL); }
"&^=" { PROCESS_TOKEN(AMPER_CARET); }

"&&" { PROCESS_TOKEN(AMPER_AMPER); }
"||" { PROCESS_TOKEN(PIPE_PIPE); }

"++" { PROCESS_TOKEN(PLUS_PLUS); }
"--" { PROCESS_TOKEN(MINUS_MINUS); }

"<<" { PROCESS_TOKEN(LESS_LESS); }
">>" { PROCESS_TOKEN(GREATER_GREATER); }

":=" { PROCESS_TOKEN(COLON_EQUAL); }
"<-" { PROCESS_TOKEN(ARROW_DASH); }
"..." { PROCESS_TOKEN(DOT_DOT_DOT); }

    /*--- Keywords ---*/

"break" { PROCESS_TOKEN(BREAK); }
"case" { PROCESS_TOKEN(CASE); }
"chan" { PROCESS_TOKEN(CHAN); }
"continue" { PROCESS_TOKEN(CONTINUE); }
"default" { PROCESS_TOKEN(DEFAULT); }
"defer" { PROCESS_TOKEN(DEFER); }
"else" { PROCESS_TOKEN(ELSE); }
"fallthrough" { PROCESS_TOKEN(FALLTHROUGH); }
"for" { PROCESS_TOKEN(FOR); }
"func" { PROCESS_TOKEN(FUNC); }
"go" { PROCESS_TOKEN(GO); }
"goto" { PROCESS_TOKEN(GOTO); }
"if" { PROCESS_TOKEN(IF); }
"import" { PROCESS_TOKEN(IMPORT); }
"interface" { PROCESS_TOKEN(INTERFACE); }
"map" { PROCESS_TOKEN(MAP); }
"package" { PROCESS_TOKEN(PACKAGE); }
"range" { PROCESS_TOKEN(RANGE); }
"return" { PROCESS_TOKEN(RETURN); }
"select" { PROCESS_TOKEN(SELECT); }
"struct" { PROCESS_TOKEN(STRUCT); }
"switch" { PROCESS_TOKEN(SWITCH); }
"type" { PROCESS_TOKEN(TYPE); }
"var" { PROCESS_TOKEN(VAR); }

    /*--- Builtin types ---*/

"bool" { PROCESS_TOKEN(BOOL); }
"byte" { PROCESS_TOKEN(BYTE); }
"complex64" { PROCESS_TOKEN(COMPLEX_FLOAT64); }
"complex128" { PROCESS_TOKEN(COMPLEX_REAL); }
"const" { PROCESS_TOKEN(CONST); }
"float32" { PROCESS_TOKEN(FLOAT32); }
"float64" { PROCESS_TOKEN(FLOAT64); }
"int" { PROCESS_TOKEN(INT); }
"int8" { PROCESS_TOKEN(INT8); }
"int16" { PROCESS_TOKEN(INT16); }
"int32" { PROCESS_TOKEN(INT32); }
"int64" { PROCESS_TOKEN(INT64); }
"rune" { PROCESS_TOKEN(RUNE); }
"uint" { PROCESS_TOKEN(UINT); }
"uint8" { PROCESS_TOKEN(UINT8); }
"uint16" { PROCESS_TOKEN(UINT16); }
"uint32" { PROCESS_TOKEN(UINT32); }
"uint64" { PROCESS_TOKEN(UINT64); }
"uintptr" { PROCESS_TOKEN(UINT64); }

    /*--- Comments ---*/

"//"[^\n]*\n { HANDLE_AUTO_SEMICOLON; PROCESS_COMMENT(COMMENT); }
"/*" { BEGIN BCOMMENT; ENTER_STATE; yymore(); }
<BCOMMENT>"*/" { BEGIN INITIAL; LEAVE_STATE; PROCESS_COMMENT(MULTILINE_COMMENT); }
<BCOMMENT>. { yymore(); }
<BCOMMENT>"\n" { HANDLE_AUTO_SEMICOLON; PROCESS_UNTERMINATED_COMMENT(MULTILINE_COMMENT); yymore(); }

    /*--- Literals ---*/

"true" { PROCESS_TOKEN(TRUE_LITERAL); }
"false" { PROCESS_TOKEN(FALSE_LITERAL); }
"nil" { PROCESS_TOKEN(NULL_LITERAL); }
"\"" { BEGIN DQSTRING; ENTER_STATE; yymore(); }
<DQSTRING>"\\" { go_yyxprevstate = DQSTRING; BEGIN ESCSEQ; yymore(); }
<DQSTRING>"\n" { yymore(); }
<DQSTRING>"\"" { BEGIN INITIAL; LEAVE_STATE; PROCESS_STRING_LITERAL; }
<DQSTRING>. { yymore(); };
"`" { BEGIN RAWSTRING; ENTER_STATE; yymore(); }
<RAWSTRING>"`" { BEGIN INITIAL; LEAVE_STATE; PROCESS_STRING_LITERAL; }
<RAWSTRING>"\n" { yymore(); };
<RAWSTRING>. { yymore(); };
"\'" { BEGIN QCHAR; ENTER_STATE; yymore(); }
<QCHAR>"\\" { go_yyxprevstate = QCHAR; BEGIN ESCSEQ; yymore(); }
<QCHAR>"\n" { yymore(); };
<QCHAR>"\'" { BEGIN INITIAL; LEAVE_STATE; PROCESS_CHAR_LITERAL; }
<QCHAR>. { yymore(); };
<ESCSEQ>. { BEGIN go_yyxprevstate; yymore(); }
[0-9][0-9_]*[i]? |
0[xX][0-9a-fA-F_]*[i]? {  PROCESS_INTEGER_LITERAL; }
([0-9]+[0-9_]*\.)/[^\.0-9_]{1} |
([0-9]+[0-9_]*\.?[0-9_]+)((E|e)(\+|\-)?[0-9_]+)?[i]? |
([0-9]+[0-9_]*)(E|e)(\+|\-)?[0-9_]+[i]? |
(\.[0-9]+[0-9_]*)((E|e)(\+|\-)?[0-9_]+)?[i]? { PROCESS_FLOAT_LITERAL; }

    /*--- Identifier ---*/

[a-zA-Z_]([a-zA-Z0-9_])* { PROCESS_IDENTIFIER; }

    /*--- New line ---*/

"\n" { HANDLE_AUTO_SEMICOLON; }

    /*--- Whitespace ---*/

[ \t\r\f] ;

    /*--- EOF/EOP ---*/

<INITIAL><<EOF>> { BEGIN WAITING; return EOP; }
<WAITING>\n      { FINISH_OR_POSTPONE; }
<WAITING>.       { FINISH_OR_POSTPONE; }
<WAITING><<EOF>> { FINISH_OR_POSTPONE; }

. { PRINT_TRACE("unknown token %s at %d,%d\n", yytext, yylineno, yycolumn); }
%%
