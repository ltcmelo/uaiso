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
#include <utility>
#include <initializer_list>

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
    std::pair<bool, Token> maybeConsume(const std::initializer_list<Token>&);
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

    using Name = std::unique_ptr<NameAst>;
    using Stmt = std::unique_ptr<StmtAst>;
    using Expr = std::unique_ptr<ExprAst>;
    using ExprList = std::unique_ptr<ExprAstList>;

    bool isTestAhead() const;
    bool isNonLambdaTestAhead() const;
    bool isExprAhead() const;
    bool isFactorAhead() const;
    bool isAtomAhead() const;

    //--- Statements ---//

    std::unique_ptr<StmtAst> parseStmt();
    std::unique_ptr<StmtAst> parseSimpleStmt();
    std::unique_ptr<StmtAst> parseSmallStmt();
    std::unique_ptr<StmtAst> parseExprStmt();
    std::unique_ptr<StmtAst> parsePrintStmt();
    std::unique_ptr<StmtAst> parseDelStmt();
    std::unique_ptr<StmtAst> parsePassStmt();
    std::unique_ptr<StmtAst> parseFlowStmt();
    std::unique_ptr<StmtAst> parseImportStmt();
    std::unique_ptr<StmtAst> parseGlobalStmt();
    std::unique_ptr<StmtAst> parseExecStmt();
    std::unique_ptr<StmtAst> parseAssertStmt();
    std::unique_ptr<StmtAst> parseCompoundStmt();
    std::unique_ptr<StmtAst> parseIfStmt();
    std::unique_ptr<StmtAst> parseWhileStmt();
    std::unique_ptr<StmtAst> parseForStmt();
    std::unique_ptr<StmtAst> parseTryStmt();
    std::unique_ptr<StmtAst> parseWithStmt();
    std::unique_ptr<StmtAst> parseFuncDef();
    std::unique_ptr<StmtAst> parseClassDef();
    std::unique_ptr<StmtAst> parseDecorated();
    std::unique_ptr<StmtAst> parseContinueStmt();
    std::unique_ptr<StmtAst> parseBreakStmt();
    std::unique_ptr<StmtAst> parseYieldStmt();
    std::unique_ptr<StmtAst> parseThrowStmt();
    std::unique_ptr<StmtAst> parseReturnStmt();

    //--- Expressions ---//

    std::unique_ptr<ExprAst> parseTest();
    std::unique_ptr<ExprAst> parseOldTest();
    std::unique_ptr<ExprAstList> parseTestList();
    std::unique_ptr<ExprAst> parseLambdaDef();
    std::unique_ptr<ExprAst> parseOldLambdaDef();
    std::unique_ptr<ExprAst> parseOrTest();
    std::unique_ptr<ExprAst> parseAndTest();
    std::unique_ptr<ExprAst> parseNotTest();
    std::unique_ptr<ExprAst> parseComparison();
    std::unique_ptr<ExprAst> parseExpr();
    std::unique_ptr<ExprAst> parseBinaryExpr(Precedence precedence);
    std::unique_ptr<ExprAst> parseFactor();
    std::unique_ptr<ExprAst> parsePower();
    std::unique_ptr<ExprAst> parseAtom();
    std::unique_ptr<ExprAst> parseArg();
    std::unique_ptr<ExprAstList> parseArgList();
    std::unique_ptr<ExprAstList> parseSubscriptList();
    std::unique_ptr<ListCompreExprAst> parseCompFor(std::unique_ptr<ListCompreExprAst>);
    std::unique_ptr<ListCompreExprAst> parseCompIf(std::unique_ptr<ListCompreExprAst>);
    std::unique_ptr<ExprAstList> parseExprList();

    // Helpers

    template <class LitAstT>
    std::unique_ptr<ExprAst> completeLitExpr();
    template <class UnaryAstT>
    std::unique_ptr<ExprAst> completeUnaryExpr(Expr (PyParser::*parseFunc) ());
    template <class BinaryAstT>
    std::unique_ptr<ExprAst> completeBinaryExpr(Expr expr, Expr (PyParser::*parseFunc) ());
    std::unique_ptr<ExprAst> completeAssignExpr(Expr expr, Expr (PyParser::*parseFunc) ());
    std::unique_ptr<NameAst> completeName();

    template <class AstListT>
    std::pair<std::unique_ptr<AstListT>, bool>
    parseList(Token tk,
              bool (PyParser::*checkAhead) () const,
              std::unique_ptr<typename AstListT::AstType> (PyParser::*) ());

    Lexer* lexer_ { nullptr };
    ParsingContext* context_ { nullptr };
    Token ahead_ { TK_INVALID };
    SourceLoc lastLoc_;
};

} // namespace uaiso

#endif
