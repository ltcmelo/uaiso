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

%glr-parser

%expect 2
/* Expected shift/reduce conflicts:
   - Ambiguity with opening braces, which might initiate a composite literal
     or a block statement. From Go's website: if (x == T{a,b,c}[i]) {...}
     Official Go's parser is not GLR, but it handles the issue lexically
     by introducing an artificial `{` token for blocks.
   - In the expression part of a type switch statement. */


%define api.prefix "GO_yy"
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }
%parse-param { uaiso::GoParsingContext* context }
%locations
%output  "GoParser.cpp"
%defines "GoParser.h"

%code top {
/* Detailed parsing information (enables yydebug). */
#ifdef GO_YYDEBUG
#undef GO_YYDEBUG
#endif
#define GO_YYDEBUG 1
}

%code requires {

/* Make scanner type available. */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

/* The YYSTYPE depends on Ast. */
#include "Ast/Ast.h"

/* Forward declare the context, it's a yyparse parameter. */
namespace uaiso { class GoParsingContext; }

/* Location enhanced with filename. */
typedef struct GO_YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  int prev_last_column;
  const char* filename;
} GO_YYLTYPE;
# define GO_YYLTYPE_IS_DECLARED 1
# define GO_YYLTYPE_IS_TRIVIAL 1

# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (YYID (N))                                                     \
      {                                                                 \
         (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;         \
         (Current).first_column = YYRHSLOC (Rhs, 1).first_column;       \
         (Current).last_line    = YYRHSLOC (Rhs, N).last_line;          \
         (Current).last_column  = YYRHSLOC (Rhs, N).last_column;        \
         (Current).filename     = YYRHSLOC (Rhs, N).filename;           \
      }                                                                 \
      else                                                              \
      {                                                                 \
         (Current).first_line   = (Current).last_line   =               \
           YYRHSLOC (Rhs, 0).last_line;                                 \
         (Current).first_column = (Current).last_column =               \
           YYRHSLOC (Rhs, 0).last_column;                               \
         (Current).filename     = 0;                                    \
      }                                                                 \
    while (YYID (0))
}

%code provides {
/* Ensure that YYSTYPE and YYLTYPE are valid types. */
#define YYSTYPE GO_YYSTYPE
#define YYLTYPE GO_YYLTYPE
}

%{
#include "Go/GoParsingContext.h"
#include "Go/GoParser.h"
#include "Go/GoLexer.h"
#include "Go/GoFlexBison__.h"
#include <stdlib.h>
#include <stdio.h>

using namespace uaiso;

void GO_yyerror(const YYLTYPE* yylloc,
                yyscan_t scanner,
                uaiso::ParsingContext* context,
                const char *s);

%}

%union {
    uaiso::NameAst* name_;
    uaiso::NameAstList* names_;
    uaiso::SpecAst* spec_;
    uaiso::SpecAstList* specs_;
    uaiso::DeclAst* decl_;
    uaiso::DeclAstList* decls_;
    uaiso::ExprAst* expr_;
    uaiso::ExprAstList* exprs_;
    uaiso::StmtAst* stmt_;
    uaiso::StmtAstList* stmts_;
}

    /*--- Ast type declarations ---*/

%type <name_> Ident
%type <names_> NestedIdent

%type <spec_> Type BuiltinType PlainType CompositeType PointerType CastType
%type <spec_> StructType InterfaceType SpecialArgType Signature FuncType
%type <spec_> SenderChanType RecvChanType BidirChanType NonExprType
%type <specs_> NonExprTypeList

%type <decl_> Decl VarGroupDecl VarDecl VarSectionDecl
%type <decl_> FieldDecl RecordDecl TypeGroupDecl ConstDecl InterfaceMember
%type <decl_> FuncDecl FuncRecvDecl ParamGroupDecl ParamClauseDecl ParamDecl
%type <decl_> ImportClauseDecl ImportModuleDecl
%type <decls_> FieldDecls InterfaceMembers VarGroupDeclList RecordDeclList
%type <decls_> ParamGroupDeclList ParamDeclList VarDeclList Decls ImportList

%type <expr_> Expr UnaryExpr PostfixExpr PrimaryExpr SubrangeExpr ConvExpr Init
%type <expr_> BoolLit NullLit CharLit StringLit NumLit FuncLit CompositeLit
%type <expr_> EffectExpr
%type <exprs_> ExprList InitList

%type <stmt_> Stmt BlockStmt LabeledStmt IfStmt ForStmt GotoStmt
%type <stmt_> ReturnStmt BreakStmt ContinueStmt SwitchStmt CaseClauseStmt SelectStmt
%type <stmt_> GoStmt DeferStmt FallthroughStmt
%type <stmts_> StmtList CaseClauseStmts

%destructor { delete $$; } <*>
%destructor { delete $$->finishSR(); } <names_>
%destructor { delete $$->finishSR(); } <specs_>
%destructor { delete $$->finishSR(); } <decls_>
%destructor { delete $$->finishSR(); } <exprs_>
%destructor { delete $$->finishSR(); } <stmts_>


    /*--------------------------------------------------*/
    /*---            Token declarations              ---*/
    /*---                                            ---*/
    /*---  This section is AUTOMATICALLY GENERATED.  ---*/
    /*--- Do NOT edit manually, changes will be lost ---*/
    /*---       Please refer to Tokens.def           ---*/
    /*--------------------------------------------------*/
