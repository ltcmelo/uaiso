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

#include "Haskell/HsParser.h"
#include "Haskell/HsLexer.h"
#include "Ast/Ast.h"
#include "Common/Assert.h"
#include "Common/Trace__.h"
#include "Parsing/Lexeme.h"
#include "Parsing/ParsingContext.h"

#define TRACE_NAME "HsParser"

using namespace uaiso;

namespace
{
// Contextual keywords.
const char* const kAs = "as";
const char* const kHiding = "hiding";
const char* const kQualified = "qualified";
}

HsParser::HsParser()
{}

bool HsParser::parse(Lexer* lexer, ParsingContext* context)
{
    UAISO_ASSERT(lexer, return false);
    UAISO_ASSERT(context && context->fileName(), return false);

    setLexer(lexer);
    setContext(context);
    init();
    consumeToken();
    if (ahead_ == TK_EOP)
        return false;

    auto prog = std::unique_ptr<ProgramAst>(newAst<ProgramAst>());
    if (ahead_ == TK_MODULE)
        prog->setModule(parseModule());
    if (ahead_ == TK_LBRACE)
        prog->setDecls(parseBody());
    context->takeAst(std::unique_ptr<Ast>(prog.release()));

    return true;
}

Parser::Decl HsParser::parseModule()
{
    UAISO_ASSERT(ahead_ == TK_MODULE, return Decl());

    auto module = ModuleDeclAst::create();
    consumeToken();
    module->setKeyLoc(prevLoc_);
    module->setName(parseModid());
    if (ahead_ == TK_LPAREN)
        module->setExpot(parseExport());
    match(TK_WHERE);
    module->setTerminLoc(prevLoc_);

    return std::move(module);
}

Parser::Decl HsParser::parseExport()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Decl());

    auto xport = ExportDeclAst::create();
    ParenMatcher<ExportDeclAst> wrap(this, "parseExport", xport.get());
    xport->setSelections(parseSelection(true));

    return std::move(xport);
}

Parser::Decl HsParser::parseImport()
{
    UAISO_ASSERT(ahead_ == TK_IMPORT, return Decl());

    auto group = ImportGroupDeclAst::create();
    consumeToken();
    group->setKeyLoc(prevLoc_);

    // Utility lambda to compare contextual keywords.
    auto contextKeyAhead = [this] (const std::string& s) {
        if (ahead_ == TK_IDENT) {
            const Ident* ident = context_->fetchLexeme<Ident>(currentLoc().lineCol());
            return ident && ident->str() == s;
        }
        return false;
    };

    auto import = ImportDeclAst::create();
    if (contextKeyAhead(kQualified))
        import->setMode(parseName(TK_IDENT));

    auto target = IdentExprAst::create();
    target->setName(parseModid());
    import->setTarget(std::move(target));
    if (contextKeyAhead(kAs)) {
        consumeToken();
        import->setAsLoc(prevLoc_);
        import->setLocalName(parseModid());
    }

    // Selected imports, either to be visible or to be hidden.
    if (contextKeyAhead(kHiding))
        consumeToken(); // TODO: Store hidden names.
    if (ahead_ == TK_LPAREN) {
        ParenMatcher<ImportDeclAst> wrap(this, "parseImport", import.get());
        import->setSelections(parseSelection(false));
    }

    group->addModule(std::move(import));
    return std::move(group);
}

Parser::DeclList HsParser::parseSelection(bool isExport)
{
    DeclList selects;
    do {
        auto select = ExportSelectionDeclAst::create();
        switch (ahead_) {
        case TK_RPAREN:
            break; // Empty selection, we're done.

        case TK_MODULE:
            consumeToken();
            if (isExport) {
                select->setName(parseModid());
            } else {
                // Since we encountered an incorrect `module', perhaps the
                // module specification is also there. If so, we skip it.
                if (ahead_ == TK_PROPER_IDENT_QUAL
                        || ahead_ == TK_PROPER_IDENT) {
                    parseModid();
                }
                context_->trackReport(Diagnostic::UnexpectedToken, prevLoc_);
            }
            break;

        case TK_IDENT:
            select->setName(parseVarId());
            break;

        case TK_IDENT_QUAL:
            select->setName(parseQName(TK_IDENT_QUAL, &HsParser::parseVarId));
            break;

        case TK_LPAREN: {
            if (isExport)
                select->setName(parseQVarSymWrap());
            else
                select->setName(parseVarSymWrap());
            break;
        }

        default:
            Name name;
            if (isExport)
                name = parseQName(TK_PROPER_IDENT_QUAL, &HsParser::parseConId);
            else
                name = parseConId();

            // The import/export of a type constructor or type class may specify
            // individual members. But if we have no name, an error happened and
            // we try skipping to a coma.
            if (name) {
                if (ahead_ == TK_LPAREN) {
                    ParenMatcher<> wrap(this, "parseVarOrConOrDotDot");
                    if (maybeConsume(TK_DOT_DOT)) {
                        // TODO: Mark export all.
                    } else {
                        do {
                            parseVarOrCon(); // TODO: Store "sub-selections".
                        } while (maybeConsume(TK_COMMA));
                    }
                }
            } else {
                skipTo(TK_COMMA);
            }
            select->setName(std::move(name));
            break;
        }

        // Each item is part of the current import/export selection.
        addToList(selects, std::move(select));
    } while (maybeConsume(TK_COMMA));

    return selects;
}

