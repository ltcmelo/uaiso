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

#ifndef UAISO_PARSINGCONTEXT_H__
#define UAISO_PARSINGCONTEXT_H__

#include "Ast/AstFwd.h"
#include "Common/Config.h"
#include "Common/LineCol.h"
#include "Parsing/Diagnostic.h"
#include "Parsing/Token.h"
#include <memory>
#include <utility>
#include <vector>

namespace uaiso {

class LexemeMap;
class Phrasing;
class TokenMap;

/*!
 * \brief The ParsingContext class
 */
class UAISO_API ParsingContext
{
public:
    /*!
     * \brief ParsingContext
     */
    ParsingContext();

    ParsingContext(const ParsingContext&) = delete;
    ParsingContext(ParsingContext&&) = delete;
    ParsingContext& operator=(const ParsingContext&) = delete;
    ParsingContext& operator=(ParsingContext&&) = delete;
    virtual ~ParsingContext();

    /*!
     * \brief setFileName
     * \param fullFileName
     */
    void setFileName(const char* fullFileName);

    /*!
     * \brief fileName
     * \return
     */
    const char* fileName() const;

    /*!
     * \brief setAllowComments
     * \param enable
     */
    void setAllowComments(bool enable);

    /*!
     * \brief allowComments
     * \return
     */
    bool allowComments() const;

    /*!
     * \brief collectLexemes
     * \param lexemes
     *
     * Specify the map in which lexemes should be collected. If not set,
     * lexemes won't be stored anyhow.
     */
    void collectLexemes(LexemeMap* lexemes) { lexemes_ = lexemes; }

    /*!
     * \brief collectTokens
     * \param tokens
     *
     * Specify the map in which tokens should be collected. If not set,
     * tokens won't be stored anyhow.
     */
    void collectTokens(TokenMap* tokens) { tokens_ = tokens; }

    /*!
     * \brief collectPhrasing
     * \param phrase
     *
     * Specify where to collect parsing phrases. If not set, phrasing
     * won't be stored anyhow.
     */
    void collectPhrasing(Phrasing* phrase) { phrasing_ = phrase; }

    /*!
     * \brief collectReports
     * \param reports
     *
     * Specify where to collect parsing diagnostics. If not set, reports
     * won't be collected.
     */
    void collectReports(DiagnosticReports* reports) { reports_ = reports; }

    /*!
     * \brief trackLexeme
     * \param lexeme     - must be null terminated
     * \param lineCol    - line and column
     */
    template <class LexemeT>
    void trackLexeme(const char* lexeme, const LineCol& lineCol);

    /*!
     * \brief trackLexeme
     * \param lexeme     - doesn't need to be null terminated
     * \param count      - number of lexeme characters
     * \param lineCol    - line and column
     */
    template <class LexemeT>
    void trackLexeme(const char* lexeme, int count, const LineCol& lineCol);

    /*!
     * \brief trackToken
     * \param tk
     * \param lineCol
     */
    void trackToken(Token tk, const LineCol& lineCol);

    /*!
     * \brief trackPhrase
     * \param tk
     * \param lineCol
     * \param len
     * \param unterminated
     */
    void trackPhrase(Token tk,
                     const LineCol& lineCol,
                     int len,
                     bool unterminated = false);

    template <class... Args>
    void trackReport(Args&&... args);

    /*!
     * \brief setStopMark
     * \param lineCol
     */
    void setStopMark(const LineCol& lineCol);

    /*!
     * \brief stopMark
     * \return
     */
    const LineCol& stopMark() const;

    /*!
     * \brief hasStopMark
     * \return
     */
    bool hasStopMark() const;

    /*!
     * \brief clearStopMark
     */
    void clearStopMark();

    /*!
     * \brief notifyProgramMatched
     *
     * Informs that the a ProgramAst has been successfully parsed. Future
     * attemps from the lexer to postponeEoF will fail.
     */
    void notifyProgramMatched();

    /*!
     * \brief postponeEoF
     * \return
     *
     * We use an EOP (End-Of-Program) token to signal to a Bison parser that
     * ent of input has been reached, but giving it an opportunity to match
     * the current rule in the case - we cannot use EOF because only the top-
     * level rule is allowed to match `$end`.
     *
     * This method is a query to decide whether or not the lexer should feed
     * the parser with an EOP token instead of the real EOF one.
     *
     * \sa notifyProgramMatched
     */
    bool postponeEoF();

    void enterTokenState();
    void leaveTokenState();
    bool hasTokenState() const;

    /*!
     * \brief takeAst
     * \param ast
     *
     * Let the context take ownership of the parsed AST.
     */
    void takeAst(std::unique_ptr<Ast> ast);

    /*!
     * \brief releaseAst
     * \return
     *
     * Release AST onwerhip from the context.
     */
    Ast* releaseAst();

    /*!
     * \brief interceptRawToken
     * \param token
     * \return
     *
     * A "raw" token is an int-compatible uaiso::Token representation,
     * such as the one coming from flex/bison.
     */
    virtual int interceptRawToken(int token);

protected:
    const char* fileName_ { nullptr };
    LexemeMap* lexemes_ { nullptr };
    TokenMap* tokens_ { nullptr };
    Phrasing* phrasing_ { nullptr };
    DiagnosticReports* reports_ { nullptr };
    std::unique_ptr<Ast> ast_;

    //! Premature stop of lexing, useful for completion.
    LineCol stopMark_;

    //! How long the lexer will continue feeding the parser with an EOP
    //! until it eventually sends an EOF.
    size_t toleranceCounter_ { 20 };

    struct BitFields
    {
        uint8_t comments_                 : 1;
        uint8_t prevTkTerminated_         : 1;
        uint8_t acceptEOF_                : 1;
        uint8_t hasState_                 : 1;
    };
    union
    {
        BitFields bit_;
        uint8_t   bits_;
    };
};

template <class... Args>
void ParsingContext::trackReport(Args&&... args)
{
    if (reports_)
        reports_->add(std::forward<Args>(args)...);
}

} // namespace uaiso

#endif
