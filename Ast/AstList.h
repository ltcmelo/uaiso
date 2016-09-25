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

#ifndef UAISO_ASTLIST_H__
#define UAISO_ASTLIST_H__

#include "Ast/AstFwd.h"
#include "Common/Assert.h"
#include "Parsing/SourceLoc.h"
#include <iostream>
#include <memory>
#include <utility>

namespace uaiso {

template <class AstT>
class ListTraits
{
public:
    using AstType = AstT;
};

/*!
 * \brief AstList
 *
 * In addition to conventional linked-list methods, this list also provides a
 * convenient interface that can be used with shift-reduce (SR) parsing
 * while preserving the original order of items. The methods that operate on
 * raw pointers originally exist for consistency with the C Bison-based parsers.
 */
template <class DerivedListT, class ListTraitsT>
class UAISO_API AstList
{
public:
    using ListType = DerivedListT;
    using AstType = typename ListTraitsT::AstType;

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
     * \brief createSR
     * \param ast
     * \return
     * \warning It's a bug not to call finishSR after creating a list this way.
     * \sa handleSR(), finishSR
     *
     * Create a list in such a way that it can be easily used for shift-reduce
     * (SR) parsing. A list created through this method requires that elements
     * arebe added by calling handleSR. Once all elements have been "handled",
     * the list must be put into linear form by calling finishSR.
     */
    static ListType* createSR(AstType* ast)
    {
        UAISO_ASSERT(ast, return nullptr, "invalid ast");

        auto list = new ListType(ast, nullptr);
        list->next_.reset(list); // Self-referencing until being linearized.
        return list;
    }

    /*!
     * \brief handleSR
     * \param ast
     * \return the new head of the list
     * \sa finishSR, createSR
     *
     * Add an item to a list created for shift-reduce (SR) parsing. The returned
     * pointer is the new head of the list. It is the caller responsibility to
     * take ownership over it.
     */
    ListType* handleSR(AstType* ast)
    {
        UAISO_ASSERT(ast, return nullptr, "invalid ast");
        UAISO_ASSERT(next_.get(),
                     return nullptr,
                     "list has already been finished (linearized)");

        auto list = new ListType(ast, next_.release());
        next_.reset(list);
        return list;
    }

    /*!
     * \brief finishSR
     * \return the new head of the list
     *
     * Put a list created for shift-reduce (SR) parsing into linear form. The
     * returned pointer is the new head of the list. It is the caller responsibility
     * to take ownership over it.
     */
    ListType* finishSR()
    {
        UAISO_ASSERT(next_.get() != nullptr, return nullptr, "list is linear");

        return next_.release();
    }

    /*!
     * \brief mergeSR
     * \param list
     * \return
     *
     * Merge two shift-reduce (SR) lists.
     */
    ListType* mergeSR(ListType* list)
    {
        UAISO_ASSERT(list, return nullptr, "invalid list node");

        list->next_.release();
        list->next_.reset(next_.release());
        next_.reset(list);
        return list;
    }

    /*!
     * \brief create
     * \param ast
     * \return
     *
     * Create a list.
     */
    static std::unique_ptr<ListType> create(std::unique_ptr<AstType> ast)
    {
        UAISO_ASSERT(ast, return std::unique_ptr<ListType>(), "invalid ast");

        return std::unique_ptr<ListType>(new ListType(ast.release(), nullptr));
    }

    void append(std::unique_ptr<AstType> ast)
    {
        UAISO_ASSERT(ast, return, "invalid ast");

        next_ ? next_->append(std::move(ast))
              : next_.reset(new ListType(ast.release(), nullptr));
    }

    void merge(std::unique_ptr<ListType> list)
    {
        if (!list)
            return;
        next_ ? next_->merge(std::move(list))
              : static_cast<void>(next_ = std::move(list));
    }

    ListType* subList() const { return next_.get(); }

    ListType* lastSubList() const
    {
        return next_ ? next_->lastSubList()
                     : const_cast<ListType*>(static_cast<const ListType*>(this));
    }

    AstType* front() const { return ast_.get(); }

    AstType* back() const
    {
        return next_ ? next_->back() : ast_.get();
    }

    /*!
     * \brief detachHead
     * \return
     *
     * Detach the list's head, returning the front item and the new head.
     */
    std::pair<std::unique_ptr<AstType>, std::unique_ptr<ListType>> detachHead()
    {
        std::unique_ptr<ListType> newHead = std::move(next_);
        next_.reset(nullptr);
        return std::make_pair(std::move(ast_), std::move(newHead));
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

        AstType* operator*() { return list_->front(); }

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
    AstList(AstType* ast, ListType* next)
        : ast_(ast), next_(next)
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
void appendOrCreate(std::unique_ptr<AstListT>& list,
                    std::unique_ptr<typename AstListT::AstType> ast)
{
    list ? list->append(std::move(ast))
         : static_cast<void>(list = AstListT::create(std::move(ast)));
}

template <class AstListT>
void mergeOrReplace(std::unique_ptr<AstListT>& list,
                    std::unique_ptr<AstListT> other)
{
    list ? list->merge(std::move(other))
         : static_cast<void>(list = std::move(other));
}

} // namespace uaiso

#endif
