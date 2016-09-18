/******************************************************************************
 * Copyright (c) 2014-2016 Leandro T. C. Melo (ltcmelo@gmail.com)
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
%type <decl_> ImportGroupDecl ImportDecl
%type <decls_> FieldDecls InterfaceMembers VarGroupDeclList RecordDeclList
%type <decls_> ParamGroupDeclList ParamDeclList VarDeclList Decls ImportList

%type <expr_> Expr UnaryExpr PostfixExpr PriExpr SubrangeExpr ConvExpr Init
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
%token ABSTRACT 357 "abstract"
%token ALIAS 358 "alias"
%token ALIGN 359 "align"
%token ALIGNAS 360 "alignas"
%token ALIGNOF 361 "aligonf"
%token AMPER_AMPER 306 "&&"
%token AMPER_CARET 307 "&^"
%token AMPER_CARET_EQ 308 "&^="
%token AMPER_EQ 309 "&="
%token AND 362 "and"
%token ARROW_DASH 310 "<-"
%token AS 363 "as"
%token ASM 364 "asm"
%token ASSERT 365 "assert"
%token AUTO 366 "auto"
%token BEGIN_BUILTIN_TYPES 502 "$builtin_types_begin_marker$"
%token BEGIN_CHAR_LIT 286 "$char_lit_begin_marker$"
%token BEGIN_COMMENT 274 "$comments_begin_marker$"
%token BEGIN_KEYWORD 356 "$keyword_begin_marker$"
%token BEGIN_LIT 281 "$lit_begin_marker$"
%token BEGIN_MULTICHAR_OPRTR 305 "$multichar_oprtr_begin_marker$"
%token BEGIN_NUM_LIT 282 "$num_lit_begin_marker$"
%token BEGIN_STR_LIT 291 "$str_lit_begin_marker$"
%token BODY 367 "body"
%token BOOL 503 "bool"
%token BREAK 368 "break"
%token BYTE 504 "byte"
%token CARET_CARET 313 "^^"
%token CARET_CARET_EQ 314 "^^="
%token CARET_EQ 315 "^="
%token CASE 369 "case"
%token CAST 371 "cast"
%token CATCH 372 "catch"
%token CENT 505 "cent"
%token CHAN 370 "chan"
%token CHAR 509 "char"
%token CHAR_LIT 287 "<char_lit>"
%token CHAR_UTF16 510 "char_utf16"
%token CHAR_UTF16_LIT 288 "<char_utf16_lit>"
%token CHAR_UTF32 511 "char_utf32"
%token CHAR_UTF32_LIT 289 "<char_utf32_lit>"
%token CLASS 373 "class"
%token COLON_COLON 316 "::"
%token COLON_EQ 317 ":="
%token COMMENT 275 "<comment>"
%token COMPLETION 264 "<completion>"
%token COMPLEX_FLOAT32 507 "complex_float32"
%token COMPLEX_FLOAT64 506 "complex_float64"
%token COMPLEX_REAL 508 "complex_real"
%token CONST 374 "const"
%token CONSTEXPR 376 "constexpr"
%token CONST_CAST 375 "const_cast"
%token CONTINUE 377 "continue"
%token DASH_ARROW 311 "->"
%token DASH_ARROW_STAR 312 "->*"
%token DATA 378 "data"
%token DEBUG 379 "debug"
%token DECLTYPE 380 "decltype"
%token DEDENT 262 "<dedent>"
%token DEF 381 "def"
%token DEFAULT 382 "default"
%token DEFER 383 "defer"
%token DELEGATE 384 "delegate"
%token DELETE 385 "delete"
%token DEPRECATED 386 "deprecated"
%token DERIVING 387 "deriving"
%token DISABLE 388 "disable"
%token DO 389 "do"
%token DOT_DOT 321 ".."
%token DOT_DOT_DOT 322 "..."
%token DOT_STAR 323 ".*"
%token DOXY_COMMENT 276 "<doxy_comment>"
%token DYNAMIC_CAST 390 "dynamic_cast"
%token ELIF 391 "elif"
%token ELSE 392 "else"
%token END_ASCII 259 "$ascii_end_marker$"
%token END_BUILTIN_TYPES 532 "$builtin_types_end_marker$"
%token END_CHAR_LIT 290 "$char_lit_end_marker$"
%token END_COMMENT 280 "$comments_end_marker$"
%token END_KEYWORD 501 "$keyword_end_marker$"
%token END_LIT 304 "$lit_end_marker$"
%token END_MULTICHAR_OPRTR 355 "$multichar_oprtr_end_marker$"
%token END_NUM_LIT 285 "$num_lit_end_marker$"
%token END_STR_LIT 300 "$str_lit_end_marker$"
%token ENUM 393 "enum"
%token EOP 258 "<end_of_program>"
%token EQ_ARROW 325 "=>"
%token EQ_EQ 324 "=="
%token EXCEPT 394 "except"
%token EXCLAM_EQ 326 "!="
%token EXCLAM_GR 327 "!>"
%token EXCLAM_GR_EQ 328 "!>="
%token EXCLAM_LS 329 "!<"
%token EXCLAM_LS_EQ 330 "!<="
%token EXCLAM_LS_GR 331 "!<>"
%token EXCLAM_LS_GR_EQ 332 "!<>="
%token EXEC 395 "exec"
%token EXPLICIT 396 "explicit"
%token EXPORT 397 "export"
%token EXTERN 398 "extern"
%token FALLTHROUGH 399 "fallthrough"
%token FALSE_VALUE 302 "<false_value>"
%token FINAL 400 "final"
%token FINALLY 401 "finally"
%token FLOAT32 513 "float32"
%token FLOAT64 512 "float64"
%token FLOAT_LIT 284 "<float_lit>"
%token FOR 402 "for"
%token FOREACH 403 "foreach"
%token FOREACH_REVERSE 404 "foreach_reverse"
%token FOREIGN 405 "foreign"
%token FRIEND 406 "friend"
%token FROM 407 "from"
%token FUNC 408 "func"
%token FUNCTION 409 "function"
%token GLOBAL 410 "global"
%token GO 411 "go"
%token GOTO 412 "goto"
%token GR_EQ 333 ">="
%token GR_GR 334 ">>"
%token GR_GR_EQ 337 ">>="
%token GR_GR_GR 335 ">>>"
%token GR_GR_GR_EQ 336 ">>>="
%token IDENT 266 "<ident>"
%token IDENT_QUAL 267 "<ident_qual>"
%token IF 413 "if"
%token IMAG_FLOAT32 515 "imaginary_float32"
%token IMAG_FLOAT64 514 "imaginary_float64"
%token IMAG_REAL 516 "imaginary_real"
%token IMMUTABLE 414 "immutable"
%token IMPORT 415 "import"
%token IN 416 "in"
%token INDENT 261 "<indent>"
%token INFIX 418 "infix"
%token INFIXL 419 "infixl"
%token INFIXR 420 "infixr"
%token INLINE 421 "inline"
%token INOUT 422 "inout"
%token INSTANCE 423 "instance"
%token INT 517 "int"
%token INT16 519 "int16"
%token INT32 520 "int32"
%token INT64 521 "int64"
%token INT8 518 "int8"
%token INTERFACE 424 "interface"
%token INT_LIT 283 "<int_lit>"
%token INVALID 260 "<invalid>"
%token INVARIANT 425 "invariant"
%token IN_LBRACE_HACK 417 "in_{_hack"
%token IS 426 "is"
%token JOKER 265 "<joker>"
%token LAMBDA 427 "lambda"
%token LAZY 428 "lazy"
%token LET 429 "let"
%token LS_EQ 338 "<="
%token LS_GR 341 "<>"
%token LS_GR_EQ 342 "<>="
%token LS_LS 339 "<<"
%token LS_LS_EQ 340 "<<="
%token MACRO 430 "macro"
%token MAP 431 "map"
%token MINUS_EQ 343 "-="
%token MINUS_MINUS 344 "--"
%token MIXIN 432 "mixin"
%token MODULE 433 "module"
%token MULTILINE_COMMENT 277 "<multiline_comment>"
%token MULTILINE_DOXY_COMMENT 278 "<multiline_doxy_comment>"
%token MUTABLE 434 "mutable"
%token NAMESPACE 435 "namespace"
%token NESTING_COMMENT 279 "<nesting_comment>"
%token NEW 436 "new"
%token NEWLINE 263 "<newline>"
%token NEWTYPE 437 "newtype"
%token NOEXCEPT 438 "noexcept"
%token NOGC 439 "nogc"
%token NONLOCAL 440 "nonlocal"
%token NOT 441 "not"
%token NOTHROW 442 "nothrow"
%token NOT_IN_HACK 443 "!_in_hack"
%token NOT_IS_HACK 444 "!_is_hack"
%token NULL_VALUE 301 "<null_value>"
%token OF 446 "of"
%token OPRTR 445 "OPRTR"
%token OR 447 "or"
%token OUT 448 "out"
%token OVERRIDE 449 "override"
%token PACKAGE 450 "package"
%token PASS 451 "pass"
%token PERCENT_EQ 345 "%="
%token PIPE_EQ 346 "|="
%token PIPE_PIPE 347 "||"
%token PLUS_EQ 348 "+="
%token PLUS_PLUS 349 "++"
%token POUND_POUND 350 "##"
%token PRAGMA 452 "pragma"
%token PRINT 453 "print"
%token PRIVATE 454 "private"
%token PROPERTY 455 "property"
%token PROPER_IDENT 268 "<proper_ident>"
%token PROPER_IDENT_QUAL 269 "<proper_ident_qual>"
%token PROTECTED 456 "protected"
%token PUBLIC 457 "public"
%token PUNC_IDENT 270 "<punc_ident>"
%token PUNC_IDENT_QUAL 271 "<punc_ident_qual>"
%token PURE 458 "pure"
%token RAISE 459 "raise"
%token RANGE 460 "range"
%token RAW_STR_LIT 296 "<raw_str_lit>"
%token RAW_UTF16_STR_LIT 298 "<raw_utf16_str_lit>"
%token RAW_UTF32_STR_LIT 299 "<raw_utf32_str_lit>"
%token RAW_UTF8_STR_LIT 297 "<raw_utf8_str_lit>"
%token REAL 522 "real"
%token REF 461 "ref"
%token REGISTER 462 "register"
%token REINTERPRET_CAST 463 "reinterpret_cast"
%token RETURN 464 "return"
%token RUNE 523 "rune"
%token SAFE 465 "safe"
%token SCOPE 466 "scope"
%token SELECT 467 "select"
%token SHARED 468 "shared"
%token SIZEOF 469 "sizeof"
%token SLASH_EQ 318 "/="
%token SLASH_SLASH 319 "//"
%token SLASH_SLASH_EQ 320 "//="
%token SPECIAL_IDENT 272 "<special_ident>"
%token SPECIAL_IDENT_QUAL 273 "<special_ident_qual>"
%token STAR_EQ 351 "*="
%token STAR_STAR 352 "**"
%token STAR_STAR_EQ 353 "**="
%token STATIC 470 "static"
%token STATIC_ASSERT 471 "static_assert"
%token STATIC_CAST 472 "static_cast"
%token STRUCT 473 "struct"
%token STR_LIT 292 "<str_lit>"
%token STR_UTF16_LIT 294 "<str_utf16_lit>"
%token STR_UTF32_LIT 295 "<str_utf32_lit>"
%token STR_UTF8_LIT 293 "<str_utf8_lit>"
%token SUPER 474 "super"
%token SWITCH 475 "switch"
%token SYNCHRONIZED 476 "synchronized"
%token SYSTEM 477 "system"
%token TEMPLATE 478 "template"
%token THEN 479 "then"
%token THIS 480 "this"
%token THREAD_LOCAL 481 "thread_local"
%token THROW 482 "throw"
%token TILDE_EQ 354 "~="
%token TRUE_VALUE 303 "<true_value>"
%token TRUSTED 483 "trusted"
%token TRY 484 "try"
%token TYPE 485 "type"
%token TYPEDEF 486 "typedef"
%token TYPEID 487 "typeid"
%token TYPENAME 488 "typename"
%token TYPEOF 489 "typeof"
%token UBYTE 524 "ubyte"
%token UCENT 525 "ucent"
%token UINT 526 "uint"
%token UINT16 528 "uint16"
%token UINT32 529 "uint32"
%token UINT64 530 "uint64"
%token UINT8 527 "uint8"
%token UNION 490 "union"
%token UNITTEST 491 "unittest"
%token USING 492 "using"
%token VAR 493 "var"
%token VERSION 494 "version"
%token VIRTUAL 495 "virtual"
%token VOID 531 "void"
%token VOLATILE 496 "volatile"
%token WHERE 498 "where"
%token WHILE 499 "while"
%token WITH 500 "with"
%token YIELD 497 "yield"
%token __ATTRIBUTE__ 533 "__attribute__"
%token __DATE__MACRO 534 "__date__"
%token __EOF__MACRO 535 "__eof__"
%token __FILE__MACRO 536 "__file__"
%token __FUNCTION__MACRO 537 "__function__"
%token __GSHARED 538 "__gshared"
%token __LINE__MACRO 539 "__line__"
%token __MODULE__MACRO 540 "__module__"
%token __PARAMETERS 541 "__parameters"
%token __PRETTY_FUNCTION__MACRO 542 "__pretty_function__"
%token __THREAD 543 "__thread"
%token __TIMESTAMP__MACRO 545 "__timestamp__"
%token __TIME__MACRO 544 "__time__"
%token __TRAITS 546 "__traits"
%token __VECTOR 547 "__vector"
%token __VENDOR__MACRO 548 "__vendor__"
%token __VERSION__MACRO 549 "__version__"
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
        $$ = newAst<LogicOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "&&" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<LogicAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "==" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "!=" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '<' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "<=" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '>' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr ">=" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '+' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AddExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '-' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<SubExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '|' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<BitOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '^' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<BitXorExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '*' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<MulExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '/' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<DivExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '%' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ModExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "<<" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr ">>" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr '&' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<BitAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   Expr "&^" Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<BitAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
;

UnaryExpr:
    '&' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<AddrOfExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '*' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<PtrDerefExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '-' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<MinusExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '+' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<PlusExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '!' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<LogicNotExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '~' UnaryExpr
    {
        // TODO: Report error, complement is with ^
        DECL_1_LOC(@1);
        $$ = newAst<BitCompExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   '^' UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<BitCompExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   "<-" UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<ChanExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   PostfixExpr
;

PostfixExpr:
    PriExpr
|   PostfixExpr '.' Ident
    {
        DECL_1_LOC(@2);
        $$ = newAst<MemberAccessExprAst>()->setExpr($1)->setOprLoc(locA)->setName($3);
    }
|   PostfixExpr '{' '}'
    {
        DECL_2_LOC(@2, @3);
        auto spec = detail::extractSpecFromExpr($1);
        if (!spec) {
            yyerror(&yylloc, scanner, context, "expected record name");
            $$ = newAst<ErrorExprAst>();
        } else {
            $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)
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
            $$ = newAst<ErrorExprAst>();
        } else {
            $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)
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
            $$ = newAst<ErrorExprAst>();
        } else {
            $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)
                    ->setSpec(spec)->setInitsSR($3);
        }
    }
|   PostfixExpr '[' Expr ']'
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' Expr error PostfixExprSync
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' SubrangeExpr ']'
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' SubrangeExpr error PostfixExprSync
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '.' '(' Type ')'
    {
        DECL_3_LOC(@2, @3, @5);
        $$ = newAst<TypeAssertExprAst>()->setBase($1)->setOprLoc(locA)->setLDelimLoc(locB)
                ->setSpec($4)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' ')'
    {
        DECL_2_LOC(@2, @3);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList ')'
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList error PostfixExprSync
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList ',' ')'
    {
        DECL_2_LOC(@2, @5);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList ',' error PostfixExprSync
    {
        DECL_2_LOC(@2, @5);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' ExprList "..." ')'
    {
        DECL_3_LOC(@2, @4, @5);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)
                ->setPackLoc(locB)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' ExprList "..." ',' ')'
    {
        DECL_3_LOC(@2, @4, @6);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)
                ->setPackLoc(locB)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ')'
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList ')'
    {
        DECL_3_LOC(@2, @4, @6);
        $$ = newAst<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList error PostfixExprSync
    {
        DECL_3_LOC(@2, @4, @6);
        $$ = newAst<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList ',' ')'
    {
        DECL_3_LOC(@2, @4, @7);
        $$ = newAst<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList ',' error PostfixExprSync
    {
        DECL_3_LOC(@2, @4, @7);
        $$ = newAst<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setRDelimLoc(locC);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList "..." ')'
    {
        DECL_4_LOC(@2, @4, @6, @7);
        $$ = newAst<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setPackLoc(locC)->setRDelimLoc(locD);
    }
|   PostfixExpr '(' SpecialArgType ',' ExprList "..." ',' ')'
    {
        DECL_4_LOC(@2, @4, @6, @8);
        $$ = newAst<MakeExprAst>()->setBase($1)->setLDelimLoc(locA)->setSpec($3)->setSplitLoc(locB)
                ->setArgsSR($5)->setPackLoc(locC)->setRDelimLoc(locD);
    }
;

PostfixExprSync: ']'| ')';

PriExpr:
    Ident
    {
        $$ = newAst<IdentExprAst>()->setName($1);
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
        $$ = newAst<WrappedExprAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locB);
    }
;

ConvExpr:
    CastType '(' Expr ')'
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<CastExprAst>()->setSpec($1)->setLDelimLoc(locA)->setExpr($3)->setRDelimLoc(locB);
    }
|   CastType '(' Expr ',' ')'
    {
        DECL_2_LOC(@2, @5);
        $$ = newAst<CastExprAst>()->setSpec($1)->setLDelimLoc(locA)->setExpr($3)->setRDelimLoc(locB);
    }
;

SubrangeExpr:
    ':'
    {
        DECL_1_LOC(@1);
        $$ = newAst<SubrangeExprAst>()->setDelim1Loc(locA);
    }
|   ':' Expr
    {
        DECL_1_LOC(@1);
        $$ = newAst<SubrangeExprAst>()->setDelim1Loc(locA)->setHi($2);
    }
|   ':' Expr ':' Expr
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<SubrangeExprAst>()->setDelim1Loc(locA)->setHi($2)->setDelim2Loc(locB)
                ->setMax($4);
    }
|   Expr ':'
    {
        DECL_1_LOC(@2);
        $$ = newAst<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA);
    }
|   Expr ':' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setHi($3);
    }
|   Expr ':' Expr ':'
    {
        // TODO: Report error, max must be specified.
        DECL_1_LOC(@2);
        $$ = newAst<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setHi($3);
    }
|   Expr ':' Expr ':' Expr
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setHi($3)
                ->setDelim2Loc(locB)->setMax($5);
    }
|   Expr ':' ':'
    {
        // TODO: Report error, max and subs2 must be specified.
        DECL_1_LOC(@2);
        $$ = newAst<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA);
    }
|   Expr ':' ':' Expr
    {
        // TODO: Report error, subs2 must be specified.
        DECL_1_LOC(@2);
        $$ = newAst<SubrangeExprAst>()->setLow($1)->setDelim1Loc(locA)->setMax($4);
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
        $$ = newAst<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue($3);
    }
|   Expr ':' '{' InitList '}'
    {
        DECL_3_LOC(@2, @3, @5);
        auto init = newAst<RecordInitExprAst>()->setLDelimLoc(locB)->setInitsSR($4)->setRDelimLoc(locC);
        $$ = newAst<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue(init);
    }
|   Expr ':' '{' InitList ',' '}'
    {
        DECL_3_LOC(@2, @3, @6);
        auto init = newAst<RecordInitExprAst>()->setLDelimLoc(locB)->setInitsSR($4)->setRDelimLoc(locC);
        $$ = newAst<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue(init);
    }
|   Expr ':' '{' '}'
    {
        DECL_3_LOC(@2, @3, @4);
        auto init = newAst<RecordInitExprAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = newAst<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue(init);
    }
|   '{' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' InitList '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
|   '{' InitList ',' '}'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
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
        $$ = newAst<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   Expr "--"
    {
        DECL_1_LOC(@2);
        $$ = newAst<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   ExprList '=' ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::Basic);
        $$ = expr;
    }
|   ExprList "+=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByAdd);
        $$ = expr;
    }
|   ExprList "-=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::BySub);
        $$ = expr;
    }
|   ExprList "|=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByOr);
        $$ = expr;
    }
|   ExprList "^=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByXor);
        $$ = expr;
    }
|   ExprList "*=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByMul);
        $$ = expr;
    }
|   ExprList "/=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByDiv);
        $$ = expr;
    }
|   ExprList "%=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByMod);
        $$ = expr;
    }
|   ExprList "<<=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByShift);
        $$ = expr;
    }
|   ExprList ">>=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByShift);
        $$ = expr;
    }
|   ExprList "&=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByAnd);
        $$ = expr;
    }
|   ExprList "&^=" ExprList
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->setExpr1sSR($1)->setOprLoc(locA)->setExpr2sSR($3);
        expr->setVariety(AssignVariety::ByAnd);
        $$ = expr;
    }
|   ExprList ":=" ExprList
    {
        // Do not assign a variety, leave it as Unknown, will be processed
        // later and converted into a short declaration.
        $$ = newAst<AssignExprAst>()->setExpr1sSR($1)->setExpr2sSR($3);
    }
|   Expr "<-" Expr
    {
        DECL_1_LOC(@2);
        auto expr = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
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
        auto name = newAst<NestedNameAst>()->setNamesSR($1);
        $$ = newAst<NamedSpecAst>()->setName(name);
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
        $$ = newAst<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($2);
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
        $$ = newAst<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($3);
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
        $$ = newAst<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($2);
    }
;

CompositeType:
    '[' ']' Type
    {
        DECL_2_LOC(@1, @2);
        auto array = newAst<ArraySpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locB)->setBaseSpec($3);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   '[' Expr ']' Type
    {
        DECL_2_LOC(@1, @3);
        auto array = newAst<ArraySpecAst>()->setLDelimLoc(locA)->setExpr($2)
                ->setRDelimLoc(locB)->setBaseSpec($4);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   '[' "..." ']' Type
    {
        DECL_3_LOC(@1, @2, @3);
        auto array = newAst<ArraySpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locC)->setBaseSpec($4);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   MAP '[' Type ']' Type
    {
        DECL_3_LOC(@1, @2, @4);
        auto array = newAst<ArraySpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setSpec($3)
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
        $$ = newAst<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec($2);
    }
;

SenderChanType:
    CHAN "<-" Type
    {
        DECL_2_LOC(@1, @2);
        auto chann = newAst<ChanSpecAst>()->setKeyLoc(locA)->setDirLoc(locB)->setBaseSpec($3);
        chann->setVariety(ChanVariety::Sender);
        $$ = chann;
    }
;

RecvChanType:
    "<-" CHAN Type
    {
        DECL_2_LOC(@1, @2);
        auto chann = newAst<ChanSpecAst>()->setKeyLoc(locA)->setDirLoc(locB)->setBaseSpec($3);
        chann->setVariety(ChanVariety::Sender);
        $$ = chann;
    }
;

BidirChanType:
    CHAN BuiltinType
    {
        DECL_1_LOC(@1);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN NestedIdent
    {
        DECL_1_LOC(@1);
        auto name = newAst<NestedNameAst>()->setNamesSR($2);
        auto spec = newAst<NamedSpecAst>()->setName(name);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec(spec);
    }
|   CHAN FuncType
    {
        DECL_1_LOC(@1);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN PointerType
    {
        DECL_1_LOC(@1);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN CompositeType
    {
        DECL_1_LOC(@1);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN SenderChanType
    {
        DECL_1_LOC(@1);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN BidirChanType
    {
        DECL_1_LOC(@1);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($2);
    }
|   CHAN '(' Type ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<ChanSpecAst>()->setKeyLoc(locA)->setBaseSpec($3);
    }
;

StructType:
    STRUCT '{' '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto type = newAst<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Struct);
        $$ = type;
    }
|   STRUCT '{' FieldDecls '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto type = newAst<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Struct);
        detail::splitBaseDeclsAndFields(type, $3->finishSR());
        $$ = type;
    }
|   STRUCT '{' FieldDecls ';' '}'
    {
        DECL_3_LOC(@1, @2, @5);
        auto type = newAst<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Struct);
        detail::splitBaseDeclsAndFields(type, $3->finishSR());
        $$ = type;
    }
;

InterfaceType:
    INTERFACE '{' '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto type = newAst<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Interface);
        $$ = type;
    }
|   INTERFACE '{' InterfaceMembers '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto type = newAst<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
            ->setVariety(RecordVariety::Interface);
        detail::splitBaseDeclsAndFields(type, $3->finishSR());
        $$ = type;
    }
|   INTERFACE '{' InterfaceMembers ';' '}'
    {
        DECL_3_LOC(@1, @2, @5);
        auto type = newAst<RecordSpecAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)
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
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT8
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT16
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT32
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT64
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT8
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT16
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT32
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT64
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   FLOAT32
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   FLOAT64
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   COMPLEX_FLOAT64
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   COMPLEX_REAL
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   BYTE
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   RUNE
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
;


    /*--------------------*/
    /*--- Declarations ---*/
    /*--------------------*/

    /* Trailing semi-colons are not kept in the ASTs, they are
       normally optional, except for a case block (where there's
       no closing brace after the statement list). */

