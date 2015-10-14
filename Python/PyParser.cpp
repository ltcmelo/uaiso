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

#include "Python/PyParser.h"
#include "Python/PyLexer.h"
#include "Ast/Ast.h"
#include "Common/Assert.h"
#include "Common/Trace__.h"
#include "Common/Util__.h"
#include "Parsing/ParsingContext.h"
#include <tuple>

#define TRACE_NAME "PyParser"

using namespace uaiso;

PyParser::PyParser()
{}

void PyParser::consumeToken()
{
    if (ahead_ == TK_EOP)
        return;

    // Track last location for further reference.
    lastLoc_ = lexer_->tokenLoc();
    lastLoc_.fileName_ = context_->fileName();

    ahead_ = lexer_->lex();
}

bool PyParser::maybeConsume(Token tk)
{
    if (ahead_ == tk) {
        consumeToken();
        return true;
    }

    return false;
}

std::pair<bool, Token> PyParser::maybeConsume(const std::initializer_list<Token>& tks)
{
    for (auto tk : tks) {
        if (ahead_ == tk) {
            consumeToken();
            return std::make_pair(true, tk);
        }
    }

    return std::make_pair(false, TK_INVALID);
}

void PyParser::skipTo(Token tk)
{
    while (!(ahead_ == tk || ahead_ == TK_EOP))
        consumeToken();
}

bool PyParser::match(Token tk)
{
    auto actual = ahead_;
    consumeToken(); // Move on, regardless of a match.

    if (actual != tk) {
        failMatch(false);
        return false;
    }

    return true;
}

void PyParser::failMatch(bool consume)
{
    // Location in the report is always from the lastly consumed token.
    if (consume)
        consumeToken();

    DEBUG_TRACE("error at %d:%d unexpected token (%s)\n",
                lastLoc_.lastLine_, lastLoc_.lastCol_, lastLoc_.fileName_.c_str());
    context_->trackReport(Diagnostic::UnexpectedToken, lastLoc_);
}

bool PyParser::isTestAhead() const
{
    switch (ahead_) {
    case TK_LAMBDA:
        return true;

    default:
        return isNonLambdaTestAhead();
    }
}

bool PyParser::isNonLambdaTestAhead() const
{
    switch (ahead_) {
    case TK_NOT:
        return true;

    default:
        return isExprAhead();
    }
}

bool PyParser::isExprAhead() const
{
    return isFactorAhead();
}

bool PyParser::isFactorAhead() const
{
    switch (ahead_) {
    case TK_PLUS:
    case TK_MINUS:
    case TK_TILDE:
        return true;

    default:
        return isAtomAhead();
    }
}

bool PyParser::isAtomAhead() const
{
    switch (ahead_) {
    case TK_LPAREN:
    case TK_RPAREN:
    case TK_LBRACE:
    case TK_BACKTICK:
    case TK_IDENTIFIER:
    case TK_INTEGER_LITERAL:
    case TK_FLOAT_LITERAL:
    case TK_STRING_LITERAL:
        return true;

    default:
        return false;
    }
}

std::pair<PyParser::Precedence, std::unique_ptr<BinaryExprAst>>
PyParser::fetchPrecAhead() const
{
    switch (ahead_) {
    case TK_PIPE:
        return std::make_pair(Precedence::Or, makeAst<BitOrExprAst>());

    case TK_CARET:
        return std::make_pair(Precedence::Xor, makeAst<BitXorExprAst>());

    case TK_AMPER:
        return std::make_pair(Precedence::And, makeAst<BitAndExprAst>());

    case TK_LESS_LESS:
    case TK_GREATER_GREATER:
        return std::make_pair(Precedence::Shift, makeAst<ShiftExprAst>());

    case TK_PLUS:
        return std::make_pair(Precedence::Term, makeAst<AddExprAst>());

    case TK_MINUS:
        return std::make_pair(Precedence::Term, makeAst<SubExprAst>());

    case TK_STAR:
        return std::make_pair(Precedence::Factor, makeAst<MulExprAst>());

    case TK_SLASH:
        return std::make_pair(Precedence::Factor, makeAst<DivExprAst>());

    case TK_SLASH_SLASH:
        return std::make_pair(Precedence::Factor, makeAst<DivExprAst>());

    case TK_PERCENT:
        return std::make_pair(Precedence::Factor, makeAst<ModExprAst>());

    default:
        return std::make_pair(Precedence::Zero, std::unique_ptr<BinaryExprAst>());
    }
}

