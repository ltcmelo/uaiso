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

%expect 1
%expect-rr 16
/* Not sure how many of these conflicts can still be removed. They come
   from ambiguities in declaration x statements, template type parameter,
   function literals, member access expression, and to get completion. */


%define api.prefix "D_yy"
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { yyscan_t scanner }
%parse-param { uaiso::DParsingContext* context }
%locations
%output  "DParser.cpp"
%defines "DParser.h"

%code top {
/* Detailed parsing information (enables yydebug). */
#ifdef D_YYDEBUG
#undef D_YYDEBUG
#endif
#define D_YYDEBUG 1
}

%code requires {
/* Make scanner type available. */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

/* The YYSTYPE depends on Ast. */
#include "Ast/Ast.h"

/* The YYSTYPE depends on PartialParse. */
namespace uaiso { namespace detail { struct PartialParse; } }

/* Forward declare the context, it's a yyparse parameter. */
namespace uaiso { class DParsingContext; }

/* Location enhanced with filename. */
typedef struct D_YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  const char* filename;
} D_YYLTYPE;
# define D_YYLTYPE_IS_DECLARED 1
# define D_YYLTYPE_IS_TRIVIAL 1

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
#define YYSTYPE D_YYSTYPE
#define YYLTYPE D_YYLTYPE
}

%{
#include "D/DParsingContext.h"
#include "D/DParser.h"
#include "D/DLexer.h"
#include "D/DFlexBison__.h"
#include <stdlib.h>
#include <stdio.h>

using namespace uaiso;

void D_yyerror(YYLTYPE* yylloc,
               yyscan_t scanner,
               uaiso::ParsingContext* context,
               const char *s);
%}

%union {
    uaiso::NameAst* name_;
    uaiso::NameAstList* names_;
    uaiso::SpecAst* spec_;
    uaiso::SpecAstList* specs_;
    uaiso::AttrAst* attr_;
    uaiso::AttrAstList* attrs_;
    uaiso::DeclAst* decl_;
    uaiso::DeclAstList* decls_;
    uaiso::ExprAst* expr_;
    uaiso::ExprAstList* exprs_;
    uaiso::StmtAst* stmt_;
    uaiso::StmtAstList* stmts_;
    uaiso::TemplateArgAst* tmplArg_;
    uaiso::TemplateArgAstList* tmplArgs_;

    uaiso::detail::PartialParse* partial_;
}

    /*--- Ast type declarations ---*/

%type <name_> Ident IdentOrTemplateInst TemplateInst
%type <names_> IdentList NestedIdent NestedIdentOrTemplateInst

%type <spec_> Type BuiltinType UnnamedType FullType TypeofExprType
%type <spec_> UnqualType NonExprType Signature RecordType Composer

%type <attr_> Attr Annot TypeQual FuncQual FuncAttr AccessSpec
%type <attr_> StorageClass AlignAttr LinkageAttr
%type <attrs_> Attrs TypeQuals FuncAttrs

%type <decl_> Decl RecordDecl RecordTemplateDecl AttrRecordDecl BaseRecordDecl
%type <decl_> BasicDecl VarGroupDecl VarDecl FuncDecl ParamGroupDecl ParamClauseDecl
%type <decl_> CtorDecl DtorDecl Postblit SelectiveDecl InvariantDecl
%type <decl_> EnumDecl EnumMemberDecl EnumBody EnumEnd AliasThis AliasDecl AliasBindDecl
%type <decl_> UnitTestDecl ImportDecl ImportGroupDecl ImportSelectionDecl VersionDecl
%type <decl_> StaticAssert DebugDecl RestrictTemplateParamClause
%type <decl_> TemplateDecl TemplateParamClause RestrictTemplateParam
%type <decl_> TemplateTypeParam TemplateValueParam TemplateAliasParam TemplateTupleParam
%type <decl_> TemplateParam Constraint ForeachDecl TemplateThisParam
%type <decls_> BaseRecordList ParamGroupDeclList TemplateParamList AliasBindDeclList VarDeclList
%type <decls_> RestrictTemplateParamList Decls EnumMemberDeclList ImportList ImportSelectionDeclList

%type <expr_> Expr AssignExpr CondExpr BinaryExpr UnaryExpr PrimaryExpr PostfixExpr
%type <expr_> ThisExpr SuperExpr NewExpr TypeidExpr AssertExpr MixinExpr SpecialKeyword
%type <expr_> PointerLit BoolLit NumLit StringLit CharLit ArrayLit FuncLit TypeQueryExpr
%type <expr_> Init NonVoidInit StructInit ArrayInit StructMemberInit ArrayMemberInit
%type <exprs_> StructMemberInits ArrayMemberInits ExprList

%type <stmt_> Stmt BlockStmt FuncEnd FuncBody FuncOutStmt CondStmt ExprStmt
%type <stmt_> LabeledStmt IfStmt WhileStmt DoStmt ForStmt ForeachStmt BreakStmt
%type <stmt_> SwitchStmt CaseClauseStmt SynchronizedStmt PlainStmt WithStmt
%type <stmt_> TryStmt CatchClauseStmt LastCatchClauseStmt FinallyClauseStmt
%type <stmt_> ReturnStmt ContinueStmt GotoStmt ThrowStmt DeferredStmt
%type <stmts_> StmtList CatchClauseStmts

%type <tmplArg_> TemplateArg TemplateSingleArg
%type <tmplArgs_> TemplateArgList

%type <partial_> UnifiedParamClauseDecl RecordKey VersionCond DebugCond
%type <partial_> StaticIf ConditionDecl


%destructor { delete $$; } <*>
%destructor { delete $$->finishSR(); } <names_>
%destructor { delete $$->finishSR(); } <attrs_>
%destructor { delete $$->finishSR(); } <specs_>
%destructor { delete $$->finishSR(); } <decls_>
%destructor { delete $$->finishSR(); } <exprs_>
%destructor { delete $$->finishSR(); } <stmts_>
%destructor { delete $$->finishSR(); } <tmplArgs_>
%destructor { delete $$; } <partial_>


    /*--------------------------------------------------*/
    /*---            Token declarations              ---*/
    /*---                                            ---*/
    /*---  This section is AUTOMATICALLY GENERATED.  ---*/
    /*--- Do NOT edit manually, changes will be lost ---*/
    /*---       Please refer to Tokens.def           ---*/
    /*--------------------------------------------------*/
