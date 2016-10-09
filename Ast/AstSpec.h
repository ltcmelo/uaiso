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

#ifndef UAISO_ASTSPEC_H__
#define UAISO_ASTSPEC_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"

namespace uaiso {

/*!
 * \brief The SpecAst class
 */
class UAISO_API SpecAst : public Ast
{
public:
    using Ast::Ast;
};

/*!
 * \brief The VoidSpecAst class
 *
 * The void specifier designates a type not inhabited by any value. This is
 * different from the unit specifier.
 */
class UAISO_API VoidSpecAst final : public SpecAst
{
public:
    AST_CLASS(Void, Spec)
    CREATE_WITH_LOC(Key)

    VoidSpecAst()
        : SpecAst(Kind::VoidSpec)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

/*!
 * \brief The UnitSpecAst class
 *
 * The unit specifier designates a type inhabited a single value. This is
 * different from the unit specifier.
 */
class UAISO_API UnitSpecAst final : public SpecAst
{
public:
    AST_CLASS(Unit, Spec)
    CREATE_WITH_LOC(Key)

    UnitSpecAst()
        : SpecAst(Kind::UnitSpec)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

/*!
 * \brief The BuiltinSpecAst class
 */
class UAISO_API BuiltinSpecAst final : public SpecAst
{
public:
    AST_CLASS(Builtin, Spec)

    BuiltinSpecAst()
        : SpecAst(Kind::BuiltinSpec)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

/*!
 * \brief The NamedSpecAst class
 */
class UAISO_API NamedSpecAst final : public SpecAst
{
public:
    AST_CLASS(Named, Spec)
    CREATE_WITH_AST(Name, Name)

    NamedSpecAst()
        : SpecAst(Kind::NamedSpec)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)

    std::unique_ptr<NameAst> name_;
};

/*!
 * \brief The AlphaSpecAst class
 */
class UAISO_API AlphaSpecAst final : public SpecAst
{
public:
    AST_CLASS(Alpha, Spec)
    CREATE_WITH_AST(Name, Name)

    AlphaSpecAst()
        : SpecAst(Kind::AlphaSpec)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)

    std::unique_ptr<NameAst> name_;
};

/*!
 * \brief The TypeofSpecAst class
 */
class UAISO_API TypeofSpecAst final : public SpecAst
{
public:
    AST_CLASS(Typeof, Spec)

    TypeofSpecAst()
        : SpecAst(Kind::TypeofSpec)
    {}

    NAMED_LOC_PARAM(Opr, opr)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc oprLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The RecordSpecAst class
 */
class UAISO_API RecordSpecAst final : public SpecAst
{
public:
    AST_CLASS(Record, Spec)
    VARIETY_AST(RecordVariety)

    RecordSpecAst()
        : SpecAst(Kind::RecordSpec)
    {
        INIT_VARIETY(RecordVariety::Struct);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_LIST_PARAM(Base, bases, DeclAst)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_AST_PARAM(Proto, proto, StmtAst)

    /*!
     * \brief isTemplate
     * \return
     *
     * Return whether the record accepts type parameters.
     */
    bool isTemplate() const { return templ_.get(); }

    /*!
     * \brief templateParam
     * \return
     */
    DeclAst* templateParam() const { return nullptr; }

    bool isEmpty() const { return !decls_; }

    SourceLoc keyLoc_;
    SourceLoc delimLoc_;
    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<DeclAstList> bases_;
    std::unique_ptr<DeclAst> templ_;
    std::unique_ptr<DeclAstList> decls_;
    std::unique_ptr<StmtAst> proto_;
};

/*!
 * \brief The FuncSpecAst class
 */
class UAISO_API FuncSpecAst : public SpecAst
{
public:
    AST_CLASS(Func, Spec)
    VARIETY_AST(NotationVariety)

    FuncSpecAst()
        : SpecAst(Kind::FuncSpec)
    {
        INIT_VARIETY(NotationVariety::Standard);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_LIST_PARAM(Input, inputs, SpecAst)
    NAMED_LOC_PARAM(Arrow, arrow)
    NAMED_AST_PARAM(Output, output, SpecAst)

    SourceLoc keyLoc_;

    //! The type is uncurried form. Parameters are stored in the input list.
    std::unique_ptr<SpecAstList> inputs_;

    //! The last arrow.
    SourceLoc arrowLoc_;

    //! Multiple returns can be represeted with a tuple.
    std::unique_ptr<SpecAst> output_;
};

/*!
 * \brief The OpaqueSpecAst class
 */
class UAISO_API OpaqueSpecAst : public SpecAst
{
public:
    AST_CLASS(Opaque, Spec)

