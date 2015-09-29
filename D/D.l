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

/* Known issues:
   - Artificial tokens for `! in' and `! is' to avoid shift/reduce conflicts, but they break on
     new lines. We could use states instead, like how it's done for `in {`. */

%option yylineno noyywrap nodefault stack
%option outfile="DLexer.cpp" header-file="DLexer.h"
%option reentrant bison-bridge bison-locations
%option prefix="D_yy"
%option extra-type="uaiso::DParsingContext*"

%{
#include "Ast/Ast.h"
#include "Common/Trace__.h"
#include "D/DParser.h"
#include "D/DParsingContext.h"
#include "Parsing/FlexBison__.h"
#include <cstdlib>

#define TRACE_NAME "D.l"

using namespace uaiso;

int yyxnestcommlevel; /* Nesting comment level */
int yyxprevstate; /* Previous state (valid under certain conditions) */

/* There are a few conflicts in the grammar which can be solved by lexically
   joining certain tokens. See known issues. */
#define HANDLE_TOKEN_JOINING(PTK, C, NTK) \
{ \
    BEGIN INITIAL; \
    if (yytext[0] == C) \
        PROCESS_TOKEN(NTK); \
    unput(yytext[0]); \
    PROCESS_TOKEN(PTK); \
}
%}

%x WAITING BCOMMENT NBCOMMENT DQSTRING QCHAR ESCSEQ JOINBRACE

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
"?" |
"," |
";" |
":" |
"$" |
"%" |
"*" |
"^" |
"~" |
"@" |
"#" { PROCESS_TOKEN(yytext[0]); }

"==" { PROCESS_TOKEN(EQUAL_EQUAL); }
"!=" { PROCESS_TOKEN(EXCLAM_EQUAL); }

"<=" { PROCESS_TOKEN(LESS_EQUAL); }
">=" { PROCESS_TOKEN(GREATER_EQUAL); }
"<>" { PROCESS_TOKEN(LESS_GREATER); }
"<>=" { PROCESS_TOKEN(LESS_GREATER_EQUAL); }
"!<>" { PROCESS_TOKEN(EXCLAM_LESS_GREATER); }
"!<>=" { PROCESS_TOKEN(EXCLAM_LESS_GREATER_EQUAL); }
"!<" { PROCESS_TOKEN(EXCLAM_LESS); }
"!<=" { PROCESS_TOKEN(EXCLAM_LESS_EQUAL); }
"!>" { PROCESS_TOKEN(EXCLAM_GREATER); }
"!>=" { PROCESS_TOKEN(EXCLAM_GREATER_EQUAL); }

"+=" { PROCESS_TOKEN(PLUS_EQUAL); }
"-=" { PROCESS_TOKEN(MINUS_EQUAL); }
"*=" { PROCESS_TOKEN(STAR_EQUAL); }
"/=" { PROCESS_TOKEN(SLASH_EQUAL); }
"%=" { PROCESS_TOKEN(PERCENT_EQUAL); }
"^=" { PROCESS_TOKEN(CARET_EQUAL); }
"^^=" { PROCESS_TOKEN(CARET_CARET_EQUAL); }
"&=" { PROCESS_TOKEN(AMPER_EQUAL); }
"|=" { PROCESS_TOKEN(PIPE_EQUAL); }
"<<=" { PROCESS_TOKEN(LESS_LESS_EQUAL); }
">>=" { PROCESS_TOKEN(GREATER_GREATER_EQUAL); }
">>>=" { PROCESS_TOKEN(GREATER_GREATER_GREATER_EQUAL); }
"~=" { PROCESS_TOKEN(TILDE_EQUAL); }

"&&" { PROCESS_TOKEN(AMPER_AMPER); }
"||" { PROCESS_TOKEN(PIPE_PIPE); }

"++" { PROCESS_TOKEN(PLUS_PLUS); }
"--" { PROCESS_TOKEN(MINUS_MINUS); }

"<<" { PROCESS_TOKEN(LESS_LESS); }
">>" { PROCESS_TOKEN(GREATER_GREATER); }
">>>" { PROCESS_TOKEN(GREATER_GREATER_GREATER); }

"^^" { PROCESS_TOKEN(CARET_CARET); }
"=>" { PROCESS_TOKEN(EQUAL_ARROW); }
".." { PROCESS_TOKEN(DOT_DOT); }
"..." { PROCESS_TOKEN(DOT_DOT_DOT); }

    /*--- Keywords ---*/