%token ABSTRACT 353 "abstract"
%token ALIAS 354 "alias"
%token ALIGN 355 "align"
%token ALIGNAS 356 "alignas"
%token ALIGNOF 357 "aligonf"
%token AMPER_AMPER 302 "&&"
%token AMPER_CARET 303 "&^"
%token AMPER_CARET_EQ 304 "&^="
%token AMPER_EQ 305 "&="
%token AND 358 "and"
%token ARROW_DASH 306 "<-"
%token AS 359 "as"
%token ASM 360 "asm"
%token ASSERT 361 "assert"
%token AUTO 362 "auto"
%token BEGIN_BUILTIN_TYPES 498 "$builtin_types_begin_marker$"
%token BEGIN_CHAR_LIT 282 "$char_lit_begin_marker$"
%token BEGIN_COMMENT 270 "$comments_begin_marker$"
%token BEGIN_KEYWORD 352 "$keyword_begin_marker$"
%token BEGIN_LIT 277 "$lit_begin_marker$"
%token BEGIN_MULTICHAR_OPRTR 301 "$multichar_oprtr_begin_marker$"
%token BEGIN_NUM_LIT 278 "$num_lit_begin_marker$"
%token BEGIN_STR_LIT 287 "$str_lit_begin_marker$"
%token BODY 363 "body"
%token BOOL 499 "bool"
%token BREAK 364 "break"
%token BYTE 500 "byte"
%token CARET_CARET 309 "^^"
%token CARET_CARET_EQ 310 "^^="
%token CARET_EQ 311 "^="
%token CASE 365 "case"
%token CAST 367 "cast"
%token CATCH 368 "catch"
%token CENT 501 "cent"
%token CHAN 366 "chan"
%token CHAR 505 "char"
%token CHAR_LIT 283 "<char_lit>"
%token CHAR_UTF16 506 "char_utf16"
%token CHAR_UTF16_LIT 284 "<char_utf16_lit>"
%token CHAR_UTF32 507 "char_utf32"
%token CHAR_UTF32_LIT 285 "<char_utf32_lit>"
%token CLASS 369 "class"
%token COLON_COLON 312 "::"
%token COLON_EQ 313 ":="
%token COMMENT 271 "<comment>"
%token COMPLETION 264 "<completion>"
%token COMPLEX_FLOAT32 503 "complex_float32"
%token COMPLEX_FLOAT64 502 "complex_float64"
%token COMPLEX_REAL 504 "complex_real"
%token CONST 370 "const"
%token CONSTEXPR 372 "constexpr"
%token CONST_CAST 371 "const_cast"
%token CONTINUE 373 "continue"
%token DASH_ARROW 307 "->"
%token DASH_ARROW_STAR 308 "->*"
%token DATA 374 "data"
%token DEBUG 375 "debug"
%token DECLTYPE 376 "decltype"
%token DEDENT 262 "<dedent>"
%token DEF 377 "def"
%token DEFAULT 378 "default"
%token DEFER 379 "defer"
%token DELEGATE 380 "delegate"
%token DELETE 381 "delete"
%token DEPRECATED 382 "deprecated"
%token DERIVING 383 "deriving"
%token DISABLE 384 "disable"
%token DO 385 "do"
%token DOT_DOT 317 ".."
%token DOT_DOT_DOT 318 "..."
%token DOT_STAR 319 ".*"
%token DOXY_COMMENT 272 "<doxy_comment>"
%token DYNAMIC_CAST 386 "dynamic_cast"
%token ELIF 387 "elif"
%token ELSE 388 "else"
%token END_ASCII 259 "$ascii_end_marker$"
%token END_BUILTIN_TYPES 528 "$builtin_types_end_marker$"
%token END_CHAR_LIT 286 "$char_lit_end_marker$"
%token END_COMMENT 276 "$comments_end_marker$"
%token END_KEYWORD 497 "$keyword_end_marker$"
%token END_LIT 300 "$lit_end_marker$"
%token END_MULTICHAR_OPRTR 351 "$multichar_oprtr_end_marker$"
%token END_NUM_LIT 281 "$num_lit_end_marker$"
%token END_STR_LIT 296 "$str_lit_end_marker$"
%token ENUM 389 "enum"
%token EOP 258 "<end_of_program>"
%token EQ_ARROW 321 "=>"
%token EQ_EQ 320 "=="
%token EXCEPT 390 "except"
%token EXCLAM_EQ 322 "!="
%token EXCLAM_GR 323 "!>"
%token EXCLAM_GR_EQ 324 "!>="
%token EXCLAM_LS 325 "!<"
%token EXCLAM_LS_EQ 326 "!<="
%token EXCLAM_LS_GR 327 "!<>"
%token EXCLAM_LS_GR_EQ 328 "!<>="
%token EXEC 391 "exec"
%token EXPLICIT 392 "explicit"
%token EXPORT 393 "export"
%token EXTERN 394 "extern"
%token FALLTHROUGH 395 "fallthrough"
%token FALSE_VALUE 298 "<false_value>"
%token FINAL 396 "final"
%token FINALLY 397 "finally"
%token FLOAT32 509 "float32"
%token FLOAT64 508 "float64"
%token FLOAT_LIT 280 "<float_lit>"
%token FOR 398 "for"
%token FOREACH 399 "foreach"
%token FOREACH_REVERSE 400 "foreach_reverse"
%token FOREIGN 401 "foreign"
%token FRIEND 402 "friend"
%token FROM 403 "from"
%token FUNC 404 "func"
%token FUNCTION 405 "function"
%token GLOBAL 406 "global"
%token GO 407 "go"
%token GOTO 408 "goto"
%token GR_EQ 329 ">="
%token GR_GR 330 ">>"
%token GR_GR_EQ 333 ">>="
%token GR_GR_GR 331 ">>>"
%token GR_GR_GR_EQ 332 ">>>="
%token IDENT 266 "<ident>"
%token IF 409 "if"
%token IMAG_FLOAT32 511 "imaginary_float32"
%token IMAG_FLOAT64 510 "imaginary_float64"
%token IMAG_REAL 512 "imaginary_real"
%token IMMUTABLE 410 "immutable"
%token IMPORT 411 "import"
%token IN 412 "in"
%token INDENT 261 "<indent>"
%token INFIX 414 "infix"
%token INFIXL 415 "infixl"
%token INFIXR 416 "infixr"
%token INLINE 417 "inline"
%token INOUT 418 "inout"
%token INSTANCE 419 "instance"
%token INT 513 "int"
%token INT16 515 "int16"
%token INT32 516 "int32"
%token INT64 517 "int64"
%token INT8 514 "int8"
%token INTERFACE 420 "interface"
%token INT_LIT 279 "<int_lit>"
%token INVALID 260 "<invalid>"
%token INVARIANT 421 "invariant"
%token IN_LBRACE_HACK 413 "in_{_hack"
%token IS 422 "is"
%token JOKER 265 "<joker>"
%token LAMBDA 423 "lambda"
%token LAZY 424 "lazy"
%token LET 425 "let"
%token LS_EQ 334 "<="
%token LS_GR 337 "<>"
%token LS_GR_EQ 338 "<>="
%token LS_LS 335 "<<"
%token LS_LS_EQ 336 "<<="
%token MACRO 426 "macro"
%token MAP 427 "map"
%token MINUS_EQ 339 "-="
%token MINUS_MINUS 340 "--"
%token MIXIN 428 "mixin"
%token MODULE 429 "module"
%token MULTILINE_COMMENT 273 "<multiline_comment>"
%token MULTILINE_DOXY_COMMENT 274 "<multiline_doxy_comment>"
%token MUTABLE 430 "mutable"
%token NAMESPACE 431 "namespace"
%token NESTING_COMMENT 275 "<nesting_comment>"
%token NEW 432 "new"
%token NEWLINE 263 "<newline>"
%token NEWTYPE 433 "newtype"
%token NOEXCEPT 434 "noexcept"
%token NOGC 435 "nogc"
%token NONLOCAL 436 "nonlocal"
%token NOT 437 "not"
%token NOTHROW 438 "nothrow"
%token NOT_IN_HACK 439 "!_in_hack"
%token NOT_IS_HACK 440 "!_is_hack"
%token NULL_VALUE 297 "<null_value>"
%token OF 442 "of"
%token OPRTR 441 "OPRTR"
%token OR 443 "or"
%token OUT 444 "out"
%token OVERRIDE 445 "override"
%token PACKAGE 446 "package"
%token PASS 447 "pass"
%token PERCENT_EQ 341 "%="
%token PIPE_EQ 342 "|="
%token PIPE_PIPE 343 "||"
%token PLUS_EQ 344 "+="
%token PLUS_PLUS 345 "++"
%token POUND_POUND 346 "##"
%token PRAGMA 448 "pragma"
%token PRINT 449 "print"
%token PRIVATE 450 "private"
%token PROPERTY 451 "property"
%token PROPER_IDENT 267 "<proper_ident>"
%token PROTECTED 452 "protected"
%token PUBLIC 453 "public"
%token PURE 454 "pure"
%token RAISE 455 "raise"
%token RANGE 456 "range"
%token RAW_STR_LIT 292 "<raw_str_lit>"
%token RAW_UTF16_STR_LIT 294 "<raw_utf16_str_lit>"
%token RAW_UTF32_STR_LIT 295 "<raw_utf32_str_lit>"
%token RAW_UTF8_STR_LIT 293 "<raw_utf8_str_lit>"
%token REAL 518 "real"
%token REF 457 "ref"
%token REGISTER 458 "register"
%token REINTERPRET_CAST 459 "reinterpret_cast"
%token RETURN 460 "return"
%token RUNE 519 "rune"
%token SAFE 461 "safe"
%token SCOPE 462 "scope"
%token SELECT 463 "select"
%token SHARED 464 "shared"
%token SIZEOF 465 "sizeof"
%token SLASH_EQ 314 "/="
%token SLASH_SLASH 315 "//"
%token SLASH_SLASH_EQ 316 "//="
%token SPECIAL_IDENT 269 "<special_ident>"
%token STAR_EQ 347 "*="
%token STAR_STAR 348 "**"
%token STAR_STAR_EQ 349 "**="
%token STATIC 466 "static"
%token STATIC_ASSERT 467 "static_assert"
%token STATIC_CAST 468 "static_cast"
%token STRUCT 469 "struct"
%token STR_LIT 288 "<str_lit>"
%token STR_UTF16_LIT 290 "<str_utf16_lit>"
%token STR_UTF32_LIT 291 "<str_utf32_lit>"
%token STR_UTF8_LIT 289 "<str_utf8_lit>"
%token SUPER 470 "super"
%token SWITCH 471 "switch"
%token SYMBOL_IDENT 268 "<symbol_ident>"
%token SYNCHRONIZED 472 "synchronized"
%token SYSTEM 473 "system"
%token TEMPLATE 474 "template"
%token THEN 475 "then"
%token THIS 476 "this"
%token THREAD_LOCAL 477 "thread_local"
%token THROW 478 "throw"
%token TILDE_EQ 350 "~="
%token TRUE_VALUE 299 "<true_value>"
%token TRUSTED 479 "trusted"
%token TRY 480 "try"
%token TYPE 481 "type"
%token TYPEDEF 482 "typedef"
%token TYPEID 483 "typeid"
%token TYPENAME 484 "typename"
%token TYPEOF 485 "typeof"
%token UBYTE 520 "ubyte"
%token UCENT 521 "ucent"
%token UINT 522 "uint"
%token UINT16 524 "uint16"
%token UINT32 525 "uint32"
%token UINT64 526 "uint64"
%token UINT8 523 "uint8"
%token UNION 486 "union"
%token UNITTEST 487 "unittest"
%token USING 488 "using"
%token VAR 489 "var"
%token VERSION 490 "version"
%token VIRTUAL 491 "virtual"
%token VOID 527 "void"
%token VOLATILE 492 "volatile"
%token WHERE 494 "where"
%token WHILE 495 "while"
%token WITH 496 "with"
%token YIELD 493 "yield"
%token __ATTRIBUTE__ 529 "__attribute__"
%token __DATE__MACRO 530 "__date__"
%token __EOF__MACRO 531 "__eof__"
%token __FILE__MACRO 532 "__file__"
%token __FUNCTION__MACRO 533 "__function__"
%token __GSHARED 534 "__gshared"
%token __LINE__MACRO 535 "__line__"
%token __MODULE__MACRO 536 "__module__"
%token __PARAMETERS 537 "__parameters"
%token __PRETTY_FUNCTION__MACRO 538 "__pretty_function__"
%token __THREAD 539 "__thread"
%token __TIMESTAMP__MACRO 541 "__timestamp__"
%token __TIME__MACRO 540 "__time__"
%token __TRAITS 542 "__traits"
%token __VECTOR 543 "__vector"
%token __VENDOR__MACRO 544 "__vendor__"
%token __VERSION__MACRO 545 "__version__"
    /*------------------------------------------*/
    /*--- Tokens AUTOMATICALLY GENERATED end ---*/
    /*------------------------------------------*/

%nonassoc error EOP
%nonassoc PREFER_SHIFT
%left ','
%left "||"
%left "&&"
%left '|'
%left '^'
%left '&'
%left '<' '>' "==" "!=" "<=" ">=" "<>" "<>=" "!<>=" "!<>" "!<" "!<=" "!>" "!>=" IN IS NOT_IN_HACK NOT_IS_HACK
%left "<<" ">>" ">>>"
%left '+' '-' '~'
%left '*' '/' '%'
%left "^^"
%nonassoc '[' '(' '.' "..." ELSE

%start Top

%error-verbose

%%
Top:
    Program
|   error Program
;

Program:
    Decls
    {
        detail::actionProgram(context, $1);
    }
|   Decls EOP
    {
        detail::actionProgram(context, $1);
    }
|   MODULE NestedIdent ';' Decls
    {
        DECL_2_LOC(@1, @3);
        detail::actionProgram(context, locA, $2, locB, $4);
    }
|   MODULE NestedIdent ';' Decls EOP
    {
        DECL_2_LOC(@1, @3);
        detail::actionProgram(context, locA, $2, locB, $4);
    }
;

    /*-------------------*/
    /*--- Expressions ---*/
    /*-------------------*/

Expr:
    AssignExpr
|   AssignExpr ',' Expr
    {
        DECL_1_LOC(@2);
        $$ = newAst<CommaExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
;

AssignExpr:
    CondExpr
|   CondExpr '=' AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "+=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "-=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "*=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "/=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "%=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "|=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "^=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "^^=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "&=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "<<=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr ">>=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr ">>>=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "~=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
;

CondExpr:
    BinaryExpr
|   BinaryExpr '?' Expr ':' CondExpr
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<CondExprAst>()->setCond($1)->setQuestionLoc(locA)->setYes($3)
                ->setDelimLoc(locB)->setNo($5);
    }
;

BinaryExpr:
    UnaryExpr
|   BinaryExpr "||" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<LogicOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "&&" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<LogicAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '|' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<BitOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '^' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<BitXorExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '&' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<BitAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "==" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '<' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '>' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr ">=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<>=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<>=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!>=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr IS BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<IsExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr NOT_IS_HACK BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<IsExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr IN BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<InExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr NOT_IN_HACK BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<InExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<<" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr ">>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr ">>>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '+' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<AddExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '-' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<SubExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '~' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ConcatExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '*' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<MulExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '/' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<DivExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '%' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<ModExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "^^" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<PowerExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
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
        DECL_1_LOC(@1);
        $$ = newAst<BitCompExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   "++" UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<IncDecExprAst>()->setPrefixLoc(locA)->setValue($2);
    }
|   "--" UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<IncDecExprAst>()->setPrefixLoc(locA)->setValue($2);
    }
|   DELETE UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = newAst<DelExprAst>()->setKeyLoc(locA)->addExpr($2);
    }
|   CAST '(' Type ')' UnaryExpr
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<CastExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setSpec($3)
                ->setRDelimLoc(locC)->setExpr($5);
    }
|   CAST '(' ')' UnaryExpr
    {
        /* This case removes any top level type modifiers. */
        DECL_3_LOC(@1, @2, @3);
        $$ = newAst<CastExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setExpr($4);
    }
    /*    |   CAST '(' CastQual ')' UnaryExpr */