    using SpecAst::SpecAst;

    NAMED_AST_PARAM(BaseSpec, baseSpec, SpecAst)

    std::unique_ptr<SpecAst> baseSpec_;
};

/*!
 * \brief The PtrSpecAst class
 */
class UAISO_API PtrSpecAst final : public OpaqueSpecAst
{
public:
    AST_CLASS(Ptr, Spec)

    PtrSpecAst()
        : OpaqueSpecAst(Kind::PtrSpec)
    {}

    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_PARAM(BaseSpec, baseSpec, SpecAst)

    SourceLoc oprLoc_;
};

/*!
 * \brief The ArraySpecAst class
 */
class UAISO_API ArraySpecAst final : public OpaqueSpecAst
{
public:
    AST_CLASS(Array, Spec)
    VARIETY_AST(ArrayVariety)

    ArraySpecAst()
        : OpaqueSpecAst(Kind::ArraySpec)
    {
        INIT_VARIETY(ArrayVariety::Unknown);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(BaseSpec, baseSpec, SpecAst)

    Self* setSpec(SpecAst* spec);
    Self* setExpr(ExprAst* spec);
    Ast* exprOrSpec() { return exprOrSpec_.get(); }

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<Ast> exprOrSpec_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The ListSpecAst class
 */
class UAISO_API ListSpecAst final : public OpaqueSpecAst
{
public:
    AST_CLASS(List, Spec)
    VARIETY_AST(NotationVariety)

    ListSpecAst()
        : OpaqueSpecAst(Kind::ListSpec)
    {
        INIT_VARIETY(NotationVariety::Standard);
    }

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(BaseSpec, baseSpec, SpecAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc lDelimLoc_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The TupleSpecAst class
 */
class UAISO_API TupleSpecAst final : public SpecAst
{
public:
    AST_CLASS(Tuple, Spec)

    TupleSpecAst()
        : SpecAst(Kind::TupleSpec)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Spec, specs, SpecAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc lDelimLoc_;
    std::unique_ptr<SpecAstList> specs_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The ChanSpecAst class
 */
class UAISO_API ChanSpecAst final : public OpaqueSpecAst
{
public:
    AST_CLASS(Chan, Spec)
    VARIETY_AST(ChanVariety)

    ChanSpecAst()
        : OpaqueSpecAst(Kind::ChanSpec)
    {
        INIT_VARIETY(ChanVariety::Unknown);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Dir, dir)
    NAMED_AST_PARAM(BaseSpec, baseSpec, SpecAst)

    SourceLoc keyLoc_;
    SourceLoc dirLoc_;
};

/*!
 * \brief The InferredSpecAst class
 */
class UAISO_API InferredSpecAst final : public SpecAst
{
public:
    AST_CLASS(Inferred, Spec)

    InferredSpecAst()
        : SpecAst(Kind::InferredSpec)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

/*!
 * \brief The TypeAppSpecAst class
 *
 * A type application.
 */
class UAISO_API TypeAppSpecAst final : public SpecAst
{
public:
    AST_CLASS(TypeApp, Spec)

    TypeAppSpecAst()
        : SpecAst(Kind::TypeAppSpec)
    {}

    NAMED_AST_LIST_PARAM(Spec, specs, SpecAst)

    std::unique_ptr<SpecAstList> specs_;
};

/*!
 * \brief The DecoratedSpecAst class
 */
class UAISO_API DecoratedSpecAst final : public SpecAst
{
public:
    AST_CLASS(Decorated, Spec)

    DecoratedSpecAst()
        : SpecAst(Kind::DecoratedSpec)
    {}

    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_LIST_PARAM(Attr, attrs, AttrAst)

    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<AttrAstList> attrs_;
};

class UAISO_API ErrorSpecAst final : public SpecAst
{
public:
    AST_CLASS(Error, Spec)
    CREATE_WITH_LOC(Error)

    ErrorSpecAst()
        : SpecAst(Kind::ErrorSpec)
    {}

    NAMED_LOC_PARAM(Error, error)

    SourceLoc errorLoc_;
};

} // namespace uaiso

#endif
