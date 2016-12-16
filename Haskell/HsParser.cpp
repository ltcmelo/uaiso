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
#include <algorithm>
#include <functional>
#include <type_traits>

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
    ParenMatcher<ExportDeclAst> wrap(this, "closing export paren", xport.get());
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

    // Selected imports, either visible or to be hidden.
    if (contextKeyAhead(kHiding))
        consumeToken(); // TODO: Store hidden names.
    if (ahead_ == TK_LPAREN) {
        ParenMatcher<ImportDeclAst> wrap(this, "closing import paren", import.get());
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
                if (ahead_ == TK_PROPER_IDENT_QUAL || ahead_ == TK_PROPER_IDENT)
                    parseModid();
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
                select->setName(parseQVarSymParen());
            else
                select->setName(parseVarSymParen());
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
                    ParenMatcher<> wrap(this, "closing paren on import/export selection");
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
        appendOrCreate(selects, std::move(select));
    } while (maybeConsume(TK_COMMA));

    return selects;
}

Parser::DeclList HsParser::parseBody()
{
    UAISO_ASSERT(ahead_ == TK_LBRACE, return DeclList());

    BraceMatcher<> wraper(this, "closing body brace");
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
        case TK_RBRACE:
            break; // We're done.

        case TK_DATA:
            parseData();
            break;

        case TK_TYPE:
            parseTypeAlias();
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
        return ErrorDeclAst::create();

    default:
        return parsePatBindOrAnyFuncOrTypeSig();
    }
}

Parser::Decl HsParser::parsePatBindOrAnyFuncOrTypeSig()
{
    Name name = maybeParseVar();

    // Both type signatures and regular functions begin with a `var', which is
    // either a `varid', or a `(varsym)'. So if the name is empty, we can only
    // match a pattern binding, an infix function, or a wrapped function.
    if (!name)
        return parsePatBindOrInfFuncOrChainFunc();

    // If we see a `,' we try to match a `varid, ...' or `(varsym), ...', which
    // can't be neither a pattern bind nor a function, only a type signature.
    bool wantTySig = false;
    NameList vars;
    while (maybeConsume(TK_COMMA)) {
        appendOrCreate(vars, parseSymParenOrId(&HsParser::parseVarSymParen,
                                              &HsParser::parseVarId));
        wantTySig = true;
    }
    if (wantTySig || ahead_ == TK_COLON_COLON) {
        matchOrSkipTo(TK_COLON_COLON, "type signature");
        auto group = NameAstList::create(std::move(name));
        if (vars)
            group->merge(std::move(vars));
        parseContextType();
        return VarGroupDeclAst::create(); // TODO.
    }

    // If there's an `@' ahead, the name derives a pattern and we can no longer
    // match a regular function, only a pattern binding or an infix function.
    if (ahead_ == TK_AT)
        return finishPatBindOrInfixFunc(finishAsPat(std::move(name), true));

    // If the name binds to a value constructor it derives a pattern and, as
    // above, we can only match a pattern binding or an infix function.
    if (auto qConOp = maybeParseQConOp()) {
        return finishPatBindOrInfixFunc(
            finishInfixCtor(VarPatDeclAst::create(std::move(name)), std::move(qConOp)));
    }

    // Perhaps the name is a "plain" pattern, so we can still match a pattern
    // binding, but we must consider both infix and regular functions as well.
    auto func = FuncDeclAst::create();
    switch (ahead_) {
    case TK_EQ:
        return parsePatBindRhs(VarPatDeclAst::create(std::move(name)),
                               &HsParser::parsePlainRhs);

    case TK_PIPE:
        return parsePatBindRhs(VarPatDeclAst::create(std::move(name)),
                               &HsParser::parseGuardRhs);

    case TK_BACKTICK:
        func->setName(parseVarIdTick());
        parsePat();
        return parseFuncRhs(std::move(func));

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
    case TK_PUNC_IDENT: {
        consumeToken();
        auto name = PuncNameAst::create(prevLoc_);
        parsePat();
        return parseFuncRhs(std::move(func));
    }

    default:
        parseSeq<DeclAstList, HsParser>(
                    [] (const Token tk) {
                        return tk == TK_EQ || tk == TK_PIPE || tk == TK_EOP;
                    },
                    &HsParser::parseAPat);
        return parseFuncRhs(std::move(func));
    }
}