/*
 * file_input: (NEWLINE | stmt)* ENDMARKER
 */
bool PyParser::parse(Lexer* lexer, ParsingContext* context)
{
    UAISO_ASSERT(lexer, return false);
    UAISO_ASSERT(context && context->fileName(), return false);

    lexer_ = lexer;
    context_ = context;

    consumeToken();
    std::unique_ptr<StmtAstList> stmts;
    while (ahead_ != TK_EOP) {
        if (maybeConsume(TK_NEWLINE))
            continue;
        addToList(stmts, parseStmt().release());
    }

    if (!stmts)
        return false;

    // TODO: Wrap all into a declaration and make a program out of it.

    return true;
}

/*
 * stmt: simple_stmt | compound_stmt
 * simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
 * small_stmt: expr_stmt | print_stmt  | del_stmt | pass_stmt | flow_stmt |
 *             import_stmt | global_stmt | exec_stmt | assert_stmt
 * compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt | with_stmt |
 *                funcdef | classdef | decorated
 */
std::unique_ptr<StmtAst> PyParser::parseStmt()
{
    switch (ahead_) {
    case TK_PRINT:
    case TK_DELETE:
    case TK_PASS:
    case TK_IMPORT:
    case TK_GLOBAL:
    case TK_EXEC:
    case TK_ASSERT:
    case TK_BREAK:
    case TK_CONTINUE:
    case TK_RETURN:
    case TK_THROW:
    case TK_YIELD:
        return parseSimpleStmt();

    case TK_IF:
        return parseIfStmt();

    case TK_WHILE:
        return parseWhileStmt();

    case TK_FOR:
        return parseForStmt();

    case TK_TRY:
        return parseTryStmt();

    case TK_WITH:
        return parseWithStmt();

    case TK_FUNC:
        return parseFuncDef();

    case TK_CLASS:
        return parseClassDef();

    case TK_AT_SYMBOL:
        return parseDecorated();

    default:
        return parseSimpleStmt();
    }
}

/*
 * simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
 */
std::unique_ptr<StmtAst> PyParser::parseSimpleStmt()
{
    auto stmt = parseSmallStmt();
    if (maybeConsume(TK_NEWLINE))
        return stmt;

    auto block = makeAst<BlockStmtAst>();
    block->addStmt(stmt.release());
    while (maybeConsume(TK_SEMICOLON)) {
        if (ahead_ == TK_NEWLINE || ahead_ == TK_EOP)
            break;
        block->addStmt(parseSmallStmt().release());
    }

    if (!match(TK_NEWLINE)) {
        DEBUG_TRACE("parseSimpleStmt, skip to TK_NEWLINE\n");
        skipTo(TK_NEWLINE);
    }

    return Stmt(block.release());
}

/*
 * small_stmt: expr_stmt | print_stmt  | del_stmt | pass_stmt | flow_stmt |
 *             import_stmt | global_stmt | exec_stmt | assert_stmt
 */
std::unique_ptr<StmtAst> PyParser::parseSmallStmt()
{
    switch (ahead_) {
    case TK_PRINT:
        return parsePrintStmt();

    case TK_DELETE:
        return parseDelStmt();

    case TK_PASS:
        return parsePassStmt();

    case TK_IMPORT:
        return parseImportStmt();

    case TK_GLOBAL:
        return parseGlobalStmt();

    case TK_EXEC:
        return parseExecStmt();

    case TK_ASSERT:
        return parseAssertStmt();

    case TK_BREAK:
    case TK_CONTINUE:
    case TK_RETURN:
    case TK_THROW:
    case TK_YIELD:
        return parseFlowStmt();

    default:
        return parseExprStmt();
    }
}