%token ABSTRACT 349 "abstract"
%token ALIAS 350 "alias"
%token ALIGN 351 "align"
%token ALIGNAS 352 "alignas"
%token ALIGNOF 353 "aligonf"
%token AMPER_AMPER 298 "&&"
%token AMPER_CARET 299 "&^"
%token AMPER_CARET_EQUAL 300 "&^="
%token AMPER_EQUAL 301 "&="
%token ARROW_DASH 302 "<-"
%token AS 354 "as"
%token ASM 355 "asm"
%token ASSERT 356 "assert"
%token AUTO 357 "auto"
%token BEGIN_BUILTIN_TYPES 477 "$builtin_types_begin_marker$"
%token BEGIN_CHAR_LITERAL 278 "$char_literal_begin_marker$"
%token BEGIN_COMMENT 266 "$comments_begin_marker$"
%token BEGIN_KEYWORD 348 "$keyword_begin_marker$"
%token BEGIN_LITERAL 273 "$literal_begin_marker$"
%token BEGIN_MULTICHAR_OPERATOR 297 "$multichar_operator_begin_marker$"
%token BEGIN_NUMERIC_LITERAL 274 "$numeric_literal_begin_marker$"
%token BEGIN_STRING_LITERAL 283 "$string_literal_begin_marker$"
%token BODY 358 "body"
%token BOOL 478 "bool"
%token BREAK 359 "break"
%token BYTE 479 "byte"
%token CARET_CARET 305 "^^"
%token CARET_CARET_EQUAL 306 "^^="
%token CARET_EQUAL 307 "^="
%token CASE 360 "case"
%token CAST 362 "cast"
%token CATCH 363 "catch"
%token CENT 480 "cent"
%token CHAN 361 "chan"
%token CHAR 484 "char"
%token CHAR_LITERAL 279 "<char_literal>"
%token CHAR_UTF16 485 "char_utf16"
%token CHAR_UTF16_LITERAL 280 "<char_utf16_literal>"
%token CHAR_UTF32 486 "char_utf32"
%token CHAR_UTF32_LITERAL 281 "<char_utf32_literal>"
%token CLASS 364 "class"
%token COLON_COLON 308 "::"
%token COLON_EQUAL 309 ":="
%token COMMENT 267 "<comment>"
%token COMPLETION 264 "<completion>"
%token COMPLEX_FLOAT32 482 "complex_float32"
%token COMPLEX_FLOAT64 481 "complex_float64"
%token COMPLEX_REAL 483 "complex_real"
%token CONST 365 "const"
%token CONSTEXPR 367 "constexpr"
%token CONST_CAST 366 "const_cast"
%token CONTINUE 368 "continue"
%token DASH_ARROW 303 "->"
%token DASH_ARROW_STAR 304 "->*"
%token DEBUG 369 "debug"
%token DECLTYPE 370 "decltype"
%token DEDENT 262 "<dedent>"
%token DEF 371 "def"
%token DEFAULT 372 "default"
%token DEFER 373 "defer"
%token DELEGATE 374 "delegate"
%token DELETE 375 "delete"
%token DEPRECATED 376 "deprecated"
%token DISABLE 377 "disable"
%token DO 378 "do"
%token DOT_DOT 313 ".."
%token DOT_DOT_DOT 314 "..."
%token DOT_STAR 315 ".*"
%token DOXY_COMMENT 268 "<doxy_comment>"
%token DYNAMIC_CAST 379 "dynamic_cast"
%token ELIF 380 "elif"
%token ELSE 381 "else"
%token END_ASCII 259 "$ascii_end_marker$"
%token END_BUILTIN_TYPES 507 "$builtin_types_end_marker$"
%token END_CHAR_LITERAL 282 "$char_literal_end_marker$"
%token END_COMMENT 272 "$comments_end_marker$"
%token END_KEYWORD 476 "$keyword_end_marker$"
%token END_LITERAL 296 "$literal_end_marker$"
%token END_MULTICHAR_OPERATOR 347 "$multichar_operator_end_marker$"
%token END_NUMERIC_LITERAL 277 "$numeric_literal_end_marker$"
%token END_STRING_LITERAL 292 "$string_literal_end_marker$"
%token ENUM 382 "enum"
%token EOP 258 "<end_of_program>"
%token EQUAL_ARROW 317 "=>"
%token EQUAL_EQUAL 316 "=="
%token EXCLAM_EQUAL 318 "!="
%token EXCLAM_GREATER 319 "!>"
%token EXCLAM_GREATER_EQUAL 320 "!>="
%token EXCLAM_LESS 321 "!<"
%token EXCLAM_LESS_EQUAL 322 "!<="
%token EXCLAM_LESS_GREATER 323 "!<>"
%token EXCLAM_LESS_GREATER_EQUAL 324 "!<>="
%token EXEC 383 "exec"
%token EXPLICIT 384 "explicit"
%token EXPORT 385 "export"
%token EXTERN 386 "extern"
%token FALLTHROUGH 387 "fallthrough"
%token FALSE_LITERAL 294 "<false_literal>"
%token FINAL 388 "final"
%token FINALLY 389 "finally"
%token FLOAT32 488 "float32"
%token FLOAT64 487 "float64"
%token FLOAT_LITERAL 276 "<float_literal>"
%token FOR 390 "for"
%token FOREACH 391 "foreach"
%token FOREACH_REVERSE 392 "foreach_reverse"
%token FRIEND 393 "friend"
%token FROM 394 "from"
%token FUNC 395 "func"
%token FUNCTION 396 "function"
%token GLOBAL 397 "global"
%token GO 398 "go"
%token GOTO 399 "goto"
%token GREATER_EQUAL 325 ">="
%token GREATER_GREATER 326 ">>"
%token GREATER_GREATER_EQUAL 329 ">>="
%token GREATER_GREATER_GREATER 327 ">>>"
%token GREATER_GREATER_GREATER_EQUAL 328 ">>>="
%token IDENTIFIER 265 "<identifier>"
%token IF 400 "if"
%token IMAG_FLOAT32 490 "imaginary_float32"
%token IMAG_FLOAT64 489 "imaginary_float64"
%token IMAG_REAL 491 "imaginary_real"
%token IMMUTABLE 401 "immutable"
%token IMPORT 402 "import"
%token IN 403 "in"
%token INDENT 261 "<indent>"
%token INLINE 405 "inline"
%token INOUT 406 "inout"
%token INT 492 "int"
%token INT16 494 "int16"
%token INT32 495 "int32"
%token INT64 496 "int64"
%token INT8 493 "int8"
%token INTEGER_LITERAL 275 "<integer_literal>"
%token INTERFACE 407 "interface"
%token INVALID 260 "<invalid>"
%token INVARIANT 408 "invariant"
%token IN_LBRACE_HACK 404 "in_{_hack"
%token IS 409 "is"
%token LAMBDA 410 "lambda"
%token LAZY 411 "lazy"
%token LESS_EQUAL 330 "<="
%token LESS_GREATER 333 "<>"
%token LESS_GREATER_EQUAL 334 "<>="
%token LESS_LESS 331 "<<"
%token LESS_LESS_EQUAL 332 "<<="
%token MACRO 412 "macro"
%token MAP 413 "map"
%token MINUS_EQUAL 335 "-="
%token MINUS_MINUS 336 "--"
%token MIXIN 414 "mixin"
%token MODULE 415 "module"
%token MULTILINE_COMMENT 269 "<multiline_comment>"
%token MULTILINE_DOXY_COMMENT 270 "<multiline_doxy_comment>"
%token MUTABLE 416 "mutable"
%token NAMESPACE 417 "namespace"
%token NESTING_COMMENT 271 "<nesting_comment>"
%token NEW 418 "new"
%token NEWLINE 263 "<newline>"
%token NOEXCEPT 419 "noexcept"
%token NOGC 420 "nogc"
%token NONLOCAL 421 "nonlocal"
%token NOTHROW 422 "nothrow"
%token NOT_IN_HACK 423 "!_in_hack"
%token NOT_IS_HACK 424 "!_is_hack"
%token NULL_LITERAL 293 "<null_literal>"
%token OPERATOR 425 "operator"
%token OUT 426 "out"
%token OVERRIDE 427 "override"
%token PACKAGE 428 "package"
%token PASS 429 "pass"
%token PERCENT_EQUAL 337 "%="
%token PIPE_EQUAL 338 "|="
%token PIPE_PIPE 339 "||"
%token PLUS_EQUAL 340 "+="
%token PLUS_PLUS 341 "++"
%token POUND_POUND 342 "##"
%token PRAGMA 430 "pragma"
%token PRINT 431 "print"
%token PRIVATE 432 "private"
%token PROPERTY 433 "property"
%token PROTECTED 434 "protected"
%token PUBLIC 435 "public"
%token PURE 436 "pure"
%token RANGE 437 "range"
%token RAW_STRING_LITERAL 288 "<raw_string_literal>"
%token RAW_UTF16_STRING_LITERAL 290 "<raw_utf16_string_literal>"
%token RAW_UTF32_STRING_LITERAL 291 "<raw_utf32_string_literal>"
%token RAW_UTF8_STRING_LITERAL 289 "<raw_utf8_string_literal>"
%token REAL 497 "real"
%token REF 438 "ref"
%token REGISTER 439 "register"
%token REINTERPRET_CAST 440 "reinterpret_cast"
%token RETURN 441 "return"
%token RUNE 498 "rune"
%token SAFE 442 "safe"
%token SCOPE 443 "scope"
%token SELECT 444 "select"
%token SHARED 445 "shared"
%token SIZEOF 446 "sizeof"
%token SLASH_EQUAL 310 "/="
%token SLASH_SLASH 311 "//"
%token SLASH_SLASH_EQUAL 312 "//="
%token STAR_EQUAL 343 "*="
%token STAR_STAR 344 "**"
%token STAR_STAR_EQUAL 345 "**="
%token STATIC 447 "static"
%token STATIC_ASSERT 448 "static_assert"
%token STATIC_CAST 449 "static_cast"
%token STRING_LITERAL 284 "<string_literal>"
%token STRING_UTF16_LITERAL 286 "<string_utf16_literal>"
%token STRING_UTF32_LITERAL 287 "<string_utf32_literal>"
%token STRING_UTF8_LITERAL 285 "<string_utf8_literal>"
%token STRUCT 450 "struct"
%token SUPER 451 "super"
%token SWITCH 452 "switch"
%token SYNCHRONIZED 453 "synchronized"
%token SYSTEM 454 "system"
%token TEMPLATE 455 "template"
%token THIS 456 "this"
%token THREAD_LOCAL 457 "thread_local"
%token THROW 458 "throw"
%token TILDE_EQUAL 346 "~="
%token TRUE_LITERAL 295 "<true_literal>"
%token TRUSTED 459 "trusted"
%token TRY 460 "try"
%token TYPE 461 "type"
%token TYPEDEF 462 "typedef"
%token TYPEID 463 "typeid"
%token TYPENAME 464 "typename"
%token TYPEOF 465 "typeof"
%token UBYTE 499 "ubyte"
%token UCENT 500 "ucent"
%token UINT 501 "uint"
%token UINT16 503 "uint16"
%token UINT32 504 "uint32"
%token UINT64 505 "uint64"
%token UINT8 502 "uint8"
%token UNION 466 "union"
%token UNITTEST 467 "unittest"
%token USING 468 "using"
%token VAR 469 "var"
%token VERSION 470 "version"
%token VIRTUAL 471 "virtual"
%token VOID 506 "void"
%token VOLATILE 472 "volatile"
%token WHILE 474 "while"
%token WITH 475 "with"
%token YIELD 473 "yield"
%token __ATTRIBUTE__ 508 "__attribute__"
%token __DATE__MACRO 509 "__date__"
%token __EOF__MACRO 510 "__eof__"
%token __FILE__MACRO 511 "__file__"
%token __FUNCTION__MACRO 512 "__function__"
%token __GSHARED 513 "__gshared"
%token __LINE__MACRO 514 "__line__"
%token __MODULE__MACRO 515 "__module__"
%token __PARAMETERS 516 "__parameters"
%token __PRETTY_FUNCTION__MACRO 517 "__pretty_function__"
%token __THREAD 518 "__thread"
%token __TIMESTAMP__MACRO 520 "__timestamp__"
%token __TIME__MACRO 519 "__time__"
%token __TRAITS 521 "__traits"
%token __VECTOR 522 "__vector"
%token __VENDOR__MACRO 523 "__vendor__"
%token __VERSION__MACRO 524 "__version__"
    /*------------------------------------------*/
    /*--- Tokens AUTOMATICALLY GENERATED end ---*/
    /*------------------------------------------*/