Parser::DeclList HsParser::parseBody()
{
    UAISO_ASSERT(ahead_ == TK_LBRACE, return DeclList());

    BraceMatcher<> wraper(this, "parseBody");
    while (ahead_ == TK_IMPORT) {
        parseImport();
        if (!maybeConsume(TK_SEMICOLON))
            return DeclList();
    }
    parseTopDecls();

    return DeclList();
}

Parser::DeclList HsParser::parseTopDecls()
{
    do {
        switch (ahead_) {
        case TK_TYPE:
            break;

        case TK_DATA:
            break;

        case TK_NEWTYPE:
            break;

        case TK_CLASS:
            break;

        case TK_INSTANCE:
            break;

        case TK_DEFAULT:
            break;

        case TK_FOREIGN:
            break;

        default:
            parseDecl();
            break;
        }
    } while (maybeConsume(TK_SEMICOLON));

    return DeclList();
}

Parser::Decl HsParser::parseDecl()
{
    switch (ahead_) {
    case TK_SEMICOLON:
        consumeToken();
        return EmptyDeclAst::create(prevLoc_);

    case TK_INFIX:
    case TK_INFIXL:
    case TK_INFIXR:
        // TODO: Fixity declaration.
        return Decl();

    default:
        return parsePatBindOrFuncOrTypeSig();
    }
}

Parser::Decl HsParser::parsePatBindOrFuncOrTypeSig()
{
    Name name;
    if (maybeConsume(TK_IDENT)) {
        name = SimpleNameAst::create(prevLoc_);
    } else if (ahead_ == TK_LPAREN
               && isVarSym(peekToken(2))
               && peekToken(3) == TK_RPAREN) {
        consumeToken(2);
        name = PuncNameAst::create(prevLoc_);
        consumeToken();
    }

    // A type signature begins with a `var', which is either a `varid', or a
    // `(varsym)'. If the name is empty, we have a pattern binding or function.
    if (!name)
        return parsePatBindOrFunc();

    // If we see a `,' we have something like `varid,' or `(varsym),', which
    // cannot be neither a pattern bind nor a function, it's a type signature.
    bool wantTySig = false;
    NameList vars;
    while (maybeConsume(TK_COMMA)) {
        addToList(vars, parseSymWrapOrId(&HsParser::parseVarSym, &HsParser::parseVarId));
        wantTySig = true;
    }
    if (wantTySig || ahead_ == TK_COLON_COLON) {
        auto group = NameAstList::create(std::move(name));
        if (vars)
            group->merge(std::move(vars));
        return parseTypeSig(/* group */);
    }

    std::unique_ptr<PatExprAst> pat;
    if (ahead_ == TK_AT) {
        parseAsPat();
    }

    if (auto name = maybeParseQConOp()) {
        parsePat();
    }

    switch (ahead_) {
    case TK_EQ:
        // TODO: Expression.
        break;

    case TK_PIPE:
        // TODO: Guard.
        break;

    default:
        if (pat)
            return parseInfixFunc(/* pat */);
        return parseFunc(/* name */);
    }

    return Decl();
}

Parser::Decl HsParser::parsePatBindOrFunc()
{
    return Decl();
}

Parser::Decl HsParser::parseTypeSig()
{
    UAISO_ASSERT(ahead_ == TK_COLON_COLON, return Decl());
    consumeToken();
    return Decl();
}

Parser::Decl HsParser::parsePatBind()
{
    return Decl();
}

Parser::Decl HsParser::parseInfixFunc()
{
    return Decl();
}

Parser::Decl HsParser::parseFunc()
{
    return Decl();
}

    //--- Expressions ---//

Parser::Expr HsParser::parsePat()
{
    parseLPat();

    if (auto name = maybeParseQConOp()) {
        parsePat();
    }

    return Expr();
}