"alias" { PROCESS_TOKEN(ALIAS); }
"assert" { PROCESS_TOKEN(ASSERT); }
"body" { PROCESS_TOKEN(BODY); }
"break" { PROCESS_TOKEN(BREAK); }
"case" { PROCESS_TOKEN(CASE); }
"cast" { PROCESS_TOKEN(CAST); }
"catch" { PROCESS_TOKEN(CATCH); }
"class" { PROCESS_TOKEN(CLASS); }
"continue" { PROCESS_TOKEN(CONTINUE); }
"debug" { PROCESS_TOKEN(DEBUG); }
"default" { PROCESS_TOKEN(DEFAULT); }
"delegate" { PROCESS_TOKEN(DELEGATE); }
"delete" { PROCESS_TOKEN(DELETE); }
"do" { PROCESS_TOKEN(DO); }
"else" { PROCESS_TOKEN(ELSE); }
"enum" { PROCESS_TOKEN(ENUM); }
"finally" { PROCESS_TOKEN(FINALLY); }
"for" { PROCESS_TOKEN(FOR); }
"foreach" { PROCESS_TOKEN(FOREACH); }
"foreach_reverse" { PROCESS_TOKEN(FOREACH_REVERSE); }
"function" { PROCESS_TOKEN(FUNCTION); }
"goto" { PROCESS_TOKEN(GOTO); }
"if" { PROCESS_TOKEN(IF); }
"import" { PROCESS_TOKEN(IMPORT); }
"in" { BEGIN JOINBRACE; }
"!"[ \t]*"in"/[ \t\(] { PROCESS_TOKEN(NOT_IN_HACK); } /* Avoid shift/reduce conflict. Read known issues. */
"interface" { PROCESS_TOKEN(INTERFACE); }
"invariant" { PROCESS_TOKEN(INVARIANT); }
"is" { PROCESS_TOKEN(IS); }
"!"[ \t]*"is"/[ \t\(] { PROCESS_TOKEN(NOT_IS_HACK); } /* Avoid shift/reduce conflict. Read known issues. */
"macro" { PROCESS_TOKEN(MACRO); }
"mixin" { PROCESS_TOKEN(MIXIN); }
"module" { PROCESS_TOKEN(MODULE); }
"new" { PROCESS_TOKEN(NEW); }
"pragma" { PROCESS_TOKEN(PRAGMA); }
"property" { PROCESS_TOKEN(PROPERTY); }
"return" { PROCESS_TOKEN(RETURN); }
"struct" { PROCESS_TOKEN(STRUCT); }
"super" { PROCESS_TOKEN(SUPER); }
"switch" { PROCESS_TOKEN(SWITCH); }
"synchronized" { PROCESS_TOKEN(SYNCHRONIZED); }
"template" { PROCESS_TOKEN(TEMPLATE); }
"throw" { PROCESS_TOKEN(THROW); }
"try" { PROCESS_TOKEN(TRY); }
"typedef" { PROCESS_TOKEN(TYPEDEF); }
"typeid" { PROCESS_TOKEN(TYPEID); }
"typeof" { PROCESS_TOKEN(TYPEOF); }
"union" { PROCESS_TOKEN(UNION); }
"unittest" { PROCESS_TOKEN(UNITTEST); }
"version" { PROCESS_TOKEN(VERSION); }
"while" { PROCESS_TOKEN(WHILE); }
"with" { PROCESS_TOKEN(WITH); }
"__FILE__" { PROCESS_TOKEN(__FILE__MACRO); }
"__MODULE__" { PROCESS_TOKEN(__MODULE__MACRO); }
"__LINE__" { PROCESS_TOKEN(__LINE__MACRO); }
"__FUNCTION__" { PROCESS_TOKEN(__FUNCTION__MACRO); }
"__PRETTY_FUNCTION__" { PROCESS_TOKEN(__PRETTY_FUNCTION__MACRO); }
"__EOF__" { PROCESS_TOKEN(__EOF__MACRO); }
"__TIME__" { PROCESS_TOKEN(__TIME__MACRO); }
"__DATE__" { PROCESS_TOKEN(__DATE__MACRO); }
"__TIMESTAMP__" { PROCESS_TOKEN(__TIMESTAMP__MACRO); }
"__VERSION__" { PROCESS_TOKEN(__VERSION__MACRO); }
"__VENDOR__" { PROCESS_TOKEN(__VENDOR__MACRO); }
"__gshared" { PROCESS_TOKEN(__GSHARED); }
"__traits" { PROCESS_TOKEN(__TRAITS); }
"__vector" { PROCESS_TOKEN(__VECTOR); }
"__parameters" { PROCESS_TOKEN(__PARAMETERS); }

    /*--- Attributes ---*/

"abstract" { PROCESS_TOKEN(ABSTRACT); }
"align" { PROCESS_TOKEN(ALIGN); }
"asm" { PROCESS_TOKEN(ASM); }
"auto" { PROCESS_TOKEN(AUTO); }
"const" { PROCESS_TOKEN(CONST); }
"deprecated" { PROCESS_TOKEN(DEPRECATED); }
"disable" { PROCESS_TOKEN(DISABLE); }
"export" { PROCESS_TOKEN(EXPORT); }
"extern" { PROCESS_TOKEN(EXTERN); }
"final" { PROCESS_TOKEN(FINAL); }
"immutable" { PROCESS_TOKEN(IMMUTABLE); }
"inout" { PROCESS_TOKEN(INOUT); }
"lazy" { PROCESS_TOKEN(LAZY); }
"nogc" { PROCESS_TOKEN(NOGC); }
"nothrow" { PROCESS_TOKEN(NOTHROW); }
"out" { PROCESS_TOKEN(OUT); }
"override" { PROCESS_TOKEN(OVERRIDE); }
"package" { PROCESS_TOKEN(PACKAGE); }
"private" { PROCESS_TOKEN(PRIVATE); }
"protected" { PROCESS_TOKEN(PROTECTED); }
"pure" { PROCESS_TOKEN(PURE); }
"public" { PROCESS_TOKEN(PUBLIC); }
"ref" { PROCESS_TOKEN(REF); }
"safe" { PROCESS_TOKEN(SAFE); }
"scope" { PROCESS_TOKEN(SCOPE); }
"shared" { PROCESS_TOKEN(SHARED); }
"static" { PROCESS_TOKEN(STATIC); }
"system" { PROCESS_TOKEN(SYSTEM); }
"trusted" { PROCESS_TOKEN(TRUSTED); }
"volatile" { PROCESS_TOKEN(VOLATILE); }

    /*--- Builtin types ---*/

"bool" { PROCESS_TOKEN(BOOL); }
"byte" { PROCESS_TOKEN(BYTE); }
"cdouble" { PROCESS_TOKEN(COMPLEX_FLOAT64); }
"cent" { PROCESS_TOKEN(CENT); }
"cfloat" { PROCESS_TOKEN(COMPLEX_FLOAT32); }
"char" { PROCESS_TOKEN(CHAR); }
"creal" { PROCESS_TOKEN(COMPLEX_REAL); }
"dchar" { PROCESS_TOKEN(CHAR_UTF32); }
"double" { PROCESS_TOKEN(FLOAT64); }
"float" { PROCESS_TOKEN(FLOAT32); }
"ifloat" { PROCESS_TOKEN(IMAG_FLOAT32); }
"int" { PROCESS_TOKEN(INT); }
"ireal" { PROCESS_TOKEN(IMAG_REAL); }
"idouble" { PROCESS_TOKEN(IMAG_FLOAT64); }
"long" { PROCESS_TOKEN(INT64); }
"real" { PROCESS_TOKEN(REAL); }
"this" { PROCESS_LEXEME(Ident, THIS); }
"short" { PROCESS_TOKEN(INT16); }
"ubyte" { PROCESS_TOKEN(UBYTE); }
"ucent" { PROCESS_TOKEN(UCENT); }
"uint" { PROCESS_TOKEN(UINT); }
"ulong" { PROCESS_TOKEN(UINT64); }
"ushort" { PROCESS_TOKEN(UINT16); }
"wchar" { PROCESS_TOKEN(CHAR_UTF16); }
"void" { PROCESS_TOKEN(VOID); }

    /*--- Hacks ---*/

<JOINBRACE>[ \t\n] ;
<JOINBRACE>. { HANDLE_TOKEN_JOINING(IN, '{', IN_LBRACE_HACK) }

    /*--- Comments ---*/

"//"[^\n]*\n { yycolumn = 0; PROCESS_COMMENT(COMMENT); }
"/*" { BEGIN BCOMMENT; ENTER_STATE; yymore(); }
"/+" { ++yyxnestcommlevel; BEGIN NBCOMMENT; ENTER_STATE; yymore(); }
<BCOMMENT>"*/" { BEGIN INITIAL; LEAVE_STATE; PROCESS_COMMENT(MULTILINE_COMMENT); }
<BCOMMENT>. { yymore(); };
<BCOMMENT>"\n" { PROCESS_UNTERMINATED_COMMENT(MULTILINE_COMMENT); yymore(); }
<NBCOMMENT>"+/" {
                    if (--yyxnestcommlevel == 0) {
                        BEGIN INITIAL;
                        LEAVE_STATE;
                    }
                    PROCESS_COMMENT(MULTILINE_COMMENT);
                }
<NBCOMMENT>. { yymore(); };
<NBCOMMENT>"\n" { PROCESS_UNTERMINATED_COMMENT(MULTILINE_COMMENT); yymore(); }

    /*--- Literals ---*/

"true" { PROCESS_TOKEN(TRUE_LITERAL); }
"false" { PROCESS_TOKEN(FALSE_LITERAL); }
"null" { PROCESS_TOKEN(NULL_LITERAL); }
"\"" { BEGIN DQSTRING; ENTER_STATE; yymore(); }
<DQSTRING>"\\" { yyxprevstate = DQSTRING; BEGIN ESCSEQ; yymore(); }
<DQSTRING>"\n" { yymore(); };
<DQSTRING>"\"" { BEGIN INITIAL; LEAVE_STATE; PROCESS_STRING_LITERAL; }
<DQSTRING>. { yymore(); };
"\'" { BEGIN QCHAR; ENTER_STATE; yymore(); }
<QCHAR>"\\" { yyxprevstate = QCHAR; BEGIN ESCSEQ; yymore(); }
<QCHAR>"\n" { yymore(); };
<QCHAR>"\'" { BEGIN INITIAL; LEAVE_STATE; PROCESS_CHAR_LITERAL; }
<QCHAR>. { yymore(); };
<ESCSEQ>. { BEGIN yyxprevstate; yymore(); }
[0-9][0-9_]*[uUlL]{0,2}? |
0[bB][0-1_]*[uUlL]{0,2}? |
0[xX][0-9a-fA-F_]*[uUlL]{0,2}? {  PROCESS_INTEGER_LITERAL; }
([0-9]+[0-9_]*\.)/[^\.0-9_]{1} |
([0-9]+[0-9_]*\.?[0-9_]+)((E|e)(\+|\-)?[0-9_]+)?([fFLi]{0,2})? |
([0-9]+[0-9_]*)(E|e)(\+|\-)?[0-9_]+([fFLi]{0,2})? |
(\.[0-9]+[0-9_]*)((E|e)(\+|\-)?[0-9_]+)?([fFLi]{0,2})? |
0[xX]([0-9a-fA-F]+[0-9a-fA-F]*\.)/[^\.0-9_]{1} |
0[xX]([0-9a-fA-F]+[0-9a-fA-F]*\.?[0-9a-fA-F_]+)((P|p)(\+|\-)?[0-9_]+)?([fFLi]{0,2})? |
0[xX]([0-9a-fA-F]+[0-9a-fA-F]*)(P|p)(\+|\-)?[0-9_]+([fFLi]{0,2})? |
0[xX](\.[0-9a-fA-F]+[0-9a-fA-F]*)((P|p)(\+|\-)?[0-9_]+)?([fFLi]{0,2})? { PROCESS_FLOAT_LITERAL; }

    /*--- Identifier ---*/

[a-zA-Z_]([a-zA-Z0-9_])* { PROCESS_IDENTIFIER; }

    /*--- New line ---*/

"\n" { yycolumn = 0; }

    /*--- Whitespace ---*/

[ \t\r\f] ;

    /*--- EOF/EOP ---*/

<INITIAL><<EOF>> { BEGIN WAITING; return EOP; }
<WAITING>\n      { FINISH_OR_POSTPONE; }
<WAITING>.       { FINISH_OR_POSTPONE; }
<WAITING><<EOF>> { FINISH_OR_POSTPONE; }

. { PRINT_TRACE("Unknown token %s at %d,%d\n", yytext, yylineno, yycolumn); }
%%