Decl:
    ImportGroupDecl
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
        $$ = DeclAstList::createSR(newAst<ErrorDeclAst>());
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

ImportGroupDecl:
    IMPORT ImportDecl
    {
        DECL_1_LOC(@1);
        $$ = newAst<ImportGroupDeclAst>()->setKeyLoc(locA)->addModule($2);
    }
|   IMPORT '(' ')'
    {
        DECL_3_LOC(@1, @2, @3);
        $$ = newAst<ImportGroupDeclAst>()->setKeyLoc(locA)
            ->setLDelimLoc(locB)->setRDelimLoc(locC);
    }
|   IMPORT '(' ImportList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<ImportGroupDeclAst>()->setKeyLoc(locA)
            ->setLDelimLoc(locB)->setModulesSR($3)->setRDelimLoc(locC);
    }
|   IMPORT '(' ImportList ';' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        $$ = newAst<ImportGroupDeclAst>()->setKeyLoc(locA)
            ->setLDelimLoc(locB)->setModulesSR($3)->setRDelimLoc(locC);
    }
;

ImportDecl:
    StringLit
    {
        $$ = newAst<ImportDeclAst>()->setTarget($1);
    }
|   Ident StringLit
    {
        $$ = newAst<ImportDeclAst>()->setLocalName($1)->setTarget($2);
    }
