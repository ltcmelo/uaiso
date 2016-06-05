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
#include "Parsing/ParsingContext.h"

#define TRACE_NAME "HsParser"

using namespace uaiso;

HsParser::HsParser()
{}

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
    // TODO: Exports
    match(TK_WHERE);
    module->setTerminLoc(lastLoc_);

    return Decl();
}

Parser::Name HsParser::parseModidName()
{
    auto fullName = NestedNameAst::create();
    do {
        match(TK_PROPER_IDENT);
        auto name = SimpleNameAst::create();
        name->setNameLoc(lastLoc_);
        fullName->addName(name.release());
    } while (maybeConsume(TK_JOKER));

    // For names we don't have the luxury of simply failing a match because we
    // would end up with a identifier without a corresponding location in the
    // lexeme map (which is not allowed in our design).
    if (!fullName->names())
        return Name(newAst<ErrorNameAst>()->setErrorLoc(lastLoc_));

    return Name(fullName.release());
}

Parser::DeclList HsParser::parseBody()
{
    return DeclList();
}

Parser::Expr HsParser::parseExpr()
{
    return parseAExpr();
}

/*
 * Rules: aexp
 */
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
