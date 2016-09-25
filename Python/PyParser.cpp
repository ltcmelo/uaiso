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


std::pair<PyParser::Precedence, std::unique_ptr<BinExprAst>>
PyParser::fetchPrecAhead() const
{
    switch (ahead_) {
    case TK_PIPE:
        return std::make_pair(Precedence::Or, BitOrExprAst::create());

    case TK_CARET:
        return std::make_pair(Precedence::Xor, BitXorExprAst::create());

    case TK_AMPER:
        return std::make_pair(Precedence::And, BitAndExprAst::create());

    case TK_LS_LS:
    case TK_GR_GR:
        return std::make_pair(Precedence::Shift, ShiftExprAst::create());

    case TK_PLUS:
        return std::make_pair(Precedence::Term, AddExprAst::create());

    case TK_MINUS:
        return std::make_pair(Precedence::Term, SubExprAst::create());

    case TK_STAR:
        return std::make_pair(Precedence::Factor, MulExprAst::create());

    case TK_SLASH:
        return std::make_pair(Precedence::Factor, DivExprAst::create());

    case TK_SLASH_SLASH:
        return std::make_pair(Precedence::Factor, DivExprAst::create());

    case TK_PERCENT:
        return std::make_pair(Precedence::Factor, ModExprAst::create());

    default:
        return std::make_pair(Precedence::Zero, std::unique_ptr<BinExprAst>());
    }
}

/*
 * file_input: (NEWLINE | stmt)* ENDMARKER
 */
bool PyParser::parse(Lexer* lexer, ParsingContext* context)
{
    UAISO_ASSERT(lexer, return false);
    UAISO_ASSERT(context && context->fileName(), return false);

    setLexer(lexer);
    setContext(context);
    consumeToken();
    StmtList stmts;
    while (ahead_ != TK_EOP) {
        if (maybeConsume(TK_NEWLINE))
            continue;
        appendOrCreate(stmts, parseStmt());
    }

    if (stmts) {
        auto prog = std::unique_ptr<ProgramAst>(newAst<ProgramAst>());
        prog->setStmts(std::move(stmts));
        context->takeAst(std::unique_ptr<Ast>(prog.release()));
        return true;
    }

    return false;
}

/*
 * stmt: simple_stmt | compound_stmt
 * simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
 * small_stmt: expr_stmt | print_stmt  | del_stmt | pass_stmt | flow_stmt |
 *             import_stmt | global_stmt | exec_stmt | assert_stmt
 * compound_stmt: if_stmt | while_stmt | for_stmt | try_stmt | with_stmt |
 *                funcdef | classdef | decorated
 */
Parser::Stmt PyParser::parseStmt()
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
    case TK_RAISE:
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

    case TK_DEF:
        return parseFuncDef();

    case TK_CLASS:
        return parseClassDef();

    case TK_AT:
        return parseDecorated();

    default:
        return parseSimpleStmt();
    }
}

/*
 * simple_stmt: small_stmt (';' small_stmt)* [';'] NEWLINE
 */
Parser::Stmt PyParser::parseSimpleStmt()
{
    auto stmt = parseSmallStmt();
    if (maybeConsume(TK_NEWLINE))
        return stmt;

    auto block = BlockStmtAst::create();
    block->addStmt(std::move(stmt));
    while (maybeConsume(TK_SEMICOLON)) {
        if (ahead_ == TK_NEWLINE || ahead_ == TK_EOP)
            break;
        block->addStmt(parseSmallStmt());
    }
    matchOrSkipTo(TK_NEWLINE, "parseSimpleStmt");
    return std::move(block);
}

/*
 * small_stmt: expr_stmt | print_stmt  | del_stmt | pass_stmt | flow_stmt |
 *             import_stmt | global_stmt | exec_stmt | assert_stmt
 */
Parser::Stmt PyParser::parseSmallStmt()
{
    switch (ahead_) {
    case TK_PRINT:
        return parsePrintStmt();

    case TK_DELETE:
        return parseDelStmt();

    case TK_PASS:
        return parsePassStmt();

    case TK_IMPORT:
    case TK_FROM:
        return parseImportStmt();

    case TK_GLOBAL:
        return parseGlobalStmt();

    case TK_EXEC:
        return parseExecStmt();

    case TK_ASSERT:
        return parseAssertStmt();

    case TK_BREAK:
        return parseBreakStmt();

    case TK_CONTINUE:
        return parseContinueStmt();

    case TK_RETURN:
        return parseReturnStmt();

    case TK_RAISE:
        return parseRaiseStmt();

    case TK_YIELD:
        return parseYieldStmt();

    default:
        return parseExprStmt();
    }
}

/*
 * expr_stmt: testlist (augassign (yield_expr|testlist) |
 *                      ('=' (yield_expr|testlist))*)
 * augassign: ('+=' | '-=' | '*=' | '/=' | '%=' | '&=' | '|=' | '^=' |
 *             '<<=' | '>>=' | '**=' | '//=')
 */
Parser::Stmt PyParser::parseExprStmt()
{
    auto exprs = parseTestList();
    bool augmented = false;
    while (true) {
        switch (ahead_) {
        case TK_PLUS_EQ:
        case TK_MINUS_EQ:
        case TK_STAR_EQ:
        case TK_SLASH_EQ:
        case TK_PERCENT_EQ:
        case TK_AMPER_EQ:
        case TK_PIPE_EQ:
        case TK_CARET_EQ:
        case TK_LS_LS_EQ:
        case TK_GR_GR_EQ:
        case TK_STAR_STAR_EQ:
        case TK_SLASH_SLASH_EQ:
            augmented = true;
            // Fallthrough

        case TK_EQ: {
            consumeToken();
            // DESIGN: Proper model this expression and make expression
            // statement take only an expression instead of a list.
            auto assign = AssignExprAst::create();
            assign->setOprLoc(prevLoc_);
            assign->setExpr1s(std::move(exprs));
            if (ahead_ == TK_YIELD)
                assign->setExpr2s(ExprAstList::create(parseYieldExpr()));
            else
                assign->setExpr2s(parseTestList());
            exprs = ExprAstList::create(std::move(assign));

            if (augmented || ahead_ != TK_EQ)
                return ExprStmtAst::create(std::move(exprs));

            break;
        }

        default:
            return ExprStmtAst::create(std::move(exprs));
        }
    }
}

/*
 * print_stmt: 'print' ( [ test (',' test)* [','] ] |
 *                       '>>' test [ (',' test)+ [','] ] )
 */
Parser::Stmt PyParser::parsePrintStmt()
{
    UAISO_ASSERT(ahead_ == TK_PRINT, return Stmt());

    auto print = PrintExprAst::create();
    consumeToken();
    print->setKeyLoc(prevLoc_);

    bool wantTest = false;
    if (maybeConsume(TK_GR_GR)) {
        print->setOprLoc(prevLoc_);
        print->addExpr(parseTest());
        if (!maybeConsume(TK_COMMA))
            return ExprStmtAst::create(std::move(print));

        if (print->exprs())
            print->exprs()->lastSubList()->delim_ = prevLoc_;
        wantTest = true;
    }

    if (wantTest || isTestFIRST()) {
        print->addExpr(parseTest());
        if (maybeConsume(TK_COMMA)) {
            if (print->exprs())
                print->exprs()->lastSubList()->delim_ = prevLoc_;
            if (isTestFIRST())
                print->mergeExprs(parseTestList());
        }
    }

    return ExprStmtAst::create(std::move(print));
}