|   '.' StringLit
    {
        DECL_1_LOC(@1);
        auto dot = newAst<GenNameAst>()->setNameLoc(locA);
        context->trackLexeme<Ident>(".", locA.lineCol());
        $$ = newAst<ImportDeclAst>()->setMode(dot)->setTarget($2);
    }
;

ImportList:
    ImportDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   ImportList ';' ImportDecl
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
        $$ = newAst<VarGroupDeclAst>()->setDeclsSR($1)->setSpec($2);
    }
|   VarDeclList Type StringLit
    {
        $$ = newAst<VarTagDeclAst>()->setDeclsSR($1)->setSpec($2)->setTag($3);
    }
|   '*' NestedIdent
    {
        auto name = newAst<NestedNameAst>()->setNamesSR($2);
        $$ = newAst<BaseDeclAst>()->setName(name);
    }
|   '*' NestedIdent StringLit
    {
        IGNORE_FOR_NOW($3);
        auto name = newAst<NestedNameAst>()->setNamesSR($2);
        $$ = newAst<BaseDeclAst>()->setName(name);
    }
|   NestedIdent
    {
        auto name = newAst<NestedNameAst>()->setNamesSR($1);
        $$ = newAst<BaseDeclAst>()->setName(name);
    }
|   NestedIdent StringLit
    {
        IGNORE_FOR_NOW($2);
        auto name = newAst<NestedNameAst>()->setNamesSR($1);
        $$ = newAst<BaseDeclAst>()->setName(name);
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
        $$ = newAst<BaseDeclAst>()->setName($1);
    }
