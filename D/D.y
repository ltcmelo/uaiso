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
%type <decl_> UnitTestDecl Import ImportDecl ImportBindDecl VersionDecl
%type <decl_> StaticAssert DebugDecl RestrictTemplateParamClause
%type <decl_> TemplateDecl TemplateParamClause RestrictTemplateParam
%type <decl_> TemplateTypeParam TemplateValueParam TemplateAliasParam TemplateTupleParam
%type <decl_> TemplateParam Constraint ForeachDecl TemplateThisParam
%type <decls_> BaseRecordList ParamGroupDeclList TemplateParamList AliasBindDeclList VarDeclList
%type <decls_> RestrictTemplateParamList Decls EnumMemberDeclList ImportList ImportBindList

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
        $$ = makeAstRaw<CommaExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
;

AssignExpr:
    CondExpr
|   CondExpr '=' AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "+=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "-=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "*=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "/=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "%=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "|=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "^=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "^^=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "&=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "<<=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr ">>=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr ">>>=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
|   CondExpr "~=" AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AssignExprAst>()->addExpr1($1)->setOprLoc(locA)->addExpr2($3);
    }
;

CondExpr:
    BinaryExpr
|   BinaryExpr '?' Expr ':' CondExpr
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<CondExprAst>()->setCond($1)->setQuestionLoc(locA)->setYes($3)
                ->setDelimLoc(locB)->setNo($5);
    }
;

BinaryExpr:
    UnaryExpr
|   BinaryExpr "||" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<LogicOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "&&" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<LogicAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '|' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<BitOrExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '^' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<BitXorExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '&' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<BitAndExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "==" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<EqExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '<' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '>' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr ">=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<>=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<>=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!<=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "!>=" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<RelExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr IS BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<IsExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr NOT_IS_HACK BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<IsExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr IN BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<InExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr NOT_IN_HACK BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<InExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "<<" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr ">>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr ">>>" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ShiftExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '+' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<AddExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '-' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<SubExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '~' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ConcatExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '*' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<MulExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '/' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<DivExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr '%' BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<ModExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
    }
|   BinaryExpr "^^" BinaryExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<PowerExprAst>()->setExpr1($1)->setOprLoc(locA)->setExpr2($3);
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
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BitCompExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   "++" UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<IncDecExprAst>()->setPrefixLoc(locA)->setValue($2);
    }
|   "--" UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<IncDecExprAst>()->setPrefixLoc(locA)->setValue($2);
    }
|   DELETE UnaryExpr
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<DelExprAst>()->setOprLoc(locA)->setExpr($2);
    }
|   CAST '(' Type ')' UnaryExpr
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<CastExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setSpec($3)
                ->setRDelimLoc(locC)->setExpr($5);
    }
|   CAST '(' ')' UnaryExpr
    {
        /* This case removes any top level type modifiers. */
        DECL_3_LOC(@1, @2, @3);
        $$ = makeAstRaw<CastExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setExpr($4);
    }
    /*    |   CAST '(' CastQual ')' UnaryExpr */
|   '(' Type ')' '.' NestedIdentOrTemplateInst %dprec 1
    {
        DECL_3_LOC(@1, @3, @4);
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($5);
        $$ = makeAstRaw<MemberAccessExprAst>()->setSpec($2)->setOprLoc(locC)->setName(name);
    }
|   PostfixExpr %dprec 2
;

PostfixExpr:
    PrimaryExpr
|   PostfixExpr "++"
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   PostfixExpr "--"
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<IncDecExprAst>()->setValue($1)->setSuffixLoc(locA);
    }
|   PostfixExpr '.' IdentOrTemplateInst
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<MemberAccessExprAst>()->setExpr($1)->setOprLoc(locA)->setName($3);
    }
|   PostfixExpr '.' NewExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<NestedNewExprAst>()->setBase($1)->setOprLoc(locA)->setNestedNew($3);
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
        DECL_2_LOC(@2, @5);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
        yyerrok;
    }
|   PostfixExpr '(' ExprList EOP
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<CallExprAst>()->setBase($1)->setLDelimLoc(locA)->setArgsSR($3)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   PostfixExpr '[' AssignExpr ']'
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' AssignExpr error PostfixExprSync
    {
        DECL_2_LOC(@2, @5);
        $$ = makeAstRaw<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
        yyerrok;
    }
|   PostfixExpr '[' AssignExpr EOP
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<ArrayIndexExprAst>()->setBase($1)->setLDelimLoc(locA)->setIndex($3)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   PostfixExpr '[' ']'
    {
        /* This expr creates a slice as large as the array. */
        DECL_2_LOC(@2, @3);
        $$ = makeAstRaw<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   PostfixExpr '[' AssignExpr ".." AssignExpr ']'
    {
        DECL_3_LOC(@2, @4, @6);
        auto range = makeAstRaw<SubrangeExprAst>()->setLow($3)->setDelim1Loc(locB)->setHi($5);
        $$ = makeAstRaw<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange(range)
                ->setRDelimLoc(locC);
    }
|   PostfixExpr '[' AssignExpr ".." AssignExpr error PostfixExprSync
    {
        DECL_3_LOC(@2, @4, @7);
        auto range = makeAstRaw<SubrangeExprAst>()->setLow($3)->setDelim1Loc(locB)->setHi($5);
        $$ = makeAstRaw<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange(range)
                ->setRDelimLoc(locC);
        yyerrok;
    }
|   PostfixExpr '[' AssignExpr ".." AssignExpr EOP
    {
        DECL_3_LOC(@2, @4, @6);
        auto range = makeAstRaw<SubrangeExprAst>()->setLow($3)->setDelim1Loc(locB)->setHi($5);
        $$ = makeAstRaw<ArraySliceExprAst>()->setBase($1)->setLDelimLoc(locA)->setRange(range)
                ->setRDelimLoc(locC);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
;

PostfixExprSync: ']' | ')' | EOP;

PrimaryExpr:
    IdentOrTemplateInst
    {
        $$ = makeAstRaw<IdentExprAst>()->setName($1);
    }
|   '.' IdentOrTemplateInst
    {
        DECL_1_LOC(@1);
        context->trackLexeme<Ident>(".", locA.fileName_.c_str(), locA.lineCol());
        auto dot = makeAstRaw<GenNameAst>()->setGenLoc(locA);
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR(NameAstList::createSR(dot)->handleSR($2));
        $$ = makeAstRaw<IdentExprAst>()->setName(name);
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
        $$ = makeAstRaw<MemberAccessExprAst>()->setSpec($1)->setOprLoc(locA)->setName($3);
    }
|   '$'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ArrayLengthExprAst>()->setKeyLoc(locA);
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
        $$ = makeAstRaw<WrappedExprAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locB);
    }
;

ThisExpr:
    THIS
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ThisExprAst>()->setKeyLoc(locA);
    }
;

SuperExpr:
    SUPER
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<SuperExprAst>()->setKeyLoc(locA);
    }
;

NewExpr:
    /* Empty parenthesized arguments and trailing semi-colons
       don't go into the AST. */
    NEW Type %prec PREFER_SHIFT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setSpec($2);
    }
|   NEW Type '(' ')'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setSpec($2);
    }
|   NEW Type '(' ExprList ')'
    {
        DECL_3_LOC(@1, @3, @5);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setSpec($2)->setLArgDelimLoc(locB)
                ->setArgsSR($4)->setRArgDelimLoc(locC);
    }
|   NEW '(' ')' Type %prec PREFER_SHIFT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setSpec($4);
    }