/*
 * del_stmt: 'del' exprlist
 */
Parser::Stmt PyParser::parseDelStmt()
{
    UAISO_ASSERT(ahead_ == TK_DELETE, return Stmt());

    auto del = DelExprAst::create();
    consumeToken();
    del->setKeyLoc(prevLoc_);
    del->setExprs(parseExprList());
    return ExprStmtAst::create(std::move(del));
}

/*
 * pass_stmt: 'pass'
 */
Parser::Stmt PyParser::parsePassStmt()
{
    UAISO_ASSERT(ahead_ == TK_PASS, return Stmt());

    consumeToken();
    return EmptyStmtAst::create(prevLoc_);
}

/*
 * import_stmt: import_name | import_from
 * import_name: 'import' dotted_as_names
 * import_from: 'from' ('.'* dotted_name | '.'+) 'import' sub_import
 */
Parser::Stmt PyParser::parseImportStmt()
{
    UAISO_ASSERT(ahead_ == TK_IMPORT || ahead_ == TK_FROM, return Stmt());

    switch (ahead_) {
    case TK_IMPORT: {
        consumeToken();
        auto import = ImportGroupDeclAst::create();
        import->setKeyLoc(prevLoc_);
        do {
            if (import->modules_)
                import->modules_->delim_ = prevLoc_;
            auto module = ImportDeclAst::create();
            module->setTarget(IdentExprAst::create(parseDottedName()));
            if (maybeConsume(TK_AS)) {
                module->setAsLoc(prevLoc_);
                module->setLocalName(parseName());
            }
            import->addModule(std::move(module));
        } while (maybeConsume(TK_COMMA));
        if (!import->modules_)
            fail();

        return DeclStmtAst::create(std::move(import));
    }

    case TK_FROM: {
        consumeToken();
        auto import = ImportGroupDeclAst::create();
        import->setKeyLoc(prevLoc_);

        // DESIGN: Store the dots (relative location info).
        bool wantName = true;
        while (maybeConsume(TK_DOT) || maybeConsume(TK_DOT_DOT_DOT))
            wantName = false;

        // Confusing rules... The thing comming after 'from' might be a module,
        // preceeded or not by dots (relative indication), or dots alone. In
        // this latter case, specifying the folder of the upcoming 'import'.
        // Details in PEP 0328:
        //
        // package/
        //     __init__.py
        //     subpackage1/
        //         __init__.py
        //         moduleX.py
        //         moduleY.py
        //     subpackage2/
        //         __init__.py
        //         moduleZ.py
        //     moduleA.py
        //
        // from .moduleY import spam
        // from .moduleY import spam as ham
        // from . import moduleY
        // from ..subpackage1 import moduleY
        // from ..subpackage2.moduleZ import eggs
        // from ..moduleA import foo
        // from ...package import bar
        // from ...sys import path
        // from .. import moduleY

        if (wantName || isNameFIRST()) {
            // A selective import, items specified after 'import'.
            auto module = ImportDeclAst::create();
            module->setTarget(IdentExprAst::create(parseDottedName()));
            match(TK_IMPORT);
            module->setLDelimLoc(prevLoc_);
            if (maybeConsume(TK_STAR))
                module->setMode(SimpleNameAst::create(prevLoc_));
            else
                module->setSelections(parseSubImports(true));
            import->addModule(std::move(module));
        } else {
            // An "ordinary" (non-selective) import, 'from' is just to indicate
            // the module after 'import' is relative.
            match(TK_IMPORT);
            import->setHintLoc(prevLoc_);
            import->setModules(parseSubImports(false));
        }

        return DeclStmtAst::create(std::move(import));
    }

    default:
        fail();
        return ErrorStmtAst::create(prevLoc_);
    }
}

/*
 * sub_import: ('*' | '(' import_as_names ')' | import_as_names)
 * import_as_name: NAME ['as' NAME]
 * import_as_names: import_as_name (',' import_as_name)* [',']
 */
std::unique_ptr<DeclAstList> PyParser::parseSubImports(bool selective)
{
    bool wantParen = false;
    switch (ahead_) {
    case TK_LPAREN:
        consumeToken();
        wantParen = true;
        // Fallthrough

    default:
        DeclList decls;
        if (selective) {
            do {
                if (decls)
                    decls->delim_ = prevLoc_;
                auto item = ImportSelectionDeclAst::create();
                item->setActualName(parseName());
                if (maybeConsume(TK_AS)) {
                    item->setAsLoc(prevLoc_);
                    item->setAlternateName(parseName());
                }
                appendOrCreate(decls, std::move(item));
            } while (maybeConsume(TK_COMMA));
        } else {
            do {
                if (decls)
                    decls->delim_ = prevLoc_;
                auto module = ImportDeclAst::create();
                module->setTarget(IdentExprAst::create(parseName()));
                if (maybeConsume(TK_AS)) {
                    module->setAsLoc(prevLoc_);
                    module->setLocalName(parseName());
                }
                appendOrCreate(decls, std::move(module));
            } while (maybeConsume(TK_COMMA));
        }
        if (wantParen && !match(TK_RPAREN)) {
            DEBUG_TRACE("parseSubImports, skip to TK_RPAREN\n");
            skipTo(TK_RPAREN);
        }
        if (!decls)
            fail();

        return decls;
    }
}

/*
 * global_stmt: 'global' NAME (',' NAME)*
 */
Parser::Stmt PyParser::parseGlobalStmt()
{
    UAISO_ASSERT(ahead_ == TK_GLOBAL, return Stmt());

    auto group = VarGroupDeclAst::create();
    consumeToken();
    group->setSpec(InferredSpecAst::create());
    group->setKeyLoc(prevLoc_);
    group->addDecl(VarDeclAst::create(parseName()));
    while (maybeConsume(TK_COMMA)) {
        group->decls_->delim_ = prevLoc_;
        group->addDecl(VarDeclAst::create(parseName()));
    }
    return DeclStmtAst::create(std::move(group));
}

/*
 * exec_stmt: 'exec' expr ['in' test [',' test]]
 */
Parser::Stmt PyParser::parseExecStmt()
{
    UAISO_ASSERT(ahead_ == TK_EXEC, return Stmt());

    auto eval = EvalStmtAst::create();
    consumeToken();
    eval->setKeyLoc(prevLoc_);
    eval->setExpr(parseExpr());

    // DESIGN: Model remaining items.
    if (maybeConsume(TK_IN)) {
        parseTest();
        if (maybeConsume(TK_COMMA))
            parseTest();
    }

    return std::move(eval);
}

