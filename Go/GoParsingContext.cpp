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

#include "Go/GoParsingContext.h"
#include "Ast/Ast.h"
#include "Parsing/Token.h"
#include <iostream>

using namespace uaiso;

GoParsingContext::GoParsingContext()
    : mayAddSemicolon_(false)
{}

int GoParsingContext::interceptRawToken(int token)
{
    switch (token) {
    case TK_COMPLETION:
    case TK_IDENTIFIER:
    case TK_STRING_LITERAL:
    case TK_CHAR_LITERAL:
    case TK_INTEGER_LITERAL:
    case TK_FLOAT32_LITERAL:
    case TK_TRUE_LITERAL:
    case TK_FALSE_LITERAL:
    case TK_NULL_LITERAL:
    case TK_BREAK:
    case TK_CONTINUE:
    case TK_FALLTHROUGH:
    case TK_RETURN:
    case TK_PLUS_PLUS:
    case TK_MINUS_MINUS:
    case TK_RPAREN:
    case TK_RBRACKET:
    case TK_RBRACE:
    // Go's spec treats builtins as identifiers, we don't.
    case TK_BOOL:
    case TK_BYTE:
    case TK_COMPLEX_FLOAT32:
    case TK_COMPLEX_FLOAT64:
    case TK_FLOAT32:
    case TK_FLOAT64:
    case TK_INT:
    case TK_INT8:
    case TK_INT16:
    case TK_INT32:
    case TK_INT64:
    case TK_RUNE:
    case TK_UINT:
    case TK_UINT8:
    case TK_UINT16:
    case TK_UINT32:
    case TK_UINT64:
        mayAddSemicolon_ = true;
        break;

    default:
        mayAddSemicolon_ = false;
        break;
    }

    return token;
}