|   NEW '(' ExprList ')' Type %prec PREFER_SHIFT
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($5);
    }
|   NEW '(' ExprList ',' ')' Type %prec PREFER_SHIFT
    {
        DECL_3_LOC(@1, @2, @5);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($6);
    }
|   NEW '(' ')' Type '(' ')'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setSpec($4);
    }
|   NEW '(' ExprList ')' Type '(' ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($5);
    }
|   NEW '(' ExprList ',' ')' Type '(' ')'
    {
        DECL_3_LOC(@1, @2, @5);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
                ->setRAllocDelimLoc(locC)->setSpec($6);
    }
|   NEW '(' ')' Type '(' ExprList ')'
    {
        DECL_3_LOC(@1, @5, @6);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setSpec($4)->setLArgDelimLoc(locB)
                ->setArgsSR($6)->setRArgDelimLoc(locC);
    }
|   NEW '(' ExprList ')' Type '(' ExprList ')'
    {
        DECL_5_LOC(@1, @2, @4, @6, @7);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
            ->setRAllocDelimLoc(locC)->setSpec($5)->setLArgDelimLoc(locD)->setArgsSR($7)->setRArgDelimLoc(locE);
    }
|   NEW '(' ExprList ',' ')' Type '(' ExprList ')'
    {
        DECL_5_LOC(@1, @2, @5, @6, @8);
        $$ = makeAstRaw<NewExprAst>()->setKeyLoc(locA)->setLAllocDelimLoc(locB)->setAllocArgsSR($3)
            ->setRAllocDelimLoc(locC)->setSpec($6)->setLArgDelimLoc(locD)->setArgsSR($8)->setRArgDelimLoc(locE);
    }
;

TypeidExpr:
    TYPEID '(' NonExprType ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<TypeidExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setSpec($3)
                ->setRDelimLoc(locC);
    }
|   TYPEID '(' Expr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<TypeidExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC);
    }
;

AssertExpr:
    ASSERT '(' AssignExpr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<AssertExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
|   ASSERT '(' AssignExpr ',' AssignExpr ')'
    {
        DECL_4_LOC(@1, @2, @4, @6);
        $$ = makeAstRaw<AssertExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setMDelimLoc(locC)
                ->setMessage($5)->setRDelimLoc(locD);
    }
;

MixinExpr:
    MIXIN '(' AssignExpr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<MixinExprAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
;

TypeQueryExpr:
    IS '(' Type ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto intro = makeAstRaw<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type ':' IntrospectSpecialization ')'
    {
        DECL_3_LOC(@1, @2, @6);
        auto intro = makeAstRaw<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type "==" IntrospectSpecialization ')'
    {
        DECL_3_LOC(@1, @2, @6);
        auto intro = makeAstRaw<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type Ident ')'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @2, @5);
        auto intro = makeAstRaw<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type Ident ':' IntrospectSpecialization ')'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @2, @7);
        auto intro = makeAstRaw<TypeQueryExprAst>();
        intro->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setSpec($3);
        $$ = intro;
    }
|   IS '(' Type Ident "==" IntrospectSpecialization ')'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @2, @7);
        auto intro = makeAstRaw<TypeQueryExprAst>();
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
        $$ = makeAstRaw<DecoratedSpecAst>()->setAttrsSR($1)->setSpec($2);
    }
|   TypeQual '(' Type ')'
    {
        $$ = makeAstRaw<DecoratedSpecAst>()->addAttr($1)->setSpec($3);
    }
|   Attrs TypeQual '(' Type ')'
    {
        auto attrs = $1->handleSR($2);
        $$ = makeAstRaw<DecoratedSpecAst>()->setAttrsSR(attrs)->setSpec($4);
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
        $$ = makeAstRaw<DecoratedSpecAst>()->setAttrsSR($1)->setSpec($2);
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
        auto array = makeAstRaw<ArraySpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        array->setVariety(ArrayVariety::Plain);
        $$ = array;
    }
|   '[' NonExprType ']'
    {
        /* Associative arrays of unnamed types are matched
           here. The others are matched as an `AssignExpr`. */
        DECL_2_LOC(@1, @3);
        auto array = makeAstRaw<ArraySpecAst>()->setLDelimLoc(locA)->setSpec($2)->setRDelimLoc(locB);
        array->setVariety(ArrayVariety::Associative);
        $$ = array;
    }
|   '[' AssignExpr ']'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ArraySpecAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locB);
    }
|   '[' AssignExpr ".." AssignExpr ']'
    {
        // TODO: Must be a type list.
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @3, @5);
        $$ = makeAstRaw<ArraySpecAst>()->setLDelimLoc(locA)->setExpr($2)->setRDelimLoc(locC);
    }
|   '*'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<PtrSpecAst>()->setOprLoc(locA);
    }
;

Signature:
    FUNCTION ParamClauseDecl
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
    }
|   FUNCTION ParamClauseDecl FuncAttrs
    {
        DECL_1_LOC(@1);
        auto spec = makeAstRaw<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
        $$ = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
    }
|   DELEGATE ParamClauseDecl
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
    }
|   DELEGATE ParamClauseDecl FuncAttrs
    {
        DECL_1_LOC(@1);
        auto spec = makeAstRaw<FuncSpecAst>()->setKeyLoc(locA)->setParam($2);
        $$ = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
    }
;

FullType:
    UnnamedType
|   NestedIdentOrTemplateInst %prec PREFER_SHIFT
    {
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($1);
        $$ = makeAstRaw<NamedSpecAst>()->setName(name);
    }
|   '.' NestedIdentOrTemplateInst
    {
        DECL_1_LOC(@1);
        auto global = (new GenNameAst("<global>"))->setGenLoc(locA);
        auto names = NameAstList::createSR(global);
        names = names->mergeSR($2);
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR(names);
        $$ = makeAstRaw<NamedSpecAst>()->setName(name);
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
        $$ = makeAstRaw<TypeofSpecAst>()->setOprLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
|   TYPEOF '(' RETURN ')'  %prec PREFER_SHIFT
    {
        // TODO: Deal with `return`.
        DECL_4_LOC(@1, @2, @3, @4);
        $$ = makeAstRaw<TypeofSpecAst>()->setOprLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC);
    }
;

BuiltinType:
    BOOL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   BYTE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UBYTE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT16
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT16
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   INT64
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   UINT64
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   CHAR
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   CHAR_UTF16
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   CHAR_UTF32
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
|   REAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   IMAG_FLOAT32
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   IMAG_FLOAT64
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   IMAG_REAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<BuiltinSpecAst>()->setKeyLoc(locA);
    }
|   COMPLEX_FLOAT32
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
|   VOID
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<VoidSpecAst>()->setKeyLoc(locA);
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
        $$ = makeAstRaw<BlockDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   Attrs '{' Decls '}'
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<BlockDeclAst>()->setAttrsSR($1)->setLDelimLoc(locA)->setDeclsSR($3)->setRDelimLoc(locB);
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
|   ImportDecl
;

VarGroupDecl:
    Type VarDeclList ';'
    {
        DECL_1_LOC(@3);
        $$ = detail::actionVarGroupDecl($1, $2, locA);
    }
|   Type error
    {
        $$ = makeAstRaw<VarGroupDeclAst>()->setSpec($1);
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
        $$ = makeAstRaw<VarDeclAst>()->setName($1);
    }
|   Ident '=' Init
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<VarDeclAst__<VarInit__>>()->setName($1)->setAssignLoc(locA)->setInit($3);
    }