%nonassoc PREFER_SHIFT
%left ','
%left "||"
%left "&&"
%left '<' '>' "<=" ">=" "==" "!="
%left '+' '-' '|' '^'
%left '*' '/' '%' '&' "<<" ">>" "&^"
%left '('

%start Top

%error-verbose

%%
Top:
    Program
|   error Program
;

Program:
    PACKAGE Ident ';'
    {
        DECL_2_LOC(@1, @3);
        detail::actionProgram(context, locA, $2, locB);
    }
|   PACKAGE Ident ';' EOP
    {
        DECL_2_LOC(@1, @3);
        detail::actionProgram(context, locA, $2, locB);
    }
|   PACKAGE Ident ';' Decls
    {
        DECL_2_LOC(@1, @3);
        detail::actionProgram(context, locA, $2, locB, $4);
    }
|   PACKAGE Ident ';' Decls EOP
    {
        DECL_2_LOC(@1, @3);
        detail::actionProgram(context, locA, $2, locB, $4);
    }
;


    /*-------------------*/
    /*--- Expressions ---*/
    /*-------------------*/

Expr:
    UnaryExpr
|   Expr "||" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<LogicOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "&&" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<LogicAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "==" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "!=" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '<' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "<=" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '>' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr ">=" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '+' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AddExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '-' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<SubExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '|' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<BitOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '^' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<BitXorExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '*' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<MulExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '/' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<DivExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '%' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ModExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "<<" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr ">>" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '&' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<BitAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "&^" Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<BitAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
;

UnaryExpr:
    '&' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<AddrOfExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '*' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<PtrDerefExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '-' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<MinusExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '+' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<PlusExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '!' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<LogicNotExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '~' UnaryExpr
    {
        // TODO: Report error, complement is with ^
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BitCompExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '^' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BitCompExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   "<-" UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ChanExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   PostfixExpr
;

PostfixExpr:
    PrimaryExpr
|   PostfixExpr '.' Ident
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<MemberAccessExprAst>()->setExpr($1)->setOprLoc(locA)->setName($3);
    }
|   PostfixExpr '{' '}'
    {
        DECL_2_LOC(@2, @3);
        auto spec = detail::extractSpecFromExpr($1);
        if (!spec) {
            yyerror(&yylloc, scanner, context, "expected record name");
            $$ = makeAstRaw<ErrorExprAst>();
        } else {
            $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)
                    ->setSpec(spec);
        }
    }
|   PostfixExpr '{' InitList '}'
    {
        DECL_2_LOC(@2, @4);
        auto spec = detail::extractSpecFromExpr($1);
        if (!spec) {
            yyerror(&yylloc, scanner, context, "expected record name");
            delete $3->finishSR();
            $$ = makeAstRaw<ErrorExprAst>();
        } else {
            $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)
                    ->setSpec(spec)->setInitsSR($3);
        }
    }
|   PostfixExpr '{' InitList ',' '}'
    {
        DECL_2_LOC(@2, @5);
        auto spec = detail::extractSpecFromExpr($1);
        if (!spec) {
            yyerror(&yylloc, scanner, context, "expected record name");
            delete $3->finishSR();
            $$ = makeAstRaw<ErrorExprAst>();
        } else {
            $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)
                    ->setSpec(spec)->setInitsSR($3);
        }
    }