|   Ident Signature
    {
        $$ = newAst<FuncDeclAst>()->setName($1)->setSpec($2);
    }
;

FuncRecvDecl:
    FUNC ParamClauseDecl Ident Signature
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($4)->setKeyLoc(locA);
        auto func = newAst<FuncRecvDeclAst>()->setRecv($2)->setName($3)->setSpec(spec);
        func->setVariety(FuncVariety::Method);
        $$ = func;
    }
|   FUNC ParamClauseDecl Ident Signature BlockStmt
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($4)->setKeyLoc(locA);
        auto func = newAst<FuncRecvDeclAst>()->setRecv($2)->setName($3)->setSpec(spec)->setStmt($5);
        func->setVariety(FuncVariety::Method);
        $$ = func;
    }
;

FuncDecl:
    FUNC Ident Signature
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($3)->setKeyLoc(locA);
        $$ = newAst<FuncDeclAst>()->setName($2)->setSpec(spec);
    }
|   FUNC Ident Signature BlockStmt
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($3)->setKeyLoc(locA);
        $$ = newAst<FuncDeclAst>()->setName($2)->setSpec(spec)->setStmt($4);
    }
;

Signature:
    /* There's an ambiguity here during a conversion when the cast type is a
       function type. It resolves always to the function signature unless the
       cast type is parenthesized. See `Conversions` in Go's language spec. */
    ParamClauseDecl %prec PREFER_SHIFT
    {
        DECL_1_LOC(@1);
        $$ = newAst<FuncSpecAst>()->setParam($1)->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
    }
