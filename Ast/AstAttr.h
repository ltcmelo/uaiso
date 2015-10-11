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

#ifndef UAISO_ASTATTR_H__
#define UAISO_ASTATTR_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"

namespace uaiso {

class UAISO_API AttrAst : public Ast
{
public:
    using Ast::Ast;
};

class UAISO_API StorageClassAttrAst final : public AttrAst
{
public:
    using Self = StorageClassAttrAst;

    StorageClassAttrAst()
        : AttrAst(Kind::StorageClassAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API LinkageAttrAst final : public AttrAst
{
public:
    using Self = LinkageAttrAst;

    LinkageAttrAst()
        : AttrAst(Kind::LinkageAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};


/*!
 * \brief The VisibilityAttrAst class
 *
 * An access specifier.
 */
class UAISO_API VisibilityAttrAst final : public AttrAst
{
public:
    using Self = VisibilityAttrAst;

    VisibilityAttrAst()
        : AttrAst(Kind::VisibilityAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

/*!
 * \brief The DeclAttrAst class
 */
class UAISO_API DeclAttrAst : public AttrAst
{
public:
    using Self = DeclAttrAst;

    DeclAttrAst()
        : AttrAst(Kind::DeclAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API AutoAttrAst : public AttrAst
{
public:
    using Self = AutoAttrAst;

    AutoAttrAst()
        : AttrAst(Kind::AutoAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

/*!
 * \brief The TypeQualAttrAst class
 */
class UAISO_API TypeQualAttrAst final : public AttrAst
{
public:
    using Self = TypeQualAttrAst;

    TypeQualAttrAst()
        : AttrAst(Kind::TypeQualAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API ParamDirAttrAst final : public AttrAst
{
public:
    using Self = ParamDirAttrAst;

    ParamDirAttrAst()
        : AttrAst(Kind::ParamDirAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API EvalStrategyAttrAst final : public AttrAst
{
public:
    using Self = EvalStrategyAttrAst;

    EvalStrategyAttrAst()
        : AttrAst(Kind::EvalStrategyAttr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

/*!
 * \brief The AnnotAttrAst class
 *
 * An annotation.
 */
class UAISO_API AnnotAttrAst final : public AttrAst
{
public:
    using Self = AnnotAttrAst;

    AnnotAttrAst()
        : AttrAst(Kind::AnnotAttr)
    {}

    NAMED_LOC_PARAM(At, at)
    NAMED_LOC_PARAM(Text, text)

    SourceLoc atLoc_;
    SourceLoc textLoc_;
};

/*!
 * \brief The CodegenAttrAst class
 */
class UAISO_API CodegenAttrAst final : public AttrAst
{
public:
    using Self = CodegenAttrAst;

    CodegenAttrAst()
        : AttrAst(Kind::CodegenAttr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
};

} // namespace uaiso

#endif