|   '(' Type ')' '.' NestedIdentOrTemplateInst %dprec 1
    {
        DECL_3_LOC(@1, @3, @4);
        auto name = newAst<NestedNameAst>()->setNamesSR($5);
        $$ = newAst<MemberAccessExprAst>()->setSpec($2)->setOprLoc(locC)->setName(name);
    }
|   PostfixExpr %dprec 2
;

PostfixExpr:
    PrimaryExpr
|   PostfixExpr "++"
    {
        DECL_1_LOC(@2);
        $$ = newAst<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   PostfixExpr "--"
    {
        DECL_1_LOC(@2);
        $$ = newAst<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   PostfixExpr '.' IdentOrTemplateInst
    {
        DECL_1_LOC(@2);
        $$ = newAst<MemberAccessExprAst>()->setExpr($1)->setOprLoc(locA)->setName($3);
    }
|   PostfixExpr '.' NewExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<NestedNewExprAst>()->setBase($1)->setOprLoc(locA)->setNestedNew($3);
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
        DECL_2_LOC(@2, @5);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
        yyerrok;
    }
|   PostfixExpr '(' ExprList EOP
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   PostfixExpr '[' AssignExpr ']'
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' AssignExpr error PostfixExprSync
    {
        DECL_2_LOC(@2, @5);
        $$ = newAst<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
        yyerrok;
    }
|   PostfixExpr '[' AssignExpr EOP
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   PostfixExpr '[' ']'
    {
        /* This expr creates a slice as large as the array. */
        DECL_2_LOC(@2, @3);
        $$ = newAst<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' AssignExpr ".." AssignExpr ']'
    {
        DECL_3_LOC(@2, @4, @6);
        auto range = newAst<SubrangeExprAst>()->setLow($3)->setDelim1Loc(locB)->setHi($5);
        $$ = newAst<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange(range)
                ->setRDelimLoc(locC);
    }
|   PostfixExpr '[' AssignExpr ".." AssignExpr error PostfixExprSync
    {
        DECL_3_LOC(@2, @4, @7);
        auto range = newAst<SubrangeExprAst>()->setLow($3)->setDelim1Loc(locB)->setHi($5);
        $$ = newAst<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange(range)
                ->setRDelimLoc(locC);
        yyerrok;
    }
|   PostfixExpr '[' AssignExpr ".." AssignExpr EOP
    {
        DECL_3_LOC(@2, @4, @6);
        auto range = newAst<SubrangeExprAst>()->setLow($3)->setDelim1Loc(locB)->setHi($5);
        $$ = newAst<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange(range)
                ->setRDelimLoc(locC);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
;

PostfixExprSync: ']' | ')' | EOP;

PrimaryExpr:
    IdentOrTemplateInst
    {
        $$ = newAst<IdentExprAst>()->setName($1);
    }
|   '.' IdentOrTemplateInst
    {
        DECL_1_LOC(@1);
        context->trackLexeme<Ident>(".", locA.lineCol());
        auto dot = newAst<GenNameAst>()->setNameLoc(locA);
        auto name = newAst<NestedNameAst>()->setNamesSR(NameAstList::createSR(dot)->handleSR($2));
        $$ = newAst<IdentExprAst>()->setName(name);
    }
|   ThisExpr
|   SuperExpr
|   NewExpr
|   TypeidExpr
|   AssertExpr
|   MixinExpr
|   TypeQueryExpr
|   BuiltinType '.' Ident
    {
        DECL_1_LOC(@1);
        $$ = newAst<MemberAccessExprAst>()->setSpec($1)->setOprLoc(locA)->setName($3);
    }
|   '$'
    {
        DECL_1_LOC(@1);
        $$ = newAst<ArrayLengthExprAst>()->setKeyLoc(locA);
    }
|   PointerLit
|   BoolLit
|   NumLit
|   StringLit
|   CharLit
|   ArrayLit
|   FuncLit
|   '(' Expr ')'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<WrappedExprAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locB);
    }
;

ThisExpr:
    THIS
    {
        DECL_1_LOC(@1);
        $$ = newAst<ThisExprAst>()->setKeyLoc(locA);
    }
;

SuperExpr:
    SUPER
    {
        DECL_1_LOC(@1);
        $$ = newAst<SuperExprAst>()->setKeyLoc(locA);
    }
;

NewExpr:
    /* Empty parenthesized arguments and trailing semi-colons
       don't go into the AST. */
    NEW Type %prec PREFER_SHIFT
    {
        DECL_1_LOC(@1);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setSpec($2);
    }
|   NEW Type '(' ')'
    {
        DECL_1_LOC(@1);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setSpec($2);
    }
|   NEW Type '(' ExprList ')'
    {
        DECL_3_LOC(@1, @3, @5);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setSpec($2)->setLArgDelimLoc(locB)
                ->setArgsSR($4)->setRArgDelimLoc(locC);
    }
|   NEW '(' ')' Type %prec PREFER_SHIFT
    {
        DECL_1_LOC(@1);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setSpec($4);
    }
|   NEW '(' ExprList ')' Type %prec PREFER_SHIFT
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($5);
    }
|   NEW '(' ExprList ',' ')' Type %prec PREFER_SHIFT
    {
        DECL_3_LOC(@1, @2, @5);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($6);
    }
|   NEW '(' ')' Type '(' ')'
    {
        DECL_1_LOC(@1);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setSpec($4);
    }
|   NEW '(' ExprList ')' Type '(' ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($5);
    }
|   NEW '(' ExprList ',' ')' Type '(' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($6);
    }
|   NEW '(' ')' Type '(' ExprList ')'
    {
        DECL_3_LOC(@1, @5, @6);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setSpec($4)->setLArgDelimLoc(locB)
                ->setArgsSR($6)->setRArgDelimLoc(locC);
    }
|   NEW '(' ExprList ')' Type '(' ExprList ')'
    {
        DECL_5_LOC(@1, @2, @4, @6, @7);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
            ->setRAllocDelimLoc(locC)->setSpec($5)->setLArgDelimLoc(locD)->setArgsSR($7)->setRArgDelimLoc(locE);
    }
|   NEW '(' ExprList ',' ')' Type '(' ExprList ')'
    {
        DECL_5_LOC(@1, @2, @5, @6, @8);
        $$ = newAst<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
            ->setRAllocDelimLoc(locC)->setSpec($6)->setLArgDelimLoc(locD)->setArgsSR($8)->setRArgDelimLoc(locE);
    }
;

TypeidExpr:
    TYPEID '(' NonExprType ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<TypeidExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setSpec($3)
                ->setRDelimLoc(locC);
    }
|   TYPEID '(' Expr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<TypeidExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC);
    }
;

AssertExpr:
    ASSERT '(' AssignExpr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<AssertExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
|   ASSERT '(' AssignExpr ',' AssignExpr ')'
    {
        DECL_4_LOC(@1, @2, @4, @6);
        $$ = newAst<AssertExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setMDelimLoc(locC)
                ->setMessage($5)->setRDelimLoc(locD);
    }
;

MixinExpr:
    MIXIN '(' AssignExpr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<MixinExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
;

TypeQueryExpr:
    IS '(' Type ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto intro = newAst<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type ':' IntrospectSpecialization ')'
    {
        DECL_3_LOC(@1, @2, @6);
        auto intro = newAst<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type "==" IntrospectSpecialization ')'
    {
        DECL_3_LOC(@1, @2, @6);
        auto intro = newAst<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type Ident ')'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @2, @5);
        auto intro = newAst<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type Ident ':' IntrospectSpecialization ')'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @2, @7);
        auto intro = newAst<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type Ident "==" IntrospectSpecialization ')'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @2, @7);
        auto intro = newAst<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
;

IntrospectSpecialization:
    Type
|   STRUCT
|   UNION
|   CLASS
|   INTERFACE
|   ENUM
|   FUNCTION
|   DELEGATE
|   SUPER
|   CONST
|   IMMUTABLE
|   INOUT
|   SHARED
|   RETURN
;

ExprList:
    AssignExpr
    {
        $$ = ExprAstList::createSR($1);
    }
|   ExprList ',' AssignExpr
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
    FullType
    {
        $$ = $1;
    }
|   Attrs FullType
    {
        $$ = newAst<DecoratedSpecAst>()->setAttrsSR($1)->setSpec($2);
    }
|   TypeQual '(' Type ')'
    {
        $$ = newAst<DecoratedSpecAst>()->addAttr($1)->setSpec($3);
    }
|   Attrs TypeQual '(' Type ')'
    {
        auto attrs = $1->handleSR($2);
        $$ = newAst<DecoratedSpecAst>()->setAttrsSR(attrs)->setSpec($4);
    }
|   Type Composer
    {
        $$ = OpaqueSpec_Cast($2)->setBaseSpec($1);
    }
|   Type Signature
    {
        $$ = FuncSpec_Cast($2)->setResult($1);
    }
;

UnqualType:
    FullType
|   UnqualType Composer
    {
        $$ = OpaqueSpec_Cast($2)->setBaseSpec($1);
    }
;

NonExprType:
    /* This is an artificial non-terminal for a type that will not be matched
       as an expression.
       TODO: This not really correct yet. */
    UnnamedType
|   TypeQuals FullType
    {
        $$ = newAst<DecoratedSpecAst>()->setAttrsSR($1)->setSpec($2);
    }
|   NonExprType Composer
    {
        $$ = OpaqueSpec_Cast($2)->setBaseSpec($1);
    }
;

Composer:
    '[' ']'
    {
        DECL_2_LOC(@1, @2);
        auto array = newAst<ArraySpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   '[' NonExprType ']'
    {
        /* Associative arrays of unnamed types are matched
           here. The others are matched as an `AssignExpr`. */
        DECL_2_LOC(@1, @3);
        auto array = newAst<ArraySpecAst>()->setLDelimLoc(locA)->setSpec($2)->setRDelimLoc(locB);
        array->setVariety(ArrayVariety::Associative);
        $$ = array;
    }
|   '[' AssignExpr ']'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ArraySpecAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locB);
    }
|   '[' AssignExpr ".." AssignExpr ']'
    {
        // TODO: Must be a type list.
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @3, @5);
        $$ = newAst<ArraySpecAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locC);
    }
|   '*'
    {
        DECL_1_LOC(@1);
        $$ = newAst<PtrSpecAst>()->setOprLoc(locA);
    }
;

Signature:
    FUNCTION ParamClauseDecl
    {
        DECL_1_LOC(@1);
        $$ = newAst<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
    }
|   FUNCTION ParamClauseDecl FuncAttrs
    {
        DECL_1_LOC(@1);
        auto spec = newAst<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
        $$ = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
    }
|   DELEGATE ParamClauseDecl
    {
        DECL_1_LOC(@1);
        $$ = newAst<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
    }
|   DELEGATE ParamClauseDecl FuncAttrs
    {
        DECL_1_LOC(@1);
        auto spec = newAst<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
        $$ = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
    }
;

FullType:
    UnnamedType
|   NestedIdentOrTemplateInst %prec PREFER_SHIFT
    {
        auto name = newAst<NestedNameAst>()->setNamesSR($1);
        $$ = newAst<NamedSpecAst>()->setName(name);
    }