|   PostfixExpr '[' Expr ']'
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' Expr error PostfixExprSync
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' SubrangeExpr ']'
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' SubrangeExpr error PostfixExprSync
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '.' '(' Type ')'
    {
        DECL_3_LOC(@2, @3, @5);
        $$ = makeAstRaw<TypeAssertExprAst>()->setBase($1)->setOprLoc(locA)->setLDelimLoc(locB)
                ->setSpec($4)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' ')'
    {
        DECL_2_LOC(@2, @3);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList ')'
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList error PostfixExprSync
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList ',' ')'
    {
        DECL_2_LOC(@2, @5);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList ',' error PostfixExprSync
    {
        DECL_2_LOC(@2, @5);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList "..." ')'
    {
        DECL_3_LOC(@2, @4, @5);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)
                ->setPackLoc(locB)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' ExprList "..." ',' ')'
    {
        DECL_3_LOC(@2, @4, @6);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)
                ->setPackLoc(locB)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ')'
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList ')'
    {
        DECL_3_LOC(@2, @4, @6);
        $$ = makeAstRaw<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList error PostfixExprSync
    {
        DECL_3_LOC(@2, @4, @6);
        $$ = makeAstRaw<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList ',' ')'
    {
        DECL_3_LOC(@2, @4, @7);
        $$ = makeAstRaw<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList ',' error PostfixExprSync
    {
        DECL_3_LOC(@2, @4, @7);
        $$ = makeAstRaw<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList "..." ')'
    {
        DECL_4_LOC(@2, @4, @6, @7);
        $$ = makeAstRaw<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setPackLoc(locC)->setRDelimLoc(locD);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList "..." ',' ')'
    {
        DECL_4_LOC(@2, @4, @6, @8);
        $$ = makeAstRaw<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setPackLoc(locC)->setRDelimLoc(locD);
    }
;

PostfixExprSync: ']'| ')';

PrimaryExpr:
    Ident
    {
        $$ = makeAstRaw<IdentExprAst>()->setName($1);
    }
|   NumLit
|   StringLit
|   BoolLit
|   NullLit
|   CharLit
|   FuncLit
|   CompositeLit
|   ConvExpr
|   '(' Expr ')'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<WrappedExprAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locB);
    }
;

ConvExpr:
    CastType '(' Expr ')'
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<CastExprAst>()->setSpec($1)->setLDelimLoc(locA)->setExpr($3)->setRDelimLoc(locB);
    }
|   CastType '(' Expr ',' ')'
    {
        DECL_2_LOC(@2, @5);
        $$ = makeAstRaw<CastExprAst>()->setSpec($1)->setLDelimLoc(locA)->setExpr($3)->setRDelimLoc(locB);
    }
;

SubrangeExpr:
    ':'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<SubrangeExprAst>()->setDelim1Loc(locA);
    }
|   ':' Expr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<SubrangeExprAst>()->setDelim1Loc(locA)->setHi($2);
    }
|   ':' Expr ':' Expr
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<SubrangeExprAst>()->setDelim1Loc(locA)->setHi($2)->setDelim2Loc(locB)
                ->setMax($4);
    }
|   Expr ':'
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA);
    }
|   Expr ':' Expr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setHi($3);
    }
|   Expr ':' Expr ':'
    {
        // TODO: Report error, max must be specified.
        DECL_1_LOC(@2);
        $$ = makeAstRaw<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setHi($3);
    }
|   Expr ':' Expr ':' Expr
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setHi($3)
                ->setDelim2Loc(locB)->setMax($5);
    }
|   Expr ':' ':'
    {
        // TODO: Report error, max and subs2 must be specified.
        DECL_1_LOC(@2);
        $$ = makeAstRaw<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA);
    }
|   Expr ':' ':' Expr
    {
        // TODO: Report error, subs2 must be specified.
        DECL_1_LOC(@2);
        $$ = makeAstRaw<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setMax($4);
    }
;

InitList:
    Init
    {
        $$ = ExprAstList::createSR($1);
    }
|   InitList ',' Init
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

Init:
    Expr
|   Expr ':' Expr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue($3);
    }
|   Expr ':' '{' InitList '}'
    {
        DECL_3_LOC(@2, @3, @5);
        auto init = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locB)->setInitsSR($4)->setRDelimLoc(locC);
        $$ = makeAstRaw<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue(init);
    }
|   Expr ':' '{' InitList ',' '}'
    {
        DECL_3_LOC(@2, @3, @6);
        auto init = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locB)->setInitsSR($4)->setRDelimLoc(locC);
        $$ = makeAstRaw<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue(init);
    }
|   Expr ':' '{' '}'
    {
        DECL_3_LOC(@2, @3, @4);
        auto init = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = makeAstRaw<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue(init);
    }
|   '{' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' InitList '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
|   '{' InitList ',' '}'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
;

EffectExpr:
    Expr
    {
        $$ = $1;
    }
|   Expr "++"
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   Expr "--"
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   ExprList '=' ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::Basic);
        $$ = expr;
    }
|   ExprList "+=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByAdd);
        $$ = expr;
    }
|   ExprList "-=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::BySub);
        $$ = expr;
    }
|   ExprList "|=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByOr);
        $$ = expr;
    }
|   ExprList "^=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByXor);
        $$ = expr;
    }
|   ExprList "*=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByMul);
        $$ = expr;
    }
|   ExprList "/=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByDiv);
        $$ = expr;
    }
|   ExprList "%=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByMod);
        $$ = expr;
    }
|   ExprList "<<=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByShift);
        $$ = expr;
    }
|   ExprList ">>=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByShift);
        $$ = expr;
    }
|   ExprList "&=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByAnd);
        $$ = expr;
    }
|   ExprList "&^=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByAnd);
        $$ = expr;
    }
|   ExprList ":=" ExprList
    {
        // Do not assign a variety, leave it as Unknown, will be processed
        // later and converted into a short declaration.
        $$ = makeAstRaw<AssignExprAst>()->setExpr1sSR($1)->setExpr2sSR($3);
    }
|   Expr "<-" Expr
    {
        DECL_1_LOC(@2);
        auto expr = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
        expr->setVariety(AssignVariety::Basic);
        $$ = expr;
    }
;

ExprList:
    Expr
    {
        $$ = ExprAstList::createSR($1);
    }
|   ExprList ',' Expr
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;


    /*-------------*/
    /*--- Types ---*/
    /*-------------*/

Type:
    PlainType
|   '(' Type ')'
    {
        $$ = $2;
    }
;

PlainType:
    BuiltinType
|   NestedIdent
    {
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($1);
        $$ = makeAstRaw<NamedSpecAst>()->setName(name);
    }
|   FuncType
|   PointerType
|   CompositeType
|   SenderChanType
|   RecvChanType
|   BidirChanType
;

NonExprType:
    BuiltinType
|   FuncType
|   '*' NonExprType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($2);
    }
|   CompositeType
|   SenderChanType
|   RecvChanType
|   BidirChanType
;

NonExprTypeList:
    NonExprType
    {
        $$ = SpecAstList::createSR($1);
    }
|   NonExprTypeList ',' NonExprType
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

CastType:
    BuiltinType
|   FuncType
|   CompositeType
|   SenderChanType
|   BidirChanType
|   '(' CastType ')'
    {
        $$ = $2;
    }
|   '(' '*' CastType ')'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($3);
    }
;

SpecialArgType:
    /* To handle "fake" functions calls such as `make` and `new`, which take
       a type instead of an expression as the first argument. */
    BuiltinType
|   FuncType
|   CompositeType
|   SenderChanType
|   RecvChanType
|   BidirChanType
|   '*' SpecialArgType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($2);
    }
;

CompositeType:
    '[' ']' Type
    {
        DECL_2_LOC(@1, @2);
        auto array = makeAstRaw<ArraySpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)->setBaseSpec($3);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   '[' Expr ']' Type
    {
        DECL_2_LOC(@1, @3);
        auto array = makeAstRaw<ArraySpecAst>()->setLDelimLoc(locA)->setExpr($2)
                ->setRDelimLoc(locB)->setBaseSpec($4);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   '[' "..." ']' Type
    {
        DECL_3_LOC(@1, @2, @3);
        auto array = makeAstRaw<ArraySpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locC)->setBaseSpec($4);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   MAP '[' Type ']' Type
    {
        DECL_3_LOC(@1, @2, @4);
        auto array = makeAstRaw<ArraySpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setSpec($3)
                ->setRDelimLoc(locC)->setBaseSpec($5);
        array->setVariety(ArrayVariety::Associative);
        $$ = array;
    }
|   StructType
|   InterfaceType
;

PointerType:
    '*' Type
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($2);
    }