/*
 * assert_stmt: 'assert' test [',' test]
 */
Parser::Stmt PyParser::parseAssertStmt()
{
    UAISO_ASSERT(ahead_ == TK_ASSERT, return Stmt());

    auto expr = AssertExprAst::create();
    consumeToken();
    expr->setKeyLoc(prevLoc_);
    expr->setExpr(parseTest());

    // TODO: Generalize "message" from AssertExprAst to "action".
    if (maybeConsume(TK_COMMA))
        parseTest();

    return ExprStmtAst::create(std::move(expr));
}

/*
 * if_stmt: 'if' if_else
 */
Parser::Stmt PyParser::parseIfStmt()
{
    UAISO_ASSERT(ahead_ == TK_IF, return Stmt());

    consumeToken();
    return parseIfElseIfStmt();
}

/*
 * if_else: test ':' suite ('elif' test ':' suite)* ['else' ':' suite]
 *
 * Note: 'elif' parsed as 'else' 'if'.
 */
Parser::Stmt PyParser::parseIfElseIfStmt()
{
    auto iff = IfStmtAst::create();
    iff->setIfLoc(prevLoc_);
    iff->setExpr(parseTest());
    match(TK_COLON);
    iff->setThen(parseSuite());
    if (maybeConsume(TK_ELIF)) {
        iff->setElseLoc(prevLoc_);
        iff->setNotThen(parseIfElseIfStmt());
    } else if (maybeConsume(TK_ELSE)) {
        iff->setElseLoc(prevLoc_);
        match(TK_COLON);
        iff->setNotThen(parseSuite());
    }

    return std::move(iff);
}

/*
 * while_stmt: 'while' test ':' suite ['else' ':' suite]
 */
Parser::Stmt PyParser::parseWhileStmt()
{
    UAISO_ASSERT(ahead_ == TK_WHILE, return Stmt());

    auto whyle = WhileStmtAst::create();
    consumeToken();
    whyle->setWhileLoc(prevLoc_);
    whyle->setExpr(parseTest());
    match(TK_COLON);
    whyle->setStmt(parseSuite());

    if (maybeConsume(TK_ELSE)) {
        match(TK_COLON);
        // DESIGN: WhileStmtAst needs an else clause.
        parseSuite();
    }

    return std::move(whyle);
}

/*
 * for_stmt: 'for' exprlist 'in' testlist ':' suite ['else' ':' suite]
 */
Parser::Stmt PyParser::parseForStmt()
{
    UAISO_ASSERT(ahead_ == TK_FOR, return Stmt());

    auto four = ForeachStmtAst::create();
    consumeToken();
    // Convert the exprs (when plain identifiers) into var decls.
    auto exprs = parseExprList();
    auto group = VarGroupDeclAst::create();
    group->setSpec(InferredSpecAst::create());
    for (auto expr : *exprs) {
        if (expr->kind() != Ast::Kind::IdentExpr)
            continue;
        group->addDecl(VarDeclAst::create(std::move(IdentExpr_Cast(expr)->name_)));
    }
    four->setDecl(std::move(group));

    match(TK_IN);
    auto test = parseTestList();
    if (test) {
        // DESIGN: Extend ForeachStmtAst to allow exprs? For now, take the first one.
        four->setExpr(std::move(test->detachHead().first));
    }
    match(TK_COLON);
    four->setStmt(parseSuite());

    if (maybeConsume(TK_ELSE)) {
        match(TK_COLON);
        // DESIGN: ForeachStmtAst needs an else clause.
        parseSuite();
    }

    return std::move(four);
}

/*
 * try_stmt: ('try' ':' suite
 *            ((except_clause ':' suite)+
 *             ['else' ':' suite]
 *             ['finally' ':' suite] |
 *             'finally' ':' suite))
 * except_clause: 'except' [test [('as' | ',') test]]
 */
Parser::Stmt PyParser::parseTryStmt()
{
    UAISO_ASSERT(ahead_ == TK_TRY, return Stmt());

    auto trie = TryStmtAst::create();
    consumeToken();
    trie->setKeyLoc(prevLoc_);
    match(TK_COLON);
    trie->setStmt(parseSuite());

    bool seenElse = false;
    while (true) {
        switch (ahead_) {
        case TK_EXCEPT: {
            consumeToken();
            auto catche = CatchClauseStmtAst::create();
            catche->setKeyLoc(prevLoc_);
            if (isTestFIRST()) {
                auto test = parseTest();
                if (test) {
                    auto group = ParamGroupDeclAst::create();
                    if (test->kind() != Ast::Kind::IdentExpr) {
                        // TODO: This is an error, right?
                    } else {
                        group->setSpec(NamedSpecAst::create(std::move(
                                IdentExpr_Cast(test.get())->name_)));
                    }
                    if (maybeConsume(TK_AS) || maybeConsume(TK_COMMA)) {
                        auto ident = parseTest();
                        if (ident) {
                            if (ident->kind() != Ast::Kind::IdentExpr) {
                                context_->trackReport(Diagnostic::IdentifierExpected,
                                                      prevLoc_);
                            } else {
                                group->addDecl(ParamDeclAst::create(std::move(
                                    IdentExpr_Cast(ident.get())->name_)));
                            }
                        }
                    }
                    catche->setDecl(std::move(group));
                }
            }
            match(TK_COLON);
            catche->setStmt(parseSuite());
            trie->addCatch(std::move(catche));
            break;
        }

        case TK_FINALLY: {
            consumeToken();
            auto finaly = FinallyClauseStmtAst::create();
            finaly->setKeyLoc(prevLoc_);
            match(TK_COLON);
            finaly->setStmt(parseSuite());
            trie->setFinal(std::move(finaly));
            return std::move(trie);
        }

        case TK_ELSE: {
            if (!trie->catchs() || seenElse) {
                // TODO: Once else is added to stmt, check against it (no seenElse).
                fail();
            } else {
                consumeToken();
                match(TK_COLON);
                // DESIGN: ForeachStmtAst needs an else clause.
                parseSuite();
                seenElse = true;
            }
            break;
        }

        default:
            // Check for 'except' only ('finally' always returns).
            if (!trie->catchs())
                fail();
            return std::move(trie);
        }
    }
}

/*
 * with_stmt: 'with' with_item (',' with_item)* ':' suite
 */
Parser::Stmt PyParser::parseWithStmt()
{
    UAISO_ASSERT(ahead_ == TK_WITH, return Stmt());

    auto with = WithStmtAst::create();
    consumeToken();
    with->setKeyLoc(prevLoc_);
    with->setExprs(parseDSeq<ExprAstList, PyParser>(TK_COMMA, &PyParser::parseWithItem));
    if (!with->exprs())
        fail();
    matchOrSkipTo(TK_COLON, "parseWithStmt");
    with->setStmt(parseSuite());

    return std::move(with);
}

/*
 * with_item: test ['as' expr]
 */