|   ParamClauseDecl ParamClauseDecl
    {
        $$ = newAst<FuncSpecAst>()->setParam($1)->setResult($2);
    }
|   ParamClauseDecl PlainType
    {
        $$ = newAst<FuncSpecAst>()->setParam($1)->setResult($2);
    }
;

ParamClauseDecl:
    '(' ')'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '(' error ParamClauseDeclSync
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        yyerrok;
    }
|   '(' ParamGroupDeclList ')'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList error ParamClauseDeclSync
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' error ParamClauseDeclSync
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' Ident "..." Type ')'
    {
        DECL_4_LOC(@1, @3, @5, @7);
        auto param = newAst<ParamDeclAst__<ParamVariadic__>>()->setName($4)->setVariadicLoc(locC);
        auto group = newAst<ParamGroupDeclAst>()->addDecl(param)->setSpec($6);
        auto list = $2->handleSR(group);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR(list)->setRDelimLoc(locD);
    }
|   '(' ParamGroupDeclList ',' "..." Type ')'
    {
        DECL_4_LOC(@1, @3, @4, @6);
        auto param = newAst<ParamDeclAst__<ParamVariadic__>>()->setVariadicLoc(locC);
        auto group = newAst<ParamGroupDeclAst>()->addDecl(param)->setSpec($5);
        auto list = $2->handleSR(group);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR(list)->setRDelimLoc(locD);
    }
