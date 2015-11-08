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

#include "Python/PyUnit.h"
#include "Python/PyLexer.h"
#include "Python/PyParser.h"
#include "Ast/Ast.h"
#include "Parsing/ParsingContext.h"
#include "Parsing/Diagnostic.h"
#include "Parsing/Token.h"
#include "Parsing/Unit__.h"

#define TRACE_NAME "PyUnit"

using namespace uaiso;

void PyUnit::parseCore(TokenMap* tokens,
                       LexemeMap* lexemes,
                       ParsingContext* context)
{
    P->ast_.reset(nullptr);

    context->collectLexemes(lexemes);
    context->collectTokens(tokens);
    context->collectReports(P->reports_.get());
    context->setFileName(P->fullFileName_.c_str());

    PyLexer lexer;
    lexer.setContext(context);
    std::unique_ptr<std::string> buff;
    if (P->bit_.readFromFile_) {
        fseek(P->file_, 0, SEEK_END);
        buff.reset(new std::string);
        buff->resize(ftell(P->file_));
        rewind(P->file_);
        fread(&(*buff)[0], 1, buff->size(), P->file_);
        lexer.setBuffer(buff->c_str(), buff->size());
    } else {
        lexer.setBuffer(P->source_->c_str(), P->source_->size());
    }


    PyParser parser;
    bool success = parser.parse(&lexer, context);
    if (success)
        P->ast_.reset(context->releaseAst());
}

void PyUnit::parse(TokenMap* tokens, LexemeMap* lexemes)
{
    ParsingContext context;
    parseCore(tokens, lexemes, &context);
}

void PyUnit::parse(TokenMap* tokens,
                   LexemeMap* lexemes,
                   const LineCol& lineCol)
{
    ParsingContext context;
    context.setStopMark(lineCol);
    parseCore(tokens, lexemes, &context);
}