Parser::Decl HsParser::parsePatBindOrInfFuncOrChainFunc()
{
    // If we a `(' ahead, it becomes more complicated because now it's also
    // possible to match "chained" functions, such as `(f . g) x = f (g x)'.
    if (ahead_ == TK_LPAREN)
        return parsePatBindOrChainFunc(std::vector<SourceLoc>());

    // We must match either a pattern binding or an infix function and we know
    // neither of them start with a name, so the only alternative is a pattern.
    auto pat = parsePat();
    switch (ahead_) {
    case TK_EQ:
        return parsePatBindRhs(std::move(pat), &HsParser::parsePlainRhs);

    case TK_PIPE:
        return parsePatBindRhs(std::move(pat), &HsParser::parseGuardRhs);

    default:
        auto func = FuncDeclAst::create();
        if (ahead_ == TK_BACKTICK)
            func->setName(parseVarIdTick());
        else
            func->setName(parseVarSym());
        parsePat();
        return parseFuncRhs(std::move(func));
    }
}

Parser::Decl HsParser::parsePatBindOrChainFunc(std::vector<SourceLoc>&& parens)
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Decl());

    // We've just seen a `(', a lot of matches are possible:
    //   1) a regular function that begins with a `(varsym)'
    //   2) an infix function with a wrapped pattern at front
    //   3) a pattern binding
    //   4) a further wrapping of a pattern or "chaining" of a function
    // The strategy we take is to accumulate the openning paranthesis and
    // recursively call this function. Eventually, we'll identify what we can
    // match. We then exhaust the closing parenthesis and finish the rule.

    consumeToken();
    parens.emplace_back(std::move(prevLoc_)); // Pile up opening parens.

    if (ahead_ == TK_LPAREN)
       return parsePatBindOrChainFunc(std::move(parens));

    // A generic lambda that will exhaust the parenthesis and either wrap the
    // pattern or chain a function.
    auto exhaustParens = [this](auto patOrFunc, const std::vector<SourceLoc> &parens)
    {
        for (auto it = parens.rbegin(); it != parens.rend(); ++it) {
            patOrFunc = std::remove_pointer<
                    typename decltype(patOrFunc)::element_type>::type::
                        create(std::move(patOrFunc));
            patOrFunc->setLDelimLoc(*it);
            if (!match(TK_RPAREN)) {
                skipTo(TK_RPAREN);
                patOrFunc->setRDelimLoc(prevLoc_);
                break;
            }
            patOrFunc->setRDelimLoc(prevLoc_);
        };
        return std::move(patOrFunc);
    };

    // Finish parsing by matching the chained function rule.
    auto finishChainFunc = [this, &parens, exhaustParens] (Decl pat, Name name) {
        auto func = FuncDeclAst::create();
        func->setName(std::move(name));
        parsePat();
        auto chainFunc =
            exhaustParens(ChainedFuncDeclAst::create(std::move(func)), parens);
        parseSeq<DeclAstList, HsParser>(
                    [] (const Token tk) {
                        return tk == TK_EQ || tk == TK_PIPE || tk == TK_EOP;
                    },
                    &HsParser::parseAPat);
        return parseFuncRhs(std::move(chainFunc));
    };

    // Finish parsing by matching the wrapped pattern binding function.
    auto finishChainFuncOrWrapPatBind =
            [this, &parens, exhaustParens, finishChainFunc] (Decl pat) {
        if (ahead_ == TK_RPAREN) {
            auto wrapPat =
                exhaustParens(WrappedPatDeclAst::create(std::move(pat)), parens);
            return parsePatBindRhs(std::move(wrapPat), nullptr);
        }

        auto parse = &HsParser::parseVarSym;
        if (ahead_ == TK_BACKTICK)
            parse = &HsParser::parseVarIdTick;
        return finishChainFunc(std::move(pat), ((this)->*(parse))());
    };

    switch (ahead_) {
    case TK_IDENT: {
        consumeToken();
        Name name = SimpleNameAst::create(prevLoc_);

        if (auto qConOp = maybeParseQConOp()) {
            return finishChainFuncOrWrapPatBind(finishInfixCtor(
                    VarPatDeclAst::create(std::move(name)), std::move(qConOp)));
        }

        switch (ahead_) {
        case TK_AT:
            return finishChainFuncOrWrapPatBind(finishAsPat(std::move(name), true));

        case TK_BACKTICK:
            return finishChainFunc(VarPatDeclAst::create(std::move(name)), parseVarIdTick());

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
        case TK_PUNC_IDENT:
            return finishChainFunc(VarPatDeclAst::create(std::move(name)), parseVarSym());

        case TK_RPAREN: {
            auto wrapPat = exhaustParens(WrappedPatDeclAst::create(
                VarPatDeclAst::create(std::move(name))), parens);
            return parsePatBindRhs(std::move(wrapPat), nullptr);
        }

        default:
            auto func = FuncDeclAst::create();
            parseSeq<DeclAstList, HsParser>(
                        [] (const Token tk) {
                return tk == TK_RPAREN || tk == TK_EOP;
            },
            &HsParser::parseAPat);
            auto chainFunc = exhaustParens(ChainedFuncDeclAst::create(std::move(func)), parens);
            parseSeq<DeclAstList, HsParser>(
                        [] (const Token tk) {
                return tk == TK_EQ || tk == TK_PIPE || tk == TK_EOP;
            },
            &HsParser::parseAPat);
            return parseFuncRhs(std::move(chainFunc));
        }
    }

    // Within this function, a `(' has already been seen, so we're looking for a `(varsym)' ahead.
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
    case TK_PUNC_IDENT: {
        consumeToken();
        auto name = PuncNameAst::create(prevLoc_);
        match(TK_RPAREN);
        std::rotate(parens.begin(), parens.begin() + 1, parens.end());
        parens.pop_back();
        auto func = FuncDeclAst::create();
        parseSeq<DeclAstList, HsParser>(
                    [] (const Token tk) {
                        return tk == TK_RPAREN || tk == TK_EOP;
                    },
                    &HsParser::parseAPat);
        if (!parens.empty()) {
            auto chainFunc = exhaustParens(ChainedFuncDeclAst::create(std::move(func)), parens);
            parseSeq<DeclAstList, HsParser>(
                        [] (const Token tk) {
                            return tk == TK_EQ || tk == TK_PIPE || tk == TK_EOP;
                        },
                        &HsParser::parseAPat);
            return parseFuncRhs(std::move(chainFunc));
        }
        return parseFuncRhs(std::move(func));
    }

    default:
        return finishChainFuncOrWrapPatBind(parsePat());
    }
}