|   '(' Ident "..." Type ')'
    {
        DECL_3_LOC(@1, @3, @5);
        auto param = newAst<ParamDeclAst__<ParamVariadic__>>()->setName($2)->setVariadicLoc(locB);
        auto group = newAst<ParamGroupDeclAst>()->addDecl(param)->setSpec($4);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->addDecl(group)->setRDelimLoc(locC);
    }
|   '(' "..." Type ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto param = newAst<ParamDeclAst__<ParamVariadic__>>()->setVariadicLoc(locB);
        auto group = newAst<ParamGroupDeclAst>()->addDecl(param)->setSpec($3);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->addDecl(group)->setRDelimLoc(locC);
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
        $$ = newAst<ParamGroupDeclAst>()->setSpec($1);
    }
|   '*' NestedIdent
    {
        DECL_1_LOC(@1);
        auto name = newAst<NestedNameAst>()->setNamesSR($2);
        auto namedSpec = newAst<NamedSpecAst>()->setName(name);
        auto ptrSpec = newAst<PtrSpecAst>()->setOprLoc(locA)->setBaseSpec(namedSpec);
        $$ = newAst<ParamGroupDeclAst>()->setSpec(ptrSpec);
    }
|   ParamDeclList %prec PREFER_SHIFT
    {
        $$ = newAst<ParamGroupDeclAst>()->setDeclsSR($1);
    }
|   ParamDeclList Type
    {
        $$ = newAst<ParamGroupDeclAst>()->setDeclsSR($1)->setSpec($2);
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
        $$ = newAst<ParamDeclAst>()->setName($1);
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
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
|   VAR '(' VarGroupDeclList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
|   VAR '(' VarGroupDeclList ';' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
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
        auto decls = DeclAstList::createSR(newAst<VarDeclAst>()->setName($3));
        auto group = newAst<VarGroupDeclAst>()->setDeclsSR(decls)
            ->setSpec(newAst<InferredSpecAst>());
        $$ = $1->handleSR(group);
    }
;

VarGroupDecl:
    VarDeclList Type
    {
        $$ = newAst<VarGroupDeclAst>()->setDeclsSR($1)->setSpec($2);
    }
|   VarDeclList EOP
    {
        $$ = newAst<VarGroupDeclAst>()->setDeclsSR($1)->setSpec(newAst<InferredSpecAst>());
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   VarDeclList Type '=' ExprList
    {
        DECL_1_LOC(@3);
        $$ = newAst<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
            ->setSpec($2)->setAssignLoc(locA)->setInitsSR($4);
    }
|   VarDeclList Type '=' ExprList EOP
    {
        DECL_1_LOC(@3);
        $$ = newAst<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
            ->setSpec($2)->setAssignLoc(locA)->setInitsSR($4);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   VarDeclList '=' ExprList
    {
        DECL_1_LOC(@2);
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        $$ = newAst<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
            ->setSpec(inferred)->setAssignLoc(locA)->setInitsSR($3);
    }
|   VarDeclList '=' ExprList EOP
    {
        DECL_1_LOC(@2);
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        $$ = newAst<VarGroupDeclAst__<VarGroupInits__>>()->setDeclsSR($1)
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
        $$ = newAst<VarDeclAst>()->setName($1);
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
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->setVariety(SectionVariety::Vars);
        $$ = sect;
    }
|   CONST '(' VarGroupDeclList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto decls = $3->finishSR();
        for (auto decl : *decls)
            detail::constifyVarGroupDecl(decl, locA);
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
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
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
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
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
        sect->setVariety(SectionVariety::Types);
        $$ = sect;
    }
|   TYPE '(' RecordDeclList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
        sect->setVariety(SectionVariety::Types);
        $$ = sect;
    }
|   TYPE '(' RecordDeclList ';' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setDeclsSR($3);
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
            $$ = newAst<RecordDeclAst>()->setName($1)->setSpec($2);
        else
            $$ = newAst<AliasDeclAst>()->setName($1)->setSpec($2);
    }
;


    /*------------------*/
    /*--- Statements ---*/
    /*------------------*/

Stmt:
    Decl
    {
        $$ = newAst<DeclStmtAst>()->setDecl($1);
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
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' error BlockStmtSync
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        yyerrok;
    }
|   '{' StmtList '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
    }
|   '{' StmtList error BlockStmtSync
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        yyerrok;
    }
|   '{' StmtList ';' '}'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
    }
|   '{' StmtList ';' error BlockStmtSync
    {
        DECL_2_LOC(@1, @5);
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
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
        $$ = newAst<LabeledStmtAst>()->setLabel($1)->setDelimLoc(locA)->setStmt($3);
    }
;

GoStmt:
    GO Expr
    {
        // TODO: Report error if not callable expr.
        DECL_1_LOC(@1);
        $$ = newAst<AsyncStmtAst>()->setKeyLoc(locA)->setExpr($2);
    }
;

GotoStmt:
    GOTO Ident
    {
        DECL_1_LOC(@1);
        $$ = newAst<GotoStmtAst>()->setKeyLoc(locA)->setName($2);
    }
