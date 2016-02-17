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

#ifndef UAISO_ASTLOCATOR_H__
#define UAISO_ASTLOCATOR_H__

#include "Ast/AstFwd.h"
#include "Ast/AstDefs.h"
#include "Common/Config.h"
#include "Parsing/SourceLoc.h"
#include <memory>

namespace uaiso {

class UAISO_API AstLocator
{
public:
    virtual ~AstLocator();

    const SourceLoc& loc(Ast*) const;
    const SourceLoc& lastLoc(Ast*) const;

#define DECLARE_FUNCS(AST_NODE, AST_KIND) \
    virtual const SourceLoc& loc(AST_NODE##AST_KIND##Ast*) const; \
    virtual const SourceLoc& lastLoc(AST_NODE##AST_KIND##Ast*) const;

#define DECLARE_NAME_FUNCS(AST_NODE, UNUSED) DECLARE_FUNCS(AST_NODE, Name)
#define DECLARE_SPEC_FUNCS(AST_NODE, UNUSED) DECLARE_FUNCS(AST_NODE, Spec)
#define DECLARE_ATTR_FUNCS(AST_NODE, UNUSED) DECLARE_FUNCS(AST_NODE, Attr)
#define DECLARE_DECL_FUNCS(AST_NODE, UNUSED) DECLARE_FUNCS(AST_NODE, Decl)
#define DECLARE_EXPR_FUNCS(AST_NODE, UNUSED) DECLARE_FUNCS(AST_NODE, Expr)
#define DECLARE_STMT_FUNCS(AST_NODE, UNUSED) DECLARE_FUNCS(AST_NODE, Stmt)

NAME_AST_MIXIN(DECLARE_NAME_FUNCS)
SPEC_AST_MIXIN(DECLARE_SPEC_FUNCS)
ATTR_AST_MIXIN(DECLARE_ATTR_FUNCS)
DECL_AST_MIXIN(DECLARE_DECL_FUNCS)
EXPR_AST_MIXIN(DECLARE_EXPR_FUNCS)
STMT_AST_MIXIN(DECLARE_STMT_FUNCS)

#undef DECLARE_FUNCS
#undef DECLARE_NAME_FUNCS
#undef DECLARE_SPEC_FUNCS
#undef DECLARE_ATTR_FUNCS
#undef DECLARE_DECL_FUNCS
#undef DECLARE_EXPR_FUNCS
#undef DECLARE_STMT_FUNCS
};

template <class AstT>
SourceLoc fullLoc(AstT* ast, const AstLocator* locator)
{
    return joinedLoc(locator->loc(ast), locator->lastLoc(ast));
}

template <class AstT>
SourceLoc fullLoc(AstT* ast, const std::unique_ptr<const AstLocator>& locator)
{
    return fullLoc(ast, locator.get());
}

} // namespace uaiso

#endif