std::unique_ptr<StmtAst> PyParser::parseExprStmt()
{
    return Stmt();
}

/*
 * print_stmt: 'print' ( [ test (',' test)* [','] ] |
 *                       '>>' test [ (',' test)+ [','] ] )
 */
std::unique_ptr<StmtAst> PyParser::parsePrintStmt()
{
    UAISO_ASSERT(ahead_ == TK_PRINT, return Stmt());

    consumeToken();
    auto print = makeAst<PrintExprAst>();
    print->setKeyLoc(lastLoc_);

    bool wantTest = false;
    if (maybeConsume(TK_GREATER_GREATER)) {
        print->setOprLoc(lastLoc_);
        print->addExpr(parseTest().release());
        if (!maybeConsume(TK_COMMA))
            return Stmt(makeAstRaw<ExprStmtAst>()->setExpr(print.release()));

        if (print->exprs())
            print->exprs()->lastSubList()->delim_ = lastLoc_;
        wantTest = true;
    }

    if (isTestAhead()) {
        print->addExpr(parseTest().release());
        if (maybeConsume(TK_COMMA)) {
            if (print->exprs())
                print->exprs()->lastSubList()->delim_ = lastLoc_;
            if (isTestAhead())
                print->mergeExprs(parseTestList().release());
        }
    } else if (wantTest) {
        failMatch(true);
    }

    return Stmt(makeAstRaw<ExprStmtAst>()->setExpr(print.release()));
}