;

BreakStmt:
    BREAK
    {
        DECL_1_LOC(@1);
        $$ = newAst<BreakStmtAst>()->setKeyLoc(locA);
    }
|   BREAK Ident
    {
        DECL_1_LOC(@1);
        $$ = newAst<BreakStmtAst>()->setKeyLoc(locA)->setName($2);
    }
;

ReturnStmt:
    RETURN
    {
        DECL_1_LOC(@1);
        $$ = newAst<ReturnStmtAst>()->setKeyLoc(locA);
    }
|   RETURN ExprList
    {
        DECL_1_LOC(@1);
        $$ = newAst<ReturnStmtAst>()->setKeyLoc(locA)->setExprsSR($2);
    }
;

ContinueStmt:
    CONTINUE
    {
        DECL_1_LOC(@1);
        $$ = newAst<ContinueStmtAst>()->setKeyLoc(locA);
    }
|   CONTINUE Ident
    {
        DECL_1_LOC(@1);
        $$ = newAst<ContinueStmtAst>()->setKeyLoc(locA)->setName($2);
    }
;

FallthroughStmt:
    FALLTHROUGH
    {
        DECL_1_LOC(@1);
        $$ = newAst<FallthroughStmtAst>()->setKeyLoc(locA);
    }
;

IfStmt:
    IF Expr BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setExpr($2)->setThen($3);
    }
|   IF Expr BlockStmt ELSE IfStmt
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setExpr($2)->setThen($3)
                ->setElseLoc(locB)->setNotThen($5);
    }
|   IF Expr BlockStmt ELSE BlockStmt
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setExpr($2)->setThen($3)
                ->setElseLoc(locB)->setNotThen($5);
    }
|   IF EffectExpr ';' Expr BlockStmt
    {
        DECL_2_LOC(@1, @3);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setPreamble(stmt)->setExpr($4)
                ->setThen($5);
    }
|   IF EffectExpr ';' Expr BlockStmt ELSE IfStmt
    {
        DECL_3_LOC(@1, @3, @6);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setPreamble(stmt)->setExpr($4)
                ->setThen($5)->setElseLoc(locC)->setNotThen($7);
    }
|   IF EffectExpr ';' Expr BlockStmt ELSE BlockStmt
    {
        DECL_3_LOC(@1, @3, @6);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setPreamble(stmt)->setExpr($4)
                ->setThen($5)->setElseLoc(locC)->setNotThen($7);
    }
;

SwitchStmt:
    SWITCH '{' '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
|   SWITCH '{' CaseClauseStmts '}'
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($3)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
|   SWITCH Expr '{' '}'
    {
        DECL_3_LOC(@1, @3, @4);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH Expr '{' CaseClauseStmts '}'
    {
        DECL_3_LOC(@1, @3, @5);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($4)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH Ident ":=" PostfixExpr '.' '(' TYPE ')' '{' '}'
    {
        IGNORE_FOR_NOW($2);
        DECL_6_LOC(@1, @3, @6, @8, @9, @10);
        auto spec = newAst<TypeofSpecAst>()->setOprLoc(locC)->setLDelimLoc(locB)->setExpr($4)
                ->setRDelimLoc(locD);
        $$ = newAst<TypeSwitchStmtAst>()->setKeyLoc(locA)->setSpec(spec);
    }
|   SWITCH Ident ":=" PostfixExpr '.' '(' TYPE ')' '{' CaseClauseStmts '}'
    {
        IGNORE_FOR_NOW($2);
        DECL_6_LOC(@1, @3, @6, @8, @9, @11);
        auto spec = newAst<TypeofSpecAst>()->setOprLoc(locC)->setLDelimLoc(locB)->setExpr($4)
                ->setRDelimLoc(locD);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($10)->setRDelimLoc(locC);
        $$ = newAst<TypeSwitchStmtAst>()->setKeyLoc(locA)->setSpec(spec)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' '{' '}'
    {
        DECL_3_LOC(@1, @4, @5);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' '{' CaseClauseStmts '}'
    {
        DECL_3_LOC(@1, @4, @6);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($5)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setExpr($2)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' Expr '{' '}'
    {
        DECL_4_LOC(@1, @3, @5, @6);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        auto preamble = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setPreamble(preamble)
                ->setExpr($4)->setStmt(stmt);
    }
|   SWITCH EffectExpr ';' Expr '{' CaseClauseStmts '}'
    {
        DECL_4_LOC(@1, @3, @5, @7);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($6)->setRDelimLoc(locC);
        auto preamble = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setPreamble(preamble)
                ->setExpr($4)->setStmt(stmt);
    }
;

SelectStmt:
    SELECT '{' '}'
    {
        // TODO: Different AST from switch statement or a variety?
        DECL_3_LOC(@1, @2, @3);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
|   SELECT '{' CaseClauseStmts '}'
    {
        // TODO: Different AST from switch statement or a variety?
        DECL_3_LOC(@1, @2, @3);
        auto stmt = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($3)->setRDelimLoc(locC);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
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
        $$ = newAst<CaseClauseStmtAst>()->setKeyLoc(locA)->setExprsSR($2)->setDelimLoc(locB);
    }
|   CASE ExprList ':' StmtList ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<CaseClauseStmtAst>()->setKeyLoc(locA)->setExprsSR($2)->setDelimLoc(locB)
                ->setStmtsSR($4);
    }
|   CASE ExprList '=' Expr ':'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4);

        // TODO: This applies only to select statements.
        $$ = newAst<CaseClauseStmtAst>();
    }
|   CASE ExprList '=' Expr ':' StmtList ';'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        // TODO: This applies only to select statements.
        $$ = newAst<CaseClauseStmtAst>();
    }
|   CASE ExprList ":=" Expr ':'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4);

        // TODO: This applies only to select statements.
        $$ = newAst<CaseClauseStmtAst>();
    }
|   CASE ExprList ":=" Expr ':' StmtList ';'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        // TODO: This applies only to select statements.
        $$ = newAst<CaseClauseStmtAst>();
    }
|   CASE Expr "<-" Expr ':'
    {
        IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($4);

        // TODO: This applies only to select statements.
        $$ = newAst<CaseClauseStmtAst>();
    }