|   Ident RestrictTemplateParamClause '=' Init
    {
        IGNORE_FOR_NOW($2);

        DECL_1_LOC(@3);
        $$ = makeAstRaw<VarDeclAst__<VarInit__>>()->setName($1)->setAssignLoc(locA)->setInit($4);
    }
;

FuncDecl:
    Type Ident UnifiedParamClauseDecl FuncEnd
    {
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($4);
    }
|   Type Ident UnifiedParamClauseDecl Constraint FuncEnd
    {
        IGNORE_FOR_NOW($4);

        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($5);
    }
|   Type Ident UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName($2)->setStmt($5);
    }
|   Type Ident UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($5);

        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult($1);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($6);
    }
|   Attrs Ident UnifiedParamClauseDecl FuncEnd
    {
        DECL_1_LOC(@1);

        using namespace detail;
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        auto result = makeAstRaw<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($4);
    }
|   Attrs Ident UnifiedParamClauseDecl Constraint FuncEnd
    {
        IGNORE_FOR_NOW($4);

        DECL_1_LOC(@1);
        using namespace detail;
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        auto result = makeAstRaw<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName($2)->setStmt($5);
    }
|   Attrs Ident UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        DECL_1_LOC(@1);
        using namespace detail;
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        auto result = makeAstRaw<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName($2)->setStmt($5);
    }
|   Attrs Ident UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($5);

        DECL_1_LOC(@1);
        using namespace detail;
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        auto result = makeAstRaw<DecoratedSpecAst>()->setAttrsSR($1)->setSpec(inferred);
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())->setResult(result);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($4);
        $$ = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName($2)->setStmt($6);
    }
;

CtorDecl:
    THIS UnifiedParamClauseDecl FuncEnd
    {
        DECL_1_LOC(@1);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($2));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($3);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   THIS UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        DECL_1_LOC(@1);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($2));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($4);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   THIS UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($4);

        DECL_1_LOC(@1);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($2));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($3);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   Attrs THIS UnifiedParamClauseDecl FuncEnd
    {
        DECL_1_LOC(@2);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR($1);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($4);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   Attrs THIS UnifiedParamClauseDecl FuncAttrs FuncEnd
    {
        DECL_1_LOC(@2);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto attrs = $1->mergeSR($4);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR(attrs);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   Attrs THIS UnifiedParamClauseDecl FuncAttrs Constraint FuncEnd
    {
        IGNORE_FOR_NOW($5);

        DECL_1_LOC(@2);
        using namespace detail;
        std::unique_ptr<FullParam> fullParam(static_cast<FullParam*>($3));
        auto spec = makeAstRaw<FuncSpecAst>();
        spec->setParam(fullParam->param_.release())
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto attrs = $1->mergeSR($4);
        auto attrSpec = makeAstRaw<DecoratedSpecAst>()->setSpec(spec)->setAttrsSR(attrs);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(attrSpec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
;

DtorDecl:
    '~' THIS '(' ')' FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto spec = makeAstRaw<FuncSpecAst>()->setParam(param)
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locB);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
|   '~' THIS '(' ')' FuncAttrs FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto baseSpec = makeAstRaw<FuncSpecAst>()->setParam(param)
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = makeAstRaw<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR($5);
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locB);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
|   Attrs '~' THIS '(' ')' FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@2, @3, @4, @5);
        auto param = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto baseSpec = makeAstRaw<FuncSpecAst>()->setParam(param)
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = makeAstRaw<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR($1);
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locB);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
|   Attrs '~' THIS '(' ')' FuncAttrs FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto attrs = $1->mergeSR($6);
        auto param = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto baseSpec = makeAstRaw<FuncSpecAst>()->setParam(param)
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = makeAstRaw<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR(attrs);
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locB);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($7);
        func->setVariety(FuncVariety::Destructor);
        $$ = func;
    }
;

Postblit:
    THIS '(' THIS ')' FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        auto spec = makeAstRaw<FuncSpecAst>()->setParam(param)
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($5);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
|   THIS '(' THIS ')' FuncAttrs FuncEnd
    {
        /* TODO: Join ~ and this */
        DECL_4_LOC(@1, @2, @3, @4);
        auto param = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locB)->setRDelimLoc(locC);
        auto baseSpec = makeAstRaw<FuncSpecAst>()->setParam(param)
            ->setResult(makeAstRaw<VoidSpecAst>()->setKeyLoc(locA));
        auto spec = makeAstRaw<DecoratedSpecAst>()->setSpec(baseSpec)->setAttrsSR($5);
        auto name = makeAstRaw<SimpleNameAst>()->setNameLoc(locA);
        auto func = makeAstRaw<FuncDeclAst>()->setSpec(spec)->setName(name)->setStmt($6);
        func->setVariety(FuncVariety::Constructor);
        $$ = func;
    }
;

UnifiedParamClauseDecl:
    '(' ')'
    {
        DECL_2_LOC(@1, @2);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param);
    }
|   '(' ParamGroupDeclList ')'
    {
        DECL_2_LOC(@1, @3);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param);
    }
|   '(' ParamGroupDeclList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param);
    }
|   '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_4_LOC(@1, @3, @4, @5);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($2)->setVariadicLoc(locC)->setRDelimLoc(locD);
        $$ = new detail::FullParam(param);
    }
|   '(' "..." ')'
    {
        DECL_3_LOC(@1, @2, @3);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setVariadicLoc(locB)->setRDelimLoc(locC);
        $$ = new detail::FullParam(param);
    }