;

SenderChanType:
    CHAN "<-" Type
    {
        DECL_2_LOC(@1, @2);
        auto chann = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setDirLoc(locB)->setBaseSpec($3);
        chann->setVariety(ChanVariety::Sender);
        $$ = chann;
    }
;

RecvChanType:
    "<-" CHAN Type
    {
        DECL_2_LOC(@1, @2);
        auto chann = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setDirLoc(locB)->setBaseSpec($3);
        chann->setVariety(ChanVariety::Sender);
        $$ = chann;
    }
;

BidirChanType:
    CHAN BuiltinType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN NestedIdent
    {
        DECL_1_LOC(@1);
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($2);
        auto spec = makeAstRaw<NamedSpecAst>()->setName(name);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec(spec);
    }
|   CHAN FuncType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN PointerType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN CompositeType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN SenderChanType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN BidirChanType
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN '(' Type ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($3);
    }
;

StructType:
    STRUCT '{' '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto type = makeAstRaw<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Struct);
        $$ = type;
    }
|   STRUCT '{' FieldDecls '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto type = makeAstRaw<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Struct);
        detail::splitBaseDeclsAndFields(type, $3->finishSR());
        $$ = type;
    }
|   STRUCT '{' FieldDecls ';' '}'
    {
        DECL_3_LOC(@1, @2, @5);
        auto type = makeAstRaw<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Struct);
        detail::splitBaseDeclsAndFields(type, $3->finishSR());
        $$ = type;
    }
;

InterfaceType:
    INTERFACE '{' '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto type = makeAstRaw<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Interface);
        $$ = type;
    }
|   INTERFACE '{' InterfaceMembers '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto type = makeAstRaw<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Interface);
        detail::splitBaseDeclsAndFields(type, $3->finishSR());
        $$ = type;
    }
|   INTERFACE '{' InterfaceMembers ';' '}'
    {
        DECL_3_LOC(@1, @2, @5);
        auto type = makeAstRaw<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Interface);
        detail::splitBaseDeclsAndFields(type, $3->finishSR());
        $$ = type;
    }
;

FuncType:
    FUNC Signature
    {
        DECL_1_LOC(@1);
        $$ = FuncSpec_Cast($2)->setKeyLoc(locA);
    }
;

BuiltinType:
    BOOL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT8
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT16
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT32
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT64
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT8
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT16
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT32
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT64
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   FLOAT32
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   FLOAT64
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   COMPLEX_FLOAT64
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   COMPLEX_REAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   BYTE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   RUNE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
;


    /*--------------------*/
    /*--- Declarations ---*/
    /*--------------------*/

    /* Trailing semi-colons are not kept in the ASTs, they are
       normally optional, except for a case block (where there's
       no closing brace after the statement list). */

Decl:
    ImportClauseDecl
|   FuncDecl
|   FuncRecvDecl
|   VarSectionDecl
|   ConstDecl
|   TypeGroupDecl
;

Decls:
    Decl ';'
    {
        $$ = DeclAstList::createSR($1);
    }
|   error DeclsSync
    {
        $$ = DeclAstList::createSR(makeAstRaw<ErrorDeclAst>());
        yyerrok;
    }
|   Decl error DeclsSync
    {
        $$ = DeclAstList::createSR($1);
        yyerrok;
    }
|   Decls Decl ';'
    {
        $$ = $1->handleSR($2);
    }
|   Decls error DeclsSync
    {
        $$ = $1;
        yyerrok;
    }
|   Decls Decl error DeclsSync
    {
        $$ = $1->handleSR($2);
        yyerrok;
    }
;

DeclsSync: ';' | EOP;

ImportClauseDecl:
    IMPORT ImportModuleDecl
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ImportClauseDeclAst>()->setKeyLoc(locA)->addModule($2);
    }
|   IMPORT '(' ')'
    {
        DECL_3_LOC(@1, @2, @3);
        $$ = makeAstRaw<ImportClauseDeclAst>()->setKeyLoc(locA)
            ->setLDelimLoc(locB)->setRDelimLoc(locC);
    }
|   IMPORT '(' ImportList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<ImportClauseDeclAst>()->setKeyLoc(locA)
            ->setLDelimLoc(locB)->setModulesSR($3)->setRDelimLoc(locC);
    }
|   IMPORT '(' ImportList ';' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        $$ = makeAstRaw<ImportClauseDeclAst>()->setKeyLoc(locA)
            ->setLDelimLoc(locB)->setModulesSR($3)->setRDelimLoc(locC);
    }
;

ImportModuleDecl:
    StringLit
    {
        $$ = makeAstRaw<ImportModuleDeclAst>()->setExpr($1);
    }
|   Ident StringLit
    {
        $$ = makeAstRaw<ImportModuleDeclAst>()->setLocalName($1)->setExpr($2);
    }
|   '.' StringLit
    {
        DECL_1_LOC(@1);
        auto dot = makeAstRaw<GenNameAst>()->setGenLoc(locA);
        context->trackLexeme<Ident>(".", locA.lineCol());
        $$ = makeAstRaw<ImportModuleDeclAst>()->setLocalName(dot)->setExpr($2);
    }
;

ImportList:
    ImportModuleDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   ImportList ';' ImportModuleDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

FieldDecls:
    FieldDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   FieldDecls ';' FieldDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

FieldDecl:
    VarDeclList Type
    {
        $$ = makeAstRaw<VarGroupDeclAst>()->setDeclsSR($1)->setSpec($2);
    }
|   VarDeclList Type StringLit
    {
        $$ = makeAstRaw<VarTagDeclAst>()->setDeclsSR($1)->setSpec($2)->setTag($3);
    }
|   '*' NestedIdent
    {
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($2);
        $$ = makeAstRaw<BaseDeclAst>()->setName(name);
    }
|   '*' NestedIdent StringLit
    {
        IGNORE_FOR_NOW($3);
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($2);
        $$ = makeAstRaw<BaseDeclAst>()->setName(name);
    }
|   NestedIdent
    {
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($1);
        $$ = makeAstRaw<BaseDeclAst>()->setName(name);
    }
|   NestedIdent StringLit
    {
        IGNORE_FOR_NOW($2);
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($1);
        $$ = makeAstRaw<BaseDeclAst>()->setName(name);
    }
;

InterfaceMembers:
    InterfaceMember
    {
        $$ = DeclAstList::createSR($1);
    }
|   InterfaceMembers ';' InterfaceMember
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

InterfaceMember:
    Ident
    {
        /* Embedding the name of another interface */
        $$ = makeAstRaw<BaseDeclAst>()->setName($1);
    }
|   Ident Signature
    {
        $$ = makeAstRaw<FuncDeclAst>()->setName($1)->setSpec($2);
    }
;

FuncRecvDecl:
    FUNC ParamClauseDecl Ident Signature
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($4)->setKeyLoc(locA);
        auto func = makeAstRaw<FuncRecvDeclAst>()->setRecv($2)->setName($3)->setSpec(spec);
        func->setVariety(FuncVariety::Method);
        $$ = func;
    }
|   FUNC ParamClauseDecl Ident Signature BlockStmt
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($4)->setKeyLoc(locA);
        auto func = makeAstRaw<FuncRecvDeclAst>()->setRecv($2)->setName($3)->setSpec(spec)->setStmt($5);
        func->setVariety(FuncVariety::Method);
        $$ = func;
    }
