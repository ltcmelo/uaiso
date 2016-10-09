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

#include "Haskell/HsLexer.h"
#include "Haskell/HsKeywords.h"
#include "Haskell/HsLang.h"
#include "Common/Assert.h"
#include "Common/Trace__.h"
#include "Parsing/ParsingContext.h"
#include "Parsing/Lexeme.h"
#include <cctype>

#define TRACE_NAME "HsLexer"

using namespace uaiso;

namespace {

const HsLang hsLang;

} // anonymous

HsLexer::HsLexer()
    : bits_(0)
{
    bit_.atLineStart_ = true;
}

HsLexer::~HsLexer()
{}

Token HsLexer::pendingToken()
{
    // Check for pending lexemes from a qualified name or operator. See the
    // lexing of identifiers for a detailed explanation.
    if (bit_.delimCnt_
            && bit_.delimCnt_ == (bit_.pendQual_
                                  + bit_.pendName_
                                  + bit_.pendPropName_
                                  + (bit_.pendSym_ || bit_.pendCon_ ? 1 : 0))) {
        --bit_.delimCnt_;
        return TK_JOKER;
    }

    if (bit_.pendQual_) {
        --bit_.pendQual_;
        return (bit_.pendSym_ ?
                    TK_PUNC_IDENT_QUAL
                  : bit_.pendCon_ ?
                        TK_SPECIAL_IDENT_QUAL
                      : bit_.pendPropName_ ? TK_PROPER_IDENT_QUAL : TK_IDENT_QUAL);
    }

    if (bit_.pendName_) {
        --bit_.pendName_;
        return TK_IDENT;
    }

    if (bit_.pendPropName_) {
        --bit_.pendPropName_;
        return TK_PROPER_IDENT;
    }

    if (bit_.pendCon_) {
        --bit_.pendCon_;
        return TK_SPECIAL_IDENT;
    }

    if (bit_.pendSym_) {
        UAISO_ASSERT(bit_.pendSym_, return TK_INVALID);
        auto sym = static_cast<Token>(bit_.pendSym_);
        bit_.pendSym_ = 0;
        return sym;
    }

    return TK_INVALID;
}

Token HsLexer::lex()
{
    Token tk = pendingToken();
    if (tk != TK_INVALID)
        return tk;

    updatePos();

LexNextToken:
    mark_ = curr_; // Mark the start of the upcoming token.

    char ch = peekChar();

    if (bit_.wantBrace_) {
        maybeSkipSpaces(ch);
        bit_.wantBrace_ = false;
        if (ch == '{') {
            consumeChar();
            layoutStack_.push(std::make_pair(false, -1));
        } else {
            // Left-brace auto-insertion. Column is assigned later, when
            // indetation of the following token is discovered.
            bit_.waitOffsetMark_ = true;
            layoutStack_.push(std::make_pair(true, -1));
        }
        tk = TK_LBRACE;
        goto LexDone;
    }

    // At a line start the layout must be checked, a semicolon or a closing
    // brace might need to be inserted.
    if (bit_.atLineStart_ && !bit_.waitOffsetMark_ && !layoutStack_.empty()) {
        const Layout& layout = layoutStack_.top();
        if (layout.first) {
            maybeSkipSpaces(ch);
            if (ch && ch != '\n') {
                if (col_ == layout.second) {
                    tk = TK_SEMICOLON;
                    goto LexDone;
                }
                if (col_ < layout.second) {
                    layoutStack_.pop();
                    tk = TK_RBRACE;
                    goto LexDone;
                }
            }
        }
    }

    switch (ch) {
    case 0:
        if (!layoutStack_.empty()) {
            layoutStack_.pop();
            return TK_RBRACE;
        }
        return TK_EOP;

    case '\n':
        bit_.atLineStart_ = true;
        consumeChar();
        handleNewLine();
        updatePos();
        goto LexNextToken;

    case '\f':
    case '\t':
    case ' ':
        skipSpaces(ch);
        goto LexNextToken;

    case '(':
    case ')':
    case ',':
    case ';':
    case '[':
    case ']':
    case '`':
        tk = lexSpecial(ch);
        break;

    case '{':
        layoutStack_.push(std::make_pair(false, -1));
        tk = lexSpecial(ch);
        break;

    case '}':
        if (!layoutStack_.empty() && !layoutStack_.top().first)
            layoutStack_.pop();
        tk = lexSpecial(ch);
        break;

    case '!':
    case '#':
    case '$':
    case '%':
    case '&':
    case '*':
    case '+':
    case '/':
    case '>':
    case '?':
    case '@':
    case '\\':
    case '^':
    case '|':
    case '~':
        tk = lexAscSymbol(ch);
        break;

    case '.':
        tk = lexAscSymbol2(ch, '.', TK_DOT_DOT, TK_PUNC_IDENT);
        break;

    case '-':
        tk = lexAscSymbol2(ch, '>', TK_DASH_ARROW, TK_PUNC_IDENT);
        break;

    case '=':
        tk = lexAscSymbol2(ch, '>', TK_EQ_ARROW, TK_PUNC_IDENT);
        break;

    case '<':
        tk = lexAscSymbol2(ch, '-', TK_ARROW_DASH, TK_PUNC_IDENT);
        break;

    case ':':
        tk = lexAscSymbol2(ch, ':', TK_COLON_COLON, TK_SPECIAL_IDENT);
        break;

    case '"':
    case '\'':
        tk = lexStrLit(ch, false, &hsLang);
        context_->trackLexeme<StrLit>(mark_, curr_ - mark_, LineCol(line_, col_));
        break;

    default:
        if (hsLang.isIdentFirstChar(ch)) {
            tk = lexIdentOrKeyword(ch, &hsLang);
            break;
        }

        if (std::isdigit(ch)) {
            tk = lexNumLit(ch, &hsLang);
            context_->trackLexeme<NumLit>(mark_, curr_ - mark_, LineCol(line_, col_));
            break;
        }

        // Don't know what this is.
        consumeChar();
        PRINT_TRACE("Unknown char %c at %d,%d\n", ch, 0, 0);
        break;
    }

    if (bit_.waitOffsetMark_) {
        bit_.waitOffsetMark_ = false;
        UAISO_ASSERT(!layoutStack_.empty(), return TK_INVALID);
        layoutStack_.top().second = col_;
    }

LexDone:
    bit_.atLineStart_ = false;

    LineCol lineCol(line_, col_);
    context_->trackToken(tk, lineCol);
    context_->trackPhrase(tk, lineCol, curr_ - mark_);

    return tk;
}