Parser::Decl HsParser::parsePatBind()
{
    return ErrorDeclAst::create();
}

Parser::Decl HsParser::parseInfFunc()
{
    return ErrorDeclAst::create();
}

Parser::Decl HsParser::parseFunc()
{
    return ErrorDeclAst::create();
}

Parser::Decl HsParser::parsePat()
{
    parseLPat();

    if (auto name = maybeParseQConOp()) {
        parsePat();
    }

    return ErrorDeclAst::create();
}

Parser::Decl HsParser::parseLPat()
{
    switch (ahead_) {
    case TK_MINUS:
        consumeToken();
        if (ahead_ == TK_INT_LIT)
            return BasicPatDeclAst::create(parseIntLit());
        return BasicPatDeclAst::create(parseFloatLit());

    case TK_PROPER_IDENT:
    case TK_PROPER_IDENT_QUAL: {
        auto qConId = parseQConId();
        if (ahead_ == TK_LBRACE)
            return finishLabeledPat(std::move(qConId));
        if (isAPatFIRST(ahead_))
            parseAPatList();
        return VarPatDeclAst::create();
    }

    case TK_LBRACKET:
        consumeToken();
        return finishListConOrListPat();

    case TK_LPAREN: {
        consumeToken();
        if (ahead_ == TK_SPECIAL_IDENT_QUAL || ahead_ == TK_SPECIAL_IDENT) {
            auto qConSym = parseQConSym();
            matchOrSkipTo(TK_RPAREN, "qualified construtor symbol in lpat");
            if (ahead_ == TK_LBRACE)
                return finishLabeledPat(std::move(qConSym));
            if (isAPatFIRST(ahead_))
                parseAPatList();
            return VarPatDeclAst::create();
        }

        finishUnitOrWrapOrTupConOrTupPat();
        if (isAPatFIRST(ahead_))
            parseAPatList();
        return ErrorDeclAst::create();
    }

    default:
        return parseAPat();
    }
}