;

FuncDecl:
    FUNC Ident Signature
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($3)->setKeyLoc(locA);
        $$ = makeAstRaw<FuncDeclAst>()->setName($2)->setSpec(spec);
    }
|   FUNC Ident Signature BlockStmt
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($3)->setKeyLoc(locA);
        $$ = makeAstRaw<FuncDeclAst>()->setName($2)->setSpec(spec)->setStmt($4);
    }
;

Signature:
    /* There's an ambiguity here during a conversion when the cast type is a
       function type. It resolves always to the function signature unless the
       cast type is parenthesized. See `Conversions` in Go's language spec. */
    ParamClauseDecl %prec PREFER_SHIFT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<FuncSpecAst>()->setParam($1)->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
    }
|   ParamClauseDecl ParamClauseDecl
    {
        $$ = makeAstRaw<FuncSpecAst>()->setParam($1)->setResult($2);
    }
|   ParamClauseDecl PlainType
    {
        $$ = makeAstRaw<FuncSpecAst>()->setParam($1)->setResult($2);
    }
;

ParamClauseDecl:
    '(' ')'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '(' error ParamClauseDeclSync
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        yyerrok;
    }
|   '(' ParamGroupDeclList ')'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList error ParamClauseDeclSync
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' error ParamClauseDeclSync
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' Ident "..." Type ')'
    {
        DECL_4_LOC(@1, @3, @5, @7);
        auto param = makeAstRaw<ParamDeclAst__<ParamVariadic__>>()->setName($4)->setVariadicLoc(locC);
        auto group = makeAstRaw<ParamGroupDeclAst>()->addDecl(param)->setSpec($6);
        auto list = $2->handleSR(group);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR(list)->setRDelimLoc(locD);
    }
|   '(' ParamGroupDeclList ',' "..." Type ')'
    {
        DECL_4_LOC(@1, @3, @4, @6);
        auto param = makeAstRaw<ParamDeclAst__<ParamVariadic__>>()->setVariadicLoc(locC);
        auto group = makeAstRaw<ParamGroupDeclAst>()->addDecl(param)->setSpec($5);
        auto list = $2->handleSR(group);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR(list)->setRDelimLoc(locD);
    }
|   '(' Ident "..." Type ')'
    {
        DECL_3_LOC(@1, @3, @5);
        auto param = makeAstRaw<ParamDeclAst__<ParamVariadic__>>()->setName($2)->setVariadicLoc(locB);
        auto group = makeAstRaw<ParamGroupDeclAst>()->addDecl(param)->setSpec($4);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->addDecl(group)->setRDelimLoc(locC);
    }
|   '(' "..." Type ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto param = makeAstRaw<ParamDeclAst__<ParamVariadic__>>()->setVariadicLoc(locB);
        auto group = makeAstRaw<ParamGroupDeclAst>()->addDecl(param)->setSpec($3);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->addDecl(group)->setRDelimLoc(locC);
    }
;

ParamClauseDeclSync: ')' | EOP;

ParamGroupDeclList:
    /* In Go, parameters are either all named or all unnamed. Parameter groups
       are therefore "adjusted" (but this doesn't fix entire clause). */
    ParamGroupDecl
    {
        std::unique_ptr<ParamGroupDeclAst> group(ParamGroupDecl_Cast($1));
        if (group->spec_) {
            group.release();
            $$ = DeclAstList::createSR($1);
        } else {
            $$ = detail::adjustParamGroupDecl(group->decls_.get(), nullptr);
        }
    }
|   ParamGroupDeclList ',' ParamGroupDecl
    {
        DECL_1_LOC(@2);
        std::unique_ptr<ParamGroupDeclAst> group(ParamGroupDecl_Cast($3));
        if (group->spec_) {
            group.release();
            $1->delim_ = locA;
            $$ = $1->handleSR($3);
        } else {
            $$ = detail::adjustParamGroupDecl(group->decls_.get(), $1);
        }
    }
;

ParamGroupDecl:
    NonExprType
    {
        $$ = makeAstRaw<ParamGroupDeclAst>()->setSpec($1);
    }
|   '*' NestedIdent
    {
        DECL_1_LOC(@1);
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($2);
        auto namedSpec = makeAstRaw<NamedSpecAst>()->setName(name);
        auto ptrSpec = makeAstRaw<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec(namedSpec);
        $$ = makeAstRaw<ParamGroupDeclAst>()->setSpec(ptrSpec);
    }
|   ParamDeclList %prec PREFER_SHIFT
    {
        $$ = makeAstRaw<ParamGroupDeclAst>()->setDeclsSR($1);
    }
|   ParamDeclList Type
    {
        $$ = makeAstRaw<ParamGroupDeclAst>()->setDeclsSR($1)->setSpec($2);
    }
;

ParamDeclList:
    ParamDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   ParamDeclList ',' ParamDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

ParamDecl:
    Ident
    {
        $$ = makeAstRaw<ParamDeclAst>()->setName($1);
    }
;

VarSectionDecl:
    VAR VarGroupDecl
    {
        DECL_1_LOC(@1);
        $$ = VarGroupDecl_Cast($2)->setKeyLoc(locA);
    }