|   '(' RestrictTemplateParamList ')' '(' ')'
    {
        DECL_4_LOC(@1, @3, @4, @5);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' ParamGroupDeclList ')'
    {
        DECL_4_LOC(@1, @3, @4, @6);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($5)->setRDelimLoc(locB);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' ParamGroupDeclList ',' ')'
    {
        DECL_4_LOC(@1, @3, @4, @7);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locA)->setDeclsSR($5)->setRDelimLoc(locB);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_6_LOC(@1, @3, @4, @6, @7, @8);
        $5->delim_ = locD;
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($5)->setVariadicLoc(locE)->setRDelimLoc(locF);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ')' '(' "..." ')'
    {
        DECL_5_LOC(@1, @3, @4, @5, @6);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setVariadicLoc(locD)->setRDelimLoc(locE);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ')'
    {
        DECL_4_LOC(@1, @4, @5, @6);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setRDelimLoc(locD);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ParamGroupDeclList ')'
    {
        DECL_4_LOC(@1, @4, @5, @7);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($6)->setRDelimLoc(locD);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ParamGroupDeclList ',' ')'
    {
        DECL_4_LOC(@1, @4, @5, @8);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($6)->setRDelimLoc(locD);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_6_LOC(@1, @4, @5, @7, @8, @9);
        $6->delim_ = locD;
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setDeclsSR($6)->setVariadicLoc(locE)->setRDelimLoc(locF);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    }
|   '(' RestrictTemplateParamList ',' ')' '(' "..." ')'
    {
        DECL_5_LOC(@1, @4, @5, @6, @7);
        auto param = makeAstRaw<ParamClauseDeclAst>();
        param->setLDelimLoc(locC)->setVariadicLoc(locD)->setRDelimLoc(locE);
        auto templParam = makeAstRaw<TemplateParamClauseDeclAst>();
        templParam->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        $$ = new detail::FullParam(param, templParam);
    };

ParamClauseDecl:
    '(' ')'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ')'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' ParamGroupDeclList ',' "..." ')'
    {
        DECL_4_LOC(@1, @3, @4, @5);
        $2->delim_ = locB;
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setVariadicLoc(locC)
                ->setRDelimLoc(locD);
    }
|   '(' "..." ')'
    {
        DECL_3_LOC(@1, @2, @3);
        $$ = makeAstRaw<ParamClauseDeclAst>()->setLDelimLoc(locA)->setVariadicLoc(locB)->setRDelimLoc(locC);
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
        auto group = makeAstRaw<ParamGroupDeclAst>();
        group->setSpec($1);
        $$ = group;
    }
|   Type "..."
    {
        DECL_1_LOC(@2);
        auto param = makeAstRaw<ParamDeclAst__<ParamVariadic__>>();
        auto group = makeAstRaw<ParamGroupDeclAst>();
        group->addDecl(param)->setSpec($1);
        $$ = param;
    }
|   Type Ident
    {
        auto param = makeAstRaw<ParamDeclAst>();
        param->setName($2);
        auto group = makeAstRaw<ParamGroupDeclAst>();
        group->addDecl(param)->setSpec($1);
        $$ = group;
    }
|   Type Ident "..."
    {
        DECL_1_LOC(@3);
        auto param = makeAstRaw<ParamDeclAst__<ParamVariadic__>>();
        param->setName($2);
        auto group = makeAstRaw<ParamGroupDeclAst>();
        group->addDecl(param)->setSpec($1);
        $$ = group;
    }
|   Type Ident '=' AssignExpr
    {
        DECL_1_LOC(@3);
        auto param = makeAstRaw<ParamDeclAst__<ParamVariadic__Empty__,
                                         ParamDefaultArg__>>();
        param->setName($2)->setAssignLoc(locA)->setDefaultArg($4);
        auto group = makeAstRaw<ParamGroupDeclAst>();
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
        $$ = makeAstRaw<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' RestrictTemplateParamList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
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
        $$ = makeAstRaw<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '(' TemplateParamList ')'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '(' TemplateParamList ',' ')'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<TemplateParamClauseDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
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
        $$ = makeAstRaw<TemplateParamDeclAst>()->setName($1);
    }
|   Ident ':' Type
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__Empty__,
                                         TemplateParamSpecialization__>>()->setName($1)
            ->setBindLoc(locA)->setSpecialization($3);
    }
|   Ident ':' Type '=' Type
    {
        DECL_2_LOC(@2, @4);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__,
                                         TemplateParamSpecialization__>>()->setName($1)
                ->setAssignLoc(locA)->setDefaultArg($3)
                ->setBindLoc(locB)->setSpecialization($5);
    }
|   Ident '=' Type
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__>>()->setName($1)
                ->setAssignLoc(locA)->setDefaultArg($3);
    }
;

TemplateTupleParam:
    Ident "..."
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<TemplateParamDeclAst>()->setName($1)->setPackLoc(locA);
    }
;

TemplateValueParam:
    UnqualType Ident
    {
        $$ = makeAstRaw<TemplateParamDeclAst>()->setSpec($1)->setName($2);
    }
|   UnqualType Ident '=' AssignExpr
    {
        DECL_1_LOC(@3);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)->setSpec($1);
    }
|   UnqualType Ident '=' SpecialKeyword
    {
        DECL_1_LOC(@3);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)->setSpec($1);
    }
|   UnqualType Ident ':' CondExpr
    {
        DECL_1_LOC(@3);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__Empty__,
                                         TemplateParamSpecialization__>>()->setName($2)
            ->setBindLoc(locA)->setSpecialization($4)->setSpec($1);
    }
|   UnqualType Ident ':' CondExpr '=' AssignExpr
    {
        DECL_2_LOC(@3, @5);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__,
                                         TemplateParamSpecialization__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)
                ->setBindLoc(locB)->setSpecialization($6)->setSpec($1);
    }
|   UnqualType Ident ':' CondExpr '=' SpecialKeyword
    {
        DECL_2_LOC(@3, @5);
        $$ = makeAstRaw<TemplateParamDeclAst__<TemplateParamDefaultArg__,
                                         TemplateParamSpecialization__>>()->setName($2)
                ->setAssignLoc(locA)->setDefaultArg($4)
                ->setBindLoc(locB)->setSpecialization($6)->setSpec($1);
    }
;

TemplateAliasParam:
    ALIAS Ident
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TemplateParamAliasDeclAst>()->setAliasLoc(locA)->setName($2);
    }
|   ALIAS Type Ident
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TemplateParamAliasDeclAst>()->setAliasLoc(locA)->setSpec($2)->setName($3);
    }
    /* TODO: Regras faltando... */
;

TemplateThisParam:
    THIS TemplateTypeParam
    {
        IGNORE_FOR_NOW($2);

        DECL_1_LOC(@1);
        auto name = (new GenNameAst("<TODO:TemplateThisParam>"))->setGenLoc(locA);
        $$ = makeAstRaw<TemplateParamThisDeclAst>()->setThissLoc(locA)->setName(name);
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
        $$ = makeAstRaw<DeclAttrAst>()->setKeyLoc(locA);
    }
|   PURE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<DeclAttrAst>()->setKeyLoc(locA);
    }
|   Annot
;

Constraint:
    IF '(' Expr ')'
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<ConstraintDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC);
    }
;

Init:
    VOID
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<VoidInitExprAst>()->setKeyLoc(locA);
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
        $$ = makeAstRaw<ArrayInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '[' ArrayMemberInits ']'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<ArrayInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
|   '[' ArrayMemberInits ',' ']'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<ArrayInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
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
        $$ = makeAstRaw<DesignateExprAst>()->setId($1)->setDelimLoc(locA)->setValue($3);
    }
;

StructInit:
    '{' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' StructMemberInits '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
    }
|   '{' StructMemberInits ',' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<RecordInitExprAst>()->setLDelimLoc(locA)->setInitsSR($2)->setRDelimLoc(locB);
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
        auto id = makeAstRaw<IdentExprAst>()->setName($1);
        $$ = makeAstRaw<DesignateExprAst>()->setId(id)->setDelimLoc(locA)->setValue($3);
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
        $$ = makeAstRaw<DeclAttrAst>()->setKeyLoc(locA);
    }
|   FINAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<DeclAttrAst>()->setKeyLoc(locA);
    }
|   OVERRIDE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<DeclAttrAst>()->setKeyLoc(locA);
    }
|   AUTO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<AutoAttrAst>()->setKeyLoc(locA);
    }
|   __GSHARED
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   IN
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ParamDirAttrAst>()->setKeyLoc(locA);
    }
|   OUT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ParamDirAttrAst>()->setKeyLoc(locA);
    }
|   LAZY
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<EvalStrategyAttrAst>()->setKeyLoc(locA);
    }
;

StorageClass:
    /* Enum as a manifest constant is matched by EnumDecl. */
    DEPRECATED
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<DeclAttrAst>()->setKeyLoc(locA);
    }
|   STATIC
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StorageClassAttrAst>()->setKeyLoc(locA);
    }
|   EXTERN
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<LinkageAttrAst>()->setKeyLoc(locA);
    }
|   REF
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StorageClassAttrAst>()->setKeyLoc(locA);
    }
|   SCOPE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StorageClassAttrAst>()->setKeyLoc(locA);
    }
    /* TODO: Handle SYNCHRONIZED for this situation. */
;

AccessSpec:
    PRIVATE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   PACKAGE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   PROTECTED
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   PUBLIC
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<VisibilityAttrAst>()->setKeyLoc(locA);
    }
|   EXPORT
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<VisibilityAttrAst>()->setKeyLoc(locA);
    }
;

Annot:
    '@' PROPERTY
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' SAFE
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' TRUSTED
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' SYSTEM
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' DISABLE
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
|   '@' NOGC
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<AnnotAttrAst>()->setAtLoc(locA)->setTextLoc(locB);
    }
