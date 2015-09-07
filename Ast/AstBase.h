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

#ifndef UAISO_ASTBASE_H__
#define UAISO_ASTBASE_H__

#include "Ast/AstFwd.h"
#include "Ast/AstDefs.h"
#include "Ast/AstVariety.h"
#include "Common/Assert.h"
#include "Common/Config.h"
#include "Parsing/SourceLoc.h"
#include <cstdint>
#include <type_traits>

namespace uaiso {

/*!
 * \brief The Ast class
 *
 * The root of all AST nodes.
 */
class UAISO_API Ast
{
public:
    /*!
     * \brief The Kind enum
     */
    enum class Kind : uint16_t
    {
#define MAKE_CASE(AST_NODE, AST_KIND) AST_NODE##AST_KIND,

        Invalid,
        FirstNameMarker__,
NAME_AST_MIXIN(MAKE_NAME_CASE)
        LastNameMarker__,
        FirstSpecMarker__,
SPEC_AST_MIXIN(MAKE_SPEC_CASE)
        LastSpecMarker__,
        FirstAttrMarker__,
ATTR_AST_MIXIN(MAKE_ATTR_CASE)
        LastAttrMarker__,
        FirstDeclMarker__,
DECL_AST_MIXIN(MAKE_DECL_CASE)
        LastDeclMarker__,
        FirstExprMarker__,
EXPR_AST_MIXIN(MAKE_EXPR_CASE)
        LastExprMarker__,
        FirstStmtMarker__,
STMT_AST_MIXIN(MAKE_STMT_CASE)
        LastStmtMarker__,
        Program

#undef MAKE_CASE
    };

    using KindType = std::underlying_type<Kind>::type;

    /*!
     * \brief Ast
     */
    Ast() : bits_(0)
    {}

    /*!
     * \brief Ast
     * \param kind
     */
    Ast(Kind kind) : bits_(0)
    {
        bit_.kind = static_cast<KindType>(kind);
    }

    virtual ~Ast() {}

    /*!
     * \brief kind
     * \return
     *
     * Return the kind of the AST node.
     */
    Kind kind() const { return Kind(bit_.kind); }

    /*!
     * \brief kindStr
     * \return
     *
     * Return the kind of the AST node as a string.
     */
    static std::string kindStr(Kind kind)
    {
#define MAKE_CASE(AST_NODE, AST_KIND) \
    case static_cast<KindType>(Kind::AST_NODE##AST_KIND): \
        return #AST_NODE#AST_KIND;

        switch (static_cast<KindType>(kind)) {
            NAME_AST_MIXIN(MAKE_NAME_CASE)
            SPEC_AST_MIXIN(MAKE_SPEC_CASE)
            ATTR_AST_MIXIN(MAKE_ATTR_CASE)
            DECL_AST_MIXIN(MAKE_DECL_CASE)
            EXPR_AST_MIXIN(MAKE_EXPR_CASE)
            STMT_AST_MIXIN(MAKE_STMT_CASE)
        default:
            UAISO_ASSERT(false, return "");
            return "";
        }

#undef MAKE_CASE
    }

    bool isName() const
    {
        return checkKind(Kind::FirstNameMarker__, Kind::LastNameMarker__);
    }

    bool isSpec() const
    {
        return checkKind(Kind::FirstSpecMarker__, Kind::LastSpecMarker__);
    }

    bool isAttr() const
    {
        return checkKind(Kind::FirstAttrMarker__, Kind::LastAttrMarker__);
    }

    bool isDecl() const
    {
        return checkKind(Kind::FirstDeclMarker__, Kind::LastDeclMarker__);
    }

    bool isExpr() const
    {
        return checkKind(Kind::FirstExprMarker__, Kind::LastExprMarker__);
    }

    bool isStmt() const
    {
        return checkKind(Kind::FirstStmtMarker__, Kind::LastStmtMarker__);
    }

    bool isModule() const
    {
        return bit_.kind == static_cast<KindType>(Kind::Program);
    }

protected:
    // Bits are assigned by specific AST nodes.
    struct BitFields
    {
        uint32_t kind       : 16;
        uint32_t variety    : 4;
        uint32_t alloc      : 2;
    };
    union
    {
        BitFields bit_;
        uint32_t bits_;
    };

private:
    bool checkKind(Kind firstMarker, Kind lastMarker) const
    {
        return bit_.kind > static_cast<KindType>(firstMarker)
                && bit_.kind < static_cast<KindType>(lastMarker);
    }
};

template <class AstT>
AstT* make()
{
    return new AstT;
}

} // namespace uaiso

// Macros for named parameters AST functions

#define NAMED_AST_PARAM(NAME, MEMBER, PARAM_TYPE) \
    Self* NAME(PARAM_TYPE* param) \
    { \
        MEMBER##_.reset(param); \
        return this; \
    }

#define NAMED_AST_PARAM__BASE__(NAME, PARAM_TYPE) \
    virtual Self* NAME(PARAM_TYPE*) { return nullptr; }

#define NAMED_AST_PARAM__(NAME, TEMPLATE, PARAM_TYPE) \
    Self* NAME(PARAM_TYPE* param) override \
    { \
        TEMPLATE::NAME##__(param); \
        return this; \
    }

#define NAMED_AST_LIST_PARAM(NAME, MEMBER, PARAM_TYPE) \
    Self* NAME(PARAM_TYPE* param) \
    { \
        MEMBER##_.reset(PARAM_TYPE##List::create(param)); \
        return this; \
    } \
    Self* NAME(PARAM_TYPE##List* param) \
    { \
        MEMBER##_.reset(param->finishSR()); \
        return this; \
    } \

#define NAMED_AST_LIST_PARAM__BASE__(NAME, PARAM_TYPE) \
    virtual Self* NAME(PARAM_TYPE##List*) { return nullptr; } \
    virtual Self* NAME(PARAM_TYPE*) { return nullptr; } \

#define NAMED_AST_LIST_PARAM__(NAME, TEMPLATE, PARAM_TYPE) \
    Self* NAME(PARAM_TYPE* param) override \
    { \
        TEMPLATE::NAME##__(PARAM_TYPE##List::create(param)); \
        return this; \
    } \
    Self* NAME(PARAM_TYPE##List* param) override \
    { \
        TEMPLATE::NAME##__(param->finishSR()); \
        return this; \
    }

#define NAMED_LOC_PARAM(NAME, MEMBER) \
    Self* NAME##Loc(const SourceLoc& param) \
    { \
        MEMBER##Loc_ = param; \
        return this; \
    }

#define NAMED_LOC_PARAM__BASE__(NAME) \
    virtual Self* NAME(const SourceLoc&) { return nullptr; } \

#define NAMED_LOC_PARAM__(NAME, BASE_TEMPLATE) \
    Self* NAME(const SourceLoc& param) override \
    { \
        BASE_TEMPLATE::NAME##__(param); \
        return this; \
    }

#endif
