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
 */
class UAISO_API VoidSpecAst final : public SpecAst
{
public:
    using Self = VoidSpecAst;

    VoidSpecAst()
        : SpecAst(Kind::VoidSpec)
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
    using Self = BuiltinSpecAst;

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
    using Self = NamedSpecAst;

    NamedSpecAst()
        : SpecAst(Kind::NamedSpec)
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
    using Self = TypeofSpecAst;

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
    using Self = RecordSpecAst;

    RecordSpecAst()
        : SpecAst(Kind::RecordSpec)
    {
        INIT_VARIETY(RecordVariety::Struct);
    }

    APPLY_VARIETY(RecordVariety)

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_LIST_PARAM(Base, bases, DeclAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

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
    std::unique_ptr<DeclAstList> bases_;
    std::unique_ptr<DeclAst> templ_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> decls_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The FuncSpecAst class
 *
 * To allow multi-valued named results, the return of a function is
 * typically a ParamClausedDeclAst. In a language that supports a
 * "simple" return type, the parameter clause would be inhabited by
 * a single unnamed ParamDeclAst.
 */
class UAISO_API FuncSpecAst : public SpecAst
{
public:
    using Self = FuncSpecAst;

    FuncSpecAst()
        : SpecAst(Kind::FuncSpec)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Param, param, DeclAst)
    NAMED_AST_PARAM(Result, result, DeclAst)
    NAMED_AST_PARAM__BASE__(TemplateParam, DeclAst)

    /*!
     * \brief setResult
     * \param spec
     * \return
     *
     * Conveniece for creating a ParamClauseDeclAst out of a single
     * type specifier.
     */
    FuncSpecAst* setResult(SpecAst* spec);

    /*!
     * \brief isTemplate
     * \return
     *
     * Return wether this function accepts type parameters.
     */
    virtual bool isTemplate() const { return false; }

    /*!
     * \brief templateParam
     * \return
     */
    virtual DeclAst* templateParam() const { return nullptr; }

    SourceLoc keyLoc_;
    std::unique_ptr<DeclAst> param_;
    std::unique_ptr<DeclAst> result_;
};

/*
 * Helper type that "injects" a template param.
 */
struct FuncTemplateParam__
{
    static bool checkTemplateParam__() { return true; }

    void setTemplateParam__(DeclAst* decl) { templateParam_.reset(decl); }
    DeclAst* templateParam__() const { return templateParam_.get(); }

    std::unique_ptr<DeclAst> templateParam_;
};

struct FuncTemplateParam__Empty__
{
    static bool checkTemplateParam__() { return false; }
    void setTemplateParam__(const DeclAst*) {}
    DeclAst* templateParam__() const { return nullptr; }
};

/*!
 * A parameterized subclass of FuncSpecAst that avoids bloating the AST
 * memory size with members that are commonly absent. It works in combination
 *  with the "member" injection helpers.
 */
template <class TemplateParamT = FuncTemplateParam__Empty__>
class FuncSpecAst__
        : public FuncSpecAst
        , private TemplateParamT
{
public:
    using Self = FuncSpecAst__<TemplateParamT>;
    using FuncSpecAst::FuncSpecAst;

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Param, param, DeclAst)
    NAMED_AST_PARAM(Result, result, DeclAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM__(TemplateParam, TemplateParamT, DeclAst)

    bool isTemplate() const override { return TemplateParamT::checkTemplateParam__(); }
    DeclAst* templateParam() const override { return TemplateParamT::templateParam__(); }

    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The OpaqueSpecAst class
 */
class UAISO_API OpaqueSpecAst : public SpecAst
{
public:
    using Self = OpaqueSpecAst;
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
    using Self = PtrSpecAst;

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
    using Self = ArraySpecAst;

    ArraySpecAst()
        : OpaqueSpecAst(Kind::ArraySpec)
    {
        INIT_VARIETY(ArrayVariety::Unknown);
    }

    APPLY_VARIETY(ArrayVariety)

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(BaseSpec, baseSpec, SpecAst)

    Self* setSpec(SpecAst* spec);
    Self* setExpr(ExprAst* spec);

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<Ast> exprOrSpec_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The ChanSpecAst class
 */
class UAISO_API ChanSpecAst final : public OpaqueSpecAst
{
public:
    using Self = ChanSpecAst;

    ChanSpecAst()
        : OpaqueSpecAst(Kind::ChanSpec)
    {
        INIT_VARIETY(ChanVariety::Unknown);
    }

    APPLY_VARIETY(ChanVariety)

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
    using Self = InferredSpecAst;

    InferredSpecAst()
        : SpecAst(Kind::InferredSpec)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_; // Dummy
};

/*!
 * \brief The DecoratedSpecAst class
 */
class UAISO_API DecoratedSpecAst final : public SpecAst
{
public:
    using Self = DecoratedSpecAst;

    DecoratedSpecAst()
        : SpecAst(Kind::DecoratedSpec)
    {}

    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_LIST_PARAM(Attr, attrs, AttrAst)

    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<AttrAstList> attrs_;
};

} // namespace uaiso

#endif