|   CASE Expr "<-" Expr ':' StmtList ';'
    {
        IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        // TODO: This applies only to select statements.
        $$ = newAst<CaseClauseStmtAst>();
    }
|   CASE NonExprTypeList ':'
    {
        IGNORE_LIST_FOR_NOW($2);

        // TODO: Clause of a type switch
        $$ = newAst<CaseClauseStmtAst>();
    }
|   CASE NonExprTypeList ':' StmtList ';'
    {
        IGNORE_LIST_FOR_NOW($2); IGNORE_LIST_FOR_NOW($4);

        // TODO: Clause of a type switch
        $$ = newAst<CaseClauseStmtAst>();
    }
|   DEFAULT ':'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<DefaultClauseStmtAst>()->setKeyLoc(locA)->setDelimLoc(locB);
    }
|   DEFAULT ':' StmtList ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<DefaultClauseStmtAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setStmtsSR($3);
    }
;

ForStmt:
    FOR BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
|   FOR EffectExpr BlockStmt
    {
        DECL_1_LOC(@1);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setStmt($3);
    }
|   FOR EffectExpr ';' ';' BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setStmt($5);
    }
|   FOR EffectExpr ';' EffectExpr ';' BlockStmt
    {
        DECL_3_LOC(@1, @3, @5);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($4)
                ->setDelimLoc(locC)->setStmt($6);
    }
|   FOR EffectExpr ';' EffectExpr ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @3, @5);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($4)
                ->setDelimLoc(locC)->setPost($6)->setStmt($7);
    }
|   FOR EffectExpr ';' ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto stmt = detail::ExprOrShortVarDecl().inspect($2, locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setPost($5)->setStmt($6);
    }
|   FOR ';' EffectExpr ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($3)
                ->setDelimLoc(locC)->setPost($5)->setStmt($6);
    }
|   FOR ';' ';' EffectExpr BlockStmt
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setPost($4)->setStmt($5);
    }
|   FOR ';' EffectExpr ';' BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setCond($3)
                ->setDelimLoc(locC)->setStmt($5);
    }
|   FOR ';' ';' BlockStmt
    {
        DECL_3_LOC(@1, @2, @3);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locB);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setPreamble(stmt)->setDelimLoc(locC)
                ->setStmt($4);
    }
|   FOR ExprList '=' RANGE Expr BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto group = detail::turnExprsIntoVarGroupDecl($2->finishSR());
        auto unpack = newAst<UnpackExprAst>()->setKeyLoc(locC)->setExpr($5);
        $$ = newAst<ForeachStmtAst>()->setDecl(group)->setExpr(unpack)->setStmt($6);
    }
|   FOR ExprList ":=" RANGE Expr BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto group = detail::turnExprsIntoVarGroupDecl($2->finishSR());
        auto unpack = newAst<UnpackExprAst>()->setKeyLoc(locC)->setExpr($5);
        $$ = newAst<ForeachStmtAst>()->setDecl(group)->setExpr(unpack)->setStmt($6);
    }
|   FOR RANGE Expr BlockStmt
    {
        DECL_2_LOC(@1, @2);
        auto unpack = newAst<UnpackExprAst>()->setKeyLoc(locB)->setExpr($3);
        $$ = newAst<ForeachStmtAst>()->setExpr(unpack)->setStmt($4);
    }
;

DeferStmt:
    DEFER Expr
    {
         // TODO: Report error if not callable expr.
         DECL_1_LOC(@1);
         auto stmt = newAst<ExprStmtAst>()->addExpr($2);
         $$ = newAst<DeferredStmtAst>()->setKeyLoc(locA)->setStmt(stmt);
    }
;


    /*-------------*/
    /*--- Names ---*/
    /*-------------*/

Ident:
    IDENT
    {
        DECL_1_LOC(@1);
        $$ = newAst<SimpleNameAst>()->setNameLoc(locA);
    }
|   COMPLETION
    {
        DECL_1_LOC(@1);
        $$ = newAst<CompletionNameAst>()->setNameLoc(locA);
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
    CHAR_LIT
    {
        DECL_1_LOC(@1);
        $$ = newAst<CharLitExprAst>()->setLitLoc(locA);
    }
;

StringLit:
    STR_LIT
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
;

NumLit:
    INT_LIT
    {
        DECL_1_LOC(@1);
        $$ = newAst<NumLitExprAst>()->setLitLoc(locA);
    }
|   FLOAT_LIT
    {
        DECL_1_LOC(@1);
        $$ = newAst<NumLitExprAst>()->setLitLoc(locA);
    }
;

BoolLit:
    TRUE_VALUE
    {
        DECL_1_LOC(@1);
        $$ = newAst<BoolLitExprAst>()->setLitLoc(locA);
    }
|   FALSE_VALUE
    {
        DECL_1_LOC(@1);
        $$ = newAst<BoolLitExprAst>()->setLitLoc(locA);
    }
;

NullLit:
    NULL_VALUE
    {
        DECL_1_LOC(@1);
        $$ = newAst<NullLitExprAst>()->setLitLoc(locA);
    }
;

FuncLit:
    FUNC Signature BlockStmt
    {
        DECL_1_LOC(@1);
        auto spec = FuncSpec_Cast($2)->setKeyLoc(locA);
        $$ = newAst<FuncLitExprAst>()->setSpec(spec)->setStmt($3);
    }
;

CompositeLit:
    CompositeType '{' '}'
    {
        DECL_2_LOC(@2, @3);
        auto init = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        $$ = newAst<RecordLitExprAst>()->setSpec($1)->setInit(init);
    }
|   CompositeType '{' InitList '}'
    {
        DECL_2_LOC(@2, @4);
        auto init = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($3)->setRDelimLoc(locB);
        $$ = newAst<RecordLitExprAst>()->setSpec($1)->setInit(init);
    }
|   CompositeType '{' InitList ',' '}'
    {
        DECL_2_LOC(@2, @5);
        auto init = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($3)->setRDelimLoc(locB);
        $$ = newAst<RecordLitExprAst>()->setSpec($1)->setInit(init);
    }
;
%%