;

LinkageAttr:
    EXTERN '(' IdentList ')'
    {
        DECL_3_LOC(@1, @2, @4);
        auto expr = makeAstRaw<IdentExprAst>()->setName($3);
        $$ = makeAstRaw<CodegenAttrAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr(expr)->setRDelimLoc(locC);
    }
|   EXTERN '(' Ident "++" ',' IdentList ')' /* For C++ */
    {
        IGNORE_FOR_NOW($3); // Handle C++

        DECL_3_LOC(@1, @2, @7);
        auto expr = makeAstRaw<IdentExprAst>()->setName($6);
        $$ = makeAstRaw<CodegenAttrAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr(expr)->setRDelimLoc(locC);
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
        $$ = makeAstRaw<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   IMMUTABLE
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   INOUT
    {
        /* From D docs: "The inout forms a wildcard that stands in for
           any of mutable, const, immutable, inout, or inout const". */
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TypeQualAttrAst>()->setKeyLoc(locA);
    }
|   SHARED
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TypeQualAttrAst>()->setKeyLoc(locA);
    }
;

AlignAttr:
    ALIGN
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<CodegenAttrAst>()->setKeyLoc(locA);
    }
|   ALIGN '(' NumLit ')'
    {
        /* TODO: Report error if not integer literal */
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<CodegenAttrAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)->setRDelimLoc(locC);
    }
;

StaticAssert:
    STATIC ASSERT '(' AssignExpr ')' ';'
    {
        DECL_5_LOC(@1, @2, @3, @5, @6);
        /* TODO: Join static and assert */
        $$ = makeAstRaw<StaticAssertDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locC)->setExpr($4)
                ->setRDelimLoc(locD)->setTerminLoc(locE);
    }
|   STATIC ASSERT '(' AssignExpr ',' AssignExpr ')' ';'
    {
        DECL_6_LOC(@1, @2, @3, @5, @7, @8);
        /* TODO: Join static and assert */
        $$ = makeAstRaw<StaticAssertDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locC)->setExpr($4)
                ->setMDelimLoc(locD)->setMessage($6)->setRDelimLoc(locE)->setTerminLoc(locF);
    }
;

VersionDecl:
    VERSION '=' Ident ';'
    {
        DECL_3_LOC(@1, @2, @4);
        auto expr = makeAstRaw<IdentExprAst>()->setName($3);
        $$ = makeAstRaw<VersionDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr(expr)->setTerminLoc(locC);
    }
|   VERSION '=' NumLit ';'
    {
        /* TODO: Report error if not integer literal */
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<VersionDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr($3)->setTerminLoc(locC);
    }
;

DebugDecl:
    DEBUG '=' Ident ';'
    {
        DECL_3_LOC(@1, @2, @4);
        auto expr = makeAstRaw<IdentExprAst>()->setName($3);
        $$ = makeAstRaw<DebugDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr(expr)->setTerminLoc(locC);
    }
|   DEBUG '=' NumLit ';'
    {
        /* TODO: Report error if not integer literal */
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<DebugDeclAst>()->setKeyLoc(locA)->setDelimLoc(locB)->setExpr($3)->setTerminLoc(locC);
    }
;

UnitTestDecl:
    UNITTEST BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<UnitTestDeclAst>()->setKeyLoc(locA)->setStmt($2);
    }
;

InvariantDecl:
    INVARIANT BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<InvariantDeclAst>()->setKeyLoc(locA)->setStmt($2);
    }
|   INVARIANT '(' ')' BlockStmt
    {
        DECL_3_LOC(@1, @2, @3);
        $$ = makeAstRaw<InvariantDeclAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setRDelimLoc(locC)->setStmt($4);
    }
;

SelectiveDecl:
    ConditionDecl Decl %prec PREFER_SHIFT
    {
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = makeAstRaw<SelectiveDeclAst>()->setKeyLoc(cond->keyLoc_)
            ->setLDelimLoc(cond->lDelimLoc_)->setExpr(cond->expr_.release())
            ->setRDelimLoc(cond->rDelimLoc_)->setIfDecl($2);
    }
|   ConditionDecl Decl ELSE Decl
    {
        DECL_1_LOC(@3);
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = makeAstRaw<SelectiveDeclAst>()->setKeyLoc(cond->keyLoc_)
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
        auto name = makeAstRaw<IdentExprAst>()->setName($3);
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
        auto name = makeAstRaw<IdentExprAst>()->setName($3);
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
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locC);
        $$ = makeAstRaw<AliasDeclAst>()->setKeyLoc(locA)->setName($2)->setSpec(inferred)->setTerminLoc(locC);
    }
;

AliasDecl:
    ALIAS AliasBindDeclList ';'
    {
        DECL_2_LOC(@1, @3);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setDeclsSR($2)->setTerminLoc(locB);
        sect->setVariety(SectionVariety::Types);
        $$ = sect;
    }
|   ALIAS Type Ident ';'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<AliasDeclAst>()->setKeyLoc(locA)->setName($3)->setSpec($2)->setTerminLoc(locB);
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
        $$ = makeAstRaw<AliasDeclAst>()->setName($1)->setEqLoc(locA)->setSpec($3);
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
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        $$ = EnumDecl_Cast($4)->setKeyLoc(locA)->setSepLoc(locB)->setSpec($3);
    }
|   ENUM Ident '=' AssignExpr ';'
    {
        DECL_3_LOC(@1, @3, @5);
        auto inferred = makeAstRaw<InferredSpecAst>()->setKeyLoc(locA);
        auto var = makeAstRaw<VarDeclAst__<VarInit__>>();
        var->setName($2)->setAssignLoc(locB)->setInit($4);
        auto group = makeAstRaw<VarGroupDeclAst>();
        group->setKeyLoc(locA)->addDecl(var)->setSpec(inferred)->setTerminLoc(locC);
        group->setAllocScheme(AllocScheme::CompileTime);
        $$ = group;
    }
|   ENUM Type Ident '=' AssignExpr ';'
    {
        DECL_3_LOC(@1, @4, @6);
        auto var = makeAstRaw<VarDeclAst__<VarInit__>>();
        var->setName($3)->setAssignLoc(locB)->setInit($5);
        auto group = makeAstRaw<VarGroupDeclAst>();
        group->setKeyLoc(locA)->setSpec($2)->addDecl(var)->setTerminLoc(locC);
        group->setAllocScheme(AllocScheme::CompileTime);
        $$ = group;
    }
|   ENUM Type Ident RestrictTemplateParamClause '=' AssignExpr ';'
    {
        IGNORE_FOR_NOW($4);

        DECL_3_LOC(@1, @5, @7);
        auto var = makeAstRaw<VarDeclAst__<VarInit__>>();
        var->setName($3)->setAssignLoc(locB)->setInit($6);
        auto group = makeAstRaw<VarGroupDeclAst>();
        group->setKeyLoc(locA)->setSpec($2)->addDecl(var)->setTerminLoc(locC);
        group->setAllocScheme(AllocScheme::CompileTime);
        $$ = group;
    }
;

EnumEnd:
    ';'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<EnumDeclAst>()->setTerminLoc(locA);
    }
|   EnumBody
;

EnumBody:
    '{' EnumMemberDeclList '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<EnumDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '{' EnumMemberDeclList ',' '}'
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<EnumDeclAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
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
        $$ = makeAstRaw<EnumMemberDeclAst>()->setName($1);
    }
|   Ident '=' AssignExpr
    {
        DECL_1_LOC(@2);
        $$ = makeAstRaw<EnumMemberDeclAst>()->setName($1)->setAssignLoc(locA)->setInit($3);
    }