|   '.' NestedIdentOrTemplateInst
    {
        DECL_1_LOC(@1);
        auto global = (new GenNameAst("<global>"))->setNameLoc(locA);
        auto names = NameAstList::createSR(global);
        names = names->mergeSR($2);
        auto name = newAst<NestedNameAst>()->setNamesSR(names);
        $$ = newAst<NamedSpecAst>()->setName(name);
    }
;

UnnamedType:
    BuiltinType
|   TypeofExprType %prec PREFER_SHIFT
|   TypeofExprType '.' NestedIdentOrTemplateInst
    {
        IGNORE_LIST_FOR_NOW($3);

        DECL_1_LOC(@2);
        $$ = $1;
    }
;

TypeofExprType:
    TYPEOF '(' Expr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<TypeofSpecAst>()->setOprLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
|   TYPEOF '(' RETURN ')'  %prec PREFER_SHIFT
    {
        // TODO: Deal with `return`.
        DECL_4_LOC(@1, @2, @3, @4);
        $$ = newAst<TypeofSpecAst>()->setOprLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
    }
;

BuiltinType:
    BOOL
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   BYTE
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UBYTE
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT16
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT16
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT64
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT64
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   CHAR
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   CHAR_UTF16
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   CHAR_UTF32
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
|   REAL
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   IMAG_FLOAT32
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   IMAG_FLOAT64
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   IMAG_REAL
    {
        DECL_1_LOC(@1);
        $$ = newAst<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   COMPLEX_FLOAT32
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
|   VOID
    {
        DECL_1_LOC(@1);
        $$ = newAst<VoidSpecAst>()->setKeyLoc(locA);
    }
;


    /*--------------------*/
    /*--- Declarations ---*/
    /*--------------------*/

Decl:
    BasicDecl
|   TemplateDecl
|   CtorDecl
|   DtorDecl
|   Postblit
|   VersionDecl
|   DebugDecl
|   StaticAssert
|   UnitTestDecl
|   AliasThis
|   SelectiveDecl
|   InvariantDecl
|  '{' Decls '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<BlockDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   Attrs '{' Decls '}'
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<BlockDeclAst>()->setAttrsSR($1)->setLDelimLoc(locA)->setDeclsSR($3)->setRDelimLoc(locB);
    }
|   Attrs ':' Decl
    {
        IGNORE_LIST_FOR_NOW($1); // Embed the attributes in the decl.

        DECL_1_LOC(@2);
        $$ = $3;
    }
;

BasicDecl:
    VarGroupDecl
|   FuncDecl
|   AliasDecl
|   EnumDecl
|   RecordDecl
|   AttrRecordDecl
|   ImportGroupDecl
;

VarGroupDecl:
    Type VarDeclList ';'
    {
        DECL_1_LOC(@3);
        $$ = detail::actionVarGroupDecl($1, $2, locA);
    }
|   Type error
    {
        $$ = newAst<VarGroupDeclAst>()->setSpec($1);
    }
|   Type VarDeclList EOP
    {
        DECL_1_LOC(@3);
        $$ = detail::actionVarGroupDecl($1, $2, locA);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   Type VarDeclList error VarGroupDeclSync
    {
        DECL_1_LOC(@3);
        $$ = detail::actionVarGroupDecl($1, $2, locA);
        yyerrok;
    }
|   Attrs VarDeclList ';'
    {
        DECL_1_LOC(@3);
        $$ = detail::actionVarGroupDecl($1, $2, locA);
    }
|   Attrs VarDeclList EOP
    {
        DECL_1_LOC(@3);
        $$ = detail::actionVarGroupDecl($1, $2, locA);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   Attrs VarDeclList error VarGroupDeclSync
    {
        DECL_1_LOC(@3);
        $$ = detail::actionVarGroupDecl($1, $2, locA);
        yyerrok;
    }
;

VarGroupDeclSync: ';' | EOP

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
|   Ident '=' Init
    {
        DECL_1_LOC(@2);
        $$ = newAst<VarDeclAst__<VarInit__>>()->setName($1)->setAssignLoc(locA)->setInit($3);
    }
|   Ident RestrictTemplateParamClause '=' Init
    {
        IGNORE_FOR_NOW($2);

        DECL_1_LOC(@3);
        $$ = newAst<VarDeclAst__<VarInit__>>()->setName($1)->setAssignLoc(locA)->setInit($4);
    }
;

FuncDecl:
    Type Ident UnifiedParamClauseDecl FuncEnd
    {
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        $$ = newAst<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($4);
    }
|   Type Ident UnifiedParamClauseDecl Constraint FuncEnd
    {
        IGNORE_FOR_NOW($4);

        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        $$ = newAst<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($5);
    }
|   Type Ident UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName($2)->setStmt($5);
    }
|   Type Ident UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($5);

        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = newAst<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($6);
    }
|   Attrs Ident UnifiedParamClauseDecl FuncEnd
    {
        DECL_1_LOC(@1);

        using namespace detail;
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        auto result = newAst<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        $$ = newAst<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($4);
    }
|   Attrs Ident UnifiedParamClauseDecl Constraint FuncEnd
    {
        IGNORE_FOR_NOW($4);

        DECL_1_LOC(@1);
        using namespace detail;
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        auto result = newAst<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        $$ = newAst<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($5);
    }
|   Attrs Ident UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        DECL_1_LOC(@1);
        using namespace detail;
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        auto result = newAst<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName($2)->setStmt($5);
    }
|   Attrs Ident UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($5);

        DECL_1_LOC(@1);
        using namespace detail;
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        auto result = newAst<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName($2)->setStmt($6);
    }
;

CtorDecl:
    THIS UnifiedParamClauseDecl FuncEnd
    {
        DECL_1_LOC(@1);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($2));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto func = newAst<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($3);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   THIS UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        DECL_1_LOC(@1);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($2));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
        auto func = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($4);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   THIS UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($4);

        DECL_1_LOC(@1);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($2));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
        auto func = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   Attrs THIS UnifiedParamClauseDecl FuncEnd
    {
        DECL_1_LOC(@2);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($1);
        auto func = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($4);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   Attrs THIS UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        DECL_1_LOC(@2);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto attrs = $1->mergeSR($4);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR(attrs);
        auto func = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   Attrs THIS UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($5);

        DECL_1_LOC(@2);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = newAst<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto attrs = $1->mergeSR($4);
        auto attrSpec = newAst<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR(attrs);
        auto func = newAst<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
;

DtorDecl:
    '~' THIS '(' ')' FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = newAst<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto spec = newAst<FuncSpecAst>()->setParam(param)
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locB);
        auto func = newAst<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
|   '~' THIS '(' ')' FuncAttrs FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = newAst<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto baseSpec = newAst<FuncSpecAst>()->setParam(param)
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = newAst<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR($5);
        auto name = newAst<SimpleNameAst>()->setNameLoc(locB);
        auto func = newAst<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
|   Attrs '~' THIS '(' ')' FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@2, @3, @4, @5);
        auto param = newAst<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto baseSpec = newAst<FuncSpecAst>()->setParam(param)
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = newAst<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR($1);
        auto name = newAst<SimpleNameAst>()->setNameLoc(locB);
        auto func = newAst<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
|   Attrs '~' THIS '(' ')' FuncAttrs FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto attrs = $1->mergeSR($6);
        auto param = newAst<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto baseSpec = newAst<FuncSpecAst>()->setParam(param)
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = newAst<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR(attrs);
        auto name = newAst<SimpleNameAst>()->setNameLoc(locB);
        auto func = newAst<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($7);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
;

Postblit:
    THIS '(' THIS ')' FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = newAst<ParamClauseDeclAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        auto spec = newAst<FuncSpecAst>()->setParam(param)
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto func = newAst<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   THIS '(' THIS ')' FuncAttrs FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = newAst<ParamClauseDeclAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        auto baseSpec = newAst<FuncSpecAst>()->setParam(param)
            ->setResult(newAst<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = newAst<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR($5);
        auto name = newAst<SimpleNameAst>()->setNameLoc(locA);
        auto func = newAst<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
;

UnifiedParamClauseDecl:
    '(' ')'
    {
        DECL_2_LOC(@1, @2);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param);
    }
|   '(' ParamGroupDeclList ')'
    {
        DECL_2_LOC(@1, @3);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param);
    }
|   '(' ParamGroupDeclList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param);
    }
|   '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_4_LOC(@1, @3, @4, @5);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($2)->setVariadicLoc(locC)->setRDelimLoc(locD);
        $$ = new detail::FullParam(param);
    }
|   '(' "..." ')'
    {
        DECL_3_LOC(@1, @2, @3);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setVariadicLoc(locB)->setRDelimLoc(locC);
        $$ = new detail::FullParam(param);
    }
