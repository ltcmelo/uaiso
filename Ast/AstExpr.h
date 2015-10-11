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

#ifndef UAISO_ASTEXPR_H__
#define UAISO_ASTEXPR_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"
#include "Semantic/TypeFwd.h"

namespace uaiso {

class UAISO_API ExprAst : public Ast
{
public:
    using Ast::Ast;
};

/*!
 * \brief The CondExprAst class
 *
 * A conditional expression.
 */
class UAISO_API CondExprAst final : public ExprAst
{
public:
    using Self = CondExprAst;

    CondExprAst()
        : ExprAst(Kind::CondExpr)
    {}

    NAMED_AST_PARAM(Cond, cond, ExprAst)
    NAMED_LOC_PARAM(Question, question)
    NAMED_AST_PARAM(Yes, yes, ExprAst)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_PARAM(No, no, ExprAst)

    std::unique_ptr<ExprAst> cond_;
    SourceLoc questionLoc_;
    std::unique_ptr<ExprAst> yes_;
    SourceLoc delimLoc_;
    std::unique_ptr<ExprAst> no_;
};

class UAISO_API PrimaryExprAst : public ExprAst
{
public:
    using Self = PrimaryExprAst;
    using ExprAst::ExprAst;
};

class UAISO_API CharLitExprAst final : public PrimaryExprAst
{
public:
    using Self = CharLitExprAst;