Parser::Expr HsParser::parseLPat()
{
    switch (ahead_) {
    case TK_MINUS:
        consumeToken();
        if (maybeConsume(TK_INT_LIT))
            return NumLitExprAst::create(prevLoc_, NumLitVariety::IntFormat);
        match(TK_FLOAT_LIT);
        return NumLitExprAst::create(prevLoc_, NumLitVariety::FloatFormat);

    case TK_LBRACKET:
        return parseListConOrLitPat();

    case TK_LPAREN: {
        const Token peek = peekToken(2);
        if (peek == TK_PROPER_IDENT || isConSym(peek)) {
            parseQCon();
            parsePatList();
            return CallExprAst::create();
        }
        return parseTupleConOrLitOrWrapOrUnitPat();
    }

    case TK_PROPER_IDENT:
        parseQConId();
        parsePatList();
        return CallExprAst::create();

    default:
        return parseAPat();
    }
}

Parser::Expr HsParser::parseAPat()
{
    switch (ahead_) {
    case TK_INT_LIT:
        return parseIntLit();

    case TK_FLOAT_LIT:
        return parseFloatLit();

    case TK_TRUE_VALUE:
    case TK_FALSE_VALUE:
        return parseBoolLit();

    case TK_CHAR_LIT:
        return parseCharLit();

    case TK_STR_LIT:
        return parseStrLit();

    case TK_UNDERSCORE:
        consumeToken();
        return WildCardExprAst::create(prevLoc_);

    case TK_TILDE:
        // TODO: Irrefutable pattern.
        return Expr();

    default:
        break;
    }

    return Expr();
}

Parser::Expr HsParser::parseListConOrLitPat()
{
    UAISO_ASSERT(ahead_ == TK_LBRACKET, return Expr());
    consumeToken();

    if (maybeConsume(TK_RBRACKET))
        return CallExprAst::create(); // List's data con `[ ]'.

    parsePatDList();
    matchOrSkipTo(TK_RBRACKET, "parseListConOrLit");
    return Expr();
}

Parser::Expr HsParser::parseTupleConOrLitOrWrapOrUnitPat()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Expr());
    consumeToken();

    if (maybeConsume(TK_RPAREN))
        return NullLitExprAst::create(); // Unit value.

    size_t tupleCnt = 0;
    while (maybeConsume(TK_COMMA)) {
        ++tupleCnt;
    }
    if (tupleCnt) {
        matchOrSkipTo(TK_LPAREN, "parseTupleConOrLitOrWrapOrUnit");
        parsePatList();
        return Expr();
    }

    // Tuple literal.
    parsePatDList();
    matchOrSkipTo(TK_LPAREN, "parseTupleConOrLitOrWrapOrUnit");
    return Expr();
}

Parser::ExprList HsParser::parsePatList()
{
    return ExprList();
}

Parser::ExprList HsParser::parsePatDList()
{
    return ExprList();
}

Parser::ExprList HsParser::parseAPatList()
{
    return ExprList();
}

Parser::ExprList HsParser::parseAPatDList()
{
    return ExprList();
}

Parser::Expr HsParser::parseAsPat()
{
    UAISO_ASSERT(ahead_ == TK_AT, return Expr());
    consumeToken();
    parseAPat();
    return Expr();
}

Parser::Expr HsParser::parseExpr()
{
    return parseAExpr();
}

Parser::Expr HsParser::parseAExpr()
{
    switch (ahead_) {
    case TK_INT_LIT:
        return parseIntLit();

    case TK_FLOAT_LIT:
        return parseFloatLit();

    case TK_TRUE_VALUE:
    case TK_FALSE_VALUE:
        return parseBoolLit();

    default:
        fail();
        return Expr();
    }
}

Parser::Expr HsParser::parseIntLit()
{
    UAISO_ASSERT(ahead_ == TK_INT_LIT, return Expr());
    consumeToken();
    return NumLitExprAst::create(prevLoc_, NumLitVariety::IntFormat);
}

Parser::Expr HsParser::parseFloatLit()
{
    UAISO_ASSERT(ahead_ == TK_FLOAT_LIT, return Expr());
    consumeToken();
    return NumLitExprAst::create(prevLoc_, NumLitVariety::FloatFormat);
}

Parser::Expr HsParser::parseStrLit()
{
    UAISO_ASSERT(ahead_ == TK_STR_LIT, return Expr());
    consumeToken();
    return StrLitExprAst::create(prevLoc_);
}

Parser::Expr HsParser::parseCharLit()
{
    UAISO_ASSERT(ahead_ == TK_CHAR_LIT, return Expr());
    consumeToken();
    return CharLitExprAst::create(prevLoc_);
}

Parser::Expr HsParser::parseBoolLit()
{
    UAISO_ASSERT(ahead_ == TK_TRUE_VALUE
                 || ahead_ == TK_FALSE_VALUE, return Expr());
    consumeToken();
    return BoolLitExprAst::create(prevLoc_);
}

    //--- Names ---//