|   '(' RestrictTemplateParamList ')' '(' ')'
    {
        DECL_4_LOC(@1, @3, @4, @5);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' ParamGroupDeclList ')'
    {
        DECL_4_LOC(@1, @3, @4, @6);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($5)->setRDelimLoc(locB);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' ParamGroupDeclList ',' ')'
    {
        DECL_4_LOC(@1, @3, @4, @7);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($5)->setRDelimLoc(locB);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_6_LOC(@1, @3, @4, @6, @7, @8);
        $5->delim_ = locD;
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($5)->setVariadicLoc(locE)->setRDelimLoc(locF);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' "..." ')'
    {
        DECL_5_LOC(@1, @3, @4, @5, @6);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setVariadicLoc(locD)->setRDelimLoc(locE);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ')'
    {
        DECL_4_LOC(@1, @4, @5, @6);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ParamGroupDeclList ')'
    {
        DECL_4_LOC(@1, @4, @5, @7);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($6)->setRDelimLoc(locD);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ParamGroupDeclList ',' ')'
    {
        DECL_4_LOC(@1, @4, @5, @8);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($6)->setRDelimLoc(locD);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_6_LOC(@1, @4, @5, @7, @8, @9);
        $6->delim_ = locD;
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($6)->setVariadicLoc(locE)->setRDelimLoc(locF);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' "..." ')'
    {
        DECL_5_LOC(@1, @4, @5, @6, @7);
        auto param = newAst<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setVariadicLoc(locD)->setRDelimLoc(locE);
        auto templParam = newAst<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    };

ParamClauseDecl:
    '(' ')'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ')'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_4_LOC(@1, @3, @4, @5);
        $2->delim_ = locB;
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setVariadicLoc(locC)
                ->setRDelimLoc(locD);
    }
|   '(' "..." ')'
    {
        DECL_3_LOC(@1, @2, @3);
        $$ = newAst<ParamClauseDeclAst>()->setLDelimLoc(locA)->setVariadicLoc(locB)->setRDelimLoc(locC);
    }
;

ParamGroupDeclList:
    ParamGroupDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   ParamGroupDeclList ',' ParamGroupDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

ParamGroupDecl:
    Type
    {
        auto group = newAst<ParamGroupDeclAst>();
        group->setSpec($1);
        $$ = group;
    }
|   Type "..."
    {
        DECL_1_LOC(@2);
        auto param = newAst<ParamDeclAst__<ParamVariadic__>>();
        auto group = newAst<ParamGroupDeclAst>();
        group->addDecl(param)->setSpec($1);
        $$ = param;
    }
|   Type Ident
    {
        auto param = newAst<ParamDeclAst>();
        param->setName($2);
        auto group = newAst<ParamGroupDeclAst>();
        group->addDecl(param)->setSpec($1);
        $$ = group;
    }
|   Type Ident "..."
    {
        DECL_1_LOC(@3);
        auto param = newAst<ParamDeclAst__<ParamVariadic__>>();
        param->setName($2);
        auto group = newAst<ParamGroupDeclAst>();
        group->addDecl(param)->setSpec($1);
        $$ = group;
    }
|   Type Ident '=' AssignExpr
    {
        DECL_1_LOC(@3);
        auto param = newAst<ParamDeclAst__<ParamVariadic__Empty__,
                                         ParamDefaultArg__>>();
        param->setName($2)->setAssignLoc(locA)->setDefaultArg($4);
        auto group = newAst<ParamGroupDeclAst>();
        group->addDecl(param)->setSpec($1);
        $$ = group;
    }
;

Decls:
    Decl
    {
        $$ = DeclAstList::createSR($1);
    }
|   Decls Decl
    {
        $$ = $1->handleSR($2);
    }
|   Decls error DeclsSync
    {
        $$ = $1;
        yyerrok;
    }
;

DeclsSync: ';';

RestrictTemplateParamClause:
    '(' RestrictTemplateParamList ')'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' RestrictTemplateParamList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
;

RestrictTemplateParamList:
    RestrictTemplateParam
    {
        $$ = DeclAstList::createSR($1);
    }
|   RestrictTemplateParamList ',' RestrictTemplateParam
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

RestrictTemplateParam:
    TemplateTypeParam
|   TemplateAliasParam
|   TemplateTupleParam
;

TemplateParamClause:
    '(' ')'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '(' TemplateParamList ')'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' TemplateParamList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
;

TemplateParamList:
    TemplateParam
    {
        $$ = DeclAstList::createSR($1);
    }
|   TemplateParamList ',' TemplateParam
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

TemplateParam:
    TemplateTypeParam
|   TemplateTupleParam
|   TemplateValueParam
|   TemplateAliasParam
|   TemplateThisParam
;

TemplateTypeParam:
    Ident
    {
        $$ = newAst<TemplateParamDeclAst>()->setName($1);
    }
|   Ident ':' Type
    {
        DECL_1_LOC(@2);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__Empty__,
                                         TemplateParamSpecialization__>>()->setName($1)
            ->setBindLoc(locA)->setSpecialization($3);
    }
|   Ident ':' Type '=' Type
    {
        DECL_2_LOC(@2, @4);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__,
                                         TemplateParamSpecialization__>>()->setName($1)
                ->setAssignLoc(locA)->setDefaultArg($3)
                ->setBindLoc(locB)->setSpecialization($5);
    }
|   Ident '=' Type
    {
        DECL_1_LOC(@2);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__>>()->setName($1)
                ->setAssignLoc(locA)->setDefaultArg($3);
    }
;

TemplateTupleParam:
    Ident "..."
    {
        DECL_1_LOC(@2);
        $$ = newAst<TemplateParamDeclAst>()->setName($1)->setPackLoc(locA);
    }
;

TemplateValueParam:
    UnqualType Ident
    {
        $$ = newAst<TemplateParamDeclAst>()->setSpec($1)->setName($2);
    }
|   UnqualType Ident '=' AssignExpr
    {
        DECL_1_LOC(@3);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)->setSpec($1);
    }
|   UnqualType Ident '=' SpecialKeyword
    {
        DECL_1_LOC(@3);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)->setSpec($1);
    }
|   UnqualType Ident ':' CondExpr
    {
        DECL_1_LOC(@3);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__Empty__,
                                         TemplateParamSpecialization__>>()->setName($2)
            ->setBindLoc(locA)->setSpecialization($4)->setSpec($1);
    }
|   UnqualType Ident ':' CondExpr '=' AssignExpr
    {
        DECL_2_LOC(@3, @5);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__,
                                         TemplateParamSpecialization__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)
                ->setBindLoc(locB)->setSpecialization($6)->setSpec($1);
    }
|   UnqualType Ident ':' CondExpr '=' SpecialKeyword
    {
        DECL_2_LOC(@3, @5);
        $$ = newAst<TemplateParamDeclAst__<TemplateParamDefaultArg__,
                                         TemplateParamSpecialization__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)
                ->setBindLoc(locB)->setSpecialization($6)->setSpec($1);
    }
;

TemplateAliasParam:
    ALIAS Ident
    {
        DECL_1_LOC(@1);
        $$ = newAst<TemplateParamAliasDeclAst>()->setAliasLoc(locA)->setName($2);
    }
|   ALIAS Type Ident
    {
        DECL_1_LOC(@1);
        $$ = newAst<TemplateParamAliasDeclAst>()->setAliasLoc(locA)->setSpec($2)->setName($3);
    }
    /* TODO: Regras faltando... */
;

TemplateThisParam:
    THIS TemplateTypeParam
    {
        IGNORE_FOR_NOW($2);

        DECL_1_LOC(@1);
        auto name = (new GenNameAst("<TODO:TemplateThisParam>"))->setNameLoc(locA);
        $$ = newAst<TemplateParamThisDeclAst>()->setThissLoc(locA)->setName(name);
    }
;

FuncAttrs:
    FuncAttr
    {
        $$ = AttrAstList::createSR($1);
    }
|   FuncAttrs FuncAttr
    {
        $$ = $1->handleSR($2);
    }
;

FuncAttr:
    FuncQual
|   TypeQual
;

FuncQual:
    NOTHROW
    {
        DECL_1_LOC(@1);
        $$ = newAst<DeclAttrAst>()->setKeyLoc(locA);
    }
|   PURE
    {
        DECL_1_LOC(@1);
        $$ = newAst<DeclAttrAst>()->setKeyLoc(locA);
    }
|   Annot
;

Constraint:
    IF '(' Expr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<ConstraintDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC);
    }
;

Init:
    VOID
    {
        DECL_1_LOC(@1);
        $$ = newAst<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   NonVoidInit
;

NonVoidInit:
    /* An ArrayInit is an ArrayLit, which is already matched
       by an AssignExpr (as a PrimaryExpr). */
    AssignExpr
|   StructInit
;

ArrayInit:
    '[' ']'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<ArrayInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '[' ArrayMemberInits ']'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<ArrayInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
|   '[' ArrayMemberInits ',' ']'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<ArrayInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
;

ArrayMemberInits:
    ArrayMemberInit
    {
        $$ = ExprAstList::createSR($1);
    }
|   ArrayMemberInits ',' ArrayMemberInit
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

ArrayMemberInit:
    NonVoidInit
|   AssignExpr ':' NonVoidInit
    {
        DECL_1_LOC(@1);
        $$ = newAst<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue($3);
    }
;

StructInit:
    '{' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' StructMemberInits '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
|   '{' StructMemberInits ',' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
;

StructMemberInits:
    StructMemberInit
    {
        $$ = ExprAstList::createSR($1);
    }
|   StructMemberInits ',' StructMemberInit
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

StructMemberInit:
    NonVoidInit
|   Ident ':' NonVoidInit
    {
        DECL_1_LOC(@1);
        auto id = newAst<IdentExprAst>()->setName($1);
        $$ = newAst<DesignateExprAst>()->setId(id)->setDelimLoc(locA)->setValue($3);
    }
;

Attrs:
    Attr
    {
        $$ = AttrAstList::createSR($1);
    }
|   Attrs Attr
    {
        $$ = $1->handleSR($2);
    }
;

Attr:
    StorageClass
|   AccessSpec
|   FuncQual
|   LinkageAttr
|   AlignAttr
|   TypeQual
|   ABSTRACT
    {
        DECL_1_LOC(@1);
        $$ = newAst<DeclAttrAst>()->setKeyLoc(locA);
    }
|   FINAL
    {
        DECL_1_LOC(@1);
        $$ = newAst<DeclAttrAst>()->setKeyLoc(locA);
    }
|   OVERRIDE
    {
        DECL_1_LOC(@1);
        $$ = newAst<DeclAttrAst>()->setKeyLoc(locA);
    }
|   AUTO
    {
        DECL_1_LOC(@1);
        $$ = newAst<AutoAttrAst>()->setKeyLoc(locA);
    }
|   __GSHARED
    {
        DECL_1_LOC(@1);
        $$ = newAst<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   IN
    {
        DECL_1_LOC(@1);
        $$ = newAst<ParamDirAttrAst>()->setKeyLoc(locA);
    }
|   OUT
    {
        DECL_1_LOC(@1);
        $$ = newAst<ParamDirAttrAst>()->setKeyLoc(locA);
    }
|   LAZY
    {
        DECL_1_LOC(@1);
        $$ = newAst<EvalStrategyAttrAst>()->setKeyLoc(locA);
    }
;

StorageClass:
    /* Enum as a manifest constant is matched by EnumDecl. */
    DEPRECATED
    {
        DECL_1_LOC(@1);
        $$ = newAst<DeclAttrAst>()->setKeyLoc(locA);
    }
|   STATIC
    {
        DECL_1_LOC(@1);
        $$ = newAst<StorageClassAttrAst>()->setKeyLoc(locA);
    }
|   EXTERN
    {
        DECL_1_LOC(@1);
        $$ = newAst<LinkageAttrAst>()->setKeyLoc(locA);
    }
|   REF
    {
        DECL_1_LOC(@1);
        $$ = newAst<StorageClassAttrAst>()->setKeyLoc(locA);
    }
|   SCOPE
    {
        DECL_1_LOC(@1);
        $$ = newAst<StorageClassAttrAst>()->setKeyLoc(locA);
    }
    /* TODO: Handle SYNCHRONIZED for this situation. */
;

AccessSpec:
    PRIVATE
    {
        DECL_1_LOC(@1);
        $$ = newAst<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   PACKAGE
    {
        DECL_1_LOC(@1);
        $$ = newAst<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   PROTECTED
    {
        DECL_1_LOC(@1);
        $$ = newAst<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   PUBLIC
    {
        DECL_1_LOC(@1);
        $$ = newAst<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   EXPORT
    {
        DECL_1_LOC(@1);
        $$ = newAst<VisibilityAttrAst>()->setKeyLoc(locA);
    }
;

Annot:
    '@' PROPERTY
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' SAFE
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' TRUSTED
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' SYSTEM
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' DISABLE
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' NOGC
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
;

LinkageAttr:
    EXTERN '(' IdentList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto expr = newAst<IdentExprAst>()->setName($3);
        $$ = newAst<CodegenAttrAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr(expr)->setRDelimLoc(locC);
    }
|   EXTERN '(' Ident "++" ',' IdentList ')' /* For C++ */
    {
        IGNORE_FOR_NOW($3); // Handle C++

        DECL_3_LOC(@1, @2, @7);
        auto expr = newAst<IdentExprAst>()->setName($6);
        $$ = newAst<CodegenAttrAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr(expr)->setRDelimLoc(locC);
    }
;

TypeQuals:
    TypeQual
    {
        $$ = AttrAstList::createSR($1);
    }
|   TypeQuals TypeQual
    {
        $$ = $1->handleSR($2);
    }
;

TypeQual:
    CONST
    {
        DECL_1_LOC(@1);
        $$ = newAst<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   IMMUTABLE
    {
        DECL_1_LOC(@1);
        $$ = newAst<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   INOUT
    {
        /* From D docs: "The inout forms a wildcard that stands in for
           any of mutable, const, immutable, inout, or inout const". */
        DECL_1_LOC(@1);
        $$ = newAst<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   SHARED
    {
        DECL_1_LOC(@1);
        $$ = newAst<TypeQualAttrAst>()->setKeyLoc(locA);
    }
;

AlignAttr:
    ALIGN
    {
        DECL_1_LOC(@1);
        $$ = newAst<CodegenAttrAst>()->setKeyLoc(locA);
    }
|   ALIGN '(' NumLit ')'
    {
        /* TODO: Report error if not integer literal */
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<CodegenAttrAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
;

StaticAssert:
    STATIC ASSERT '(' AssignExpr ')' ';'
    {
        DECL_5_LOC(@1, @2, @3, @5, @6);
        /* TODO: Join static and assert */
        $$ = newAst<StaticAssertDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locC)->setExpr($4)
                ->setRDelimLoc(locD)->setTerminLoc(locE);
    }
|   STATIC ASSERT '(' AssignExpr ',' AssignExpr ')' ';'
    {
        DECL_6_LOC(@1, @2, @3, @5, @7, @8);
        /* TODO: Join static and assert */
        $$ = newAst<StaticAssertDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locC)->setExpr($4)
                ->setMDelimLoc(locD)->setMessage($6)->setRDelimLoc(locE)->setTerminLoc(locF);
    }
;

VersionDecl:
    VERSION '=' Ident ';'
    {
        DECL_3_LOC(@1, @2, @4);
        auto expr = newAst<IdentExprAst>()->setName($3);
        $$ = newAst<VersionDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr(expr)->setTerminLoc(locC);
    }
|   VERSION '=' NumLit ';'
    {
        /* TODO: Report error if not integer literal */
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<VersionDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr($3)->setTerminLoc(locC);
    }
;

DebugDecl:
    DEBUG '=' Ident ';'
    {
        DECL_3_LOC(@1, @2, @4);
        auto expr = newAst<IdentExprAst>()->setName($3);
        $$ = newAst<DebugDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr(expr)->setTerminLoc(locC);
    }
|   DEBUG '=' NumLit ';'
    {
        /* TODO: Report error if not integer literal */
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<DebugDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr($3)->setTerminLoc(locC);
    }
;

UnitTestDecl:
    UNITTEST BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<UnitTestDeclAst>()->setKeyLoc(locA)->setStmt($2);
    }
;

InvariantDecl:
    INVARIANT BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<InvariantDeclAst>()->setKeyLoc(locA)->setStmt($2);
    }
|   INVARIANT '(' ')' BlockStmt
    {
        DECL_3_LOC(@1, @2, @3);
        $$ = newAst<InvariantDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setStmt($4);
    }
;

SelectiveDecl:
    ConditionDecl Decl %prec PREFER_SHIFT
    {
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = newAst<SelectiveDeclAst>()->setKeyLoc(cond->keyLoc_)
            ->setLDelimLoc(cond->lDelimLoc_)->setExpr(cond->expr_.release())
            ->setRDelimLoc(cond->rDelimLoc_)->setIfDecl($2);
    }
|   ConditionDecl Decl ELSE Decl
    {
        DECL_1_LOC(@3);
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = newAst<SelectiveDeclAst>()->setKeyLoc(cond->keyLoc_)
            ->setLDelimLoc(cond->lDelimLoc_)->setExpr(cond->expr_.release())
            ->setRDelimLoc(cond->rDelimLoc_)->setIfDecl($2)->setOtherKeyLoc(locA)
            ->setElseDecl($4);
    }
;

ConditionDecl:
    VersionCond
|   DebugCond
|   StaticIf
;

VersionCond:
    VERSION '(' NumLit ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = new detail::Condition(locA, locB, $3, locC);
    }
|   VERSION '(' Ident ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto name = newAst<IdentExprAst>()->setName($3);
        $$ = new detail::Condition(locA, locB, name, locC);
    }
|   VERSION '(' UNITTEST ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = new detail::Condition(locA, locB, nullptr, locC);
    }
|   VERSION '(' ASSERT ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = new detail::Condition(locA, locB, nullptr, locC);
    }
;

DebugCond:
    DEBUG %prec PREFER_SHIFT
    {
        DECL_1_LOC(@1);
        $$ = new detail::Condition(locA, kEmptyLoc, nullptr, kEmptyLoc);
    }
|   DEBUG '(' NumLit ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = new detail::Condition(locA, locB, $3, locC);
    }