namespace {

// A few tokens indexed by their corresponding ASCII value.
const int oprtrTable[] =
{
    // Category `special`
    TK_LPAREN,                                             // 0
    TK_RPAREN,
    TK_COMMA,
    TK_SEMICOLON,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_BACKTICK,
    TK_LBRACE,
    TK_RBRACE,                                             // 8

    // Category `ascSymbol`
    TK_EXCLAM,                                             // 9
    TK_POUND,
    TK_DOLLAR,
    TK_PERCENT,
    TK_AMPER,
    TK_ASTERISK,
    TK_PLUS,                                               // 15
    TK_MINUS,
    TK_DOT,
    TK_SLASH,
    TK_COLON,
    TK_LS,
    TK_EQ,
    TK_GR,
    TK_QUESTION,                                           // 23
    TK_AT,
    TK_BACKSLASH,
    TK_CARET,
    TK_PIPE,
    TK_TILDE,                                              // 28

    // Indexing
    -1, -1, -1, -1,
    9,                                                     // 33
    -1,
    10, 11, 12, 13,
    -1,                                                    // 39
    0, 1, 14, 15, 2, 16, 17, 18,
    -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1,                            // 51...57
    19, 3, 20, 21, 22, 23, 24,
    -1, -1, -1, -1, -1, -1,                                // 65...70
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 71...80
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 81...90
    4, 25, 5, 26,
    -1,
    6,
    -1, -1, -1, -1,                                        // 97...100
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 101...110
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,                // 111...120
    -1, -1,
    7, 27, 8, 28
};

} // anonymous

Token HsLexer::lexSpecial(char& ch)
{
    UAISO_ASSERT(ch >= 33 && ch <= 126, return TK_INVALID);

    const char index = oprtrTable[ch];
    ch = consumeCharPeekNext();

    return Token(oprtrTable[index]);
}

Token HsLexer::lexAscSymbol(char& ch)
{
    Token tk = lexSpecial(ch);
    if (!isAscSymbol(ch))
        return tk;

    return lexAscSymbolMore(ch, TK_PUNC_IDENT);
}

Token HsLexer::lexAscSymbol2(char& ch, const char& ch2, Token tk2, Token tkMore)
{
    Token tk = lexSpecial(ch);
    if (ch == ch2) {
        tk = tk2;
        ch = consumeCharPeekNext();
    }
    if (!isAscSymbol(ch))
        return tk;

    return lexAscSymbolMore(ch, tkMore);
}