Parser::Expr PyParser::parseWithItem()
{
    auto test = parseTest();
    if (maybeConsume(TK_AS)) {
        auto assign = AssignExprAst::create();
        assign->setOprLoc(prevLoc_);
        assign->addExpr1(std::move(test));
        assign->addExpr2(parseExpr());
        return Expr(std::move(assign));
    }

    return test;
}

/*
 * varargslist: (fpdef ['=' test] ',')*
 *              ('*' NAME [',' '**' NAME] | '**' NAME) |
 *              fpdef ['=' test] (',' fpdef ['=' test])* [',']
 * fpdef: NAME | '(' fplist ')'
 * fplist: fpdef (',' fpdef)* [',']
 */
Parser::Decl PyParser::parseVarArgsList(bool wantParen)
{
    auto clause = ParamClauseDeclAst::create();
    if (wantParen) {
        match(TK_LPAREN);
        clause->setLDelimLoc(prevLoc_);
    }

    bool seenStar = false;
    bool takeComma = true;
    do {
        if (clause->decls_)
            clause->decls_->lastSubList()->delim_ = prevLoc_;

        auto group = ParamGroupDeclAst::create();
        group->setSpec(InferredSpecAst::create());
        switch (ahead_) {
        case TK_COMPLETION:
        case TK_IDENT: {
            if (seenStar)
                break;
            auto name = parseName();
            if (maybeConsume(TK_EQ)) {
                auto param = std::unique_ptr<ParamDeclAst__<
                        ParamVariadic__Empty__, ParamDefaultArg__>>(newAst<
                        ParamDeclAst__<ParamVariadic__Empty__, ParamDefaultArg__>>());
                param->setAssignLoc(prevLoc_);
                param->setName(std::move(name));
                param->setDefaultArg(parseTest().release());
                group->addDecl(std::move(param));
            } else {
                group->addDecl(ParamDeclAst::create(std::move(name)));
            }
            clause->addDecl(std::move(group));
            break;
        }

        case TK_STAR: {
            if (seenStar)
                break;
            seenStar = true;
            consumeToken();
            clause->addDecl(completeParam(std::move(group)));
            break;
        }

        case TK_STAR_STAR: {
            consumeToken();
            clause->addDecl(completeParam(std::move(group)));
            // Fallthrough
        }

        default:
            takeComma = false;
            break;
        }
    } while (takeComma && maybeConsume(TK_COMMA));
    if (wantParen)
        matchOrSkipTo(TK_RPAREN, "parseFuncDef");

    return std::move(clause);
}

/*
 * funcdef: 'def' NAME parameters ':' suite
 * parameters: '(' [varargslist] ')'
 */
Parser::Stmt PyParser::parseFuncDef()
{
    UAISO_ASSERT(ahead_ == TK_DEF, return Stmt());

    auto spec = std::unique_ptr<FuncSpecAst__<>>(newAst<FuncSpecAst__<>>());
    consumeToken();
    spec->setKeyLoc(prevLoc_);
    auto decl = FuncDeclAst::create();
    decl->setName(parseName());
    spec->setParam(parseVarArgsList(true));

    match(TK_COLON);
    spec->setLDelimLoc(prevLoc_);
    decl->setStmt(parseSuite());
    decl->setSpec(std::move(spec));

    return DeclStmtAst::create(std::move(decl));
}

/*
 * classdef: 'class' NAME ['(' [testlist] ')'] ':' suite
 */
Parser::Stmt PyParser::parseClassDef()
{
    UAISO_ASSERT(ahead_ == TK_CLASS, return Stmt());

    auto spec = RecordSpecAst::create();
    consumeToken();
    spec->setKeyLoc(prevLoc_);
    auto decl = RecordDeclAst::create();
    decl->setName(parseName());

    if (maybeConsume(TK_LPAREN)) {
        auto bases = parseTestList();
        for (auto base : *bases) {
            if (base->kind() != Ast::Kind::IdentExpr)
                continue;
            spec->addBase(BaseDeclAst::create(std::move(IdentExpr_Cast(base)->name_)));
        }
        matchOrSkipTo(TK_RPAREN, "parseClassDef");
    }

    match(TK_COLON);
    spec->setLDelimLoc(prevLoc_);
    spec->setProto(parseSuite());
    decl->setSpec(std::move(spec));

    return DeclStmtAst::create(std::move(decl));
}

/*
 * decorated: decorators (classdef | funcdef)
 * decorators: decorator+
 * decorator: '@' dotted_name [ '(' [arglist] ')' ] NEWLINE
 */
Parser::Stmt PyParser::parseDecorated()
{
    UAISO_ASSERT(ahead_ == TK_AT, return Stmt());

    consumeToken();
    do {
        // TODO: Work on decorators.
        parseDottedName();
        if (maybeConsume(TK_LPAREN)) {
            parseArgList();
            matchOrSkipTo(TK_RPAREN, "parseDecorated");
        }
        matchOrSkipTo(TK_NEWLINE, "parseDecorated");
    } while (maybeConsume(TK_AT));

    switch (ahead_) {
    case TK_CLASS:
        return parseClassDef();

    case TK_DEF:
        return parseFuncDef();

    default:
        fail();
        return ErrorStmtAst::create(prevLoc_);
    }
}

/*
 * continue_stmt: 'continue'
 */
Parser::Stmt PyParser::parseContinueStmt()
{
    UAISO_ASSERT(ahead_ == TK_CONTINUE, return Stmt());
    consumeToken();
    return ContinueStmtAst::create(prevLoc_);
}

/*
 * break_stmt: 'break'
 */
Parser::Stmt PyParser::parseBreakStmt()
{
    UAISO_ASSERT(ahead_ == TK_BREAK, return Stmt());
    consumeToken();
    return BreakStmtAst::create(prevLoc_);
}

/*
 * yield_stmt: yield_expr
 */
Parser::Stmt PyParser::parseYieldStmt()
{
    auto yield = YieldStmtAst::create();
    yield->setExpr(parseYieldExpr());
    return std::move(yield);
}

/*
 * raise_stmt: 'raise' [test [',' test [',' test]]]
 */
Parser::Stmt PyParser::parseRaiseStmt()
{
    UAISO_ASSERT(ahead_ == TK_RAISE, return Stmt());

    auto stmt = ThrowStmtAst::create();
    consumeToken();
    stmt->setKeyLoc(prevLoc_);
    if (isTestFIRST()) {
        stmt->setExpr(parseTest());

        // DESIGN: Model remaining exprs.
        if (maybeConsume(TK_COMMA)) {
            parseTest();
            if (maybeConsume(TK_COMMA))
                parseTest();
        }
    }

    return std::move(stmt);
}

/*
 * return_stmt: 'return' [testlist]
 */
Parser::Stmt PyParser::parseReturnStmt()
{
    UAISO_ASSERT(ahead_ == TK_RETURN, return Stmt());

    auto stmt = ReturnStmtAst::create();
    consumeToken();
    stmt->setKeyLoc(prevLoc_);
    if (isTestFIRST())
        stmt->setExprs(parseTestList());
    return std::move(stmt);
}

/*
 * suite: simple_stmt | NEWLINE INDENT stmt+ DEDENT
 */