|   Type Ident '=' AssignExpr
    {
        IGNORE_FOR_NOW($1);

        DECL_1_LOC(@3);
        $$ = makeAstRaw<EnumMemberDeclAst>()->setName($2)->setAssignLoc(locA)->setInit($4);
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
        auto fwd = makeAstRaw<ForwardDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setTerminLoc(locA);
        fwd->setVariety(RecordVariety(data->variety_));
        $$ = fwd;
    }
|   RecordKey Ident RecordType
    {
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($3);
        RecordSpec_Cast($3)->setVariety(RecordVariety(data->variety_));
        $$ = record;
    }
|   RecordKey Ident ':' BaseRecordList RecordType
    {
        /* TODO: Report error if not class or interface */
        DECL_1_LOC(@3);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($5);
        RecordSpec_Cast($5)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($4);
        $$ = record;
    }
|   RecordKey RecordType
    {
        /* TODO: Report error if not union or struct */
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setSpec($2);
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
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($4);
        RecordSpec_Cast($4)->setVariety(RecordVariety(data->variety_));
        $$ = record;
    }
|   RecordKey Ident TemplateParamClause Constraint RecordType
    {
        IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($4);

        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($5);
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
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($7);
        RecordSpec_Cast($7)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($6);
        $$ = record;
    }
|   RecordKey Ident TemplateParamClause ':' BaseRecordList RecordType
    {
        IGNORE_FOR_NOW($3);

        DECL_1_LOC(@4);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($6);
        RecordSpec_Cast($6)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($5);
        $$ = record;
    }
|   RecordKey Ident TemplateParamClause ':' BaseRecordList Constraint RecordType
    {
        IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($6);

        DECL_1_LOC(@4);
        using namespace detail;
        std::unique_ptr<RecordKeyVariety> data(static_cast<RecordKeyVariety*>($1));
        auto record = makeAstRaw<RecordDeclAst>()->setKeyLoc(data->keyLoc_)->setName($2)->setSpec($7);
        RecordSpec_Cast($7)->setVariety(RecordVariety(data->variety_))->setDelimLoc(locA)->setBasesSR($5);
        $$ = record;
    }
;

TemplateDecl:
    TEMPLATE Ident TemplateParamClause '{' '}'
    {
        IGNORE_FOR_NOW($3);

        DECL_3_LOC(@1, @4, @5);
        $$ = makeAstRaw<TemplateDeclAst>()->setKeyLoc(locA)->setName($2);
    }
|   TEMPLATE Ident TemplateParamClause '{' Decls '}'
    {
        IGNORE_FOR_NOW($3); IGNORE_LIST_FOR_NOW($5);

        DECL_3_LOC(@1, @4, @6);
        $$ = makeAstRaw<TemplateDeclAst>()->setKeyLoc(locA)->setName($2);
    }
|   TEMPLATE Ident TemplateParamClause Constraint '{' Decls '}'
    {
        IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($4); IGNORE_LIST_FOR_NOW($6);

        DECL_3_LOC(@1, @5, @7);
        $$ = makeAstRaw<TemplateDeclAst>()->setKeyLoc(locA)->setName($2);
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
        auto name = makeAstRaw<NestedNameAst>()->setNamesSR($1);
        $$ = makeAstRaw<BaseDeclAst>()->setName(name);
    }
;

RecordType:
    '{' '}'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<RecordSpecAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
    }
|   '{' Decls '}'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<RecordSpecAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
|   '{' Decls EOP
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<RecordSpecAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   '{' Decls error BlockStmtSync
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<RecordSpecAst>()->setLDelimLoc(locA)->setDeclsSR($2)->setRDelimLoc(locB);
    }
;

ImportDecl:
    IMPORT ImportList ';'
    {
        DECL_2_LOC(@1, @3);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setDeclsSR($2)->setTerminLoc(locB);
        sect->setVariety(SectionVariety::Imports);
        $$ = sect;
    }
|   STATIC IMPORT ImportList ';'
    {
        /* TODO: Join static and import */
        DECL_2_LOC(@2, @4);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setDeclsSR($3)->setTerminLoc(locB);
        sect->setVariety(SectionVariety::Imports);
        $$ = sect;
    }
|   PUBLIC IMPORT ImportList ';'
    {
        /* TODO: Join public and import */
        DECL_2_LOC(@2, @4);
        auto sect = makeAstRaw<SectionDeclAst>()->setKeyLoc(locA)->setDeclsSR($3)->setTerminLoc(locB);
        sect->setVariety(SectionVariety::Imports);
        $$ = sect;
    }
;

Import:
    NestedIdent
    {
        auto name = makeAstRaw<IdentExprAst>()->setName($1);
        $$ = makeAstRaw<ImportDeclAst>()->setModule(name);
    }
|   Ident '=' NestedIdent
    {
        DECL_1_LOC(@2);
        auto name = makeAstRaw<IdentExprAst>()->setName($3);
        $$ = makeAstRaw<ImportDeclAst>()->setLocalName($1)->setBindLoc(locA)->setModule(name);
    }
|   Ident '=' NestedIdent ':' ImportBindList %prec PREFER_SHIFT
    {
        /* Once inside a selective import, append to its binding list,
           not to the import list itself. */
        DECL_2_LOC(@2, @4);
        auto name = makeAstRaw<IdentExprAst>()->setName($3);
        $$ = makeAstRaw<ImportDistinctDeclAst>()->setLocalName($1)->setBindLoc(locA)->setModule(name)
                ->setSelecLoc(locB)->setDeclsSR($5);
    }
|   NestedIdent ':' ImportBindList %prec PREFER_SHIFT /* See comment above */
    {
        DECL_1_LOC(@2);
        auto name = makeAstRaw<IdentExprAst>()->setName($1);
        $$ = makeAstRaw<ImportDistinctDeclAst>()->setModule(name)->setSelecLoc(locA)->setDeclsSR($3);
    }
;

ImportBindDecl:
    NestedIdent
    {
        // TODO: Report error if not Ident (cannot be nested).
        auto name = makeAstRaw<IdentExprAst>()->setName($1);
        $$ = makeAstRaw<ImportDeclAst>()->setModule(name);
    }
|   Ident '=' NestedIdent
    {
        DECL_1_LOC(@2);
        auto name = makeAstRaw<IdentExprAst>()->setName($3);
        $$ = makeAstRaw<ImportDeclAst>()->setLocalName($1)->setBindLoc(locA)->setModule(name);
    }
;

ImportBindList:
    ImportBindDecl
    {
        $$ = DeclAstList::createSR($1);
    }
|   ImportBindList ',' ImportBindDecl
    {
        DECL_1_LOC(@2);
        $1->delim_ = locA;
        $$ = $1->handleSR($3);
    }
;

ImportList:
    Import
    {
        $$ = DeclAstList::createSR($1);
    }
|   ImportList ',' Import
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
        $$ = makeAstRaw<DeclStmtAst>()->setDecl($1);
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
        $$ = makeAstRaw<DeclStmtAst>()->setDecl($1);
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
|   '{' StmtList EOP
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   '{' StmtList error BlockStmtSync
    {
        DECL_2_LOC(@1, @4);
        $$ = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        yyerrok;
    }
;

BlockStmtSync: '}' | EOP;

FuncEnd:
    ';'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locA);
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
        auto block = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        auto in = makeAstRaw<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = makeAstRaw<BodyStmtAst>()->setBodyLoc(locC)->setBlock($4);
        $$ = makeAstRaw<ContractStmtAst>()->setStmt1(in)->setStmt2(body);
    }