Token HsLexer::lexAscSymbolMore(char &ch, Token tk)
{
    UAISO_ASSERT(isAscSymbol(ch), return TK_INVALID);
    do {
        ch = consumeCharPeekNext();
    } while (isAscSymbol(ch));

    return tk;
}

bool HsLexer::isAscSymbol(const char ch) const
{
    switch (ch) {
    case '!':
    case '#':
    case '$':
    case '%':
    case '&':
    case '*':
    case '+':
    case '.':
    case '/':
    case '<':
    case '=':
    case '>':
    case '?':
    case '@':
    case '\\':
    case '^':
    case '|':
    case '-':
    case '~':
    case ':':
        return true;

    default:
        return false;
    }
}

Token HsLexer::filterKeyword(const char* spell, size_t len) const
{
    return HsKeywords::filter(spell, len);
}

void HsLexer::inspectKeyword(Token tk)
{
    switch (tk) {
    case TK_WHERE:
    case TK_LET:
    case TK_DO:
    case TK_OF:
        bit_.wantBrace_ = true;
        break;

    default:
        break;
    }
}

Token HsLexer::classifyIdent(char& ch)
{
    if (mark_[0] >= 97) {
        context_->trackLexeme<Ident>(mark_, curr_ - mark_, LineCol(line_, col_));
        return TK_IDENT;
    }

    // We have a capitalized identifier. It could be a lexeme on its own or it
    // could be the qualification of something like `A.B`, `A.b`, or `A.+`,
    // which also must be lexed as a single lexeme. However, we deviate slightly
    // from the Haskell report here: our design cannot handle nested names
    // within a single lexeme, we need the independent parts of the name. So
    // we lex as according to the rules but we keep the individual identifier
    // and delimiter tokens pending, which are fed to the parser separately.
    // To avoid ambiguities with `.` we use a "joker" token as delimiter.
    Token tk = TK_PROPER_IDENT;

    std::function<void ()> classifyRecursively = [&]() {
        if (ch != '.') {
            context_->trackLexeme<Ident>(mark_, curr_ - mark_, LineCol(line_, col_));
            if (bit_.pendQual_) {
                // Previous name was not a qualifier, but a proper name.
                --bit_.pendQual_;
                ++bit_.pendPropName_;
            }
            return;
        }

        // The presence of the `.` is not sufficient to characterize the
        // qualification, it must be followed by an identifier or operator.
        char next = peekChar(1);
        if (hsLang.isIdentChar(next) && !bit_.pendName_) {
            ++bit_.delimCnt_;
            ch = consumeCharPeekNext();
            mark_ = curr_;
            while (hsLang.isIdentChar(ch))
                ch = consumeCharPeekNext();
            context_->trackLexeme<Ident>(mark_, curr_ - mark_, LineCol(line_, col_));
            if (next >= 97) {
                ++bit_.pendName_;
                tk = TK_IDENT_QUAL;
                return;
            }
            ++bit_.pendQual_;
            tk = TK_PROPER_IDENT_QUAL;
            classifyRecursively();
        }

        if (isAscSymbol(next)) {
            ++bit_.delimCnt_;
            ch = consumeCharPeekNext();
            if (next == ':') {
                lexAscSymbol2(ch, ':', TK_COLON_COLON, TK_SPECIAL_IDENT);
                ++bit_.pendCon_;
                tk = TK_SPECIAL_IDENT_QUAL;
                return;
            }

            if (next == '.')
                bit_.pendSym_ = lexAscSymbol2(ch, '.', TK_DOT_DOT, TK_PUNC_IDENT);
            else if (next == '-')
                bit_.pendSym_ = lexAscSymbol2(ch, '>', TK_DASH_ARROW, TK_PUNC_IDENT);
            else if (next == '=')
                bit_.pendSym_ = lexAscSymbol2(ch, '>', TK_EQ_ARROW, TK_PUNC_IDENT);
            else if (next == '<')
                bit_.pendSym_ = lexAscSymbol2(ch, '-', TK_ARROW_DASH, TK_PUNC_IDENT);
            else
                bit_.pendSym_ = lexAscSymbol(ch);
            tk = TK_PUNC_IDENT_QUAL;
            return;
        }
    };

    classifyRecursively();

    return tk;
}
