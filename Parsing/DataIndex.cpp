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

#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/SourceLoc.h"
#include "Parsing/TokenMap.h"
#include "Common/LineCol.h"
#include <iostream>
#include <unordered_set>
#include <unordered_map>

using namespace uaiso;

namespace {

template <class DataT>
struct DataIndex
{
    using Data = std::unordered_set<DataT>;
    using LineColIndex = std::unordered_map<LineCol, typename Data::const_iterator>;
    using FileIndex = std::unordered_map<std::string, std::unique_ptr<LineColIndex>>;

    Data data_;
    FileIndex fileIndex_;
};

} // anonymous

namespace std {

// As for the equal value, this is an specialization that computes that hash
// value of a lexeme based on the underlying string (not on the pointer).
template <> struct hash<std::unique_ptr<Lexeme>>
{
    size_t operator()(const std::unique_ptr<Lexeme>& value)
    {
        return std::hash<std::string>()(value->s_);
    }
};

// As for the hash value, this is an specialization that computes whether two
// lexemes are equal based on the underlying string (not on the pointer).
template <> struct equal_to<std::unique_ptr<Lexeme>>
{
    bool operator()(const std::unique_ptr<Lexeme>& a,
                    const std::unique_ptr<Lexeme>& b)
    {
        return a->s_ == b->s_;
    }
};

} // namespace std


    /*--- LexemeMap ---*/

struct uaiso::LexemeMap::LexemeMapImpl
        : public DataIndex<std::unique_ptr<Lexeme>>
{
    const Ident* self_ { nullptr };
};

LexemeMap::LexemeMap()
    : impl_(new LexemeMapImpl)
{
    insertPredefined();
}

LexemeMap::~LexemeMap()
{}

const Ident* LexemeMap::self() const
{
    return P->self_;
}

void LexemeMap::insertPredefined()
{
    P->self_ = insertOrFind<Ident>("self", "<__predefined__>", LineCol(-1, -1));
}

template <class ValueT>
const ValueT* LexemeMap::insertOrFind(const std::string& s,
                                      const std::string& fullFileName,
                                      const LineCol& lineCol)
{
    auto &byFile = P->fileIndex_[fullFileName];
    if (!byFile) {
        byFile.reset(new DataIndex<std::unique_ptr<Lexeme>>::LineColIndex);
    } else {
        auto posIt = byFile->find(lineCol);
        if (posIt != byFile->end())
            return static_cast<const ValueT*>(posIt->second->get());
    }

    // A mapping for this line/col doesn't exist, but this value might
    // be in the global index (from another occurrence). We use a hack
    // to allow looking it up without actually allocating a new value.
    static char local[sizeof(ValueT)];
    std::unique_ptr<Lexeme> helper(new (local) ValueT(s));
    auto valIt = P->data_.find(helper);
    helper.release();

    if (valIt == P->data_.end()) {
        // The value is not in the index, we must create it.
        valIt = P->data_.emplace(new ValueT(s)).first;
    }

    // Now, add a mapping from the line/col to the value.
    byFile->emplace(lineCol, valIt);

    return static_cast<const ValueT*>(valIt->get());
}

template <class ValueT>
const ValueT* LexemeMap::find(const std::string& fullFileName,
                              const LineCol& lineCol) const
{
    auto byFileIt = P->fileIndex_.find(fullFileName);
    if (byFileIt == P->fileIndex_.end()
            || !byFileIt->second) {
        return nullptr;
    }

    auto it = byFileIt->second->find(lineCol);
    if (it == byFileIt->second->end())
        return nullptr;

    return static_cast<const ValueT*>(it->second->get());
}

template <class ValueT>
std::vector<std::tuple<const ValueT*, LineCol>>
LexemeMap::list(const std::string& fullFileName) const
{
    std::vector<std::tuple<const ValueT*, LineCol>> v;

    auto byFileIt = P->fileIndex_.find(fullFileName);
    if (byFileIt == P->fileIndex_.end() || !byFileIt->second)
        return v;

    for (auto pos : *byFileIt->second) {
        const LineCol& lineCol = pos.first;
        const ValueT* value = static_cast<const ValueT*>(pos.second->get());
        v.push_back(std::make_tuple(value, lineCol));
    }

    return v;
}

void LexemeMap::clear()
{
    P->data_.clear();
    P->fileIndex_.clear();
    insertPredefined();
}

void LexemeMap::clear(const std::string& fullFileName)
{
    auto byFileIt = P->fileIndex_.find(fullFileName);
    if (byFileIt != P->fileIndex_.end())
        byFileIt->second->clear();
}

template const Ident*
LexemeMap::insertOrFind<Ident>(const std::string&,
                               const std::string&,
                               const LineCol& lineCol);
template const StrLit*
LexemeMap::insertOrFind<StrLit>(const std::string&,
                                const std::string&,
                                const LineCol& lineCol);
template const NumLit*
LexemeMap::insertOrFind<NumLit>(const std::string&,
                                const std::string&,
                                const LineCol& lineCol);
template const Ident*
LexemeMap::find<Ident>(const std::string&, const LineCol& lineCol) const;
template const StrLit*
LexemeMap::find<StrLit>(const std::string&, const LineCol& lineCol) const;
template const NumLit*
LexemeMap::find<NumLit>(const std::string&, const LineCol& lineCol) const;

template std::vector<std::tuple<const Ident*, LineCol>>
LexemeMap::list<Ident>(const std::string&) const;
template std::vector<std::tuple<const StrLit*, LineCol>>
LexemeMap::list<StrLit>(const std::string&) const;
template std::vector<std::tuple<const NumLit*, LineCol>>
LexemeMap::list<NumLit>(const std::string&) const;


    /*--- TokenMap ---*/

struct uaiso::TokenMap::TokenMapImpl : public DataIndex<int>
{};

TokenMap::TokenMap()
    : impl_(new TokenMapImpl)
{}

TokenMap::~TokenMap()
{}

Token TokenMap::insertOrFind(int tk,
                             const std::string& file,
                             const LineCol& lineCol)
{
    auto &byFile = P->fileIndex_[file];
    if (!byFile) {
        byFile.reset(new DataIndex<int>::LineColIndex);
    } else {
        auto posIt = byFile->find(lineCol);
        if (posIt != byFile->end())
            return Token(*posIt->second);
    }

    // A mapping for this line/col doesn't exist, but this value might
    // be in the global index (from another occurrence). Insert will
    // either create it or return an iterator to the existing one.
    auto valPair = P->data_.insert(tk);

    // Now, add a mapping from the line/col to the value.
    byFile->emplace(lineCol, valPair.first);

    return Token(*valPair.first);
}

Token TokenMap::find(const std::string& fullFileName,
                     const LineCol& lineCol) const
{
    auto byFileIt = P->fileIndex_.find(fullFileName);
    if (byFileIt == P->fileIndex_.end() || !byFileIt->second)
        return Token::TK_INVALID;

    auto it = byFileIt->second->find(lineCol);
    if (it == byFileIt->second->end())
        return Token::TK_INVALID;

    return Token(*it->second);
}

void TokenMap::clear()
{
    P->data_.clear();
    P->fileIndex_.clear();
}

void TokenMap::clear(const std::string& fullFileName)
{
    auto infoIt = P->fileIndex_.find(fullFileName);
    if (infoIt != P->fileIndex_.end())
        infoIt->second->clear();
}
