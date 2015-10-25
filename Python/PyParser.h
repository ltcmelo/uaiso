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

#ifndef UAISO_PYPARSER_H__
#define UAISO_PYPARSER_H__

#include "Parsing/Parser.h"
#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Common/Test.h"
#include "Parsing/Token.h"
#include <functional>
#include <initializer_list>
#include <utility>

namespace uaiso {

class Lexer;
class ParsingContext;

/*!
 * \brief The PyParser class
 *
 * Based on https://docs.python.org/2/reference/grammar.html
 */
class UAISO_API PyParser final : public Parser
{
public:
    PyParser();

    bool parse(Lexer* lexer, ParsingContext* context) override;

private:
    DECL_CLASS_TEST(PyParser)

    void consumeToken();
    bool maybeConsume(Token tk);
    void skipTo(Token tk);
    bool match(Token tk);
    void failMatch(bool consume);

    enum Precedence
    {
        Zero = 0,
        Or,
        Xor,
        And,
        Shift,
        Term,
        Factor
    };

    std::pair<Precedence, std::unique_ptr<BinaryExprAst>> fetchPrecAhead() const;

    using Decl = std::unique_ptr<DeclAst>;
    using Expr = std::unique_ptr<ExprAst>;
    using Name = std::unique_ptr<NameAst>;
    using Stmt = std::unique_ptr<StmtAst>;
    using DeclList = std::unique_ptr<DeclAstList>;
    using ExprList = std::unique_ptr<ExprAstList>;

    using ListCompre = std::unique_ptr<ListCompreExprAst>;

    bool isTestAhead() const;
    bool isNonLambdaTestAhead() const;
    bool isExprAhead() const;
    bool isFactorAhead() const;
    bool isAtomAhead() const;
    bool isArgAhead() const;
    bool isSubscriptAhead() const;
    bool isNameAhead() const;

    //--- Names ---//

    Name parseName();
    Name parseDottedName();

    //--- Declarations ---//

    DeclList parseSubImports(bool selective);

    //--- Statements ---//

    Stmt parseStmt();
    Stmt parseSimpleStmt();
    Stmt parseSmallStmt();
    Stmt parseExprStmt();
    Stmt parsePrintStmt();
    Stmt parseDelStmt();
    Stmt parsePassStmt();
    Stmt parseFlowStmt();
    Stmt parseImportStmt();
    Stmt parseGlobalStmt();
    Stmt parseExecStmt();
    Stmt parseAssertStmt();
    Stmt parseIfStmt();
    Stmt parseWhileStmt();
    Stmt parseForStmt();
    Stmt parseTryStmt();
    Stmt parseFuncDef();
    Stmt parseClassDef();
    Stmt parseDecorated();
    Stmt parseContinueStmt();
    Stmt parseBreakStmt();
    Stmt parseYieldStmt();
    Stmt parseRaiseStmt();
    Stmt parseReturnStmt();
    Stmt parseSuite();
    Stmt parseWithStmt();

    //--- Expressions ---//

    Expr parseTest();
    Expr parseOldTest();
    ExprList parseTestList();
    ExprList parseTestList1();
    ExprList parseTestListSafe();
    Expr parseLambdaDef();
    Expr parseOldLambdaDef();
    Expr parseOrTest();
    Expr parseAndTest();
    Expr parseNotTest();
    Expr parseComparison();
    Expr parseExpr();
    Expr parseBinaryExpr(Precedence precedence);
    Expr parseFactor();
    Expr parsePower();
    Expr parseAtom();
    Expr parseArg();
    ExprList parseArgList();
    Expr parseSubscript();
    ExprList parseSubscriptList();
    Expr parseYieldExpr();
    Expr parseListMaker();
    Expr parseDictOrSetMaker();
    Expr parseWrappedOrTuple();
    ExprList parseExprList();
    Expr parseWithItem();
    ListCompre parseCompFor(ListCompre);
    ListCompre parseCompIf(ListCompre);
    ListCompre parseListFor(ListCompre);
    ListCompre parseListIf(ListCompre);
    ListCompre parseListOrCompIf(ListCompre listCompre,
                                 ListCompre (PyParser::*genFunc) (ListCompre),
                                 ListCompre (PyParser::*filterFunc) (ListCompre));

    // Helpers

    template <class LitAstT>
    Expr completeLitExpr();
    template <class UnaryAstT>
    Expr completeUnaryExpr(Expr (PyParser::*parseFunc) ());
    template <class BinaryAstT>
    Expr completeBinaryExpr(Expr expr, Expr (PyParser::*parseFunc) ());
    Expr completeAssignExpr(Expr expr, Expr (PyParser::*parseFunc) ());
    Expr completeSubrangeExpr(Expr expr);
    Stmt completeIfStmt();
    Name completeName();
    ListCompre completeListCompre(ListCompre listCompre,
                                  ListCompre (PyParser::*genFunc) (ListCompre),
                                  ListCompre (PyParser::*filterFunc) (ListCompre));
    Expr completeWrapped(const std::function<Expr ()> exprFunc);

    template <class AstListT>
    std::pair<std::unique_ptr<AstListT>, bool>
    parseList(Token tk,
              bool (PyParser::*checkAhead) () const,
              std::unique_ptr<typename AstListT::AstType> (PyParser::*) (),
              bool acceptTrailing = true);

    Lexer* lexer_ { nullptr };
    ParsingContext* context_ { nullptr };
    Token ahead_ { TK_INVALID };
    SourceLoc lastLoc_;
};

} // namespace uaiso

#endif