|   DEBUG '(' Ident ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto name = newAst<IdentExprAst>()->setName($3);
        $$ = new detail::Condition(locA, locB, name, locC);
    }
;

StaticIf:
    STATIC IF '(' AssignExpr ')'
    {
        /* TODO: Join static if */
        DECL_4_LOC(@1, @2, @3, @5);
        $$ = new detail::Condition(locA, locB, $4, locD);
    }
;

AliasThis:
    ALIAS Ident THIS ';'
    {
        /* TODO: Handle `this` alias */
        DECL_3_LOC(@1, @3, @4);
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locC);
        $$ = newAst<AliasDeclAst>()->setKeyLoc(locA)->setName($2)->setSpec(inferred)->setTerminLoc(locC);
    }
;

AliasDecl:
    ALIAS AliasBindDeclList ';'
    {
        DECL_2_LOC(@1, @3);
        auto sect = newAst<SectionDeclAst>()->setKeyLoc(locA)->setDeclsSR($2)->setTerminLoc(locB);
        sect->setVariety(SectionVariety::Types);
        $$ = sect;
    }
|   ALIAS Type Ident ';'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<AliasDeclAst>()->setKeyLoc(locA)->setName($3)->setSpec($2)->setTerminLoc(locB);
    }
    /* TODO: More rules */
;

AliasBindDeclList:
    AliasBindDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   AliasBindDeclList ',' AliasBindDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

AliasBindDecl:
    Ident '=' Type
    {
        DECL_1_LOC(@2);
        $$ = newAst<AliasDeclAst>()->setName($1)->setEqLoc(locA)->setSpec($3);
    }
;

EnumDecl:
    ENUM EnumEnd
    {
        DECL_1_LOC(@1);
        $$ = EnumDecl_Cast($2)->setKeyLoc(locA);
    }
|   ENUM Ident EnumEnd
    {
        DECL_1_LOC(@1);
        $$ = EnumDecl_Cast($3)->setKeyLoc(locA)->setName($2);
    }
|   ENUM Ident ':' Type EnumEnd
    {
        DECL_2_LOC(@1, @3);
        $$ = EnumDecl_Cast($5)->setKeyLoc(locA)->setName($2)->setSepLoc(locB)->setSpec($4);
    }
|   ENUM ':' Type EnumEnd
    {
        DECL_2_LOC(@1, @2);
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        $$ = EnumDecl_Cast($4)->setKeyLoc(locA)->setSepLoc(locB)->setSpec($3);
    }
|   ENUM Ident '=' AssignExpr ';'
    {
        DECL_3_LOC(@1, @3, @5);
        auto inferred = newAst<InferredSpecAst>()->setKeyLoc(locA);
        auto var = newAst<VarDeclAst__<VarInit__>>();
        var->setName($2)->setAssignLoc(locB)->setInit($4);
        auto group = newAst<VarGroupDeclAst>();
        group->setKeyLoc(locA)->addDecl(var)->setSpec(inferred)->setTerminLoc(locC);
        group->setAllocScheme(AllocScheme::CompileTime);
        $$ = group;
    }
|   ENUM Type Ident '=' AssignExpr ';'
    {
        DECL_3_LOC(@1, @4, @6);
        auto var = newAst<VarDeclAst__<VarInit__>>();
        var->setName($3)->setAssignLoc(locB)->setInit($5);
        auto group = newAst<VarGroupDeclAst>();
        group->setKeyLoc(locA)->setSpec($2)->addDecl(var)->setTerminLoc(locC);
        group->setAllocScheme(AllocScheme::CompileTime);
        $$ = group;
    }
|   ENUM Type Ident RestrictTemplateParamClause '=' AssignExpr ';'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @5, @7);
        auto var = newAst<VarDeclAst__<VarInit__>>();
        var->setName($3)->setAssignLoc(locB)->setInit($6);
        auto group = newAst<VarGroupDeclAst>();
        group->setKeyLoc(locA)->setSpec($2)->addDecl(var)->setTerminLoc(locC);
        group->setAllocScheme(AllocScheme::CompileTime);
        $$ = group;
    }
;

EnumEnd:
    ';'
    {
        DECL_1_LOC(@1);
        $$ = newAst<EnumDeclAst>()->setTerminLoc(locA);
    }
|   EnumBody
;

EnumBody:
    '{' EnumMemberDeclList '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<EnumDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '{' EnumMemberDeclList ',' '}'
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<EnumDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
;

EnumMemberDeclList:
    EnumMemberDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   EnumMemberDeclList ',' EnumMemberDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

EnumMemberDecl:
    Ident
    {
        $$ = newAst<EnumMemberDeclAst>()->setName($1);
    }
|   Ident '=' AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = newAst<EnumMemberDeclAst>()->setName($1)->setAssignLoc(locA)->setInit($3);
    }
|   Type Ident '=' AssignExpr
    {
        IGNORE_FOR_NOW($1);

        DECL_1_LOC(@3);
        $$ = newAst<EnumMemberDeclAst>()->setName($2)->setAssignLoc(locA)->setInit($4);
    }
;

AttrRecordDecl:
    /* TODO: Report error if not class or struct */
    Attrs RecordDecl
    {
        IGNORE_LIST_FOR_NOW($1);

        $$ = $2;
    }
|   SYNCHRONIZED RecordDecl
    {
        $$ = $2;
    }
;

RecordDecl:
    RecordKey Ident ';'
    {
        DECL_1_LOC(@3);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto fwd = newAst<ForwardDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setTerminLoc(locA);
        fwd->setVariety(RecordVariety(data->variety_));
        $$ = fwd;
    }
|   RecordKey Ident RecordType
    {
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($3);
        RecordSpec_Cast($3)->setVariety(RecordVariety(data->variety_));
        $$ = record;
    }
|   RecordKey Ident ':' BaseRecordList RecordType
    {
        /* TODO: Report error if not class or interface */
        DECL_1_LOC(@3);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($5);
        RecordSpec_Cast($5)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($4);
        $$ = record;
    }
|   RecordKey RecordType
    {
        /* TODO: Report error if not union or struct */
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setSpec($2);
        RecordSpec_Cast($2)->setVariety(RecordVariety(data->variety_));
        $$ = record;
    }
|   RecordTemplateDecl
;

RecordTemplateDecl:
    RecordKey Ident TemplateParamClause RecordType
    {
        IGNORE_FOR_NOW($3);

        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($4);
        RecordSpec_Cast($4)->setVariety(RecordVariety(data->variety_));
        $$ = record;
    }
|   RecordKey Ident TemplateParamClause Constraint RecordType
    {
        IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($4);

        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($5);
        RecordSpec_Cast($5)->setVariety(RecordVariety(data->variety_));
        $$ = record;
    }
    /* TODO: Para class/interface apenas. */
|   RecordKey Ident TemplateParamClause Constraint ':' BaseRecordList RecordType
    {
        IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($4);

        DECL_1_LOC(@5);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($7);
        RecordSpec_Cast($7)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($6);
        $$ = record;
    }