|   VAR '(' ')'
    {
        DECL_3_LOC(@1, @2, @3);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
|   VAR '(' VarGroupDeclList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
|   VAR '(' VarGroupDeclList ';' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
;

VarGroupDeclList:
    VarGroupDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   VarGroupDeclList ';' VarGroupDecl
    {
        $$ = $1->handleSR($3);
    }
|   VarGroupDeclList ';' Ident
    {
        /* TODO: - Clone spec from the previous group.
                 - Error if not a const decl. */
        auto decls = DeclAstList::createSR(makeAstRaw<VarDeclAst>()->setName($3));
        auto group = makeAstRaw<VarGroupDeclAst>()->setDeclsSR(decls)
            ->setSpec(makeAstRaw<InferredSpecAst>());
        $$ = $1->handleSR(group);
    }
;

VarGroupDecl:
    VarDeclList Type
    {
        $$ = makeAstRaw<VarGroupDeclAst>()->setDeclsSR($1)->setSpec($2);
    }
|   VarDeclList EOP
    {
        $$ = makeAstRaw<VarGroupDeclAst>()->setDeclsSR($1)->setSpec(makeAstRaw<InferredSpecAst>());
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   VarDeclList Type '=' ExprList
    {
        DECL_1_LOC(@3);
        $$ = makeAstRaw<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
            ->setSpec($2)->setAssignLoc(locA)->setInitsSR($4);
    }
|   VarDeclList Type '=' ExprList EOP
    {
        DECL_1_LOC(@3);
        $$ = makeAstRaw<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
            ->setSpec($2)->setAssignLoc(locA)->setInitsSR($4);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   VarDeclList '=' ExprList
    {
        DECL_1_LOC(@2);
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        $$ = makeAstRaw<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
            ->setSpec(inferred)->setAssignLoc(locA)->setInitsSR($3);
    }
|   VarDeclList '=' ExprList EOP
    {
        DECL_1_LOC(@2);
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        $$ = makeAstRaw<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
            ->setSpec(inferred)->setAssignLoc(locA)->setInitsSR($3);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
;

VarDeclList:
    VarDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   VarDeclList ',' VarDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

VarDecl:
    Ident
    {
        $$ = makeAstRaw<VarDeclAst>()->setName($1);
    }
;

ConstDecl:
    CONST VarGroupDecl
    {
        DECL_1_LOC(@1);
        detail::constifyVarGroupDecl($2, locA);
        $$ = VarGroupDecl_Cast($2)->setKeyLoc(locA);
    }
|   CONST '(' ')'
    {
        DECL_3_LOC(@1, @2, @3);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
|   CONST '(' VarGroupDeclList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto decls = $3->finishSR();
        for (auto decl : *decls)
            detail::constifyVarGroupDecl(decl, locA);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->decls_.reset(decls);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
|   CONST '(' VarGroupDeclList ';' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        auto decls = $3->finishSR();
        for (auto decl : *decls)
            detail::constifyVarGroupDecl(decl, locA);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->decls_.reset(decls);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
;

TypeGroupDecl:
    TYPE RecordDecl
    {
        DECL_1_LOC(@1);
        if ($2->kind() == Ast::Kind::RecordDecl) {
            $$ = RecordDecl_Cast($2)->setKeyLoc(locA);
        } else {
            $$ = AliasDecl_Cast($2)->setKeyLoc(locA);
        }
    }
|   TYPE '(' ')'
    {
        DECL_3_LOC(@1, @2, @3);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->setVariety(SectionVariety::Types);
        $$ = sect;
    }
|   TYPE '(' RecordDeclList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
        sect->setVariety(SectionVariety::Types);
        $$ = sect;
    }
|   TYPE '(' RecordDeclList ';' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
        sect->setVariety(SectionVariety::Types);
        $$ = sect;
    }
;

RecordDeclList:
    RecordDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   RecordDeclList ';' RecordDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

RecordDecl:
    Ident Type
    {
        if ($2->kind() == Ast::Kind::RecordSpec)
            $$ = makeAstRaw<RecordDeclAst>()->setName($1)->setSpec($2);
        else
            $$ = makeAstRaw<AliasDeclAst>()->setName($1)->setSpec($2);
    }
;


    /*------------------*/
    /*--- Statements ---*/
    /*------------------*/

Stmt:
    Decl
    {
        $$ = makeAstRaw<DeclStmtAst>()->setDecl($1);
    }
|   EffectExpr
    {
        $$ = detail::ExprOrShortVarDecl().inspect($1);
    }
|   EffectExpr EOP
    {
        $$ = detail::ExprOrShortVarDecl().inspect($1);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   LabeledStmt
|   GoStmt
|   ReturnStmt
|   BreakStmt
|   ContinueStmt
|   GotoStmt
|   FallthroughStmt
|   BlockStmt
|   IfStmt
|   SwitchStmt
|   SelectStmt
|   ForStmt
|   DeferStmt
;

BlockStmt:
    '{' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' error BlockStmtSync
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        yyerrok;
    }
|   '{' StmtList '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
    }
|   '{' StmtList error BlockStmtSync
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        yyerrok;
    }
|   '{' StmtList ';' '}'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
    }
|   '{' StmtList ';' error BlockStmtSync
    {
        DECL_2_LOC(@1, @5);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        yyerrok;
    }
;

BlockStmtSync: '}' | EOP;

StmtList:
    Stmt
    {
        $$ = StmtAstList::createSR($1);
    }
|   StmtList ';' Stmt
    {
        $$ = $1->handleSR($3);
    }
;

LabeledStmt:
    Ident ':' Stmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<LabeledStmtAst>()->setLabel($1)->setDelimLoc(locA)->setStmt($3);
    }
;

GoStmt:
    GO Expr
    {
        // TODO: Report error if not callable expr.
        DECL_1_LOC(@1);
        $$ = makeAstRaw<AsyncStmtAst>()->setKeyLoc(locA)->setExpr($2);
    }
;

GotoStmt:
    GOTO Ident
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<GotoStmtAst>()->setKeyLoc(locA)->setName($2);
    }
;

BreakStmt:
    BREAK
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BreakStmtAst>()->setKeyLoc(locA);
    }
|   BREAK Ident
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BreakStmtAst>()->setKeyLoc(locA)->setName($2);
    }
;

ReturnStmt:
    RETURN
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ReturnStmtAst>()->setKeyLoc(locA);
    }
|   RETURN ExprList
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ReturnStmtAst>()->setKeyLoc(locA)->setExprsSR($2);
    }
;

ContinueStmt:
    CONTINUE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ContinueStmtAst>()->setKeyLoc(locA);
    }
|   CONTINUE Ident
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ContinueStmtAst>()->setKeyLoc(locA)->setName($2);
    }
;

FallthroughStmt:
    FALLTHROUGH
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<FallthroughStmtAst>()->setKeyLoc(locA);
    }
;

IfStmt:
    IF Expr BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setExpr($2)->setThen($3);
    }
|   IF Expr BlockStmt ELSE IfStmt
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setExpr($2)->setThen($3)
                ->setElseLoc(locB)->setNotThen($5);
    }
|   IF Expr BlockStmt ELSE BlockStmt
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setExpr($2)->setThen($3)
                ->setElseLoc(locB)->setNotThen($5);
    }
|   IF EffectExpr ';' Expr BlockStmt
    {
        DECL_2_LOC(@1, @3);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setPreamble(stmt)->setExpr($4)
                ->setThen($5);
    }
|   IF EffectExpr ';' Expr BlockStmt ELSE IfStmt
    {
        DECL_3_LOC(@1, @3, @6);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setPreamble(stmt)->setExpr($4)
                ->setThen($5)->setElseLoc(locC)->setNotThen($7);
    }
|   IF EffectExpr ';' Expr BlockStmt ELSE BlockStmt
    {
        DECL_3_LOC(@1, @3, @6);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setPreamble(stmt)->setExpr($4)
                ->setThen($5)->setElseLoc(locC)->setNotThen($7);
    }
;

SwitchStmt:
    SWITCH '{' '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
