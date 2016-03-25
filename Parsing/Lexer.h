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

#ifndef UAISO_LEXER_H__
#define UAISO_LEXER_H__

#include "Common/Config.h"
#include "Parsing/SourceLoc.h"
#include "Parsing/Token.h"
#include <cstddef>
#include <memory>

namespace uaiso {

class ParsingContext;
class Lang;

/*!
 * \brief The Lexer class
 */
class UAISO_API Lexer
{
public:
    virtual ~Lexer();

    /*!
     * \brief setBuffer
     * \param buff
     * \param len
     */
    void setBuffer(const char* buff, size_t len);

    /*!
     * \brief setContext
     * \param context
     */
    void setContext(ParsingContext* context);

    /*!
     * \brief lex
     * \return
     *
     * Lex the next token.
     */
    virtual Token lex() = 0;

    /*!
     * \brief tokenLoc
     * \return
     *
     * Return the location of the lastly lexed token.
     *
     * \note The location will have no file information associated with it.
     */
    SourceLoc tokenLoc() const;

protected:
    Lexer();

    void updatePos();
    void handleNewLine();
    void handleNewLineNoColReset();

    char peekChar(size_t dist = 0) const;
    void consumeChar(size_t dist = 0);
    char consumeCharPeekNext(size_t dist = 0);

    Token lexIdentOrKeyword(char& ch, const Lang* lang);
    Token lexStrLit(char& ch, const bool mayBreak, const Lang* lang);
    Token lexStrLitEnd(char& ch, const char quote, const bool mayBreak, const Lang* lang);
    Token lexNumLit(char& ch, const Lang* lang);
    Token lexFloatLit(char& ch, const Lang* lang);

    virtual Token classifyKeyword(const char* spell, size_t len) const = 0;
    virtual Token filterIdent(char& ch);

    bool inCompletionArea() const;
    bool maybeRealizeCompletion();

    const char* buff_ { nullptr };     //!< The buffer.
    const char* mark_ { nullptr };     //!< Start of a token in buffer.
    const char* curr_ { nullptr };     //!< Current buffer position.
    const char* eof_ { nullptr };      //!< End of the buffer.

    int line_ { 0 };        //!< First line of a token.
    int col_ { 0 };         //!< First column of a token.
    int breaks_ { 0 };      //!< Number of line breaks within a token.
    int rearLeng_ { 0 };    //!< Length of a token's last line.

    ParsingContext* context_ { nullptr };
};

} // namespace uaiso

#endif