std::unique_ptr<StmtAst> PyParser::parseDelStmt()
{
    UAISO_ASSERT(ahead_ == TK_DELETE, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parsePassStmt()
{
    UAISO_ASSERT(ahead_ == TK_PASS, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseFlowStmt()
{
    switch (ahead_) {
    case TK_BREAK:
        return parseBreakStmt();

    case TK_CONTINUE:
        return parseContinueStmt();

    case TK_RETURN:
        return parseReturnStmt();

    case TK_THROW:
        return parseThrowStmt();

    case TK_YIELD:
        return parseYieldStmt();

    default:
        UAISO_ASSERT(false, {});
        return Stmt();
    }
}

std::unique_ptr<StmtAst> PyParser::parseImportStmt()
{
    UAISO_ASSERT(ahead_ == TK_IMPORT, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseGlobalStmt()
{
    UAISO_ASSERT(ahead_ == TK_GLOBAL, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseExecStmt()
{
    UAISO_ASSERT(ahead_ == TK_EXEC, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseAssertStmt()
{
    UAISO_ASSERT(ahead_ == TK_ASSERT, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseCompoundStmt()
{
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseIfStmt()
{
    UAISO_ASSERT(ahead_ == TK_IF, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseWhileStmt()
{
    UAISO_ASSERT(ahead_ == TK_WHILE, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseForStmt()
{
    UAISO_ASSERT(ahead_ == TK_FOR, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseTryStmt()
{
    UAISO_ASSERT(ahead_ == TK_TRY, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseWithStmt()
{
    UAISO_ASSERT(ahead_ == TK_WITH, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseFuncDef()
{
    UAISO_ASSERT(ahead_ == TK_FUNC, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseClassDef()
{
    UAISO_ASSERT(ahead_ == TK_CLASS, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseDecorated()
{
    UAISO_ASSERT(ahead_ == TK_AT_SYMBOL, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseContinueStmt()
{
    UAISO_ASSERT(ahead_ == TK_CONTINUE, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseBreakStmt()
{
    UAISO_ASSERT(ahead_ == TK_BREAK, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseYieldStmt()
{
    UAISO_ASSERT(ahead_ == TK_YIELD, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseThrowStmt()
{
    UAISO_ASSERT(ahead_ == TK_THROW, return Stmt());
    return Stmt();
}

std::unique_ptr<StmtAst> PyParser::parseReturnStmt()
{
    UAISO_ASSERT(ahead_ == TK_THROW, return Stmt());
    return Stmt();
}

/*
 * exprlist: expr (',' expr)* [',']
 */
std::unique_ptr<ExprAstList> PyParser::parseExprList()
{
    return parseList<ExprAstList>(TK_COMMA,
                                  &PyParser::isExprAhead,
                                  &PyParser::parseExpr).first;
}

/*
 * test: or_test ['if' or_test 'else' test] | lambdef
 */
std::unique_ptr<ExprAst> PyParser::parseTest()
{
    if (ahead_ == TK_LAMBDA)
        return parseLambdaDef();

    auto orTest = parseOrTest();
    if (maybeConsume(TK_IF)) {
        // Must be a conditional.
        auto condExpr = makeAst<CondExprAst>();
        condExpr->setYes(orTest.release());
        condExpr->setQuestionLoc(lastLoc_);
        condExpr->setCond(parseOrTest().release());
        match(TK_ELSE);
        condExpr->setDelimLoc(lastLoc_);
        condExpr->setNo(parseTest().release());
        orTest.reset(condExpr.release());
    }

    return orTest;
}

/*
 * old_test: or_test | old_lambdef
 */
std::unique_ptr<ExprAst> PyParser::parseOldTest()
{
    if (ahead_ == TK_LAMBDA)
        return parseOldLambdaDef();

    return parseOrTest();
}

/*
 * testlist: test (',' test)* [',']
 */
std::unique_ptr<ExprAstList> PyParser::parseTestList()
{
    return parseList<ExprAstList>(TK_COMMA,
                                  &PyParser::isTestAhead,
                                  &PyParser::parseTest).first;
}

/*
 * argument: test [comp_for] | test '=' test
 */
std::unique_ptr<ExprAst> PyParser::parseArg()
{
    auto test = parseTest();
    switch (ahead_) {
    case TK_FOR: {
        auto listCompre = parseCompFor(makeAst<ListCompreExprAst>());
        listCompre->setExpr(test.release());
        return Expr(listCompre.release());
    }

    case TK_EQUAL:
        consumeToken();
        return completeAssignExpr(std::move(test), &PyParser::parseTest);

    default:
        return test;
    }
}

/*
 * arglist: (argument ',')* (argument [','] |
 *                           '*' test (',' argument)* [',' '**' test] |
 *                           '**' test)
 */
std::unique_ptr<ExprAstList> PyParser::parseArgList()
{
    ExprList args;
    if (isTestAhead()) {
        auto p = parseList<ExprAstList>(TK_COMMA, &PyParser::isTestAhead,
                                        &PyParser::parseArg);
        args = std::move(p.first);
        // If there's no trailing comma, this arg must be the last one.
        if (!p.second)
            return args;
    }

    bool wantStarStar = false;
    if (maybeConsume(TK_STAR)) {
        auto unpack = makeAst<UnpackExprAst>();
        unpack->setKeyLoc(lastLoc_);
        unpack->setExpr(parseTest().release());
        addToList(args, unpack.release());

        if (maybeConsume(TK_COMMA)) {
            if (args)
                args->lastSubList()->delim_ = lastLoc_;
            if (isTestAhead()) {
                auto p = parseList<ExprAstList>(TK_COMMA, &PyParser::isTestAhead,
                                                &PyParser::parseArg);
                mergeList(args, p.first.release());
                if (p.second)
                    wantStarStar = true;
            } else {
                wantStarStar = true;
            }
        }
    }

    if (maybeConsume(TK_STAR_STAR)) {
        auto unpack = makeAst<UnpackExprAst>();
        unpack->setKeyLoc(lastLoc_);
        unpack->setExpr(parseTest().release());
        addToList(args, unpack.release());
    } else if (wantStarStar) {
        failMatch(true);
    }

    return args;
}

/*
 * comp_iter: comp_for | comp_if
 * comp_for: 'for' exprlist 'in' or_test [comp_iter]
 */
std::unique_ptr<ListCompreExprAst>
PyParser::parseCompFor(std::unique_ptr<ListCompreExprAst> listCompre)
{
    UAISO_ASSERT(ahead_ == TK_FOR, return listCompre);
    UAISO_ASSERT(listCompre, return listCompre);

    consumeToken();
    listCompre->addGen(makeAstRaw<GeneratorAst>());
    auto gen = listCompre->gens()->back();
    gen->setKeyLoc(lastLoc_);
    gen->setPatterns(parseExprList().release());
    match(TK_IN);
    gen->setOprLoc(lastLoc_);
    gen->setRange(parseOrTest().release());

    if (ahead_ == TK_FOR)
        return parseCompFor(std::move(listCompre));

    if (ahead_ == TK_IF)
        return parseCompIf(std::move(listCompre));

    return std::move(listCompre);
}

/*
 * comp_iter: comp_for | comp_if
 * comp_if: 'if' old_test [comp_iter]
 */
std::unique_ptr<ListCompreExprAst>
PyParser::parseCompIf(std::unique_ptr<ListCompreExprAst> listCompre)
{
    UAISO_ASSERT(ahead_ == TK_IF, return listCompre);
    UAISO_ASSERT(listCompre, return listCompre);
    UAISO_ASSERT(listCompre->gens(), return listCompre);

    consumeToken();
    auto gen = listCompre->gens()->back();
    gen->addFilter(makeAstRaw<FilterAst>());
    auto filter = gen->filters()->back();
    filter->setOprLoc(lastLoc_);
    filter->setExpr(parseOldTest().release());

    if (ahead_ == TK_FOR)
        return parseCompFor(std::move(listCompre));

    if (ahead_ == TK_IF)
        return parseCompIf(std::move(listCompre));

    return std::move(listCompre);
}

/*
 * or_test: and_test ('or' and_test)*
 */
std::unique_ptr<ExprAst> PyParser::parseOrTest()
{
    auto andTest = parseAndTest();
    while (maybeConsume(TK_OR)) {
        andTest = completeBinaryExpr<LogicOrExprAst>(std::move(andTest),
                                                     &PyParser::parseAndTest);
    }
    return andTest;
}

/*
 * and_test: not_test ('and' not_test)*
 */
std::unique_ptr<ExprAst> PyParser::parseAndTest()
{
    auto notTest = parseNotTest();
    while (maybeConsume(TK_AND)) {
        notTest = completeBinaryExpr<LogicAndExprAst>(std::move(notTest),
                                                      &PyParser::parseNotTest);
    }
    return notTest;
}

/*
 * not_test: 'not' not_test | comparison
 */
std::unique_ptr<ExprAst> PyParser::parseNotTest()
{
    if (maybeConsume(TK_NOT)) {
        auto notTest = makeAst<LogicNotExprAst>();
        notTest->setOprLoc(lastLoc_);
        notTest->setExpr(parseNotTest().release());
        return Expr(notTest.release());
    }

    return parseComparison();
}

/*
 * comparison: expr (comp_op expr)*
 * comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|'in'|'not' 'in'|'is'|'is' 'not'
 */
std::unique_ptr<ExprAst> PyParser::parseComparison()
{
    auto expr = parseExpr();
    while (true) {
        switch (ahead_) {
        case TK_LESS:
        case TK_GREATER:
        case TK_EQUAL_EQUAL:
        case TK_GREATER_EQUAL:
        case TK_LESS_EQUAL:
        case TK_LESS_GREATER:
        case TK_EXCLAM_EQUAL:
            consumeToken();
            expr = completeBinaryExpr<RelExprAst>(std::move(expr), &PyParser::parseExpr);
            break;

        case TK_IN:
            consumeToken();
            expr = completeBinaryExpr<InExprAst>(std::move(expr), &PyParser::parseExpr);
            break;

        case TK_IS: // May be followed by `not`.
            consumeToken();
            maybeConsume(TK_NOT);
            expr = completeBinaryExpr<IsExprAst>(std::move(expr), &PyParser::parseExpr);
            break;

        case TK_NOT: // Must be followed by `in`.
            consumeToken();
            match(TK_IN);
            expr = completeBinaryExpr<InExprAst>(std::move(expr), &PyParser::parseExpr);
            break;

        default:
            return expr;
        }
    }
}

/*
 * expr: xor_expr ('|' xor_expr)*
 * xor_expr: and_expr ('^' and_expr)*
 * and_expr: shift_expr ('&' shift_expr)*
 * shift_expr: arith_expr (('<<'|'>>') arith_expr)*
 * arith_expr: term (('+'|'-') term)*
 * term: factor (('*'|'/'|'%'|'//') factor)*
 */
std::unique_ptr<ExprAst> PyParser::parseExpr()
{
    return parseBinaryExpr(Precedence::Or);
}

std::unique_ptr<ExprAst> PyParser::parseBinaryExpr(Precedence curPrec)
{
    auto factor = parseFactor();
    while (true) {
        Precedence prec;
        std::unique_ptr<BinaryExprAst> bin;
        std::tie(prec, bin) = fetchPrecAhead();
        if (prec < curPrec)
            break;

        consumeToken();
        bin->setOprLoc(lastLoc_);
        bin->setExpr1(factor.release());
        bin->setExpr2(parseBinaryExpr(Precedence(curPrec + 1)).release());
        factor.reset(bin.release());
    }

    return factor;
}

/*
 * factor: ('+'|'-'|'~') factor | power
 */
std::unique_ptr<ExprAst> PyParser::parseFactor()
{
    std::unique_ptr<UnaryExprAst> expr;
    switch (ahead_) {
    case TK_PLUS:
        consumeToken();
        return completeUnaryExpr<PlusExprAst>(&PyParser::parseFactor);

    case TK_MINUS:
        consumeToken();
        return completeUnaryExpr<MinusExprAst>(&PyParser::parseFactor);

    case TK_TILDE:
        consumeToken();
        return completeUnaryExpr<BitCompExprAst>(&PyParser::parseFactor);

    default:
        return parsePower();
    }
}

/*
 * power: atom trailer* ['**' factor]
 * trailer: '(' [arglist] ')' | '[' subscriptlist ']' | '.' NAME
 */
std::unique_ptr<ExprAst> PyParser::parsePower()
{
    auto atom = parseAtom();
    bool trailer = true;
    while (trailer) {
        switch (ahead_) {
        case TK_LPAREN: {
            consumeToken();
            auto call = makeAst<CallExprAst>();
            call->setLDelimLoc(lastLoc_);
            call->setBase(atom.release());
            call->setArgs(parseArgList().release());
            if (!match(TK_RPAREN)) {
                DEBUG_TRACE("parsePower, skip to TK_RPAREN\n");
                skipTo(TK_RPAREN);
            }
            call->setRDelimLoc(lastLoc_);
            atom.reset(call.release());
            break;
        }

        // TODO
        case TK_LBRACKET:
            break;

        case TK_DOT: {
            consumeToken();
            auto access = makeAst<MemberAccessExprAst>();
            access->setOprLoc(lastLoc_);
            access->setExpr(atom.release());
            match(TK_IDENTIFIER);
            access->setName(completeName().release());
            atom.reset(access.release());
            break;
        }

        default:
            trailer = false;
            break;
        }
    }

    if (maybeConsume(TK_STAR_STAR))
        return completeBinaryExpr<PowerExprAst>(std::move(atom), &PyParser::parseFactor);

    return atom;
}

/*
 * atom: ('(' [yield_expr|testlist_comp] ')' |
 *        '[' [listmaker] ']' |
 *        '{' [dictorsetmaker] '}' |
 *        '`' testlist1 '`' |
 *        NAME | NUMBER | STRING+)
 */
std::unique_ptr<ExprAst> PyParser::parseAtom()
{
    switch (ahead_) {
    // TODO
    case TK_LPAREN:
    case TK_RPAREN:
    case TK_LBRACE:
    case TK_BACKTICK:
        return Expr();

    case TK_IDENTIFIER: {
        consumeToken();
        auto ident = makeAst<IdentExprAst>();
        ident->setName(completeName().release());
        return Expr(ident.release());
    }

    case TK_INTEGER_LITERAL:
    case TK_FLOAT_LITERAL:
        consumeToken();
        return completeLitExpr<NumLitExprAst>();

    case TK_STRING_LITERAL:
        consumeToken();
        return completeLitExpr<StrLitExprAst>();

    default:
        failMatch(true);
        return Expr();
    }
}

std::unique_ptr<ExprAst> PyParser::parseLambdaDef()
{
    UAISO_ASSERT(ahead_ == TK_LAMBDA, return Expr());
    // TODO: FuncSpecAst in FuncLitExprAst
    return Expr();
}

std::unique_ptr<ExprAst> PyParser::parseOldLambdaDef()
{
    UAISO_ASSERT(ahead_ == TK_LAMBDA, return Expr());
    return Expr();
}

std::unique_ptr<ExprAstList> PyParser::parseSubscriptList()
{
    return ExprList();
}

std::unique_ptr<NameAst> PyParser::completeName()
{
    auto name = makeAst<SimpleNameAst>();
    name->setNameLoc(lastLoc_);
    return Name(name.release());
}

template <class LitAstT>
std::unique_ptr<ExprAst> PyParser::completeLitExpr()
{
    auto lit = makeAst<LitAstT>();
    lit->setLitLoc(lastLoc_);
    return Expr(lit.release());
}

template <class UnaryAstT> std::unique_ptr<ExprAst>
PyParser::completeUnaryExpr(Expr (PyParser::*parseFunc) ())
{
    auto unary = makeAst<UnaryAstT>();
    unary->setOprLoc(lastLoc_);
    unary->setExpr((((this)->*(parseFunc))()).release());
    return Expr(unary.release());
}

template <class BinaryAstT> std::unique_ptr<ExprAst>
PyParser::completeBinaryExpr(Expr expr, Expr (PyParser::*parseFunc) ())
{
    auto bin = makeAst<BinaryAstT>();
    bin->setOprLoc(lastLoc_);
    bin->setExpr1(expr.release());
    bin->setExpr2((((this)->*(parseFunc))()).release());
    return Expr(bin.release());
}

std::unique_ptr<ExprAst>
PyParser::completeAssignExpr(Expr expr, Expr (PyParser::*parseFunc) ())
{
    auto assign = makeAst<AssignExprAst>();
    assign->setOprLoc(lastLoc_);
    assign->setExpr1s(ExprAstList::create(expr.release()));
    assign->setExpr2s(ExprAstList::create((((this)->*(parseFunc))()).release()));
    return Expr(assign.release());
}

/*
 * Parse a list that accepts a trailing separator (like a comma): item (',' item)* [',']
 */
template <class AstListT> std::pair<std::unique_ptr<AstListT>, bool>
PyParser::parseList(Token tk,
                    bool (PyParser::*checkAhead) () const,
                    std::unique_ptr<typename AstListT::AstType> (PyParser::*parseFunc) ())
{
    using List = std::unique_ptr<AstListT>;

    List list;
    auto item = ((this)->*(parseFunc))();
    addToList(list, item.release());
    while (maybeConsume(tk)) {
        if (list)
            list->delim_ = lastLoc_;
        // A trailing separator is OK.
        if (!(((this)->*(checkAhead))()))
            return std::make_pair(std::move(list), true);

        addToList(list, (((this)->*(parseFunc))()).release());
    }

    return std::make_pair(std::move(list), false);
}
