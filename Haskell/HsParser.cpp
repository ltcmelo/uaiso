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

namespace uaiso
{
extern std::unordered_map<std::uint16_t, const char*> tokenName;
}

namespace
{
// Contextual keywords.
const char* const kAs = "as";
const char* const kHiding = "hiding";
const char* const kQualified = "qualified";
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
        prog->setModule(parseModule().release());
    if (ahead_ == TK_LBRACE)
        prog->setDecls(parseBody().release());
    context->takeAst(std::unique_ptr<Ast>(prog.release()));

    return true;
}

Parser::Decl HsParser::parseModule()
{
    UAISO_ASSERT(ahead_ == TK_MODULE, return Decl());
    consumeToken();
    auto module = ModuleDeclAst::create();
    module->setKeyLoc(lastLoc_);
    module->setName(parseModid().release());
    if (ahead_ == TK_LPAREN)
        module->setExpot(parseExport().release());
    match(TK_WHERE);
    module->setTerminLoc(lastLoc_);

    return std::move(module);
}

Parser::Decl HsParser::parseExport()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Decl());
    consumeToken();
    auto expot = ExportDeclAst::create();
    expot->setLDelimLoc(lastLoc_);
    expot->setSelections(parseSelection(true).release());
    matchOrSkipTo(TK_RPAREN, "parseExport");
    expot->setRDelimLoc(lastLoc_);

    return std::move(expot);
}

Parser::Decl HsParser::parseImport()
{
    UAISO_ASSERT(ahead_ == TK_IMPORT, return Decl());
    consumeToken();
    auto group = ImportGroupDeclAst::create();
    group->setKeyLoc(lastLoc_);

    // Utility lambda to compare contextual keywords.
    auto matchIdent = [this] (const std::string& s) {
        if (ahead_ == TK_IDENT) {
            const Ident* ident = context_->fetchLexeme<Ident>(lexer_->tokenLoc().lineCol());
            return ident && ident->str() == s;
        }
        return false;
    };

    auto import = ImportDeclAst::create();
    if (matchIdent(kQualified))
        import->setMode(parseName(TK_IDENT).release());
    auto target = IdentExprAst::create();
    target->setName(parseModid().release());
    import->setTarget(target.release());
    if (matchIdent(kAs)) {
        consumeToken();
        import->setAsLoc(lastLoc_);
        import->setLocalName(parseModid().release());
    }
    if (matchIdent(kHiding))
        consumeToken(); // TODO: Store hidden names.
    if (maybeConsume(TK_LPAREN)) {
        import->setLDelimLoc(lastLoc_);
        import->setSelections(parseSelection(false).release());
        matchOrSkipTo(TK_RPAREN, "parseImport");
        import->setRDelimLoc(lastLoc_);
    }
    group->addModule(import.release());

    return std::move(group);
}

Parser::DeclList HsParser::parseSelection(bool allowModid)
{
    DeclList selects;
    do {
        auto select = ExportSelectionDeclAst::create();
        switch (ahead_) {
        case TK_RPAREN:
            break; // We're done.

        case TK_MODULE:
            consumeToken();
            if (allowModid) {
                select->setName(parseModid().release());
            } else {
                context_->trackReport(Diagnostic::UnexpectedToken, lastLoc_);
                if (ahead_ == TK_PROPER_IDENT)
                    parseModid(); // Parse and discard it.
            }
            break;

        case TK_LPAREN:
            select->setName(parseQVarSym().release());
            break;

        case TK_IDENT:
            select->setName(parseVarId().release());
            break;

        default:
            auto qname = NestedNameAst::create();
            do {
                addToList(qname->names_, parseConId().release());
            } while (maybeConsume(TK_JOKER) && ahead_ == TK_PROPER_IDENT);

            if (ahead_ == TK_IDENT) {
                addToList(qname->names_, parseVarId().release());
            } else if (ahead_ == TK_LPAREN) {
                consumeToken();
                if (maybeConsume(TK_DOT_DOT)) {
                    // TODO: Mark export all.
                } else {
                    do {
                        parseVarOrCon(); // TODO: Store "sub-selections".
                    } while (maybeConsume(TK_COMMA));
                }
                matchOrSkipTo(TK_RPAREN, "parseExportItem");
            }
            select->setName(qname.release());
            break;
        }
        addToList(selects, select.release());
    } while (maybeConsume(TK_COMMA));

    return selects;
}

Parser::DeclList HsParser::parseBody()
{
    UAISO_ASSERT(ahead_ == TK_LBRACE, return DeclList());
    consumeToken();

    while (ahead_ == TK_IMPORT)
        parseImport();

    matchOrSkipTo(TK_RBRACE, "parseBody");

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
        failMatch();
        return Expr();
    }
}

Parser::Name HsParser::parseModid()
{
    auto modid = NestedNameAst::create();
    do {
        modid->addName(parseName(TK_PROPER_IDENT).release());
    } while (maybeConsume(TK_JOKER));

    return std::move(modid);
}

Parser::Name HsParser::parseVarOrCon()
{
    if (maybeConsume(TK_LPAREN)) {
        Name name;
        switch (ahead_) {
        case TK_COLON:
        case TK_SPECIAL_IDENT:
            name = parseConSym();
            break;

        default:
            name = parseVarSym();
            break;
        }
        matchOrSkipTo(TK_RPAREN, "parseVarOrConName");
        return name;
    }

    if (ahead_ == TK_IDENT)
        return parseVarId();
    return parseConId();
}

Parser::Name HsParser::parseSymOrId(Name (HsParser::*parseSym)(),
                                    Name (HsParser::*parseId)())
{
    if (ahead_ == TK_LPAREN)
        return (this->*(parseSym))();
    return (this->*(parseId))();
}

Parser::Name HsParser::parseQCon()
{
    return parseSymOrId(&HsParser::parseQConSym, &HsParser::parseQConId);
}

Parser::Name HsParser::parseQVar()
{
    return parseSymOrId(&HsParser::parseQVarSym, &HsParser::parseQVarId);
}

Parser::Name HsParser::parseCon()
{
    return parseSymOrId(&HsParser::parseConSym, &HsParser::parseConId);
}

Parser::Name HsParser::parseVar()
{
    return parseSymOrId(&HsParser::parseVarSym, &HsParser::parseVarId);
}

Parser::Name HsParser::parseQConId()
{
    return parseQName(&HsParser::parseConId);
}

Parser::Name HsParser::parseQVarId()
{
    return parseQName(&HsParser::parseVarId);
}

Parser::Name HsParser::parseQConSym()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    consumeToken();
    auto name = parseQName(&HsParser::parseConSym);
    matchOrSkipTo(TK_RPAREN, "parseQConSymName");

    return std::move(name);
}

Parser::Name HsParser::parseQVarSym()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    consumeToken();
    auto name = parseQName(&HsParser::parseVarSym);
    matchOrSkipTo(TK_RPAREN, "parseQVarSymName");

    return std::move(name);
}

Parser::Name HsParser::parseConSym()
{
    switch (ahead_) {
    case TK_COLON:
    case TK_SPECIAL_IDENT:
        consumeToken();
        return SimpleNameAst::create(lastLoc_);

    default:
        failMatch();
        return ErrorNameAst::create(lastLoc_);
    }
}

Parser::Name HsParser::parseVarSym()
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
        failMatch();
        return ErrorNameAst::create(lastLoc_);
    }
}

Parser::Name HsParser::parseConId()
{
    return parseName(TK_PROPER_IDENT);
}

Parser::Name HsParser::parseVarId()
{
    return parseName(TK_IDENT);
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
