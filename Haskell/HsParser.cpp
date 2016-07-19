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
#include "Parsing/ParsingContext.h"

#define TRACE_NAME "HsParser"

using namespace uaiso;

namespace uaiso {

extern std::unordered_map<std::uint16_t, const char*> tokenName;

}

HsParser::HsParser()
{}

void HsParser::matchOrSkipTo(Token tk, const char *rule)
{
    if (!match(tk)) {
        DEBUG_TRACE("%s, skip to %s\n", rule, tokenName[tk]);
        skipTo(tk);
    }
}

bool HsParser::parse(Lexer* lexer, ParsingContext* context)
{
    UAISO_ASSERT(lexer, return false);
    UAISO_ASSERT(context && context->fileName(), return false);

    lexer_ = lexer;
    context_ = context;

    consumeToken();
    if (ahead_ == TK_EOP)
        return false;

    auto prog = std::unique_ptr<ProgramAst>(newAst<ProgramAst>());
    if (ahead_ == TK_MODULE)
        prog->setModule(parseModuleDecl().release());
    prog->setDecls(parseBody().release());
    context->takeAst(std::unique_ptr<Ast>(prog.release()));

    return true;
}

Parser::Decl HsParser::parseModuleDecl()
{
    UAISO_ASSERT(ahead_ == TK_MODULE, return Decl());

    match(TK_MODULE);
    auto module = ModuleDeclAst::create();
    module->setKeyLoc(lastLoc_);
    module->setName(parseModidName().release());
    if (ahead_ == TK_LPAREN)
        module->setExpot(parseExportDecl().release());
    match(TK_WHERE);
    module->setTerminLoc(lastLoc_);

    return std::move(module);
}

Parser::Decl HsParser::parseExportDecl()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Decl());
    consumeToken();

    auto expot = ExportDeclAst::create();
    expot->setLDelimLoc(lastLoc_);
    do {
        switch (ahead_) {
        case TK_RPAREN:
            break; // We're done.

        case TK_MODULE:
            consumeToken();
            addToList(expot->names_, parseModidName().release());
            break;

        case TK_LPAREN:
            addToList(expot->names_, parseQVarSymName().release());
            break;

        case TK_IDENT:
            addToList(expot->names_, parseVarIdName().release());
            break;

        default:
            auto qname = NestedNameAst::create();
            do {
                addToList(qname->names_, parseConIdName().release());
            } while (maybeConsume(TK_JOKER) && ahead_ == TK_PROPER_IDENT);

            if (ahead_ == TK_IDENT) {
                addToList(qname->names_, parseVarIdName().release());
            } else if (ahead_ == TK_LPAREN) {
                consumeToken();
                if (maybeConsume(TK_DOT_DOT)) {
                    // TODO: Mark export all.
                } else {
                    do {
                        // TODO
                        parseVarOrConName();
                    } while (maybeConsume(TK_COMMA));
                }
                matchOrSkipTo(TK_RPAREN, "parseExportItemDecl");
            }
            addToList(expot->names_, qname.release());
            break;
        }
    } while (maybeConsume(TK_COMMA));
    matchOrSkipTo(TK_RPAREN, "parseExportDecl");
    expot->setRDelimLoc(lastLoc_);

    return std::move(expot);
}

Parser::DeclList HsParser::parseBody()
{
    return DeclList();
}

Parser::Expr HsParser::parseExpr()
{
    return parseAExpr();
}

Parser::Expr HsParser::parseAExpr()
{
    switch (ahead_) {
    case TK_INT_LIT:
        consumeToken();
        return Expr(newAst<NumLitExprAst>()->setLitLoc(lastLoc_)
                    ->setVariety(NumLitVariety::IntFormat));

    case TK_FLOAT_LIT:
        consumeToken();
        return Expr(newAst<NumLitExprAst>()->setLitLoc(lastLoc_)
                    ->setVariety(NumLitVariety::FloatFormat));

    case TK_TRUE_VALUE:
    case TK_FALSE_VALUE:
        consumeToken();
        return Expr(newAst<BoolLitExprAst>()->setLitLoc(lastLoc_));

    default:
        failMatch(true);
        return Expr();
    }
}