|   RecordKey Ident TemplateParamClause ':' BaseRecordList RecordType
    {
        IGNORE_FOR_NOW($3);

        DECL_1_LOC(@4);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($6);
        RecordSpec_Cast($6)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($5);
        $$ = record;
    }
|   RecordKey Ident TemplateParamClause ':' BaseRecordList Constraint RecordType
    {
        IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($6);

        DECL_1_LOC(@4);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = newAst<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($7);
        RecordSpec_Cast($7)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($5);
        $$ = record;
    }
;

TemplateDecl:
    TEMPLATE Ident TemplateParamClause '{' '}'
    {
        IGNORE_FOR_NOW($3);

        DECL_3_LOC(@1, @4, @5);
        $$ = newAst<TemplateDeclAst>()->setKeyLoc(locA)->setName($2);
    }
|   TEMPLATE Ident TemplateParamClause '{' Decls '}'
    {
        IGNORE_FOR_NOW($3); IGNORE_LIST_FOR_NOW($5);

        DECL_3_LOC(@1, @4, @6);
        $$ = newAst<TemplateDeclAst>()->setKeyLoc(locA)->setName($2);
    }
|   TEMPLATE Ident TemplateParamClause Constraint '{' Decls '}'
    {
        IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        DECL_3_LOC(@1, @5, @7);
        $$ = newAst<TemplateDeclAst>()->setKeyLoc(locA)->setName($2);
    }
;

RecordKey:
    UNION
    {
        DECL_1_LOC(@1);
        $$ = new detail::RecordKeyVariety(locA, static_cast<int>(RecordVariety::Union));
    }
|   STRUCT
    {
        DECL_1_LOC(@1);
        $$ = new detail::RecordKeyVariety(locA, static_cast<int>(RecordVariety::Struct));
    }
|   CLASS
    {
        DECL_1_LOC(@1);
        $$ = new detail::RecordKeyVariety(locA, static_cast<int>(RecordVariety::Class));
    }
|   INTERFACE
    {
        DECL_1_LOC(@1);
        $$ = new detail::RecordKeyVariety(locA, static_cast<int>(RecordVariety::Interface));
    }
;

BaseRecordList:
    BaseRecordDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   BaseRecordList ',' BaseRecordDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

BaseRecordDecl:
    NestedIdentOrTemplateInst
    {
        auto name = newAst<NestedNameAst>()->setNamesSR($1);
        $$ = newAst<BaseDeclAst>()->setName(name);
    }
;

RecordType:
    '{' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<RecordSpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' Decls '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<RecordSpecAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '{' Decls EOP
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<RecordSpecAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   '{' Decls error BlockStmtSync
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<RecordSpecAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
;

ImportGroupDecl:
    IMPORT ImportList ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ImportGroupDeclAst>()->setKeyLoc(locA)
            ->setModulesSR($2)->setTerminLoc(locB);
    }
|   STATIC IMPORT ImportList ';'
    {
        /* TODO: Take care of static */
        DECL_2_LOC(@2, @4);
        $$ = newAst<ImportGroupDeclAst>()->setKeyLoc(locA)
            ->setModulesSR($3)->setTerminLoc(locB);
    }
|   PUBLIC IMPORT ImportList ';'
    {
        /* TODO: Take care of public */
        DECL_2_LOC(@2, @4);
        $$ = newAst<ImportGroupDeclAst>()->setKeyLoc(locA)
            ->setModulesSR($3)->setTerminLoc(locB);
    }
;

ImportDecl:
    NestedIdent
    {
        auto name = newAst<IdentExprAst>()->setName($1);
        $$ = newAst<ImportDeclAst>()->setTarget(name);
    }
|   Ident '=' NestedIdent
    {
        DECL_1_LOC(@2);
        auto name = newAst<IdentExprAst>()->setName($3);
        $$ = newAst<ImportDeclAst>()->setLocalName($1)->setAsLoc(locA)
            ->setTarget(name);
    }
|   Ident '=' NestedIdent ':' ImportSelectionDeclList %prec PREFER_SHIFT
    {
        /* Once inside a selective import, append to its binding list,
           not to the import list itself. */
        DECL_2_LOC(@2, @4);
        auto name = newAst<IdentExprAst>()->setName($3);
        $$ = newAst<ImportDeclAst>()->setLocalName($1)->setAsLoc(locA)
            ->setTarget(name)->setLDelimLoc(locB)->setSelectionsSR($5);
    }
|   NestedIdent ':' ImportSelectionDeclList %prec PREFER_SHIFT /* See comment above */
    {
        DECL_1_LOC(@2);
        auto name = newAst<IdentExprAst>()->setName($1);
        $$ = newAst<ImportDeclAst>()->setTarget(name)->setLDelimLoc(locA)
            ->setSelectionsSR($3);
    }
;

ImportSelectionDecl:
    NestedIdent
    {
        // TODO: Report error if not Ident (cannot be nested).
        $$ = newAst<ImportSelectionDeclAst>()
            ->setActualName(newAst<NestedNameAst>()->setNamesSR($1));
    }
|   Ident '=' NestedIdent
    {
        DECL_1_LOC(@2);
        $$ = newAst<ImportSelectionDeclAst>()->setAlternateName($1)->setAsLoc(locA)
            ->setActualName(newAst<NestedNameAst>()->setNamesSR($3));
    }
;

ImportSelectionDeclList:
    ImportSelectionDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   ImportSelectionDeclList ',' ImportSelectionDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

ImportList:
    ImportDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   ImportList ',' ImportDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;


    /*------------------*/
    /*--- Statements ---*/
    /*------------------*/

Stmt:
    // TODO: CaseRangeStmt
    BlockStmt
|   ExprStmt %dprec 1
|   BasicDecl %dprec 2 /* Prefer declarations to expressions. */
    {
        $$ = newAst<DeclStmtAst>()->setDecl($1);
    }
|   PlainStmt
|   CaseClauseStmt
;

PlainStmt:
    LabeledStmt
|   IfStmt
|   WhileStmt
|   DoStmt
|   ForStmt
|   ForeachStmt
|   ContinueStmt
|   BreakStmt
|   ReturnStmt
|   WithStmt
|   GotoStmt
|   SynchronizedStmt
|   TryStmt
|   ThrowStmt
|   DeferredStmt
|   StaticAssert
    {
        $$ = newAst<DeclStmtAst>()->setDecl($1);
    }
|   CondStmt
|   SwitchStmt
;

StmtList:
    Stmt
    {
        $$ = StmtAstList::createSR($1);
    }
|   StmtList Stmt
    {
        $$ = $1->handleSR($2);
    }
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
|   '{' StmtList EOP
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   '{' StmtList error BlockStmtSync
    {
        DECL_2_LOC(@1, @4);
        $$ = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        yyerrok;
    }
;

BlockStmtSync: '}' | EOP;

FuncEnd:
    ';'
    {
        DECL_1_LOC(@1);
        $$ = newAst<EmptyStmtAst>()->setKeyLoc(locA);
    }
|   FuncBody
;

FuncBody:
    /* We use a lexing hack to join `in {` in order to avoid conflicts
       with the `in` expressions. */
    BlockStmt
|   IN_LBRACE_HACK '}' BODY BlockStmt
    {
        DECL_3_LOC(@1, @2, @3);
        auto block = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        auto in = newAst<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = newAst<BodyStmtAst>()->setBodyLoc(locC)->setBlock($4);
        $$ = newAst<ContractStmtAst>()->setStmt1(in)->setStmt2(body);
    }
|   IN_LBRACE_HACK StmtList '}' BODY BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto block = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        auto in = newAst<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = newAst<BodyStmtAst>()->setBodyLoc(locC)->setBlock($5);
        $$ = newAst<ContractStmtAst>()->setStmt1(in)->setStmt2(body);
    }
|   IN_LBRACE_HACK '}' FuncOutStmt BODY BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        auto block = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        auto in = newAst<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = newAst<BodyStmtAst>()->setBodyLoc(locC)->setBlock($5);
        $$ = newAst<ContractStmtAst>()->setStmt1(in)->setStmt2(body)->setStmt3($3);
    }
|   IN_LBRACE_HACK StmtList '}' FuncOutStmt BODY BlockStmt
    {
        DECL_3_LOC(@1, @3, @5);
        auto block = newAst<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        auto in = newAst<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = newAst<BodyStmtAst>()->setBodyLoc(locC)->setBlock($6);
        $$ = newAst<ContractStmtAst>()->setStmt1(in)->setStmt2(body)->setStmt3($4);
    }
|   FuncOutStmt BODY BlockStmt
    {
        DECL_1_LOC(@2);
        auto body = newAst<BodyStmtAst>()->setBodyLoc(locA)->setBlock($3);
        $$ = newAst<ContractStmtAst>()->setStmt1($1)->setStmt2(body);
    }
;

FuncOutStmt:
    OUT BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<OutStmtAst>()->setOutLoc(locA)->setBlock($2);
    }
|   OUT '(' Ident ')' BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<OutStmtAst>()->setOutLoc(locA)->setLDelimLoc(locB)->setName($3)
                ->setRDelimLoc(locC)->setBlock($5);
    }
;

LabeledStmt:
    Ident ':' Stmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<LabeledStmtAst>()->setLabel($1)->setDelimLoc(locA)->setStmt($3);
    }
;

ExprStmt:
    Expr ';'
    {
        DECL_1_LOC(@1);
        $$ = newAst<ExprStmtAst>()->addExpr($1)->setTerminLoc(locA);
    }
|   Expr EOP
    {
        DECL_1_LOC(@1);
        $$ = newAst<ExprStmtAst>()->addExpr($1)->setTerminLoc(locA);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   Expr error ExprStmtSync
    {
        DECL_1_LOC(@1);
        $$ = newAst<ExprStmtAst>()->addExpr($1)->setTerminLoc(locA);
        yyerror(&yylloc, scanner, context, "unexpected token");
        yyerrok;
    }
;

ExprStmtSync: ';' | EOP;

IfStmt:
    IF '(' Expr ')' Stmt %prec PREFER_SHIFT /* For the dangling else */
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setThen($5);
    }
|   IF '(' Expr ')' Stmt ELSE Stmt
    {
        DECL_4_LOC(@1, @2, @4, @6);
        $$ = newAst<IfStmtAst>()->setIfLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setThen($5)->setElseLoc(locD)->setNotThen($7);
    }
;

WhileStmt:
    WHILE '(' Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<WhileStmtAst>()->setWhileLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt($5);
    }
;

DoStmt:
    DO Stmt WHILE '(' Expr ')'
    {
        DECL_4_LOC(@1, @3, @4, @6);
        $$ = newAst<DoWhileStmtAst>()->setDoLoc(locA)->setStmt($2)->setWhileLoc(locB)
                ->setLDelimLoc(locC)->setExpr($5)->setRDelimLoc(locD);
    }
;

ForStmt:
    FOR '(' Stmt ';' ')' Stmt
    {
        DECL_4_LOC(@1, @2, @3, @4);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setDelimLoc(locC)->setRDelimLoc(locD)->setStmt($6);
    }
|   FOR '(' Stmt Expr ';' ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @6);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setCond($4)->setDelimLoc(locC)->setRDelimLoc(locD)->setStmt($7);
    }
|   FOR '(' Stmt Expr ';' Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @7);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setCond($4)->setDelimLoc(locC)->setPost($6)->setRDelimLoc(locD)
                ->setStmt($8);
    }