Parser::Decl HsParser::parseAPat()
{
    switch (ahead_) {
    case TK_INT_LIT:
        return BasicPatDeclAst::create(parseIntLit());

    case TK_FLOAT_LIT:
        return BasicPatDeclAst::create(parseFloatLit());

    case TK_TRUE_VALUE:
    case TK_FALSE_VALUE:
        return BasicPatDeclAst::create(parseBoolLit());

    case TK_CHAR_LIT:
        return BasicPatDeclAst::create(parseCharLit());

    case TK_STR_LIT:
        return BasicPatDeclAst::create(parseStrLit());

    case TK_UNDERSCORE:
        consumeToken();
        return WildCardPatDeclAst::create(prevLoc_);

    case TK_TILDE:
        consumeToken();
        return IrrefutPatDeclAst::create(prevLoc_, parseAPat());

    case TK_IDENT: {
        consumeToken();
        auto var = SimpleNameAst::create(prevLoc_);
        if (ahead_ == TK_AT)
            return finishAsPat(std::move(var), false);
        return VarPatDeclAst::create(std::move(var));
    }

    case TK_PROPER_IDENT:
    case TK_PROPER_IDENT_QUAL: {
        auto qConId = parseQConId();
        if (ahead_ == TK_LBRACE)
            return finishLabeledPat(std::move(qConId));
        return VarPatDeclAst::create();
    }

    case TK_LBRACKET:
        consumeToken();
        return finishListConOrListPat();

    case TK_LPAREN: {
        consumeToken();
        if (ahead_ == TK_SPECIAL_IDENT_QUAL || ahead_ == TK_SPECIAL_IDENT) {
            auto qConSym = parseQConSym();
            matchOrSkipTo(TK_RPAREN, "qualified constructor sym in apat");
            if (ahead_ == TK_LBRACE)
                return finishLabeledPat(std::move(qConSym));
            return VarPatDeclAst::create();
        }
        return finishUnitOrWrapOrTupConOrTupPat();
    }

    default:
        fail();
        return ErrorDeclAst::create(prevLoc_);
    }
}

Parser::DeclList HsParser::parsePatList()
{
    return DeclList();
}

Parser::DeclList HsParser::parsePatDList()
{
    return DeclList();
}

Parser::DeclList HsParser::parseAPatList()
{
    return DeclList();
}

Parser::DeclList HsParser::parseAPatDList()
{
    return DeclList();
}

Parser::Decl HsParser::finishAsPat(Name name, bool extendMatch)
{
    UAISO_ASSERT(ahead_ == TK_AT, return Decl());

    auto pat = AsPatDeclAst::create();
    consumeToken();
    pat->setKeyLoc(prevLoc_);
    pat->setName(std::move(name));
    pat->setPat(parseAPat());

    if (extendMatch) {
        if (auto qConOp = maybeParseQConOp())
            return finishInfixCtor(std::move(pat), std::move(qConOp));
    }

    return Decl(std::move(pat));
}

Parser::Decl HsParser::finishInfixCtor(Parser::Decl pat, Parser::Name qConOp)
{
    auto ctor = CtorPatDeclAst::create();
    ctor->addPat(std::move(pat));
    ctor->setName(std::move(qConOp));
    ctor->addPat(parsePat());
    ctor->setVariety(NotationVariety::Infix);
    return std::move(ctor);
}

Parser::Decl HsParser::finishLabeledPat(Parser::Name qConId)
{
    UAISO_ASSERT(ahead_ == TK_LBRACE, return Decl());

    return ErrorDeclAst::create();
}

Parser::Decl HsParser::finishUnitOrWrapOrTupConOrTupPat()
{
    const SourceLoc paren = std::move(prevLoc_);

    // A `)' with nothing inside is the unit value.
    if (maybeConsume(TK_RPAREN))
        return BasicPatDeclAst::create(
                    NullLitExprAst::create(joinedLoc(paren, prevLoc_)));

    // A `,' immediately following a `(' is a tuple data constructor.
    if (maybeConsume(TK_COMMA)) {
        size_t tupCnt = 1;
        do {
            ++tupCnt;
        } while (maybeConsume(TK_COMMA));
        matchOrSkipTo(TK_RPAREN, "tuple data constructor");
        return VarPatDeclAst::create();
    }

    // The remaining options are a wrapped pattern, in the case of a `)' or
    // a tuple pattern in the case of a `,'.
    auto pat = parsePat();
    if (maybeConsume(TK_RPAREN)) {
        auto wrapPat = WrappedPatDeclAst::create(std::move(pat));
        wrapPat->setLDelimLoc(paren);
        wrapPat->setRDelimLoc(prevLoc_);
        return std::move(wrapPat);
    }

    auto tup = TuplePatDeclAst::create();
    tup->setLDelimLoc(paren);
    tup->addPat(std::move(pat));
    match(TK_COMMA);
    tup->mergePats(parseDSeq<DeclAstList, HsParser>(TK_COMMA, &HsParser::parsePat));
    matchOrSkipTo(TK_RPAREN, "tupple pattern");
    tup->setRDelimLoc(prevLoc_);

    return std::move(tup);
}