    CharLitExprAst()
        : PrimaryExprAst(Kind::CharLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API StrLitExprAst final : public PrimaryExprAst
{
public:
    using Self = StrLitExprAst;

    StrLitExprAst()
        : PrimaryExprAst(Kind::StrLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API NumLitExprAst final : public PrimaryExprAst
{
public:
    using Self = NumLitExprAst;

    NumLitExprAst()
        : PrimaryExprAst(Kind::NumLitExpr)
    {
        INIT_VARIETY(NumLitVariety::Unknown);
    }

    APPLY_VARIETY(NumLitVariety)

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API BoolLitExprAst final : public PrimaryExprAst
{
public:
    using Self = BoolLitExprAst;

    BoolLitExprAst()
        : PrimaryExprAst(Kind::BoolLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API NullLitExprAst final : public PrimaryExprAst
{
public:
    using Self = NullLitExprAst;

    NullLitExprAst()
        : PrimaryExprAst(Kind::NullLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API FuncLitExprAst final : public PrimaryExprAst
{
public:
    using Self = FuncLitExprAst;

    FuncLitExprAst()
        : PrimaryExprAst(Kind::FuncLitExpr)
    {}

    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<StmtAst> stmt_;
};

// TODO: Work this out.
/*!
 * \brief The RecordLitExprAst class
 */
class UAISO_API RecordLitExprAst final : public PrimaryExprAst
{
public:
    using Self = RecordLitExprAst;

    RecordLitExprAst()
        : PrimaryExprAst(Kind::RecordLitExpr)
    {}

    RecordLitExprAst* setSpec(SpecAst* spec);
    RecordLitExprAst* setExpr(ExprAst* expr);

    NAMED_AST_PARAM(Init, init, ExprAst)

    std::unique_ptr<Ast> exprOrSpec_;
    std::unique_ptr<ExprAst> init_;
};

class UAISO_API ThisExprAst final : public PrimaryExprAst
{
public:
    using Self = ThisExprAst;

    ThisExprAst()
        : PrimaryExprAst(Kind::ThisExpr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API SuperExprAst final : public PrimaryExprAst
{
public:
    using Self = SuperExprAst;

    SuperExprAst()
        : PrimaryExprAst(Kind::SuperExpr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API ArrayLengthExprAst final : public PrimaryExprAst
{
public:
    using Self = ArrayLengthExprAst;

    ArrayLengthExprAst()
        : PrimaryExprAst(Kind::ArrayLengthExpr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API IdentExprAst final : public PrimaryExprAst
{
public:
    using Self = IdentExprAst;

    IdentExprAst()
        : PrimaryExprAst(Kind::IdentExpr)
    {}

    IdentExprAst* setName(NameAstList* names);

    NAMED_AST_PARAM(Name, name, NameAst)

    std::unique_ptr<NameAst> name_;
};

/*!
 * \brief The TypeQueryExprAst class
 */
class UAISO_API TypeQueryExprAst : public PrimaryExprAst
{
public:
    using Self = TypeQueryExprAst;

    TypeQueryExprAst()
        : PrimaryExprAst(Kind::TypeQueryExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Name, name, NameAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<NameAst> name_;
    SourceLoc rDelimLoc_;
    // TODO: More forms...

    std::unique_ptr<Type> ty_;
};

class UAISO_API UnaryExprAst : public ExprAst
{
public:
    using Self = UnaryExprAst;
    using ExprAst::ExprAst;

    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_PARAM(Expr, expr, ExprAst)

    SourceLoc oprLoc_;
    std::unique_ptr<ExprAst> expr_;
};

class UAISO_API AddrOfExprAst final : public UnaryExprAst
{
public:
    using Self = AddrOfExprAst;

    AddrOfExprAst()
        : UnaryExprAst(Kind::AddrOfExpr)
    {}
};

class UAISO_API PtrDerefExprAst final : public UnaryExprAst
{
public:
    using Self = PtrDerefExprAst;

    PtrDerefExprAst()
        : UnaryExprAst(Kind::PtrDerefExpr)
    {}
};

class UAISO_API DelExprAst final : public UnaryExprAst
{
public:
    using Self = DelExprAst;

    DelExprAst()
        : UnaryExprAst(Kind::DelExpr)
    {}
};

class UAISO_API PlusExprAst final : public UnaryExprAst
{
public:
    using Self = PlusExprAst;

    PlusExprAst()
        : UnaryExprAst(Kind::PlusExpr)
    {}
};

class UAISO_API MinusExprAst final : public UnaryExprAst
{
public:
    using Self = MinusExprAst;

    MinusExprAst()
        : UnaryExprAst(Kind::MinusExpr)
    {}
};

class UAISO_API LogicNotExprAst final : public UnaryExprAst
{
public:
    using Self = LogicNotExprAst;

    LogicNotExprAst()
        : UnaryExprAst(Kind::LogicNotExpr)
    {}
};

class UAISO_API IncDecExprAst final : public UnaryExprAst
{
public:
    using Self = IncDecExprAst;

    IncDecExprAst()
        : UnaryExprAst(Kind::IncDecExpr)
    {}

    IncDecExprAst* setValue(ExprAst* expr)
    {
        expr_.reset(expr);
        return this;
    }

    /* TODO: Refactor to use Variety */
    IncDecExprAst* setPrefixLoc(const SourceLoc& loc)
    {
        oprLoc_ = loc;
        return this;
    }

    /* TODO: Refactor to use Variety */
    IncDecExprAst* setSuffixLoc(const SourceLoc& loc)
    {
        oprLoc_ = loc;
        return this;
    }
};

class UAISO_API BitCompExprAst final : public UnaryExprAst
{
public:
    using Self = BitCompExprAst;

    BitCompExprAst()
        : UnaryExprAst(Kind::BitCompExpr)
    {}
};

class UAISO_API ChanExprAst final : public UnaryExprAst
{
public:
    using Self = ChanExprAst;

    ChanExprAst()
        : UnaryExprAst(Kind::ChanExpr)
    {}
};

class UAISO_API BinaryExprAst : public ExprAst
{
public:
    using Self = BinaryExprAst;
    using ExprAst::ExprAst;

    NAMED_AST_PARAM(Expr1, expr1, ExprAst)
    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_PARAM(Expr2, expr2, ExprAst)

    std::unique_ptr<ExprAst> expr1_;
    SourceLoc oprLoc_;
    std::unique_ptr<ExprAst> expr2_;
};

class UAISO_API CommaExprAst final : public BinaryExprAst
{
public:
    using Self = CommaExprAst;

    CommaExprAst()
        : BinaryExprAst(Kind::CommaExpr)
    {}
};

class UAISO_API EqExprAst final : public BinaryExprAst
{
public:
    using Self = EqExprAst;

    EqExprAst()
        : BinaryExprAst(Kind::EqExpr)
    {}
};

class UAISO_API RelExprAst final : public BinaryExprAst
{
public:
    using Self = RelExprAst;

    RelExprAst()
        : BinaryExprAst(Kind::RelExpr)
    {}
};

class UAISO_API ShiftExprAst final : public BinaryExprAst
{
public:
    using Self = ShiftExprAst;

    ShiftExprAst()
        : BinaryExprAst(Kind::ShiftExpr)
    {}
};

class UAISO_API ConcatExprAst final : public BinaryExprAst
{
public:
    using Self = ConcatExprAst;

    ConcatExprAst()
        : BinaryExprAst(Kind::ConcatExpr)
    {}
};

class UAISO_API IsExprAst final : public BinaryExprAst
{
public:
    using Self = IsExprAst;

    IsExprAst()
        : BinaryExprAst(Kind::IsExpr)
    {}
};

class UAISO_API InExprAst final : public BinaryExprAst
{
public:
    using Self = InExprAst;

    InExprAst()
        : BinaryExprAst(Kind::InExpr)
    {}
};

class UAISO_API BitXorExprAst final : public BinaryExprAst
{
public:
    using Self = BitXorExprAst;

    BitXorExprAst()
        : BinaryExprAst(Kind::BitXorExpr)
    {}
};

class UAISO_API BitOrExprAst final : public BinaryExprAst
{
public:
    using Self = BitOrExprAst;

    BitOrExprAst()
        : BinaryExprAst(Kind::BitOrExpr)
    {}
};

class UAISO_API BitAndExprAst final : public BinaryExprAst
{
public:
    using Self = BitAndExprAst;

    BitAndExprAst()
        : BinaryExprAst(Kind::BitAndExpr)
    {}
};

class UAISO_API LogicOrExprAst final : public BinaryExprAst
{
public:
    using Self = LogicOrExprAst;

    LogicOrExprAst()
        : BinaryExprAst(Kind::LogicOrExpr)
    {}
};

class UAISO_API LogicAndExprAst final : public BinaryExprAst
{
public:
    using Self = LogicAndExprAst;

    LogicAndExprAst()
        : BinaryExprAst(Kind::LogicAndExpr)
    {}
};

class UAISO_API AddExprAst final : public BinaryExprAst
{
public:
    using Self = AddExprAst;

    AddExprAst()
        : BinaryExprAst(Kind::AddExpr)
    {}
};

class UAISO_API SubExprAst final : public BinaryExprAst
{
public:
    using Self = SubExprAst;

    SubExprAst()
        : BinaryExprAst(Kind::SubExpr)
    {}
};

class UAISO_API MulExprAst final : public BinaryExprAst
{
public:
    using Self = MulExprAst;

    MulExprAst()
        : BinaryExprAst(Kind::MulExpr)
    {}
};

class UAISO_API DivExprAst final : public BinaryExprAst
{
public:
    using Self = DivExprAst;

    DivExprAst()
        : BinaryExprAst(Kind::DivExpr)
    {}
};

class UAISO_API ModExprAst final : public BinaryExprAst
{
public:
    using Self = ModExprAst;

    ModExprAst()
        : BinaryExprAst(Kind::ModExpr)
    {}
};

class UAISO_API PowerExprAst final : public BinaryExprAst
{
public:
    using Self = PowerExprAst;

    PowerExprAst()
        : BinaryExprAst(Kind::PowerExpr)
    {}
};

class UAISO_API AssignExprAst final : public ExprAst
{
public:
    using Self = AssignExprAst;

    AssignExprAst()
        : ExprAst(Kind::AssignExpr)
    {
        INIT_VARIETY(AssignVariety::Unknow);
    }

    APPLY_VARIETY(AssignVariety)

    NAMED_AST_LIST_PARAM(Expr1, exprs1, ExprAst)
    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_LIST_PARAM(Expr2, exprs2, ExprAst)

    std::unique_ptr<ExprAstList> exprs1_;
    SourceLoc oprLoc_;
    std::unique_ptr<ExprAstList> exprs2_;
};

class UAISO_API MemberAccessExprAst final : public ExprAst
{
public:
    using Self = MemberAccessExprAst;

    MemberAccessExprAst()
        : ExprAst(Kind::MemberAccessExpr)
    {}

    MemberAccessExprAst* setSpec(SpecAst* spec);
    MemberAccessExprAst* setExpr(ExprAst* expr);

    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_PARAM(Name, name, NameAst)

    std::unique_ptr<Ast> exprOrSpec_;
    SourceLoc oprLoc_;
    std::unique_ptr<NameAst> name_;
};

/*!
 * \brief The CastExprAst class
 */
class UAISO_API CastExprAst final : public ExprAst
{
public:
    using Self = CastExprAst;

    CastExprAst()
        : ExprAst(Kind::CastExpr)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Expr, expr, ExprAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<ExprAst> expr_;

    std::unique_ptr<Type> ty_; // Type annotation
};

class UAISO_API WrappedExprAst final : public ExprAst
{
public:
    using Self = WrappedExprAst;

    WrappedExprAst()
        : ExprAst(Kind::WrappedExpr)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
};

class UAISO_API VoidInitExprAst final : public ExprAst
{
public:
    using Self = VoidInitExprAst;

    VoidInitExprAst()
        : ExprAst(Kind::VoidInitExpr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API RecordInitExprAst final : public ExprAst
{
public:
    using Self = RecordInitExprAst;

    RecordInitExprAst()
        : ExprAst(Kind::RecordInitExpr)
    {}

    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Init, inits, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<SpecAst> spec_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAstList> inits_;
    SourceLoc rDelimLoc_;
};

class UAISO_API ArrayInitExprAst final : public ExprAst
{
public:
    using Self = ArrayInitExprAst;

    ArrayInitExprAst()
        : ExprAst(Kind::ArrayInitExpr)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Init, inits, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAstList> inits_;
    SourceLoc rDelimLoc_;
};

class UAISO_API DesignateExprAst final : public ExprAst
{
public:
    using Self = DesignateExprAst;

    DesignateExprAst()
        : ExprAst(Kind::DesignateExpr)
    {}

    NAMED_AST_PARAM(Id, id, ExprAst)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_PARAM(Value, value, ExprAst)

    std::unique_ptr<ExprAst> id_;
    SourceLoc delimLoc_;
    std::unique_ptr<ExprAst> value_;
};

class UAISO_API MakeExprAst final : public ExprAst
{
public:
    using Self = MakeExprAst;

    MakeExprAst()
        : ExprAst(Kind::MakeExpr)
    {}

    NAMED_AST_PARAM(Base, base, ExprAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(Split, split)
    NAMED_AST_LIST_PARAM(Arg, args, ExprAst)
    NAMED_LOC_PARAM(Pack, pack)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<ExprAst> base_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc splitLoc_;
    std::unique_ptr<ExprAstList> args_;
    SourceLoc packLoc_;
    SourceLoc rDelimLoc_;

    std::unique_ptr<Type> ty_; // Type annotation
};

class UAISO_API NewExprAst final : public ExprAst
{
public:
    using Self = NewExprAst;

    NewExprAst()
        : ExprAst(Kind::NewExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LAllocDelim, lAllocDelim)
    NAMED_AST_LIST_PARAM(AllocArg, allocArgs, ExprAst)
    NAMED_LOC_PARAM(RAllocDelim, rAllocDelim)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(LArgDelim, lArgDelim)
    NAMED_AST_LIST_PARAM(Arg, args, ExprAst)
    NAMED_LOC_PARAM(RArgDelim, rArgDelim)

    SourceLoc keyLoc_;
    SourceLoc lAllocDelimLoc_;
    std::unique_ptr<ExprAstList> allocArgs_;
    SourceLoc rAllocDelimLoc_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc lArgDelimLoc_;
    std::unique_ptr<ExprAstList> args_;
    SourceLoc rArgDelimLoc_;

    std::unique_ptr<Type> ty_; // Type annotation
};

class UAISO_API NestedNewExprAst final : public ExprAst
{
public:
    using Self = NestedNewExprAst;

    NestedNewExprAst()
        : ExprAst(Kind::NestedNewExpr)
    {}

    NAMED_AST_PARAM(Base, base, ExprAst)
    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_PARAM(NestedNew, nestedNew, ExprAst)

    std::unique_ptr<ExprAst> base_;
    SourceLoc oprLoc_;
    std::unique_ptr<ExprAst> nestedNew_;
};

class UAISO_API CallExprAst final : public ExprAst
{
public:
    using Self = CallExprAst;

    CallExprAst()
        : ExprAst(Kind::CallExpr)
    {}

    NAMED_AST_PARAM(Base, base, ExprAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Arg, args, ExprAst)
    NAMED_LOC_PARAM(Pack, pack)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<ExprAst> base_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAstList> args_;
    SourceLoc packLoc_;
    SourceLoc rDelimLoc_;
};

class UAISO_API UnpackExprAst final : public ExprAst
{
public:
    using Self = UnpackExprAst;

    UnpackExprAst()
        : ExprAst(Kind::UnpackExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Expr, expr, ExprAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAst> expr_;
};

class UAISO_API SubrangeExprAst final : public ExprAst
{
public:
    using Self = SubrangeExprAst;

    SubrangeExprAst()
        : ExprAst(Kind::SubrangeExpr)
    {}

    NAMED_AST_PARAM(Low, low, ExprAst)
    NAMED_LOC_PARAM(Delim1, delim1)
    NAMED_AST_PARAM(Hi, hi, ExprAst)
    NAMED_LOC_PARAM(Delim2, delim2)
    NAMED_AST_PARAM(Max, max, ExprAst)

    std::unique_ptr<ExprAst> low_;
    SourceLoc delim1Loc_;
    std::unique_ptr<ExprAst> hi_;
    SourceLoc delim2Loc_;
    std::unique_ptr<ExprAst> max_;
};

class UAISO_API ArrayIndexExprAst final : public ExprAst
{
public:
    using Self = ArrayIndexExprAst;

    ArrayIndexExprAst()
        : ExprAst(Kind::ArrayIndexExpr)
    {}

    NAMED_AST_PARAM(Base, base, ExprAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Index, index, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<ExprAst> base_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> index_;
    SourceLoc rDelimLoc_;
};

class UAISO_API ArraySliceExprAst : public ExprAst
{
public:
    using Self = ArraySliceExprAst;

    ArraySliceExprAst()
        : ExprAst(Kind::ArraySliceExpr)
    {}

    NAMED_AST_PARAM(Base, base, ExprAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Range, range, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<ExprAst> base_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> range_;
    SourceLoc rDelimLoc_;
};

// TODO: Kepp only spec...
class UAISO_API TypeidExprAst final : public ExprAst
{
public:
    using Self = TypeidExprAst;

    TypeidExprAst()
        : ExprAst(Kind::TypeidExpr)
    {}

    TypeidExprAst* setSpec(SpecAst* spec);
    TypeidExprAst* setExpr(ExprAst* spec);

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<Ast> exprOrSpec_;
    SourceLoc rDelimLoc_;
};

class UAISO_API AssertExprAst final : public ExprAst
{
public:
    using Self = AssertExprAst;

    AssertExprAst()
        : ExprAst(Kind::AssertExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(MDelim, mDelim)
    NAMED_AST_PARAM(Message, message, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc mDelimLoc_;
    std::unique_ptr<ExprAst> message_;
    SourceLoc rDelimLoc_;
};

class UAISO_API TypeAssertExprAst final : public ExprAst
{
public:
    using Self = TypeAssertExprAst;

    TypeAssertExprAst()
        : ExprAst(Kind::TypeAssertExpr)
    {}

    NAMED_AST_PARAM(Base, base, ExprAst)
    NAMED_LOC_PARAM(Opr, opr)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<ExprAst> base_;
    SourceLoc oprLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc rDelimLoc_;

    std::unique_ptr<Type> ty_;
};

class UAISO_API MixinExprAst final : public ExprAst
{
public:
    using Self = MixinExprAst;

    MixinExprAst()
        : ExprAst(Kind::MixinExpr)
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

class UAISO_API ErrorExprAst final : public ExprAst
{
public:
    using Self = ErrorExprAst;

    ErrorExprAst()
        : ExprAst(Kind::ErrorExpr)
    {}

    NAMED_LOC_PARAM(Error, error)

    SourceLoc errorLoc_;
};

} // namespace uaiso

#endif