Parser::Stmt PyParser::parseSuite()
{
    if (!maybeConsume(TK_NEWLINE))
        return parseSimpleStmt();

    // Special handling that allows triggering of completion with a more
    // relaxed location requirement, where an indent would be expected.
    if (ahead_ == TK_COMPLETION) {
        auto ident = IdentExprAst::create();
        ident->setName(parseName());
        return Stmt(newAst<ExprStmtAst>()->addExpr(std::move(ident)));
    }

    match(TK_INDENT);
    auto block = BlockStmtAst::create();
    block->addStmt(parseStmt());
    while (!maybeConsume(TK_DEDENT) && ahead_ != TK_EOP)
        block->addStmt(parseStmt());

    return std::move(block);
}

/*
 * exprlist: expr (',' expr)* [',']
 */
std::unique_ptr<ExprAstList> PyParser::parseExprList()
{
    return parseDSeqTrail<ExprAstList, PyParser>(
                TK_COMMA,
                [] (const Token tk) {
                    return tk == TK_IN
                            || tk == TK_SEMICOLON
                            || tk == TK_NEWLINE;
                },
                &PyParser::parseExpr);
}

/*
 * test: or_test ['if' or_test 'else' test] | lambdef
 */
Parser::Expr PyParser::parseTest()
{
    if (ahead_ == TK_LAMBDA)
        return parseLambdaDef();

    auto orTest = parseOrTest();
    if (maybeConsume(TK_IF)) {
        // Must be a conditional.
        auto condExpr = TerExprAst::create();
        condExpr->setYes(std::move(orTest));
        condExpr->setQuestionLoc(prevLoc_);
        condExpr->setCond(parseOrTest());
        match(TK_ELSE);
        condExpr->setDelimLoc(prevLoc_);
        condExpr->setNo(parseTest());
        orTest = std::move(condExpr);
    }

    return orTest;
}

/*
 * old_test: or_test | old_lambdef
 */
Parser::Expr PyParser::parseOldTest()
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
    return parseDSeqTrail<ExprAstList, PyParser>(
                TK_COMMA,
                [] (const Token tk) {
                    switch (tk) {
                    case TK_PLUS_EQ:
                    case TK_MINUS_EQ:
                    case TK_STAR_EQ:
                    case TK_SLASH_EQ:
                    case TK_PERCENT_EQ:
                    case TK_AMPER_EQ:
                    case TK_PIPE_EQ:
                    case TK_CARET_EQ:
                    case TK_LS_LS_EQ:
                    case TK_GR_GR_EQ:
                    case TK_STAR_STAR_EQ:
                    case TK_SLASH_SLASH_EQ:
                    case TK_EQ:
                    case TK_RBRACKET:
                    case TK_RPAREN:
                    case TK_COLON:
                    case TK_SEMICOLON:
                    case TK_NEWLINE:
                        return true;
                    default:
                        return false;
                    }
                },
                &PyParser::parseTest);
}

/*
 * testlist1: test (',' test)*
 */
std::unique_ptr<ExprAstList> PyParser::parseTestList1()
{
    return parseDSeq<ExprAstList, PyParser>(TK_COMMA, &PyParser::parseTest);
}

/*
 * testlist_safe: old_test [(',' old_test)+ [',']]
 */
std::unique_ptr<ExprAstList> PyParser::parseTestListSafe()
{
    // BUG: The trailing comma is allowed only for non-singleton lists. This
    // not handled by the helper parseList function currently.

    return parseDSeqTrail<ExprAstList, PyParser>(
                TK_COMMA,
                [] (const Token tk) {
                    return tk == TK_FOR
                            || tk == TK_IF
                            || tk == TK_RBRACKET;
                },
                &PyParser::parseOldTest);
}

/*
 * argument: test [comp_for] | test '=' test
 */