Parser::Decl HsParser::finishPatBindOrInfixFunc(Parser::Decl pat)
{
    std::unique_ptr<FuncDeclAst> func;
    switch (ahead_) {
    case TK_EQ:
        return parsePatBindRhs(std::move(pat), &HsParser::parsePlainRhs);

    case TK_PIPE:
        return parsePatBindRhs(std::move(pat), &HsParser::parseGuardRhs);

    case TK_BACKTICK: {
        func = FuncDeclAst::create();
        Matcher<TK_BACKTICK, TK_BACKTICK> wrap(this, "infix function name");
        func->setName(parseVarId());
        // Fallthrough.
    }

    default:
        if (!func) {
            func = FuncDeclAst::create();
            func->setName(parseVarSym());
        }
        parsePat();
        return parseFuncRhs(std::move(func));
    }
}

Parser::Decl HsParser::parsePatBindRhs(Parser::Decl pat,
                                       Parser::Expr (HsParser::*parse)())
{
    if (!parse) {
        parse = ahead_ == TK_EQ ? &HsParser::parsePlainRhs
                                : &HsParser::parseGuardRhs;
    }

    auto patBind = PatBindDeclAst::create();
    consumeToken();
    patBind->setEqLoc(prevLoc_);
    patBind->setPat(std::move(pat));
    patBind->setBind(((this)->*(parse))());
    return std::move(patBind);
}

Parser::Decl HsParser::parseFuncRhs(Decl func)
{
    if (ahead_ == TK_EQ) {
        consumeToken();
        parsePlainRhs();
    } else {
        match(TK_PIPE);
        parseGuardRhs();
    }

    return func;
}

Parser::Decl HsParser::finishListConOrListPat()
{
    const SourceLoc lBrackLoc = std::move(prevLoc_);

    // A `)' with nothing inside is the list data constructor.
    if (maybeConsume(TK_RBRACKET))
        return VarPatDeclAst::create();

    // Otherwise, we expect a `,' delimited sequence to match a list pattern.
    auto list = ListPatDeclAst::create();
    list->setLDelimLoc(lBrackLoc);
    list->mergePats(parseDSeq<DeclAstList, HsParser>(TK_COMMA, &HsParser::parsePat));
    matchOrSkipTo(TK_RBRACKET, "list pattern");
    list->setRDelimLoc(prevLoc_);

    return std::move(list);
}

Parser::Decl HsParser::parseData()
{
    UAISO_ASSERT(ahead_ == TK_DATA, return Decl());

    consumeToken();

    // This won't parse contexts: https://prime.haskell.org/wiki/NoDatatypeContexts
    parseConId();
    Decl decl;
    std::vector<Spec> alphas;

ParseType:
    switch (ahead_) {
    case TK_IDENT:
        consumeToken();
        alphas.push_back(AlphaSpecAst::create(SimpleNameAst::create(prevLoc_)));
        goto ParseType;

    case TK_EQ:
        consumeToken();
        return parseDataCon(std::move(decl));

    case TK_DERIVING:
        return parseDeriving(std::move(decl));

    default:
        return decl;
    }
}

Parser::Decl HsParser::parseDataCon(Decl decl)
{
    switch (ahead_) {
    case TK_EXCLAM:
        consumeToken();
        parseAType();
        parseConOp();
        return finishDataConInfix(std::move(decl));

    case TK_LPAREN: {
        const Token peek = peekToken(2);
        if (peek == TK_COLON || peek == TK_SPECIAL_IDENT) {
            parseConSymParen();
            return finishDataConPrefix(std::move(decl));
        }
        // Fallthrough.
    }

    case TK_LBRACKET:
    case TK_IDENT:
    case TK_PROPER_IDENT_QUAL:
        parseBType();
        parseConOp();
        return finishDataConInfix(std::move(decl));

    default:
        parseConId();
        return finishDataConPrefix(std::move(decl));
    }
}