Parser::Name HsParser::parseModid()
{
    return parseQName(TK_PROPER_IDENT_QUAL, &HsParser::parseConId);
}

Parser::Name HsParser::parseVarOrCon()
{
    if (ahead_ == TK_LPAREN) {
        ParenMatcher<> wrap(this, "parseVarOrConSym");
        if (ahead_ == TK_COLON || ahead_ == TK_SPECIAL_IDENT) {
            consumeToken();
            return SpecialNameAst::create(prevLoc_);
        }
        return parseVarSym();
    }

    if (maybeConsume(TK_IDENT))
        return SimpleNameAst::create(prevLoc_);

    return parseConId();
}

Parser::Name HsParser::parseSymWrapOrId(Name (HsParser::*parseSymWrap)(),
                                        Name (HsParser::*parseId)())
{
    if (ahead_ == TK_LPAREN)
        return (this->*(parseSymWrap))();
    return (this->*(parseId))();
}

Parser::Name HsParser::parseQCon()
{
    return parseSymWrapOrId(&HsParser::parseQConSymWrap, &HsParser::parseQConId);
}

Parser::Name HsParser::parseQVar()
{
    return parseSymWrapOrId(&HsParser::parseQVarSymWrap, &HsParser::parseQVarId);
}

Parser::Name HsParser::parseCon()
{
    return parseSymWrapOrId(&HsParser::parseConSymWrap, &HsParser::parseConId);
}

Parser::Name HsParser::parseVar()
{
    return parseSymWrapOrId(&HsParser::parseVarSymWrap, &HsParser::parseVarId);
}

Parser::Name HsParser::parseQConId()
{
    return parseQName(TK_PROPER_IDENT_QUAL, &HsParser::parseConId);
}

Parser::Name HsParser::parseQVarId()
{
    return parseQName(TK_PROPER_IDENT_QUAL, &HsParser::parseVarId);
}

Parser::Name HsParser::parseQConSymWrap()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "parseQConSymWrap");
    return parseQName(TK_SPECIAL_IDENT_QUAL, &HsParser::parseConSym);
}

Parser::Name HsParser::parseQVarSymWrap()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "parseQVarSymWrap");
    return parseQName(TK_PUNC_IDENT_QUAL, &HsParser::parseVarSym);
}

Parser::Name HsParser::parseConSymWrap()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "parseConSymWrap");
    return parseConSym();
}

Parser::Name HsParser::parseVarSymWrap()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "parseVarSymWrap");
    return parseVarSym();
}

Parser::Name HsParser::parseConSym()
{
    if (isConSym(ahead_)) {
        consumeToken();
        return SpecialNameAst::create(prevLoc_);
    }

    fail();
    return ErrorNameAst::create(prevLoc_);
}

Parser::Name HsParser::parseVarSym()
{
    if (isVarSym(ahead_)) {
        consumeToken();
        return PuncNameAst::create(prevLoc_);
    }

    fail();
    return ErrorNameAst::create(prevLoc_);
}

Parser::Name HsParser::parseConId()
{
    return parseName(TK_PROPER_IDENT);
}

Parser::Name HsParser::parseVarId()
{
    return parseName(TK_IDENT);
}

Parser::Name HsParser::maybeParseQConOp()
{
    if (maybeConsume(TK_COLON))
        return SpecialNameAst::create(prevLoc_);

    if (ahead_ == TK_BACKTICK
            && peekToken(2) == TK_PROPER_IDENT) {
        Matcher<TK_BACKTICK, TK_BACKTICK> wrap(this, "parseQConOp");
        return parseQConId();
    }

    if (ahead_ == TK_SPECIAL_IDENT_QUAL
            || ahead_ == TK_SPECIAL_IDENT) {
        return parseQConSymWrap();
    }

    return Name();
}

Parser::Name HsParser::parseQName(Token qualTk, Name (HsParser::*parseFunc)())
{
    auto qname = NestedNameAst::create();
    while (maybeConsume(qualTk)) {
        addToList(qname->names_, SimpleNameAst::create(prevLoc_));
        match(TK_JOKER);
    }
    addToList(qname->names_, ((this->*(parseFunc))()));
    return std::move(qname);
}

HsParser::Name HsParser::parseName(Token tk)
{
    /*
     * We cannot fail a NameAst match. This would mean an identifier without a
     * corresponding location in the lexeme map, which is not allowed in our
     * design. Therefore, an ErrorNameAst must be constructed in such case.
     */
    if (match(tk))
        return SimpleNameAst::create(prevLoc_);
    return ErrorNameAst::create(prevLoc_);
}
