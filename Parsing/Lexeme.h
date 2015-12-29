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

#ifndef UAISO_LEXEME_H__
#define UAISO_LEXEME_H__

#include "Common/Assert.h"
#include "Common/Config.h"
#include <string>
#include <iostream>

namespace uaiso {

class Ident;
class StrLit;
class NumLit;

/*!
 * \brief The Lexeme class
 */
class UAISO_API Lexeme
{
public:
    enum class Kind : char
    {
        Ident,
        StrLit,
        NumLit
    };

    virtual ~Lexeme();

    /*!
     * \brief str
     * \return
     */
    virtual std::string str() const { return s_; }

protected:
    friend struct std::hash<std::unique_ptr<Lexeme>>;
    friend struct std::equal_to<std::unique_ptr<Lexeme>>;

    Lexeme(const std::string& s, Kind kind);

    Lexeme(const Lexeme& tv);
    Lexeme(Lexeme&& tv);
    Lexeme& operator=(const Lexeme&);
    Lexeme& operator=(Lexeme&&);

    std::string s_;
    Kind kind_;
};

/*!
 * \brief The Ident class
 *
 * An identifier.
 */
class UAISO_API Ident final : public Lexeme
{
public:
    Ident(const std::string& s)
        : Lexeme(s, Kind::Ident)
    {}
};

const Ident kNullIdent("<null>");
const Ident kErrorIdent("<error>");
const Ident kCompletionIdent("<completion>");

/*!
 * \brief The StrLit class
 */
class UAISO_API StrLit final : public Lexeme
{
public:
    StrLit(const std::string& s)
        : Lexeme(s, Kind::StrLit)
    {
        UAISO_ASSERT(s.length() >= 2, {}); // At least 2 delimiters.
    }

    std::string str() const override { return s_.substr(1, s_.length() - 2); }
};

/*!
 * \brief The NumLit class
 */
class UAISO_API NumLit : public Lexeme
{
public:
    using Lexeme::Lexeme;

    NumLit(const std::string& s)
        : Lexeme(s, Kind::NumLit)
    {}
};

template <class ContainterT>
std::string joinLexemes(const ContainterT& container,
                        const std::string& separator)
{
    std::string str;
    for (auto lex : container) {
        if (!str.empty())
            str += separator;
        str += lex->str();
    }
    return str;
}

} // namespace uaiso

#define ConstIdent_Cast(T) static_cast<const Ident*>(T)
#define Ident_Cast(T) static_cast<Ident*>(T)
#define Ident_ConstCast(T) const_cast<Ident*>(Ident_Cast(T))

#define ConstStrLit_Cast(T) static_cast<const StrLit*>(T)
#define StrLit_Cast(T) static_cast<StrLit*>(T)
#define StrLit_ConstCast(T) const_cast<StrLit*>(StrLit_Cast(T))

#define ConstNumLit_Cast(T) static_cast<const NumLit*>(T)
#define NumLit_Cast(T) static_cast<NumLit*>(T)
#define NumLit_ConstCast(T) const_cast<NumLit*>(NumLit_Cast(T))

#endif
