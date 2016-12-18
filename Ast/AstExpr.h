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

#ifndef UAISO_ASTEXPR_H__
#define UAISO_ASTEXPR_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"
#include "Semantic/SymbolFwd.h"
#include "Semantic/TypeFwd.h"
#include <vector>

namespace uaiso {

class UAISO_API ExprAst : public Ast
{
public:
    using Ast::Ast;
};

class UAISO_API PriExprAst : public ExprAst
{
public:
    AST_CLASS(Pri, Expr)
    using ExprAst::ExprAst;
};

class UAISO_API CharLitExprAst final : public PriExprAst
{
public:
    AST_CLASS(CharLit, Expr)
    CREATE_WITH_LOC(Lit)

    CharLitExprAst()
        : PriExprAst(Kind::CharLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API StrLitExprAst final : public PriExprAst
{
public:
    AST_CLASS(StrLit, Expr)
    CREATE_WITH_LOC(Lit)

    StrLitExprAst()
        : PriExprAst(Kind::StrLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API NumLitExprAst final : public PriExprAst
{
public:
    AST_CLASS(NumLit, Expr)
    CREATE_WITH_LOC(Lit)
    VARIETY_AST(NumLitVariety)

    static std::unique_ptr<Self> create(const SourceLoc& loc, NumLitVariety v)
    {
        auto ast = create(loc);
        ast->setVariety(v);
        return ast;
    }

    NumLitExprAst()
        : PriExprAst(Kind::NumLitExpr)
    {
        INIT_VARIETY(NumLitVariety::Unknown);
    }

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API BoolLitExprAst final : public PriExprAst
{
public:
    AST_CLASS(BoolLit, Expr)
    CREATE_WITH_LOC(Lit)

    BoolLitExprAst()
        : PriExprAst(Kind::BoolLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API NullLitExprAst final : public PriExprAst
{
public:
    AST_CLASS(NullLit, Expr)
    CREATE_WITH_LOC(Lit)

    NullLitExprAst()
        : PriExprAst(Kind::NullLitExpr)
    {}

    NAMED_LOC_PARAM(Lit, lit)

    SourceLoc litLoc_;
};

class UAISO_API LambdaExprAst final : public PriExprAst
{
public:
    AST_CLASS(Lambda, Expr)

    LambdaExprAst()
        : PriExprAst(Kind::LambdaExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(ParamClause, paramClause, ParamClauseDeclAst)
    NAMED_AST_PARAM(Result, result, SpecAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ParamClauseDeclAst> paramClause_;
    std::unique_ptr<SpecAst> result_;
    std::unique_ptr<StmtAst> stmt_;
};

// TODO: Work this out.
/*!
 * \brief The RecordLitExprAst class
 */
class UAISO_API RecordLitExprAst final : public PriExprAst
{
public:
    AST_CLASS(RecordLit, Expr)

    RecordLitExprAst()
        : PriExprAst(Kind::RecordLitExpr)
    {}

    RecordLitExprAst* setSpec(SpecAst* spec);
    RecordLitExprAst* setExpr(ExprAst* expr);

    NAMED_AST_PARAM(Init, init, ExprAst)

    std::unique_ptr<Ast> exprOrSpec_;
    std::unique_ptr<ExprAst> init_;
};

class UAISO_API ThisExprAst final : public PriExprAst
{
public:
    AST_CLASS(This, Expr)
    CREATE_WITH_LOC(Key)

    ThisExprAst()
        : PriExprAst(Kind::ThisExpr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API SuperExprAst final : public PriExprAst
{
public:
    AST_CLASS(Super, Expr)
    CREATE_WITH_LOC(Key)

    SuperExprAst()
        : PriExprAst(Kind::SuperExpr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API ArrayLengthExprAst final : public PriExprAst
{
public:
    AST_CLASS(ArrayLength, Expr)

    ArrayLengthExprAst()
        : PriExprAst(Kind::ArrayLengthExpr)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API IdentExprAst final : public PriExprAst
{
public:
    AST_CLASS(Ident, Expr)
    CREATE_WITH_AST(Name, Name)

    IdentExprAst()
        : PriExprAst(Kind::IdentExpr)
    {}

    IdentExprAst* setName(NameAstList* names);

    NAMED_AST_PARAM(Name, name, NameAst)

    std::unique_ptr<NameAst> name_;
};

/*!
 * \brief The TypeQueryExprAst class
 */
class UAISO_API TypeQueryExprAst : public PriExprAst
{
public:
    AST_CLASS(TypeQuery, Expr)

    TypeQueryExprAst()
        : PriExprAst(Kind::TypeQueryExpr)
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
    AST_CLASS(Unary, Expr)
    using ExprAst::ExprAst;

    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_PARAM(Expr, expr, ExprAst)

    SourceLoc oprLoc_;
    std::unique_ptr<ExprAst> expr_;
};

class UAISO_API AddrOfExprAst final : public UnaryExprAst
{
public:
    AST_CLASS(AddrOf, Expr)

    AddrOfExprAst()
        : UnaryExprAst(Kind::AddrOfExpr)
    {}
};

class UAISO_API PtrDerefExprAst final : public UnaryExprAst
{
public:
    AST_CLASS(PtrDeref, Expr)

    PtrDerefExprAst()
        : UnaryExprAst(Kind::PtrDerefExpr)
    {}
};

class UAISO_API PlusExprAst final : public UnaryExprAst
{
public:
    AST_CLASS(Plus, Expr)

    PlusExprAst()
        : UnaryExprAst(Kind::PlusExpr)
    {}
};

class UAISO_API MinusExprAst final : public UnaryExprAst
{
public:
    AST_CLASS(Minus, Expr)

    MinusExprAst()
        : UnaryExprAst(Kind::MinusExpr)
    {}
};

class UAISO_API LogicNotExprAst final : public UnaryExprAst
{
public:
    AST_CLASS(LogicNot, Expr)

    LogicNotExprAst()
        : UnaryExprAst(Kind::LogicNotExpr)
    {}
};

class UAISO_API IncDecExprAst final : public UnaryExprAst
{
public:
    AST_CLASS(IncDec, Expr)

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
    AST_CLASS(BitComp, Expr)

    BitCompExprAst()
        : UnaryExprAst(Kind::BitCompExpr)
    {}
};

class UAISO_API ChanExprAst final : public UnaryExprAst
{
public:
    AST_CLASS(Chan, Expr)

    ChanExprAst()
        : UnaryExprAst(Kind::ChanExpr)
    {}
};

class UAISO_API BinExprAst : public ExprAst
{
public:
    AST_CLASS(Bin, Expr)
    using ExprAst::ExprAst;

    NAMED_AST_PARAM(Expr1, expr1, ExprAst)
    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_PARAM(Expr2, expr2, ExprAst)

    std::unique_ptr<ExprAst> expr1_;
    SourceLoc oprLoc_;
    std::unique_ptr<ExprAst> expr2_;
};

class UAISO_API CommaExprAst final : public BinExprAst
{
public:
    AST_CLASS(Comma, Expr)

    CommaExprAst()
        : BinExprAst(Kind::CommaExpr)
    {}
};

class UAISO_API EqExprAst final : public BinExprAst
{
public:
    AST_CLASS(Eq, Expr)

    EqExprAst()
        : BinExprAst(Kind::EqExpr)
    {}
};

class UAISO_API RelExprAst final : public BinExprAst
{
public:
    AST_CLASS(Rel, Expr)

    RelExprAst()
        : BinExprAst(Kind::RelExpr)
    {}
};

class UAISO_API ShiftExprAst final : public BinExprAst
{
public:
    AST_CLASS(Shift, Expr)

    ShiftExprAst()
        : BinExprAst(Kind::ShiftExpr)
    {}
};

class UAISO_API ConcatExprAst final : public BinExprAst
{
public:
    AST_CLASS(Concat, Expr)

    ConcatExprAst()
        : BinExprAst(Kind::ConcatExpr)
    {}
};

class UAISO_API IsExprAst final : public BinExprAst
{
public:
    AST_CLASS(Is, Expr)

    IsExprAst()
        : BinExprAst(Kind::IsExpr)
    {}
};

class UAISO_API InExprAst final : public BinExprAst
{
public:
    AST_CLASS(In, Expr)

    InExprAst()
        : BinExprAst(Kind::InExpr)
    {}
};

class UAISO_API BitXorExprAst final : public BinExprAst
{
public:
    AST_CLASS(BitXor, Expr)

    BitXorExprAst()
        : BinExprAst(Kind::BitXorExpr)
    {}
};

class UAISO_API BitOrExprAst final : public BinExprAst
{
public:
    AST_CLASS(BitOr, Expr)

    BitOrExprAst()
        : BinExprAst(Kind::BitOrExpr)
    {}
};

class UAISO_API BitAndExprAst final : public BinExprAst
{
public:
    AST_CLASS(BitAnd, Expr)

    BitAndExprAst()
        : BinExprAst(Kind::BitAndExpr)
    {}
};

class UAISO_API LogicOrExprAst final : public BinExprAst
{
public:
    AST_CLASS(LogicOr, Expr)

    LogicOrExprAst()
        : BinExprAst(Kind::LogicOrExpr)
    {}
};

class UAISO_API LogicAndExprAst final : public BinExprAst
{
public:
    AST_CLASS(LogicAnd, Expr)

    LogicAndExprAst()
        : BinExprAst(Kind::LogicAndExpr)
    {}
};

class UAISO_API AddExprAst final : public BinExprAst
{
public:
    AST_CLASS(Add, Expr)

    AddExprAst()
        : BinExprAst(Kind::AddExpr)
    {}
};

class UAISO_API SubExprAst final : public BinExprAst
{
public:
    AST_CLASS(Sub, Expr)

    SubExprAst()
        : BinExprAst(Kind::SubExpr)
    {}
};

class UAISO_API MulExprAst final : public BinExprAst
{
public:
    AST_CLASS(Mul, Expr)

    MulExprAst()
        : BinExprAst(Kind::MulExpr)
    {}
};

class UAISO_API DivExprAst final : public BinExprAst
{
public:
    AST_CLASS(Div, Expr)

    DivExprAst()
        : BinExprAst(Kind::DivExpr)
    {}
};

class UAISO_API ModExprAst final : public BinExprAst
{
public:
    AST_CLASS(Mod, Expr)

    ModExprAst()
        : BinExprAst(Kind::ModExpr)
    {}
};

class UAISO_API PowerExprAst final : public BinExprAst
{
public:
    AST_CLASS(Power, Expr)

    PowerExprAst()
        : BinExprAst(Kind::PowerExpr)
    {}
};

class UAISO_API DelExprAst final : public ExprAst
{
public:
    AST_CLASS(Del, Expr)

    DelExprAst()
        : ExprAst(Kind::DelExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_LIST_PARAM(Expr, exprs, ExprAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAstList> exprs_;
};

class UAISO_API AssignExprAst final : public ExprAst
{
public:
    AST_CLASS(Assign, Expr)
    VARIETY_AST(AssignVariety)

    AssignExprAst()
        : ExprAst(Kind::AssignExpr)
    {
        INIT_VARIETY(AssignVariety::Unknow);
    }

    NAMED_AST_LIST_PARAM(Expr1, exprs1, ExprAst)
    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_LIST_PARAM(Expr2, exprs2, ExprAst)

    std::unique_ptr<ExprAstList> exprs1_;
    SourceLoc oprLoc_;
    std::unique_ptr<ExprAstList> exprs2_;

    // An assignment may also bind names. For instance in dynamic languages
    // such as Python.
    std::vector<Var*> syms_;
};

class UAISO_API MemberAccessExprAst final : public ExprAst
{
public:
    AST_CLASS(MemberAccess, Expr)

    MemberAccessExprAst()
        : ExprAst(Kind::MemberAccessExpr)
    {}

    MemberAccessExprAst* setSpec(SpecAst* spec);
    MemberAccessExprAst* setExpr(ExprAst* expr);
    MemberAccessExprAst* setExpr(std::unique_ptr<ExprAst> expr);
    Ast* exprOrSpec() { return exprOrSpec_.get(); }

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
    AST_CLASS(Cast, Expr)

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
    AST_CLASS(Wrapped, Expr)

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

class UAISO_API RecordInitExprAst final : public ExprAst
{
public:
    AST_CLASS(RecordInit, Expr)

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
    AST_CLASS(ArrayInit, Expr)

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

class UAISO_API TupleLitExprAst final : public ExprAst
{
public:
    AST_CLASS(TupleLit, Expr)

    TupleLitExprAst()
        : ExprAst(Kind::TupleLitExpr)
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
    AST_CLASS(Designate, Expr)

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
    AST_CLASS(Make, Expr)

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
    AST_CLASS(New, Expr)

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
    AST_CLASS(NestedNew, Expr)

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
    AST_CLASS(Call, Expr)
    CREATE_WITH_AST(Base, Expr)
    VARIETY_AST(FuncVariety)

    CallExprAst()
        : ExprAst(Kind::CallExpr)
    {
        INIT_VARIETY(FuncVariety::Unknown);
    }

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
    AST_CLASS(Unpack, Expr)

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
    AST_CLASS(Subrange, Expr)

    SubrangeExprAst()
        : ExprAst(Kind::SubrangeExpr)
    {}

    NAMED_AST_PARAM(Low, low, ExprAst)
    NAMED_LOC_PARAM(Delim1, delim1)
    NAMED_AST_PARAM(Hi, hi, ExprAst)
    NAMED_LOC_PARAM(Delim2, delim2)
    NAMED_AST_PARAM(Max, max, ExprAst) // TODO: In Python this is a step.

    std::unique_ptr<ExprAst> low_;
    SourceLoc delim1Loc_;
    std::unique_ptr<ExprAst> hi_;
    SourceLoc delim2Loc_;
    std::unique_ptr<ExprAst> max_;
};

// TODO: Merge this and slice into a common ArrayAccess.
class UAISO_API ArrayIndexExprAst final : public ExprAst
{
public:
    AST_CLASS(ArrayIndex, Expr)

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

// TODO: Merge this and index into a common ArrayAccess.
class UAISO_API ArraySliceExprAst : public ExprAst
{
public:
    AST_CLASS(ArraySlice, Expr)

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
    AST_CLASS(Typeid, Expr)

    TypeidExprAst()
        : ExprAst(Kind::TypeidExpr)
    {}

    TypeidExprAst* setSpec(SpecAst* spec);
    TypeidExprAst* setExpr(ExprAst* spec);
    Ast* exprOrSpec() { return exprOrSpec_.get(); }

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
    AST_CLASS(Assert, Expr)

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
    AST_CLASS(TypeAssert, Expr)

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
    AST_CLASS(Mixin, Expr)

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
    AST_CLASS(Error, Expr)
    CREATE_WITH_LOC(Error)

    ErrorExprAst()
        : ExprAst(Kind::ErrorExpr)
    {}

    NAMED_LOC_PARAM(Error, error)

    SourceLoc errorLoc_;
};

class UAISO_API PrintExprAst final : public ExprAst
{
public:
    AST_CLASS(Print, Expr)

    PrintExprAst()
        : ExprAst(Kind::PrintExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Opr, opr)
    NAMED_AST_LIST_PARAM(Expr, exprs, ExprAst)

    SourceLoc keyLoc_;
    SourceLoc oprLoc_;
    std::unique_ptr<ExprAstList> exprs_;
};

class UAISO_API YieldExprAst final : public ExprAst
{
public:
    AST_CLASS(Yield, Expr)

    YieldExprAst()
        : ExprAst(Kind::YieldExpr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_LIST_PARAM(Expr, exprs, ExprAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAstList> exprs_;
};

/*!
 * \brief The ListCompreExprAst class
 *
 * A list comprehension.
 */
class UAISO_API ListCompreExprAst final : public ExprAst
{
public:
    AST_CLASS(ListCompre, Expr)

    ListCompreExprAst()
        : ExprAst(Kind::ListCompreExpr)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_LOC_PARAM(Sep, sep)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_AST_LIST_PARAM(Gen, gens, GeneratorAst)

    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
    SourceLoc sepLoc_;
    std::unique_ptr<ExprAst> expr_;
    std::unique_ptr<GeneratorAstList> gens_;
};

/*!
 * \brief The TerExprAst class
 *
 * The ternary expression.
 */
class UAISO_API TerExprAst final : public ExprAst
{
public:
    AST_CLASS(Ter, Expr)

    TerExprAst()
        : ExprAst(Kind::TerExpr)
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

} // namespace uaiso

#endif