Parser::Decl HsParser::finishDataConPrefix(Decl decl)
{
    switch (ahead_) {
    case TK_PIPE:
        consumeToken();
        return parseDataCon(std::move(decl));

    case TK_DERIVING:
        return parseDeriving(std::move(decl));

    case TK_BACKTICK:
        parseConIdTick();
        return finishDataConInfix(std::move(decl));

    case TK_COLON:
    case TK_SPECIAL_IDENT:
        consumeToken();
        SpecialNameAst::create(prevLoc_);
        return finishDataConInfix(std::move(decl));

    case TK_LBRACE: {
        ParenMatcher<> wrap(this, "closing record brace");
        do {
            // TODO: Parse fields.
        } while (maybeConsume(TK_COMMA));
        return Decl();
    }

    case TK_EXCLAM:
        consumeToken();
        // Fallthrough.

    case TK_LBRACKET:
    case TK_LPAREN:
    case TK_IDENT:
    case TK_PROPER_IDENT:
    case TK_PROPER_IDENT_QUAL:
        return parseDataConType(std::move(decl), false);

    default:
        return decl;
    }
}

Parser::Decl HsParser::parseDataConType(Parser::Decl decl, bool strict)
{
    parseAType();

    switch (ahead_) {
    case TK_PIPE:
        consumeToken();
        return parseDataCon(std::move(decl));

    case TK_DERIVING:
        return parseDeriving(std::move(decl));

    case TK_EXCLAM:
        consumeToken();
        // Fallthrough.

    case TK_LBRACKET:
    case TK_LPAREN:
    case TK_IDENT:
    case TK_PROPER_IDENT:
    case TK_PROPER_IDENT_QUAL:
        return parseDataConType(std::move(decl), false);

    default:
        return decl;
    }
}

Parser::Decl HsParser::finishDataConInfix(Parser::Decl decl)
{
    if (maybeConsume(TK_EXCLAM))
        parseAType();
    else
        parseBType();

    switch (ahead_) {
    case TK_DERIVING:
        return parseDeriving(std::move(decl));

    case TK_PIPE:
        consumeToken();
        return parseDataCon(std::move(decl));

    default:
        return decl;
    }
}

Parser::Decl HsParser::parseDeriving(Decl decl)
{
    UAISO_ASSERT(ahead_ == TK_DERIVING, return Decl());

    consumeToken();
    switch (ahead_) {
    case TK_LPAREN: {
        ParenMatcher<> wrap(this, "closing deriving paren");
        parseDSeq<NameAstList, HsParser>(TK_COMMA, &HsParser::parseModid);
        return decl;
    }

    default:
        parseModid();
        return decl;
    }
}

Parser::Decl HsParser::parseTypeAlias()
{
    UAISO_ASSERT(ahead_ == TK_TYPE, return Decl());

    consumeToken();
    auto alias = AliasDeclAst::create();
    alias->setKeyLoc(prevLoc_);
    parseConId();
    while (maybeConsume(TK_IDENT)) {
        auto alpha = AlphaSpecAst::create(SimpleNameAst::create(prevLoc_));
    }
    match(TK_EQ);

    alias->setSpec(parseType());

    return std::move(alias);
}

Parser::Decl HsParser::parseNewType()
{
    UAISO_ASSERT(ahead_ == TK_NEWTYPE, return Decl());

    return Decl();
}

    //--- Specifiers ---//

Parser::Spec HsParser::parseContextType()
{
    // TODO: Context.

    return parseType();
}

Parser::Spec HsParser::parseType()
{
    auto spec = parseBType();
    if (!maybeConsume(TK_DASH_ARROW))
        return spec;

    auto funcSpec = FuncSpecAst::create();
    do {
        funcSpec->addInput(std::move(spec));
        spec = parseBType();
    } while (maybeConsume(TK_DASH_ARROW));
    funcSpec->setOutput(std::move(spec));

    return std::move(funcSpec);
}

Parser::Spec HsParser::parseBType()
{
    auto spec = parseAType();
    if (!isATypeFIRST(ahead_))
        return spec;

    auto typeApp = TypeAppSpecAst::create();
    do {
        typeApp->addSpec(std::move(spec));
        spec = parseAType();
    } while (isATypeFIRST(ahead_));

    return std::move(typeApp);
}