|   FOR '(' Stmt ';' Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @4, @6);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setDelimLoc(locC)->setPost($5)->setRDelimLoc(locD)->setStmt($7);
    }
|   FOR '(' ';' ';' ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @4, @5);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setDelimLoc(locD)->setRDelimLoc(locE)->setStmt($6);
    }
|   FOR '(' ';' Expr ';' ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @5, @6);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setCond($4)->setDelimLoc(locC)->setRDelimLoc(locD)->setStmt($7);
    }
|   FOR '(' ';' Expr ';' Expr ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @5, @7);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setCond($4)->setDelimLoc(locC)->setPost($6)->setRDelimLoc(locD)
                ->setStmt($8);
    }
|   FOR '(' ';' ';' Expr ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @4, @6);
        auto stmt = newAst<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = newAst<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setDelimLoc(locC)->setPost($5)->setRDelimLoc(locD)->setStmt($7);
    }
;

ForeachStmt:
    FOREACH '(' ForeachDecl Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @5);
        auto range = newAst<SubrangeExprAst>()->setLow($4);
        $$ = newAst<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locC)->setStmt($6);
    }
|   FOREACH '(' ForeachDecl Expr ".." Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @7);
        auto range = newAst<SubrangeExprAst>()->setLow($4)->setDelim1Loc(locC)->setHi($6);
        $$ = newAst<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locC)->setStmt($8);
    }
|   FOREACH_REVERSE '(' ForeachDecl Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @5);
        auto range = newAst<SubrangeExprAst>()->setLow($4);
        $$ = newAst<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locC)->setStmt($6);
    }
|   FOREACH_REVERSE '(' ForeachDecl Expr ".." Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @7);
        auto range = newAst<SubrangeExprAst>()->setLow($4)->setDelim1Loc(locC)->setHi($6);
        $$ = newAst<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locD)->setStmt($8);
    }
;

ForeachDecl:
    VarGroupDecl
|   IdentList ';'
    {
        DECL_1_LOC(@2);
        std::unique_ptr<NameAstList> l($1->finishSR());
        auto group = newAst<VarGroupDeclAst>()->setSpec(newAst<InferredSpecAst>());
        while (l) {
            auto p = std::move(l->detachHead());
            auto var = VarDeclAst::create();
            var->setName(std::move(p.first));
            group->decls_ ? group->decls_->pushBack(std::move(var)) :
                            (void)(group->decls_ = DeclAstList::create(std::move(var)));
            l = std::move(p.second);
        }
        $$ = group;
    }
;

ContinueStmt:
    CONTINUE ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<ContinueStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   CONTINUE Ident ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ContinueStmtAst>()->setKeyLoc(locA)->setName($2)->setTerminLoc(locB);
    }
;

BreakStmt:
    BREAK ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<BreakStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   BREAK Ident ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<BreakStmtAst>()->setKeyLoc(locA)->setName($2)->setTerminLoc(locB);
    }
;

ReturnStmt:
    RETURN ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<ReturnStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   RETURN Expr ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ReturnStmtAst>()->setKeyLoc(locA)->setExprsSR(ExprAstList::createSR($2))
                ->setTerminLoc(locB);
    }
;

GotoStmt:
    GOTO Ident ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<GotoStmtAst>()->setKeyLoc(locA)->setName($2)->setTerminLoc(locB);
    }
|   GOTO DEFAULT ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<GotoStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   GOTO CASE ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<GotoStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   GOTO CASE Expr ';'
    {
        IGNORE_FOR_NOW($3);

        DECL_2_LOC(@1, @3);
        $$ = newAst<GotoStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
;

WithStmt:
    /* Expr will match `Symbol` and `TemplateInst` as `PrimaryExpr` for the
       as the other versions of the with statement. */
    WITH '(' Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<WithStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)
                ->addExpr($3)->setRDelimLoc(locC)->setStmt($5);
    }
;

SynchronizedStmt:
    /* There is an ambiguity in the following statements, as discussed
       at http://www.digitalmars.com/d/archives/digitalmars/D/bugs/3833.html */
    SYNCHRONIZED '{' StmtList '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto block = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($3)->setRDelimLoc(locC);
        $$ = newAst<SyncedStmtAst>()->setKeyLoc(locA)->setStmt(block);
    }
|   SYNCHRONIZED PlainStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<SyncedStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
|   SYNCHRONIZED '(' Expr ')' '{' StmtList '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto block = newAst<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($6)->setRDelimLoc(locC);
        $$ = newAst<SyncedStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt(block);
    }
|   SYNCHRONIZED '(' Expr ')' PlainStmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<SyncedStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt($5);
    }
;

TryStmt:
    TRY BlockStmt CatchClauseStmts
    {
        DECL_1_LOC(@1);
        $$ = newAst<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setCatchsSR($3);
    }
|   TRY BlockStmt LastCatchClauseStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->addCatch($3);
    }
|   TRY BlockStmt CatchClauseStmts LastCatchClauseStmt
    {
        DECL_1_LOC(@1);
        auto catchs = $3->handleSR($4);
        $$ = newAst<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setCatchsSR(catchs);
    }
|   TRY BlockStmt CatchClauseStmts LastCatchClauseStmt FinallyClauseStmt
    {
        DECL_1_LOC(@1);
        auto catchs = $3->handleSR($4);
        $$ = newAst<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setCatchsSR(catchs)->setFinal($5);
    }
|   TRY BlockStmt LastCatchClauseStmt FinallyClauseStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->addCatch($3)->setFinal($4);
    }
|   TRY BlockStmt FinallyClauseStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setFinal($3);
    }
;

CatchClauseStmts:
    CatchClauseStmt
    {
        $$ = StmtAstList::createSR($1);
    }
|   CatchClauseStmts CatchClauseStmt
    {
        $$ = $1->handleSR($2);
    }
;

CatchClauseStmt:
    CATCH '(' ParamGroupDecl ')' BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<CatchClauseStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)
            ->setDecl($3)->setRDelimLoc(locC)->setStmt($5);
    }
;

LastCatchClauseStmt:
    CATCH BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<CatchClauseStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
;

FinallyClauseStmt:
    FINALLY BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = newAst<FinallyClauseStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
;

ThrowStmt:
    THROW Expr ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<ThrowStmtAst>()->setKeyLoc(locA)->setExpr($2)->setTerminLoc(locB);
    }
;

DeferredStmt:
    SCOPE '(' Ident ')' Stmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<DeferredStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)
                ->setName($3)->setRDelimLoc(locC)->setStmt($5);
    }
;

CondStmt:
    ConditionDecl Stmt %prec PREFER_SHIFT
    {
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = newAst<SelectiveStmtAst>()->setKeyLoc(cond->keyLoc_)
            ->setLDelimLoc(cond->lDelimLoc_)->setExpr(cond->expr_.release())
            ->setRDelimLoc(cond->rDelimLoc_)->setIfStmt($2);
    }
|   ConditionDecl Stmt ELSE Stmt
    {
        DECL_1_LOC(@3);
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = newAst<SelectiveStmtAst>()->setKeyLoc(cond->keyLoc_)
            ->setLDelimLoc(cond->lDelimLoc_)->setExpr(cond->expr_.release())
            ->setRDelimLoc(cond->rDelimLoc_)->setIfStmt($2)->setOtherKeyLoc(locA)
            ->setElseStmt($4);
    }
;

SwitchStmt:
    SWITCH '(' Expr ')' BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = newAst<SwitchStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt($5);
    }
;

CaseClauseStmt:
    CASE ExprList ':' Stmt
    {
        DECL_2_LOC(@1, @3);
        $$ = newAst<CaseClauseStmtAst>()->setKeyLoc(locA)->setExprsSR($2)->setDelimLoc(locB)
                ->addStmt($4);
    }
|   DEFAULT ':' Stmt
    {
        DECL_2_LOC(@1, @2);
        $$ = newAst<DefaultClauseStmtAst>()->setKeyLoc(locA)->setDelimLoc(locB)->addStmt($3);
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

TemplateInst:
    Ident '!' '(' ')'
    {
        $$ = $1;
    }
|   Ident '!' '(' TemplateArgList ')'
    {
        IGNORE_LIST_FOR_NOW($4);

        $$ = $1;
    }
|   Ident '!' '(' TemplateArgList ',' ')'
    {
        IGNORE_LIST_FOR_NOW($4);

        $$ = $1;
    }
|   Ident '!' TemplateSingleArg
    {
        IGNORE_FOR_NOW($3);

        $$ = $1;
    }
;

IdentOrTemplateInst:
    Ident %prec PREFER_SHIFT /* Allow a tuple (T...) to be shifted. */
|   TemplateInst
;

NestedIdent:
    Ident
    {
        $$ = NameAstList::createSR($1);
    }
|   NestedIdent '.' Ident
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

NestedIdentOrTemplateInst:
    IdentOrTemplateInst
    {
        $$ = NameAstList::createSR($1);
    }
|   NestedIdentOrTemplateInst '.' IdentOrTemplateInst
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

IdentList:
    Ident
    {
        $$ = NameAstList::createSR($1);
    }
|   IdentList ',' Ident
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
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

PointerLit:
    NULL_VALUE
    {
        DECL_1_LOC(@1);
        $$ = newAst<NullLitExprAst>()->setLitLoc(locA);
    }
;

ArrayLit:
    ArrayInit
;

FuncLit:
    FuncKey FuncBody
    {
        /* TODO */ IGNORE_FOR_NOW($2);

        DECL_1_LOC(@1);
        $$ = newAst<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   FuncKey Type FuncBody
    {
        /* TODO */ IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($3);

        DECL_1_LOC(@1);
        $$ = newAst<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   FuncKey Type ParamClauseDecl FuncBody
    {
        /* TODO */
        IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($4);

        DECL_1_LOC(@1);
        $$ = newAst<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   FuncKey ParamClauseDecl FuncBody
    {
        /* TODO */ IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($3);

        DECL_1_LOC(@1);
        $$ = newAst<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   ParamClauseDecl FuncBody
    {
        /* TODO */ IGNORE_FOR_NOW($1); IGNORE_FOR_NOW($2);

        DECL_1_LOC(@1);
        $$ = newAst<VoidInitExprAst>()->setKeyLoc(locA);
    }
;

FuncKey:
    FUNCTION
|   DELEGATE
;

SpecialKeyword:
    __FILE__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __MODULE__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __LINE__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __FUNCTION__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __PRETTY_FUNCTION__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __TIME__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __DATE__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __TIMESTAMP__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __VERSION__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
|   __VENDOR__MACRO
    {
        DECL_1_LOC(@1);
        $$ = newAst<StrLitExprAst>()->setLitLoc(locA);
    }
;


    /*------------*/
    /*--- Misc ---*/
    /*------------*/

TemplateArgList:
    TemplateArg
    {
        $$ = TemplateArgAstList::createSR($1);
    }
|   TemplateArgList ',' TemplateArg
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

TemplateArg:
    NonExprType
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Type);
        $$ = arg;
    }
|   AssignExpr
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
;

TemplateSingleArg:
    Ident
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::NameToLookup);
        $$ = arg;
    }
|   BuiltinType
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Type);
        $$ = arg;
    }
|   CharLit
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   StringLit
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   NumLit
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   BoolLit
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   PointerLit
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   ThisExpr
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   SpecialKeyword
    {
        auto arg = newAst<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
;
%%