Parser::Expr PyParser::parseArg()
{
    auto test = parseTest();
    switch (ahead_) {
    case TK_FOR: {
        auto listCompre = parseCompFor(ListCompreExprAst::create());
        listCompre->setExpr(std::move(test));
        return Expr(std::move(listCompre));
    }

    case TK_EQ:
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
    if (isTestFIRST()) {
        bool hasTrail;
        auto args = parseDSeqTrail<ExprAstList, PyParser>(
                    TK_COMMA,
                    [] (const Token tk) {
                        return tk == TK_RPAREN
                                || tk == TK_STAR
                                || tk == TK_STAR_STAR;
                    },
                    &PyParser::parseArg,
                    &hasTrail);
        // If there's no trailing comma, this arg must be the last one.
        if (!hasTrail)
            return args;
    }

    bool wantStarStar = false;
    if (maybeConsume(TK_STAR)) {
        auto unpack = UnpackExprAst::create();
        unpack->setKeyLoc(prevLoc_);
        unpack->setExpr(parseTest());
        appendOrCreate(args, std::move(unpack));

        if (maybeConsume(TK_COMMA)) {
            if (args)
                args->lastSubList()->delim_ = prevLoc_;
            if (isTestFIRST()) {
                bool hasTrail;
                auto arg = parseDSeqTrail<ExprAstList, PyParser>(
                            TK_COMMA,
                            [] (const Token tk) {
                                return tk == TK_RPAREN;
                            },
                            &PyParser::parseArg,
                            &hasTrail);
                mergeOrReplace(args, std::move(arg));
                if (hasTrail)
                    wantStarStar = true;
            } else {
                wantStarStar = true;
            }
        }
    }

    if (maybeConsume(TK_STAR_STAR)) {
        auto unpack = UnpackExprAst::create();
        unpack->setKeyLoc(prevLoc_);
        unpack->setExpr(parseTest());
        appendOrCreate(args, std::move(unpack));
    } else if (wantStarStar) {
        fail();
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

    match(TK_FOR);
    listCompre->addGen(GeneratorAst::create());
    auto gen = listCompre->gens()->back();
    gen->setPatterns(parseExprList());

    match(TK_IN);
    gen->setOprLoc(prevLoc_);
    gen->setRange(parseOrTest());

    return completeListCompre(std::move(listCompre),
                              &PyParser::parseCompFor, &PyParser::parseCompIf);
}

/*
 * list_iter: list_for | list_if
 * list_for: 'for' exprlist 'in' testlist_safe [list_iter]
 */
std::unique_ptr<ListCompreExprAst>
PyParser::parseListFor(std::unique_ptr<ListCompreExprAst> listCompre)
{
    UAISO_ASSERT(ahead_ == TK_FOR, return listCompre);
    UAISO_ASSERT(listCompre, return listCompre);

    match(TK_FOR);
    listCompre->addGen(GeneratorAst::create());
    auto gen = listCompre->gens()->back();
    gen->setPatterns(parseExprList());

    match(TK_IN);
    gen->setOprLoc(prevLoc_);

    auto tests = parseTestListSafe();
    if (tests) {
        // DESIGN: An expr list as the expr of a list comprehension? Wrap as a
        // comma expr?
        gen->setRange(std::move(tests->detachHead().first));
    }

    return completeListCompre(std::move(listCompre),
                              &PyParser::parseListFor, &PyParser::parseListIf);
}

/*
 * comp_iter: comp_for | comp_if
 * comp_if: 'if' old_test [comp_iter]
 */
std::unique_ptr<ListCompreExprAst>
PyParser::parseCompIf(std::unique_ptr<ListCompreExprAst> listCompre)
{
    return parseListOrCompIf(std::move(listCompre),
                             &PyParser::parseCompFor, &PyParser::parseCompIf);
}

/*
 * list_iter: list_for | list_if
 * list_if: 'if' old_test [list_iter]
 */
std::unique_ptr<ListCompreExprAst>
PyParser::parseListIf(std::unique_ptr<ListCompreExprAst> listCompre)
{
    return parseListOrCompIf(std::move(listCompre),
                             &PyParser::parseListFor, &PyParser::parseListIf);
}

std::unique_ptr<ListCompreExprAst>
PyParser::parseListOrCompIf(std::unique_ptr<ListCompreExprAst> listCompre,
                            ListCompre (PyParser::*genFunc) (ListCompre),
                            ListCompre (PyParser::*filterFunc) (ListCompre))
{
    UAISO_ASSERT(ahead_ == TK_IF, return listCompre);
    UAISO_ASSERT(listCompre, return listCompre);
    UAISO_ASSERT(listCompre->gens(), return listCompre);

    match(TK_IF);
    auto gen = listCompre->gens()->back();
    gen->addFilter(parseOldTest());

    return completeListCompre(std::move(listCompre), genFunc, filterFunc);
}

std::unique_ptr<ListCompreExprAst>
PyParser::completeListCompre(ListCompre listCompre,
                             ListCompre (PyParser::*genFunc) (ListCompre),
                             ListCompre (PyParser::*filterFunc) (ListCompre))
{
    if (ahead_ == TK_FOR)
        return ((this)->*(genFunc))(std::move(listCompre));

    if (ahead_ == TK_IF)
        return ((this)->*(filterFunc))(std::move(listCompre));

    return std::move(listCompre);
}

/*
 * or_test: and_test ('or' and_test)*
 */
Parser::Expr PyParser::parseOrTest()
{
    auto andTest = parseAndTest();
    while (maybeConsume(TK_OR)) {
        andTest = completeBinExpr<LogicOrExprAst>(std::move(andTest),
                                                  &PyParser::parseAndTest);
    }
    return andTest;
}

/*
 * and_test: not_test ('and' not_test)*
 */
Parser::Expr PyParser::parseAndTest()
{
    auto notTest = parseNotTest();
    while (maybeConsume(TK_AND)) {
        notTest = completeBinExpr<LogicAndExprAst>(std::move(notTest),
                                                   &PyParser::parseNotTest);
    }
    return notTest;
}

/*
 * not_test: 'not' not_test | comparison
 */
Parser::Expr PyParser::parseNotTest()
{
    if (maybeConsume(TK_NOT)) {
        auto notTest = LogicNotExprAst::create();
        notTest->setOprLoc(prevLoc_);
        notTest->setExpr(parseNotTest());
        return std::move(notTest);
    }

    return parseComparison();
}

/*
 * comparison: expr (comp_op expr)*
 * comp_op: '<'|'>'|'=='|'>='|'<='|'<>'|'!='|'in'|'not' 'in'|'is'|'is' 'not'
 */
Parser::Expr PyParser::parseComparison()
{
    auto expr = parseExpr();
    while (true) {
        switch (ahead_) {
        case TK_LS:
        case TK_GR:
        case TK_EQ_EQ:
        case TK_GR_EQ:
        case TK_LS_EQ:
        case TK_LS_GR:
        case TK_EXCLAM_EQ:
            consumeToken();
            expr = completeBinExpr<RelExprAst>(std::move(expr), &PyParser::parseExpr);
            break;

        case TK_IN:
            consumeToken();
            expr = completeBinExpr<InExprAst>(std::move(expr), &PyParser::parseExpr);
            break;

        case TK_IS: // May be followed by 'not'.
            consumeToken();
            maybeConsume(TK_NOT);
            expr = completeBinExpr<IsExprAst>(std::move(expr), &PyParser::parseExpr);
            break;

        case TK_NOT: // Must be followed by 'in'.
            consumeToken();
            match(TK_IN);
            expr = completeBinExpr<InExprAst>(std::move(expr), &PyParser::parseExpr);
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
Parser::Expr PyParser::parseExpr()
{
    return parseBinExpr(Precedence::Or);
}

Parser::Expr PyParser::parseBinExpr(Precedence curPrec)
{
    auto factor = parseFactor();
    while (true) {
        Precedence prec;
        std::unique_ptr<BinExprAst> bin;
        std::tie(prec, bin) = fetchPrecAhead();
        if (prec < curPrec)
            break;

        consumeToken();
        bin->setOprLoc(prevLoc_);
        bin->setExpr1(std::move(factor));
        bin->setExpr2(parseBinExpr(Precedence(curPrec + 1)));
        factor = std::move(bin);
    }

    return factor;
}

/*
 * factor: ('+'|'-'|'~') factor | power
 */
Parser::Expr PyParser::parseFactor()
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
Parser::Expr PyParser::parsePower()
{
    auto atom = parseAtom();
    bool trailer = true;
    do {
        switch (ahead_) {
        case TK_LPAREN: {
            consumeToken();
            auto call = CallExprAst::create();
            call->setLDelimLoc(prevLoc_);
            call->setBase(std::move(atom));
            if (isArgFIRST())
                call->setArgs(parseArgList());
            matchOrSkipTo(TK_RPAREN, "parsePower");
            call->setRDelimLoc(prevLoc_);
            atom = std::move(call);
            break;
        }

        case TK_LBRACKET: {
            consumeToken();
            auto arrayAccess = ArraySliceExprAst::create(); // See comment in this AST.
            arrayAccess->setLDelimLoc(prevLoc_);
            arrayAccess->setBase(std::move(atom));
            arrayAccess->setRange(parseSubscript());
            matchOrSkipTo(TK_RBRACKET, "parsePower");
            atom = std::move(arrayAccess);
            break;
        }

        case TK_DOT: {
            consumeToken();
            auto member = MemberAccessExprAst::create();
            member->setOprLoc(prevLoc_);
            member->setExpr(std::move(atom));
            member->setName(parseName());
            atom = std::move(member);
            break;
        }

        default:
            trailer = false;
            break;
        }
    } while (trailer);

    if (maybeConsume(TK_STAR_STAR))
        return completeBinExpr<PowerExprAst>(std::move(atom), &PyParser::parseFactor);

    return atom;
}

/*
 * atom: ('(' [yield_expr|testlist_comp] ')' |
 *        '[' [listmaker] ']' |
 *        '{' [dictorsetmaker] '}' |
 *        '`' testlist1 '`' |
 *        NAME | NUMBER | STRING+)
 *
 * Note: Accept 'None', 'True', and 'False, as well. The lexer process them
 * as keywords.
 */
Parser::Expr PyParser::parseAtom()
{
    switch (ahead_) {
    case TK_LPAREN:
        return parseWrappedOrTuple();

    case TK_LBRACE:
        return parseDictOrSetMaker();

    case TK_LBRACKET:
        return parseListMaker();

    case TK_BACKTICK: {
        consumeToken();
        auto str = TypeidExprAst::create();
        str->setLDelimLoc(prevLoc_);
        parseTestList1(); // Let it die.
        matchOrSkipTo(TK_BACKTICK, "parseAtom");
        str->setRDelimLoc(prevLoc_);
        return Expr(std::move(str));
    }

    case TK_COMPLETION:
    case TK_IDENT: {
        auto ident = IdentExprAst::create();
        ident->setName(parseName());
        return Expr(std::move(ident));
    }

    case TK_INT_LIT:
        consumeToken();
        return NumLitExprAst::create(prevLoc_, NumLitVariety::IntFormat);

    case TK_FLOAT_LIT:
        consumeToken();
        return NumLitExprAst::create(prevLoc_, NumLitVariety::FloatFormat);

    case TK_NULL_VALUE:
        consumeToken();
        return NullLitExprAst::create(prevLoc_);

    case TK_TRUE_VALUE:
    case TK_FALSE_VALUE:
        consumeToken();
        return BoolLitExprAst::create(prevLoc_);

    case TK_STR_LIT:
        return parseStrLit();

    default:
        fail();
        return ErrorExprAst::create(prevLoc_);
    }
}

/*
 * subscript: '.' '.' '.' | test | [test] ':' [test] [sliceop]
 * sliceop: ':' [test]
 */
Parser::Expr PyParser::parseSubscript()
{
    switch (ahead_) {
    case TK_DOT_DOT_DOT:
        // TODO: Model this kind of slice.
        consumeToken();
        return SubrangeExprAst::create();

    case TK_COLON:
        consumeToken();
        return completeSubrangeExpr(Expr());

    default:
        auto test = parseTest();
        if (maybeConsume(TK_COLON))
            return completeSubrangeExpr(std::move(test));
        return test;
    }
}

/*
 * subscriptlist: subscript (',' subscript)* [',']
 */
std::unique_ptr<ExprAstList> PyParser::parseSubscriptList()
{
    return parseDSeqTrail<ExprAstList, PyParser>(
                TK_COMMA,
                [] (const Token tk) {
                    return tk == TK_RBRACKET;
                },
                &PyParser::parseSubscript);
}

/*
 * dictorsetmaker: ( (test ':' test (comp_for | (',' test ':' test)* [','])) |
 *                   (test (comp_for | (',' test)* [','])) )
 *
 * Note: This will actually parse '{' [dictorsetmaker] '}'
 */
Parser::Expr PyParser::parseDictOrSetMaker()
{
    UAISO_ASSERT(ahead_ == TK_LBRACE, return Expr());

    auto dictOrSet = ArrayInitExprAst::create();
    consumeToken();
    // DESIGN: Differentiate a set literal, '{1, 2}', from a list literal,
    // '(1, 2)'. Both are parsed as ArrayInitExprAst. Perhaps add a variety
    // to the AST or handle through Lang.
    dictOrSet->setLDelimLoc(prevLoc_);
    if (maybeConsume(TK_RBRACE)) {
        dictOrSet->setRDelimLoc(prevLoc_);
        return std::move(dictOrSet);
    }

    auto test = parseTest();
    switch (ahead_) {
    case TK_COLON: {
        consumeToken();
        auto desig = DesignateExprAst::create();
        desig->setDelimLoc(prevLoc_);
        desig->setId(std::move(test));
        desig->setValue(parseTest());

        if (ahead_ == TK_FOR) {
            auto listCompre = parseListFor(ListCompreExprAst::create());
            listCompre->setLDelimLoc(dictOrSet->lDelimLoc());
            listCompre->setExpr(std::move(desig));
            matchOrSkipTo(TK_RBRACE, "parseDictOrSetMaker");
            listCompre->setRDelimLoc(prevLoc_);
            return std::move(listCompre);
        }

        dictOrSet->addInit(std::move(desig));
        while (maybeConsume(TK_COMMA)) {
            if (!isTestFIRST())
                break;
            if (dictOrSet->inits())
                dictOrSet->inits()->delim_ = prevLoc_;
            desig = DesignateExprAst::create();
            desig->setId(parseTest());
            match(TK_COLON);
            desig->setDelimLoc(prevLoc_);
            desig->setValue(parseTest());
            dictOrSet->addInit(std::move(desig));
        }
        matchOrSkipTo(TK_RBRACE, "parseDictOrSetMaker");
        dictOrSet->setRDelimLoc(prevLoc_);
        return std::move(dictOrSet);
    }

    case TK_FOR:  {
        auto listCompre = parseListFor(ListCompreExprAst::create());
        listCompre->setLDelimLoc(dictOrSet->lDelimLoc());
        listCompre->setExpr(std::move(test));
        matchOrSkipTo(TK_RBRACE, "parseDictOrSetMaker");
        listCompre->setRDelimLoc(prevLoc_);
        return std::move(listCompre);
    }

    case TK_COMMA:
        consumeToken();
        if (dictOrSet->inits())
            dictOrSet->inits()->delim_ = prevLoc_;
        dictOrSet->addInit(std::move(test));
        dictOrSet->mergeInits(parseTestList());
        // Fallthrough

    default:
        if (test)
            dictOrSet->addInit(std::move(test));
        matchOrSkipTo(TK_RBRACE, "parseDictOrSetMaker");
        dictOrSet->setRDelimLoc(prevLoc_);
        return std::move(dictOrSet);
    }
}

/*
 * listmaker: test ( list_for | (',' test)* [','] )
 *
 * Note: This will actually parse '[' [listmaker] ']'
 */
Parser::Expr PyParser::parseListMaker()
{
    UAISO_ASSERT(ahead_ == TK_LBRACKET, return Expr());

    auto list = ArrayInitExprAst::create();
    consumeToken();
    list->setLDelimLoc(prevLoc_);
    if (maybeConsume(TK_RBRACKET)) {
        list->setRDelimLoc(prevLoc_);
        return std::move(list);
    }

    auto test = parseTest();
    switch (ahead_) {
    case TK_FOR: {
        auto listCompre = parseListFor(ListCompreExprAst::create());
        listCompre->setLDelimLoc(list->lDelimLoc());
        listCompre->setExpr(std::move(test));
        matchOrSkipTo(TK_RBRACKET, "parseListMaker");
        listCompre->setRDelimLoc(prevLoc_);
        return std::move(listCompre);
    }

    case TK_COMMA:
        consumeToken();
        list->addInit(std::move(test));
        if (maybeConsume(TK_RBRACKET)) {
            list->setRDelimLoc(prevLoc_);
            return std::move(list);
        }
        list->mergeInits(parseTestList());
        // Fallthrough

    default:
        if (test)
            list->addInit(std::move(test));
        matchOrSkipTo(TK_RBRACKET, "parseListMaker");
        list->setRDelimLoc(prevLoc_);
        return std::move(list);
    }
}

/*
 * wrappedortuple: '(' [yield_expr|testlist_comp] ')'
 * testlist_comp: test ( comp_for | (',' test)* [','] )
 *
 * '()'     - Tuple
 * '(1)'    - Wrapped expr
 * '(1,)'   - Tuple
 * '(1, 2)' - Tuple
 */
Parser::Expr PyParser::parseWrappedOrTuple()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Expr());

    auto tuple = TupleLitExprAst::create();
    consumeToken();
    tuple->setLDelimLoc(prevLoc_);
    if (maybeConsume(TK_RPAREN)) {
        tuple->setRDelimLoc(prevLoc_);
        return std::move(tuple);
    }

    if (ahead_ == TK_YIELD)
        return completeWrapped([this] () { return parseYieldExpr(); });

    auto test = parseTest();
    switch (ahead_) {
    case TK_FOR:
        return completeWrapped([&test, this] () {
            auto listCompre = parseListFor(ListCompreExprAst::create());
            listCompre->setExpr(std::move(test));
            return listCompre;
        });

    case TK_COMMA:
        consumeToken();
        if (tuple->inits())
            tuple->inits()->delim_ = prevLoc_;
        tuple->addInit(std::move(test));
        if (isTestFIRST())
            tuple->inits()->merge(parseTestList());
        matchOrSkipTo(TK_RPAREN, "parseWrappedOrTuple");
        tuple->setRDelimLoc(prevLoc_);
        return std::move(tuple);

    default:
        return completeWrapped([&test, this]() { return std::move(test); });
    }
}

/*
 * yield_expr: 'yield' [testlist]
 */
Parser::Expr PyParser::parseYieldExpr()
{
    UAISO_ASSERT(ahead_ == TK_YIELD, return Expr());

    auto yield = YieldExprAst::create();
    consumeToken();
    yield->setKeyLoc(prevLoc_);
    if (isTestFIRST())
        yield->setExprs(parseTestList());
    return std::move(yield);
}

/*
 * lambdef: 'lambda' [varargslist] ':' test
 */
Parser::Expr PyParser::parseLambdaDef()
{
    return parseLambdaCore(&PyParser::parseTest);
}

/*
 * old_lambdef: 'lambda' [varargslist] ':' old_test
 */
Parser::Expr PyParser::parseOldLambdaDef()
{
    return parseLambdaCore(&PyParser::parseOldTest);
}

Parser::Expr PyParser::parseLambdaCore(Expr (PyParser::*parseFunc) ())
{
    UAISO_ASSERT(ahead_ == TK_LAMBDA, return Expr());

    auto spec = std::unique_ptr<FuncSpecAst__<>>(newAst<FuncSpecAst__<>>());
    consumeToken();
    spec->setKeyLoc(prevLoc_);
    spec->setParam(parseVarArgsList(false));

    match(TK_COLON);
    spec->setLDelimLoc(prevLoc_);
    auto lambda = FuncLitExprAst::create();
    lambda->setSpec(std::move(spec));
    lambda->setStmt(ExprStmtAst::create(((this)->*(parseFunc))()));

    return std::move(lambda);
}

/*
 * dotted_name: NAME ('.' NAME)*
 * dotted_as_name: dotted_name ['as' NAME]
 * dotted_as_names: dotted_as_name (',' dotted_as_name)*
 */
Parser::Name PyParser::parseDottedName()
{
    auto name = NestedNameAst::create();
    name->setNames(parseDSeq<NameAstList, PyParser>(TK_DOT, &PyParser::parseName));
    return Name(std::move(name));
}

Parser::Name PyParser::parseName()
{
    if (maybeConsume(TK_COMPLETION))
        return CompletionNameAst::create(prevLoc_);

    // If not a completion, we must match an identifier. However, if the
    // match doesn't succeed in this case we can't ignore and create a
    // simple name AST. Otherwise we end up with name without a
    // corresponding identifier in the lexeme map. An error name is then
    // created for consistency throughout the pipeline.
    if (match(TK_IDENT))
        return SimpleNameAst::create(prevLoc_);
    return ErrorNameAst::create(prevLoc_);
}

Parser::Expr PyParser::parseStrLit()
{
    UAISO_ASSERT(ahead_ == TK_STR_LIT, return Expr());

    consumeToken();
    auto str = StrLitExprAst::create(prevLoc_);
    if (ahead_ == TK_STR_LIT) {
        auto concat = ConcatExprAst::create();
        concat->setExpr1(std::move(str));
        concat->setExpr2(parseStrLit());
        return std::move(concat);
    }

    return std::move(str);
}

Parser::Expr PyParser::completeSubrangeExpr(Expr expr)
{
    auto range = SubrangeExprAst::create();
    range->setDelim1Loc(prevLoc_);
    range->setLow(std::move(expr));
    if (isTestFIRST())
        range->setHi(parseTest());
    if (maybeConsume(TK_COLON))
        range->setDelim2Loc(prevLoc_);
    if (isTestFIRST())
        range->setMax(parseTest()); // TODO: In Go, this is max.
    return std::move(range);
}

std::unique_ptr<ExprAst>
PyParser::completeWrapped(const std::function<Expr ()> exprFunc)
{
    auto wrap = WrappedExprAst::create();
    wrap->setLDelimLoc(prevLoc_);
    wrap->setExpr(exprFunc());
    matchOrSkipTo(TK_RPAREN, "completeWrapped, skip to TK_PAREN\n");
    wrap->setRDelimLoc(prevLoc_);
    return std::move(wrap);
}

template <class UnaryAstT> std::unique_ptr<ExprAst>
PyParser::completeUnaryExpr(Expr (PyParser::*parseFunc) ())
{
    auto unary = UnaryAstT::create();
    unary->setOprLoc(prevLoc_);
    unary->setExpr((((this)->*(parseFunc))()));
    return std::move(unary);
}

template <class BinaryAstT> std::unique_ptr<ExprAst>
PyParser::completeBinExpr(Expr expr, Expr (PyParser::*parseFunc) ())
{
    auto bin = BinaryAstT::create();
    bin->setOprLoc(prevLoc_);
    bin->setExpr1(std::move(expr));
    bin->setExpr2((((this)->*(parseFunc))()));
    return std::move(bin);
}

std::unique_ptr<ExprAst>
PyParser::completeAssignExpr(Expr expr, Expr (PyParser::*parseFunc) ())
{
    auto assign = AssignExprAst::create();
    assign->setOprLoc(prevLoc_);
    assign->setExpr1s(ExprAstList::create(std::move(expr)));
    assign->setExpr2s(ExprAstList::create((((this)->*(parseFunc))())));
    return std::move(assign);
}

Parser::Decl PyParser::completeParam(ParamGroup group)
{
    auto param = ParamDeclAst__<ParamVariadic__>::create();
    param->setVariadicLoc(prevLoc_);
    param->setName(parseName());
    group->addDecl(std::move(param));
    return std::move(group);
}