Parser::Spec HsParser::parseAType()
{
    switch (ahead_) {
    case TK_EOP: // This method calls itself recursively.
        return ErrorSpecAst::create();

    case TK_PROPER_IDENT_QUAL:
        return NamedSpecAst::create(parseQConId());

    case TK_PROPER_IDENT:
        consumeToken();
        return NamedSpecAst::create(SimpleNameAst::create(prevLoc_));

    case TK_IDENT:
        consumeToken();
        return AlphaSpecAst::create(SimpleNameAst::create(prevLoc_));

    case TK_LBRACKET: {
        auto list = ListSpecAst::create();
        consumeToken();
        list->setLDelimLoc(prevLoc_);
        if (maybeConsume(TK_RBRACKET)) {
            list->setRDelimLoc(prevLoc_);
            list->setBaseSpec(parseType());
            list->setVariety(NotationVariety::Infix);
            return std::move(list);
        }
        list->setBaseSpec(parseType());
        matchOrSkipTo(TK_RBRACKET, "list type closing bracket");
        list->setRDelimLoc(prevLoc_);
        return std::move(list);
    }

    default:
        match(TK_LPAREN);
        const SourceLoc paren = std::move(prevLoc_);

        switch (ahead_) {
        case TK_RPAREN:
            consumeToken();
            return UnitSpecAst::create(joinedLoc(paren, prevLoc_));

        case TK_DASH_ARROW: {
            auto func = FuncSpecAst::create();
            consumeToken();
            matchOrSkipTo(TK_RPAREN, "function type closing parenthesis");
            func->setVariety(NotationVariety::Prefix);
            func->setArrowLoc(prevLoc_);
            func->addInput(parseType());
            func->setOutput(parseType());
            return std::move(func);
        }

        case TK_COMMA: {
            auto tuple = TupleSpecAst::create();
            tuple->setLDelimLoc(paren);
            consumeToken();
            auto cnt = 1;
            while (ahead_ != TK_RPAREN) {
                match(TK_COMMA);
                ++cnt;
            }
            consumeToken(); // The closing parenthesis.
            tuple->setRDelimLoc(prevLoc_);
            for (auto i = 0; i <= cnt; ++i)
                tuple->addSpec(parseType());
            return std::move(tuple);
        }

        default:
            auto spec = parseType();
            if (maybeConsume(TK_RPAREN)) // A wrapped type.
                return spec;

            auto tuple = TupleSpecAst::create();
            tuple->setLDelimLoc(paren);
            match(TK_COMMA);
            tuple->setSpecs(parseDSeq<SpecAstList, HsParser>(
                                TK_COMMA, &HsParser::parseType));
            matchOrSkipTo(TK_RPAREN, "tuple type closing parenthesis");
            tuple->setRDelimLoc(prevLoc_);
            return std::move(tuple);
        }
    }
}

    //--- Expressions ---//

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
        return ErrorExprAst::create(prevLoc_);
    }
}

Parser::Expr HsParser::parsePlainRhs()
{
    return parseExpr();
}

Parser::Expr HsParser::parseGuardRhs()
{
    return Expr();
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
        ParenMatcher<> wrap(this, "closing var/con sym paren");
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

Parser::Name HsParser::parseSymParenOrId(Name (HsParser::*parseSymWrap)(),
                                         Name (HsParser::*parseId)())
{
    if (ahead_ == TK_LPAREN)
        return (this->*(parseSymWrap))();
    return (this->*(parseId))();
}

Parser::Name HsParser::parseSymOrIdTick(Name (HsParser::*parseSym)(),
                                        Name (HsParser::*parseIdTick)())
{
    if (ahead_ == TK_BACKTICK)
        return (this->*(parseIdTick))();
    return (this->*(parseSym))();
}

Parser::Name HsParser::parseQCon()
{
    return parseSymParenOrId(&HsParser::parseQConSymParen, &HsParser::parseQConId);
}

Parser::Name HsParser::parseQVar()
{
    return parseSymParenOrId(&HsParser::parseQVarSymParen, &HsParser::parseQVarId);
}

Parser::Name HsParser::parseCon()
{
    return parseSymParenOrId(&HsParser::parseConSymParen, &HsParser::parseConId);
}

Parser::Name HsParser::parseVar()
{
    return parseSymParenOrId(&HsParser::parseVarSymParen, &HsParser::parseVarId);
}

Parser::Name HsParser::parseConOp()
{
    return parseSymOrIdTick(&HsParser::parseConSym, &HsParser::parseConIdTick);
}

Parser::Name HsParser::parseVarOp()
{
    return parseSymOrIdTick(&HsParser::parseVarSym, &HsParser::parseVarIdTick);
}

Parser::Name HsParser::parseQConId()
{
    return parseQName(TK_PROPER_IDENT_QUAL, &HsParser::parseConId);
}

Parser::Name HsParser::parseQVarId()
{
    return parseQName(TK_PROPER_IDENT_QUAL, &HsParser::parseVarId);
}

Parser::Name HsParser::parseQConSymParen()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "qconsym in parens");
    return parseQName(TK_SPECIAL_IDENT_QUAL, &HsParser::parseConSym);
}