Parser::Name HsParser::parseModidName()
{
    auto modid = NestedNameAst::create();
    do {
        modid->addName(parseName(TK_PROPER_IDENT).release());
    } while (maybeConsume(TK_JOKER));

    return std::move(modid);
}

Parser::Name HsParser::parseVarOrConName()
{
    if (maybeConsume(TK_LPAREN)) {
        Name name;
        switch (ahead_) {
        case TK_COLON:
        case TK_SPECIAL_IDENT:
            name = parseConSymName();
            break;

        default:
            name = parseVarSymName();
            break;
        }
        matchOrSkipTo(TK_RPAREN, "parseVarOrConName");
        return name;
    }

    if (ahead_ == TK_IDENT)
        return parseVarIdName();
    return parseConIdName();
}

Parser::Name HsParser::parseQConName()
{
    if (ahead_ == TK_LPAREN)
        return parseQConSymName();
    return parseQConIdName();
}

Parser::Name HsParser::parseQVarName()
{
    if (ahead_ == TK_LPAREN)
        return parseQVarSymName();
    return parseQVarIdName();
}

Parser::Name HsParser::parseConName()
{
    if (ahead_ == TK_LPAREN)
        return parseConSymName();
    return parseConIdName();
}

Parser::Name HsParser::parseVarName()
{
    if (ahead_ == TK_LPAREN)
        return parseVarSymName();
    return parseVarIdName();
}

Parser::Name HsParser::parseQConIdName()
{
    return parseQName(&HsParser::parseConIdName);
}

Parser::Name HsParser::parseQVarIdName()
{
    return parseQName(&HsParser::parseVarIdName);
}

Parser::Name HsParser::parseQConSymName()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    consumeToken();
    auto name = parseQName(&HsParser::parseConSymName);
    matchOrSkipTo(TK_RPAREN, "parseQConSymName");

    return std::move(name);
}

Parser::Name HsParser::parseQVarSymName()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    consumeToken();
    auto name = parseQName(&HsParser::parseVarSymName);
    matchOrSkipTo(TK_RPAREN, "parseQVarSymName");

    return std::move(name);
}

Parser::Name HsParser::parseConSymName()
{
    switch (ahead_) {
    case TK_COLON:
    case TK_SPECIAL_IDENT:
        consumeToken();
        return SimpleNameAst::create(lastLoc_);

    default:
        failMatch(true);
        return ErrorNameAst::create(lastLoc_);
    }
}

Parser::Name HsParser::parseVarSymName()
{
    switch (ahead_) {
    case TK_EXCLAM:
    case TK_POUND:
    case TK_DOLLAR:
    case TK_PERCENT:
    case TK_AMPER:
    case TK_ASTERISK:
    case TK_PLUS:
    case TK_MINUS:
    case TK_DOT:
    case TK_SLASH:
    case TK_LS:
    case TK_GR:
    case TK_QUESTION:
    case TK_CARET:
    case TK_SYMBOL_IDENT: {
        consumeToken();
        return SimpleNameAst::create(lastLoc_);
    }

    default:
        failMatch(true);
        return ErrorNameAst::create(lastLoc_);
    }
}

Parser::Name HsParser::parseConIdName()
{
    return parseName(TK_PROPER_IDENT);
}

Parser::Name HsParser::parseVarIdName()
{
    return parseName(TK_IDENT);
}

Parser::NameList HsParser::parseConIdList()
{
    NameList names;
    do {
        addToList(names, parseConIdName().release());
    } while (maybeConsume(TK_JOKER));

    return names;
}

Parser::Name HsParser::parseQName(Name (HsParser::*parseFunc)())
{
    auto qname = NestedNameAst::create();
    while (maybeConsume(TK_PROPER_IDENT)) {
        addToList(qname->names_, SimpleNameAst::create(lastLoc_).release());
        match(TK_JOKER);
    }
    addToList(qname->names_, ((this->*(parseFunc))()).release());

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
        return SimpleNameAst::create(lastLoc_);
    return ErrorNameAst::create(lastLoc_);
}