|   IN_LBRACE_HACK StmtList '}' BODY BlockStmt
    {
        DECL_3_LOC(@1, @3, @4);
        auto block = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        auto in = makeAstRaw<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = makeAstRaw<BodyStmtAst>()->setBodyLoc(locC)->setBlock($5);
        $$ = makeAstRaw<ContractStmtAst>()->setStmt1(in)->setStmt2(body);
    }
|   IN_LBRACE_HACK '}' FuncOutStmt BODY BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        auto block = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setRDelimLoc(locB);
        auto in = makeAstRaw<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = makeAstRaw<BodyStmtAst>()->setBodyLoc(locC)->setBlock($5);
        $$ = makeAstRaw<ContractStmtAst>()->setStmt1(in)->setStmt2(body)->setStmt3($3);
    }
|   IN_LBRACE_HACK StmtList '}' FuncOutStmt BODY BlockStmt
    {
        DECL_3_LOC(@1, @3, @5);
        auto block = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locA)->setStmtsSR($2)->setRDelimLoc(locB);
        auto in = makeAstRaw<InStmtAst>()->setInLoc(locA)->setBlock(block);
        auto body = makeAstRaw<BodyStmtAst>()->setBodyLoc(locC)->setBlock($6);
        $$ = makeAstRaw<ContractStmtAst>()->setStmt1(in)->setStmt2(body)->setStmt3($4);
    }
|   FuncOutStmt BODY BlockStmt
    {
        DECL_1_LOC(@2);
        auto body = makeAstRaw<BodyStmtAst>()->setBodyLoc(locA)->setBlock($3);
        $$ = makeAstRaw<ContractStmtAst>()->setStmt1($1)->setStmt2(body);
    }
;

FuncOutStmt:
    OUT BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<OutStmtAst>()->setOutLoc(locA)->setBlock($2);
    }
|   OUT '(' Ident ')' BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<OutStmtAst>()->setOutLoc(locA)->setLDelimLoc(locB)->setName($3)
                ->setRDelimLoc(locC)->setBlock($5);
    }
;

LabeledStmt:
    Ident ':' Stmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<LabeledStmtAst>()->setLabel($1)->setDelimLoc(locA)->setStmt($3);
    }
;

ExprStmt:
    Expr ';'
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ExprStmtAst>()->setExpr($1)->setTerminLoc(locA);
    }
|   Expr EOP
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ExprStmtAst>()->setExpr($1)->setTerminLoc(locA);
        yyerror(&yylloc, scanner, context, "unexpected <end_of_program>");
    }
|   Expr error ExprStmtSync
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<ExprStmtAst>()->setExpr($1)->setTerminLoc(locA);
        yyerror(&yylloc, scanner, context, "unexpected token");
        yyerrok;
    }
;

ExprStmtSync: ';' | EOP;

IfStmt:
    IF '(' Expr ')' Stmt %prec PREFER_SHIFT /* For the dangling else */
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setThen($5);
    }
|   IF '(' Expr ')' Stmt ELSE Stmt
    {
        DECL_4_LOC(@1, @2, @4, @6);
        $$ = makeAstRaw<IfStmtAst>()->setIfLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setThen($5)->setElseLoc(locD)->setNotThen($7);
    }
;

WhileStmt:
    WHILE '(' Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<WhileStmtAst>()->setWhileLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt($5);
    }
;

DoStmt:
    DO Stmt WHILE '(' Expr ')'
    {
        DECL_4_LOC(@1, @3, @4, @6);
        $$ = makeAstRaw<DoWhileStmtAst>()->setDoLoc(locA)->setStmt($2)->setWhileLoc(locB)
                ->setLDelimLoc(locC)->setExpr($5)->setRDelimLoc(locD);
    }
;

ForStmt:
    FOR '(' Stmt ';' ')' Stmt
    {
        DECL_4_LOC(@1, @2, @3, @4);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setDelimLoc(locC)->setRDelimLoc(locD)->setStmt($6);
    }
|   FOR '(' Stmt Expr ';' ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @6);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setCond($4)->setDelimLoc(locC)->setRDelimLoc(locD)->setStmt($7);
    }
|   FOR '(' Stmt Expr ';' Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @7);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setCond($4)->setDelimLoc(locC)->setPost($6)->setRDelimLoc(locD)
                ->setStmt($8);
    }
|   FOR '(' Stmt ';' Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @4, @6);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble($3)
                ->setDelimLoc(locC)->setPost($5)->setRDelimLoc(locD)->setStmt($7);
    }
|   FOR '(' ';' ';' ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @4, @5);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setDelimLoc(locD)->setRDelimLoc(locE)->setStmt($6);
    }
|   FOR '(' ';' Expr ';' ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @5, @6);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setCond($4)->setDelimLoc(locC)->setRDelimLoc(locD)->setStmt($7);
    }
|   FOR '(' ';' Expr ';' Expr ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @5, @7);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setCond($4)->setDelimLoc(locC)->setPost($6)->setRDelimLoc(locD)
                ->setStmt($8);
    }
|   FOR '(' ';' ';' Expr ')' Stmt
    {
        DECL_5_LOC(@1, @2, @3, @4, @6);
        auto stmt = makeAstRaw<EmptyStmtAst>()->setKeyLoc(locC);
        $$ = makeAstRaw<ForStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setPreamble(stmt)
                ->setDelimLoc(locC)->setPost($5)->setRDelimLoc(locD)->setStmt($7);
    }
;

ForeachStmt:
    FOREACH '(' ForeachDecl Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @5);
        auto range = makeAstRaw<SubrangeExprAst>()->setLow($4);
        $$ = makeAstRaw<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locC)->setStmt($6);
    }
|   FOREACH '(' ForeachDecl Expr ".." Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @7);
        auto range = makeAstRaw<SubrangeExprAst>()->setLow($4)->setDelim1Loc(locC)->setHi($6);
        $$ = makeAstRaw<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locC)->setStmt($8);
    }
|   FOREACH_REVERSE '(' ForeachDecl Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @5);
        auto range = makeAstRaw<SubrangeExprAst>()->setLow($4);
        $$ = makeAstRaw<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locC)->setStmt($6);
    }
|   FOREACH_REVERSE '(' ForeachDecl Expr ".." Expr ')' Stmt
    {
        DECL_4_LOC(@1, @2, @5, @7);
        auto range = makeAstRaw<SubrangeExprAst>()->setLow($4)->setDelim1Loc(locC)->setHi($6);
        $$ = makeAstRaw<ForeachStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setDecl($3)
                ->setExpr(range)->setRDelimLoc(locD)->setStmt($8);
    }
;

ForeachDecl:
    VarGroupDecl
|   IdentList ';'
    {
        DECL_1_LOC(@2);
        std::unique_ptr<NameAstList> l($1->finishSR());
        auto names = l.get();
        auto decls = DeclAstList::create(makeAstRaw<VarDeclAst>()->setName(names->releaseHead().release()));
        while ((names = names->subList()))
            decls->pushBack(makeAstRaw<VarDeclAst>()->setName(names->releaseHead().release()));
        auto group = makeAstRaw<VarGroupDeclAst>()->setSpec(makeAstRaw<InferredSpecAst>());
        group->decls_.reset(decls);
        $$ = group;
    }
;

ContinueStmt:
    CONTINUE ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<ContinueStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   CONTINUE Ident ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ContinueStmtAst>()->setKeyLoc(locA)->setName($2)->setTerminLoc(locB);
    }
;

BreakStmt:
    BREAK ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<BreakStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   BREAK Ident ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<BreakStmtAst>()->setKeyLoc(locA)->setName($2)->setTerminLoc(locB);
    }