Parser::Name HsParser::parseQVarSymParen()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "qvarsym in parens");
    return parseQName(TK_PUNC_IDENT_QUAL, &HsParser::parseVarSym);
}

Parser::Name HsParser::parseConSymParen()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "consym in parens");
    return parseConSym();
}

Parser::Name HsParser::parseVarSymParen()
{
    UAISO_ASSERT(ahead_ == TK_LPAREN, return Name());
    ParenMatcher<> wrap(this, "varsym in parens");
    return parseVarSym();
}

Parser::Name HsParser::parseConIdTick()
{
    UAISO_ASSERT(ahead_ == TK_BACKTICK, return Name());
    Matcher<TK_BACKTICK, TK_BACKTICK> wrap(this, "conid in backticks");
    return parseConId();
}

Parser::Name HsParser::parseVarIdTick()
{
    UAISO_ASSERT(ahead_ == TK_BACKTICK, return Name());
    Matcher<TK_BACKTICK, TK_BACKTICK> wrap(this, "varid in backticks");
    return parseVarId();
}

Parser::Name HsParser::parseQConSym()
{
    UAISO_ASSERT(ahead_ == TK_SPECIAL_IDENT
                 || ahead_ == TK_SPECIAL_IDENT_QUAL, return Name());
    return parseQName(TK_SPECIAL_IDENT_QUAL, &HsParser::parseConSym);
}

Parser::Name HsParser::parseConSym()
{
    auto check = [this] (const Token tk) { return isConSym(tk); };
    if (match(check))
        return SpecialNameAst::create(prevLoc_);
    return ErrorNameAst::create(prevLoc_);
}

Parser::Name HsParser::parseVarSym()
{
    auto check = [this] (const Token tk) { return isVarSym(tk); };
    if (match(check))
        return PuncNameAst::create(prevLoc_);
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

Parser::Name HsParser::parseQConIdTick()
{
    UAISO_ASSERT(ahead_ == TK_BACKTICK, return Name());
    Matcher<TK_BACKTICK, TK_BACKTICK> wrap(this, "conid in backticks");
    return parseQConId();
}

Parser::Name HsParser::maybeParseVar()
{
    if (maybeConsume(TK_IDENT))
        return SimpleNameAst::create(prevLoc_);

    if (ahead_ == TK_LPAREN
               && isVarSym(peekToken(2))
               && peekToken(3) == TK_RPAREN) {
        consumeToken(2);
        auto name = PuncNameAst::create(prevLoc_);
        consumeToken(); // The closing `)'.
        return std::move(name);
    }

    return Name(); // A "maybe" function may return null.
}

Parser::Name HsParser::maybeParseQConOp()
{
    if (maybeConsume(TK_COLON))
        return SpecialNameAst::create(prevLoc_);

    if (ahead_ == TK_SPECIAL_IDENT_QUAL || ahead_ == TK_SPECIAL_IDENT)
        return parseQConSym();

    const Token peek = peekToken(2);
    if (ahead_ == TK_BACKTICK
            && (peek == TK_PROPER_IDENT || peek == TK_PROPER_IDENT_QUAL)) {
        return parseQConIdTick();
    }

    return Name(); // A "maybe" function may return null.
}

Parser::Name HsParser::parseQName(Token qualTk, Name (HsParser::*parseFunc)())
{
    auto qname = NestedNameAst::create();
    while (maybeConsume(qualTk)) {
        appendOrCreate(qname->names_, SimpleNameAst::create(prevLoc_));
        match(TK_JOKER);
    }
    appendOrCreate(qname->names_, ((this->*(parseFunc))()));
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
