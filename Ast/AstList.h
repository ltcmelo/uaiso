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

#ifndef UAISO_ASTLIST_H__
#define UAISO_ASTLIST_H__

#include "Ast/AstFwd.h"
#include "Common/Assert.h"
#include "Parsing/SourceLoc.h"
#include <iostream>

namespace uaiso {

template <class AstT>
class ListTraits
{
public:
    using AstType = AstT;
};

template <class DerivedListT, class ListTraitsT>
class UAISO_API AstList
{
public:
    using ListType = DerivedListT;
    using AstType = typename ListTraitsT::AstType;

    /*!
     * \brief createSR
     * \param ast
     * \return
     *
     * Create a list in such a way that it can be easily used for shift-
     * reduce (SR) parsing. In this list elements should be "added" by
     * calling handleSR. After parsing, the list must be put into linear
     * form by calling finishSR.
     *
     * \warning It's a bug not to call finishSR after creating a list this way.
     * \sa handleSR(), finishSR
     */
    static ListType* createSR(AstType* ast)
    {
        UAISO_ASSERT(ast, return nullptr, "invalid ast");

        return new ListType(ast);
    }

    static ListType* create(AstType* ast)
    {
        UAISO_ASSERT(ast, return nullptr, "invalid ast");

        return new ListType(ast, nullptr);
    }

    virtual ~AstList()
    {
        // TODO: Debug-mode check
        const ListType* next = next_.get();
        while (next) {
            if (next == this) {
                std::cout << "Whops, list not linear" << std::endl;
                break;
            }
            next = next->next_.get();
        }
    }

    /*!
     * \brief handleSR
     *
     * Add an item to a list created for shift-reduce parsing. The returned
     * pointer is the new head of the list and must be held by the caller.
     *
     * \param ast
     * \return the new head of the list
     *
     * \sa finishSR, createSR
     */
    ListType* handleSR(AstType* ast)
    {
        UAISO_ASSERT(ast, return nullptr, "invalid ast");
        UAISO_ASSERT(next_.get() != nullptr, return nullptr, "list is linear");

        auto node = new ListType(ast, next_.release());
        next_.reset(node);
        return node;
    }

    /*!
     * \brief finishSR
     *
     * Put a list create for shift-reduce back into linear form. The returned
     * pointer is the new head of the list and must be held by the caller.
     *
     * \return the new head of the list
     */
    ListType* finishSR()
    {
        UAISO_ASSERT(next_.get() != nullptr, return nullptr, "list is linear");

        return next_.release();
    }

    /*!
     * \brief mergeSR
     *
     * Merge two shift-reduce lists.
     *
     * \param list
     * \return
     */
    ListType* mergeSR(ListType* list)
    {
        UAISO_ASSERT(list, return nullptr, "invalid list node");

        list->next_.release();
        list->next_.reset(next_.release());
        next_.reset(list);
        return list;
    }

    ListType* subList() const
    {
        return next_.get();
    }

    ListType* lastSubList() const
    {
        return next_ ? next_->lastSubList() :
                       const_cast<ListType*>(static_cast<const ListType*>(this));
    }

    AstType* head() const
    {
        return ast_.get();
    }

    /*
     * TODO: Remove, allows one to break list integrity.
     */
    std::unique_ptr<AstType> releaseHead()
    {
        return std::move(ast_);
    }

    AstType* back() const
    {
        return next_ ? next_->back() : ast_.get();
    }

    void pushBack(AstType* ast)
    {
        UAISO_ASSERT(ast, return, "invalid ast");

        if (next_)
            next_->pushBack(ast);
        else
            next_.reset(new ListType(ast, nullptr));
    }

    void merge(ListType* list)
    {
        UAISO_ASSERT(list, return, "invalid list node");

        if (next_)
            next_->merge(list);
        else
            next_.reset(list);
    }

    class Iterator
    {
    public:
        Iterator() : list_(0) {}
        Iterator(const ListType* list) : list_(list) {}

        Iterator& operator++()
        {
            list_ = list_->subList();
            return *this;
        }

        AstType* operator*() { return list_->head(); }

        bool operator!=(const Iterator& other) const
        {
            return list_ != other.list_;
        }

    private:
        const ListType* list_;
    };

    Iterator begin() const
    {
        return Iterator(static_cast<const ListType*>(this));
    }

    Iterator end() const { return Iterator(); }

protected:
    AstList(AstType* ast)
        : ast_(ast)
        , next_(static_cast<ListType*>(this))
    {}

    AstList(AstType* ast, ListType* next)
        : ast_(ast)
        , next_(next)
    {}

    std::unique_ptr<AstType> ast_;
    std::unique_ptr<ListType> next_;
};

template <class AstT>
class UAISO_API TrivialAstList : public AstList<TrivialAstList<AstT>,
                                        ListTraits<AstT>>
{
public:
    using Self = TrivialAstList<AstT>;
    using Base = AstList<TrivialAstList<AstT>, ListTraits<AstT>>;
    using AstType = AstT;
    using Traits = ListTraits<AstT>;

    template <class, class> friend class AstList;

    using AstList<Self, Traits>::AstList;
};

template <class AstT>
class UAISO_API DelimAstList : public AstList<DelimAstList<AstT>,
                                      ListTraits<AstT>>
{
public:
    using Self = DelimAstList<AstT>;
    using Base = AstList<DelimAstList<AstT>, ListTraits<AstT>>;
    using AstType = AstT;
    using Traits = ListTraits<AstT>;

    template <class, class> friend class AstList;

    using AstList<Self, Traits>::AstList;

    SourceLoc delim_;
};

using NameAstList = DelimAstList<NameAst>;
using SpecAstList = DelimAstList<SpecAst>;
using AttrAstList = TrivialAstList<AttrAst>;
using DeclAstList = DelimAstList<DeclAst>;
using ExprAstList = DelimAstList<ExprAst>;
using StmtAstList = TrivialAstList<StmtAst>;
using TemplateArgAstList = DelimAstList<TemplateArgAst>;
using GeneratorAstList = TrivialAstList<GeneratorAst>;
using FilterAstList = TrivialAstList<FilterAst>;

    //--- Convenience ---//

template <class AstListT>
void addToList(std::unique_ptr<AstListT>& list, typename AstListT::AstType* ast)
{
    if (ast)
        list ? list->pushBack(ast) : list.reset(AstListT::create(ast));
}

template <class AstListT>
void mergeList(std::unique_ptr<AstListT>& list, AstListT* otherList)
{
    if (otherList)
        list ? list->merge(otherList) : list.reset(otherList);
}

} // namespace uaiso

#endif