;

ReturnStmt:
    RETURN ';'
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<ReturnStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   RETURN Expr ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ReturnStmtAst>()->setKeyLoc(locA)->setExprsSR(ExprAstList::createSR($2))
                ->setTerminLoc(locB);
    }
;

GotoStmt:
    GOTO Ident ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<GotoStmtAst>()->setKeyLoc(locA)->setName($2)->setTerminLoc(locB);
    }
|   GOTO DEFAULT ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<GotoStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   GOTO CASE ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<GotoStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
|   GOTO CASE Expr ';'
    {
        IGNORE_FOR_NOW($3);

        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<GotoStmtAst>()->setKeyLoc(locA)->setTerminLoc(locB);
    }
;

WithStmt:
    /* Expr will match `Symbol` and `TemplateInst` as `PrimaryExpr` for the
       as the other versions of the with statement. */
    WITH '(' Expr ')' Stmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<WithStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt($5);
    }
;

SynchronizedStmt:
    /* There is an ambiguity in the following statements, as discussed
       at http://www.digitalmars.com/d/archives/digitalmars/D/bugs/3833.html */
    SYNCHRONIZED '{' StmtList '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto block = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($3)->setRDelimLoc(locC);
        $$ = makeAstRaw<SyncedStmtAst>()->setKeyLoc(locA)->setStmt(block);
    }
|   SYNCHRONIZED PlainStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<SyncedStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
|   SYNCHRONIZED '(' Expr ')' '{' StmtList '}'
    {
        DECL_3_LOC(@1, @2, @4);
        auto block = makeAstRaw<BlockStmtAst>()->setLDelimLoc(locB)->setStmtsSR($6)->setRDelimLoc(locC);
        $$ = makeAstRaw<SyncedStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt(block);
    }
|   SYNCHRONIZED '(' Expr ')' PlainStmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<SyncedStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt($5);
    }
;

TryStmt:
    TRY BlockStmt CatchClauseStmts
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setCatchsSR($3);
    }
|   TRY BlockStmt LastCatchClauseStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->addCatch($3);
    }
|   TRY BlockStmt CatchClauseStmts LastCatchClauseStmt
    {
        DECL_1_LOC(@1);
        auto catchs = $3->handleSR($4);
        $$ = makeAstRaw<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setCatchsSR(catchs);
    }
|   TRY BlockStmt CatchClauseStmts LastCatchClauseStmt FinallyClauseStmt
    {
        DECL_1_LOC(@1);
        auto catchs = $3->handleSR($4);
        $$ = makeAstRaw<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setCatchsSR(catchs)->setFinal($5);
    }
|   TRY BlockStmt LastCatchClauseStmt FinallyClauseStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->addCatch($3)->setFinal($4);
    }
|   TRY BlockStmt FinallyClauseStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<TryStmtAst>()->setKeyLoc(locA)->setStmt($2)->setFinal($3);
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
        $$ = makeAstRaw<CatchClauseStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)
            ->setDecl($3)->setRDelimLoc(locC)->setStmt($5);
    }
;

LastCatchClauseStmt:
    CATCH BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<CatchClauseStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
;

FinallyClauseStmt:
    FINALLY BlockStmt
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<FinallyClauseStmtAst>()->setKeyLoc(locA)->setStmt($2);
    }
;

ThrowStmt:
    THROW Expr ';'
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<ThrowStmtAst>()->setKeyLoc(locA)->setExpr($2)->setTerminLoc(locB);
    }
;

DeferredStmt:
    SCOPE '(' Ident ')' Stmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<DeferredStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)
                ->setName($3)->setRDelimLoc(locC)->setStmt($5);
    }
;

CondStmt:
    ConditionDecl Stmt %prec PREFER_SHIFT
    {
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = makeAstRaw<SelectiveStmtAst>()->setKeyLoc(cond->keyLoc_)
            ->setLDelimLoc(cond->lDelimLoc_)->setExpr(cond->expr_.release())
            ->setRDelimLoc(cond->rDelimLoc_)->setIfStmt($2);
    }
|   ConditionDecl Stmt ELSE Stmt
    {
        DECL_1_LOC(@3);
        using namespace detail;
        std::unique_ptr<Condition> cond(static_cast<Condition*>($1));
        $$ = makeAstRaw<SelectiveStmtAst>()->setKeyLoc(cond->keyLoc_)
            ->setLDelimLoc(cond->lDelimLoc_)->setExpr(cond->expr_.release())
            ->setRDelimLoc(cond->rDelimLoc_)->setIfStmt($2)->setOtherKeyLoc(locA)
            ->setElseStmt($4);
    }
;

SwitchStmt:
    SWITCH '(' Expr ')' BlockStmt
    {
        DECL_3_LOC(@1, @2, @4);
        $$ = makeAstRaw<SwitchStmtAst>()->setKeyLoc(locA)->setLDelimLoc(locB)->setExpr($3)
                ->setRDelimLoc(locC)->setStmt($5);
    }
;

CaseClauseStmt:
    CASE ExprList ':' Stmt
    {
        DECL_2_LOC(@1, @3);
        $$ = makeAstRaw<CaseClauseStmtAst>()->setKeyLoc(locA)->setExprsSR($2)->setDelimLoc(locB)
                ->addStmt($4);
    }
|   DEFAULT ':' Stmt
    {
        DECL_2_LOC(@1, @2);
        $$ = makeAstRaw<DefaultClauseStmtAst>()->setKeyLoc(locA)->setDelimLoc(locB)->addStmt($3);
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

PointerLit:
    NULL_LITERAL
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<NullLitExprAst>()->setLitLoc(locA);
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
        $$ = makeAstRaw<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   FuncKey Type FuncBody
    {
        /* TODO */ IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($3);

        DECL_1_LOC(@1);
        $$ = makeAstRaw<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   FuncKey Type ParamClauseDecl FuncBody
    {
        /* TODO */
        IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($3); IGNORE_FOR_NOW($4);

        DECL_1_LOC(@1);
        $$ = makeAstRaw<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   FuncKey ParamClauseDecl FuncBody
    {
        /* TODO */ IGNORE_FOR_NOW($2); IGNORE_FOR_NOW($3);

        DECL_1_LOC(@1);
        $$ = makeAstRaw<VoidInitExprAst>()->setKeyLoc(locA);
    }
|   ParamClauseDecl FuncBody
    {
        /* TODO */ IGNORE_FOR_NOW($1); IGNORE_FOR_NOW($2);

        DECL_1_LOC(@1);
        $$ = makeAstRaw<VoidInitExprAst>()->setKeyLoc(locA);
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
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __MODULE__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __LINE__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __FUNCTION__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __PRETTY_FUNCTION__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __TIME__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __DATE__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __TIMESTAMP__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __VERSION__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
    }
|   __VENDOR__MACRO
    {
        DECL_1_LOC(@1);
        $$ = makeAstRaw<StrLitExprAst>()->setLitLoc(locA);
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
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Type);
        $$ = arg;
    }
|   AssignExpr
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
;

TemplateSingleArg:
    Ident
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::NameToLookup);
        $$ = arg;
    }
|   BuiltinType
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Type);
        $$ = arg;
    }
|   CharLit
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   StringLit
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   NumLit
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   BoolLit
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   PointerLit
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   ThisExpr
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
|   SpecialKeyword
    {
        auto arg = makeAstRaw<TemplateArgAst>()->setArg($1);
        arg->setVariety(TemplateArgVariety::Expr);
        $$ = arg;
    }
;
%%