|   SWITCH '{' CaseClauseStmts '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($3)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
|   SWITCH Expr '{' '}'
    {
        DECL_3_LOC(@1, @3, @4);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH Expr '{' CaseClauseStmts '}'
    {
        DECL_3_LOC(@1, @3, @5);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($4)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH Ident ":=" PostfixExpr '.' '(' TYPE ')' '{' '}'
    {
        IGNORE_FOR_NOW($2);
        DECL_6_LOC(@1, @3, @6, @8, @9, @10);
        auto spec = makeAstRaw<TypeofSpecAst>()->setOprLoc(locC)->setLDelimLoc(locB)->setExpr($4)
                ->setRDelimLoc(locD);
        $$ = makeAstRaw<TypeSwitchStmtAst>()->setKeyLoc(locA)->setSpec(spec);
    }
|   SWITCH Ident ":=" PostfixExpr '.' '(' TYPE ')' '{' CaseClauseStmts '}'
    {
        IGNORE_FOR_NOW($2);
        DECL_6_LOC(@1, @3, @6, @8, @9, @11);
        auto spec = makeAstRaw<TypeofSpecAst>()->setOprLoc(locC)->setLDelimLoc(locB)->setExpr($4)
                ->setRDelimLoc(locD);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($10)->setRDelimLoc(locC);
        $$ = makeAstRaw<TypeSwitchStmtAst>()->setKeyLoc(locA)->setSpec(spec)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' '{' '}'
    {
        DECL_3_LOC(@1, @4, @5);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' '{' CaseClauseStmts '}'
    {
        DECL_3_LOC(@1, @4, @6);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($5)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' Expr '{' '}'
    {
        DECL_4_LOC(@1, @3, @5, @6);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        auto preamble = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setPreamble(preamble)
                ->setExpr($4)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' Expr '{' CaseClauseStmts '}'
    {
        DECL_4_LOC(@1, @3, @5, @7);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($6)->setRDelimLoc(locC);
        auto preamble = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setPreamble(preamble)
                ->setExpr($4)->setStmt(stmt);
    }
;

SelectStmt:
    SELECT '{' '}'
    {
        // TODO: Different AST from switch statement or a variety?
        DECL_3_LOC(@1, @2, @3);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
|   SELECT '{' CaseClauseStmts '}'
    {
        // TODO: Different AST from switch statement or a variety?
        DECL_3_LOC(@1, @2, @3);
        auto stmt = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($3)->setRDelimLoc(locC);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
;

CaseClauseStmts:
    CaseClauseStmt
    {
        $$ = StmtAstList::createSR($1);
    }
|   CaseClauseStmts CaseClauseStmt
    {
        $$ = $1->handleSR($2);
    }
;

CaseClauseStmt:
    CASE ExprList ':'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<CaseClauseStmtAst>()->setKeyLoc(locA)->setExprsSR($2)->setDelimLoc(locB);
    }
|   CASE ExprList ':' StmtList ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<CaseClauseStmtAst>()->setKeyLoc(locA)->setExprsSR($2)->setDelimLoc(locB)
                ->setStmtsSR($4);
    }
|   CASE ExprList '=' Expr ':'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4);

        // TODO: This applies only to select statements.
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   CASE ExprList '=' Expr ':' StmtList ';'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        // TODO: This applies only to select statements.
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   CASE ExprList ":=" Expr ':'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4);

        // TODO: This applies only to select statements.
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   CASE ExprList ":=" Expr ':' StmtList ';'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        // TODO: This applies only to select statements.
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   CASE Expr "<-" Expr ':'
    {
        IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($4);

        // TODO: This applies only to select statements.
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   CASE Expr "<-" Expr ':' StmtList ';'
    {
        IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        // TODO: This applies only to select statements.
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   CASE NonExprTypeList ':'
    {
        IGNORE_LIST_FOR_NOW($2);

        // TODO: Clause of a type switch
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   CASE NonExprTypeList ':' StmtList ';'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_LIST_FOR_NOW($4);

        // TODO: Clause of a type switch
        $$ = makeAstRaw<CaseClauseStmtAst>();
    }
|   DEFAULT ':'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<DefaultClauseStmtAst>()->setKeyLoc(locA)->setDelimLoc(locB);
    }
|   DEFAULT ':' StmtList ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<DefaultClauseStmtAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setStmtsSR($3);
    }
;

ForStmt:
    FOR BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
|   FOR EffectExpr BlockStmt
    {
        DECL_1_LOC(@1);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setStmt($3);
    }
|   FOR EffectExpr ';' ';' BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setStmt($5);
    }
|   FOR EffectExpr ';' EffectExpr ';' BlockStmt
    {
        DECL_3_LOC(@1, @3, @5);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($4)
                ->setDelimLoc(locC)->setStmt($6);
    }
|   FOR EffectExpr ';' EffectExpr ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @3, @5);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($4)
                ->setDelimLoc(locC)->setPost($6)->setStmt($7);
    }
|   FOR EffectExpr ';' ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setPost($5)->setStmt($6);
    }
|   FOR ';' EffectExpr ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($3)
                ->setDelimLoc(locC)->setPost($5)->setStmt($6);
    }
|   FOR ';' ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setPost($4)->setStmt($5);
    }
|   FOR ';' EffectExpr ';' BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($3)
                ->setDelimLoc(locC)->setStmt($5);
    }
|   FOR ';' ';' BlockStmt
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setStmt($4);
    }
|   FOR ExprList '=' RANGE Expr BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto group = detail::turnExprsIntoVarGroupDecl($2->finishSR());
        auto unpack = makeAstRaw<UnpackExprAst>()->setKeyLoc(locC)->setExpr($5);
        $$ = makeAstRaw<ForeachStmtAst>()->setDecl(group)->setExpr(unpack)->setStmt($6);
    }
|   FOR ExprList ":=" RANGE Expr BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto group = detail::turnExprsIntoVarGroupDecl($2->finishSR());
        auto unpack = makeAstRaw<UnpackExprAst>()->setKeyLoc(locC)->setExpr($5);
        $$ = makeAstRaw<ForeachStmtAst>()->setDecl(group)->setExpr(unpack)->setStmt($6);
    }
|   FOR RANGE Expr BlockStmt
    {
        DECL_2_LOC(@1, @2);
        auto unpack = makeAstRaw<UnpackExprAst>()->setKeyLoc(locB)->setExpr($3);
        $$ = makeAstRaw<ForeachStmtAst>()->setExpr(unpack)->setStmt($4);
    }
;

DeferStmt:
    DEFER Expr
    {
         // TODO: Report error if not callable expr.
         DECL_1_LOC(@1);
         auto stmt = makeAstRaw<ExprStmtAst>()->addExpr($2);
         $$ = makeAstRaw<DeferredStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
;


    /*-------------*/
    /*--- Names ---*/
    /*-------------*/

Ident:
    IDENTIFIER
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
    }
|   COMPLETION
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<CompletionNameAst>()->setNameLoc(locA);
    }
;

NestedIdent:
    /* There's an ambiguity during a conversion when the cast type is a function
       type - this part is necessary because of method delcarations. More details
       in non-terminal `Signature`. */
    Ident %prec PREFER_SHIFT
    {
        $$ = NameAstList::createSR($1);
    }
|   Ident '.' Ident
    {
        DECL_1_LOC(@2);
        auto names = NameAstList::createSR($1);
        names->delim_ = locA;
        $$ = names->handleSR($3);
    }
;


    /*----------------*/
    /*--- Literals ---*/
    /*----------------*/

CharLit:
    CHAR_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<CharLitExprAst>()->setLitLoc(locA);
    }
;

StringLit:
    STRING_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
;

NumLit:
    INTEGER_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NumLitExprAst>()->setLitLoc(locA);
    }
|   FLOAT_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NumLitExprAst>()->setLitLoc(locA);
    }
;

BoolLit:
    TRUE_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BoolLitExprAst>()->setLitLoc(locA);
    }
|   FALSE_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BoolLitExprAst>()->setLitLoc(locA);
    }
;

NullLit:
    NULL_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NullLitExprAst>()->setLitLoc(locA);
    }
;

FuncLit:
    FUNC Signature BlockStmt
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($2)->setKeyLoc(locA);
        $$ = makeAstRaw<FuncLitExprAst>()->setSpec(spec)->setStmt($3);
    }
;

CompositeLit:
    CompositeType '{' '}'
    {
        DECL_2_LOC(@2, @3);
        auto init = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        $$ = makeAstRaw<RecordLitExprAst>()->setSpec($1)->setInit(init);
    }
|   CompositeType '{' InitList '}'
    {
        DECL_2_LOC(@2, @4);
        auto init = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($3)->setRDelimLoc(locB);
        $$ = makeAstRaw<RecordLitExprAst>()->setSpec($1)->setInit(init);
    }
|   CompositeType '{' InitList ',' '}'
    {
        DECL_2_LOC(@2, @5);
        auto init = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($3)->setRDelimLoc(locB);
        $$ = makeAstRaw<RecordLitExprAst>()->setSpec($1)->setInit(init);
    }
;
%%
