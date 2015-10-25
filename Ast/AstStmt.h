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

#ifndef UAISO_ASTSTMT_H__
#define UAISO_ASTSTMT_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"
#include "Semantic/Environment.h"

namespace uaiso {

class UAISO_API StmtAst : public Ast
{
public:
    using Ast::Ast;
};

class UAISO_API EmptyStmtAst final : public StmtAst
{
public:
    using Self = EmptyStmtAst;

    EmptyStmtAst()
        : StmtAst(Kind::EmptyStmt)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API ExprStmtAst final : public StmtAst
{
public:
    using Self = ExprStmtAst;

    ExprStmtAst()
        : StmtAst(Kind::ExprStmt)
    {}

    NAMED_AST_LIST_PARAM(Expr, exprs, ExprAst)
    NAMED_LOC_PARAM(Termin, termin)

    std::unique_ptr<ExprAstList> exprs_;
    SourceLoc terminLoc_;
};

class UAISO_API DeclStmtAst final : public StmtAst
{
public:
    using Self = DeclStmtAst;

    DeclStmtAst()
        : StmtAst(Kind::DeclStmt)
    {}

    NAMED_AST_PARAM(Decl, decl, DeclAst)

    std::unique_ptr<DeclAst> decl_;
};

class UAISO_API BlockStmtAst final : public StmtAst
{
public:
    using Self = BlockStmtAst;

    BlockStmtAst()
        : StmtAst(Kind::BlockStmt)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Stmt, stmts, StmtAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc lDelimLoc_;
    std::unique_ptr<StmtAstList> stmts_;
    SourceLoc rDelimLoc_;

    Environment env_;
};

class UAISO_API InStmtAst final : public StmtAst
{
public:
    using Self = InStmtAst;

    InStmtAst()
        : StmtAst(Kind::InStmt)
    {}

    NAMED_LOC_PARAM(In, in)
    NAMED_AST_PARAM(Block, block, StmtAst)

    SourceLoc inLoc_;
    std::unique_ptr<StmtAst> block_;
};

class UAISO_API OutStmtAst final : public StmtAst
{
public:
    using Self = OutStmtAst;

    OutStmtAst()
        : StmtAst(Kind::OutStmt)
    {}

    NAMED_LOC_PARAM(Out, out)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Block, block, StmtAst)

    SourceLoc outLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> block_;
};

class UAISO_API BodyStmtAst final : public StmtAst
{
public:
    using Self = BodyStmtAst;

    BodyStmtAst()
        : StmtAst(Kind::BodyStmt)
    {}

    NAMED_LOC_PARAM(Body, body)
    NAMED_AST_PARAM(Block, block, StmtAst)

    SourceLoc bodyLoc_;
    std::unique_ptr<StmtAst> block_;
};

class UAISO_API ContractStmtAst final : public StmtAst
{
public:
    using Self = ContractStmtAst;

    ContractStmtAst()
        : StmtAst(Kind::ContractStmt)
    {}

    NAMED_AST_PARAM(Stmt1, stmt1, StmtAst)
    NAMED_AST_PARAM(Stmt2, stmt2, StmtAst)
    NAMED_AST_PARAM(Stmt3, stmt3, StmtAst)

    std::unique_ptr<StmtAst> stmt1_;
    std::unique_ptr<StmtAst> stmt2_;
    std::unique_ptr<StmtAst> stmt3_;
};

class UAISO_API LabeledStmtAst final : public StmtAst
{
public:
    using Self = LabeledStmtAst;

    LabeledStmtAst()
        : StmtAst(Kind::LabeledStmt)
    {}

    NAMED_AST_PARAM(Label, label, NameAst)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    std::unique_ptr<NameAst> label_;
    SourceLoc delimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API IfStmtAst final : public StmtAst
{
public:
    using Self = IfStmtAst;

    IfStmtAst()
        : StmtAst(Kind::IfStmt)
    {}

    NAMED_LOC_PARAM(If, if)
    NAMED_LOC_PARAM(Else, else)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Preamble, preamble, StmtAst)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_AST_PARAM(Then, then, StmtAst)
    NAMED_AST_PARAM(NotThen, notThen, StmtAst)

    SourceLoc ifLoc_;
    SourceLoc elseLoc_;
    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> preamble_;
    std::unique_ptr<ExprAst> expr_;
    std::unique_ptr<StmtAst> then_;
    std::unique_ptr<StmtAst> notThen_;
};

class UAISO_API WhileStmtAst final : public StmtAst
{
public:
    using Self = WhileStmtAst;

    WhileStmtAst()
        : StmtAst(Kind::WhileStmt)
    {}

    NAMED_LOC_PARAM(While, while)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc whileLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API DoWhileStmtAst final : public StmtAst
{
public:
    using Self = DoWhileStmtAst;

    DoWhileStmtAst()
        : StmtAst(Kind::DoWhileStmt)
    {}

    NAMED_LOC_PARAM(Do, do)
    NAMED_LOC_PARAM(While, while)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc doLoc_;
    std::unique_ptr<StmtAst> stmt_;
    SourceLoc whileLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
    SourceLoc terminLoc_;
};

class UAISO_API ForStmtAst final : public StmtAst
{
public:
    using Self = ForStmtAst;

    ForStmtAst()
        : StmtAst(Kind::ForStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_PARAM(Preamble, preamble, StmtAst)
    NAMED_AST_PARAM(Cond, cond, ExprAst)
    NAMED_AST_PARAM(Post, post, ExprAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<StmtAst> preamble_;
    std::unique_ptr<ExprAst> cond_;
    SourceLoc delimLoc_;
    std::unique_ptr<ExprAst> post_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API ForeachStmtAst final : public StmtAst
{
public:
    using Self = ForeachStmtAst;

    ForeachStmtAst()
        : StmtAst(Kind::ForeachStmt)
    {}

    // Variety Direct or Reverse

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Decl, decl, DeclAst)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<DeclAst> decl_;
    std::unique_ptr<ExprAst> expr_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API ContinueStmtAst final : public StmtAst
{
public:
    using Self = ContinueStmtAst;

    ContinueStmtAst()
        : StmtAst(Kind::ContinueStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc terminLoc_;
};

class UAISO_API BreakStmtAst final : public StmtAst
{
public:
    using Self = BreakStmtAst;

    BreakStmtAst()
        : StmtAst(Kind::BreakStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc terminLoc_;
};

class UAISO_API ReturnStmtAst final : public StmtAst
{
public:
    using Self = ReturnStmtAst;

    ReturnStmtAst()
        : StmtAst(Kind::ReturnStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_LIST_PARAM(Expr, exprs, ExprAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAstList> exprs_;
    SourceLoc terminLoc_;
};

class UAISO_API GotoStmtAst final : public StmtAst
{
public:
    using Self = GotoStmtAst;

    GotoStmtAst()
        : StmtAst(Kind::GotoStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc terminLoc_;
};

class UAISO_API ThrowStmtAst final : public StmtAst
{
public:
    using Self = ThrowStmtAst;

    ThrowStmtAst()
        : StmtAst(Kind::ThrowStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc terminLoc_;
};

/*!
 * \brief The SwitchStmtAst class
 *
 * A traditional expression switch statement.
 */
class UAISO_API SwitchStmtAst final : public StmtAst
{
public:
    using Self = SwitchStmtAst;

    SwitchStmtAst()
        : StmtAst(Kind::SwitchStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Preamble, preamble, StmtAst)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    std::unique_ptr<StmtAst> preamble_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

/*!
 * \brief The SwitchStmtAst class
 *
 * A type switch statement, as in Go.
 */
class UAISO_API TypeSwitchStmtAst final : public StmtAst
{
public:
    using Self = TypeSwitchStmtAst;

    TypeSwitchStmtAst()
        : StmtAst(Kind::TypeSwitchStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API CaseClauseStmtAst final : public StmtAst
{
public:
    using Self = CaseClauseStmtAst;

    CaseClauseStmtAst()
        : StmtAst(Kind::CaseClauseStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_LIST_PARAM(Expr, exprs, ExprAst)
    NAMED_AST_LIST_PARAM(Stmt, stmts, StmtAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAstList> exprs_;
    SourceLoc delimLoc_;
    std::unique_ptr<StmtAstList> stmts_;
};

class UAISO_API DefaultClauseStmtAst final : public StmtAst
{
public:
    using Self = DefaultClauseStmtAst;

    DefaultClauseStmtAst()
        : StmtAst(Kind::DefaultClauseStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_LIST_PARAM(Stmt, stmts, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc delimLoc_;
    std::unique_ptr<StmtAstList> stmts_;
};

/*!
 * \brief The SyncedStmtAst class
 *
 * A synchronized statement.
 */
class UAISO_API SyncedStmtAst final : public StmtAst
{
public:
    using Self = SyncedStmtAst;

    SyncedStmtAst()
        : StmtAst(Kind::SyncedStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API WithStmtAst final : public StmtAst
{
public:
    using Self = WithStmtAst;

    WithStmtAst()
        : StmtAst(Kind::WithStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_LIST_PARAM(Expr, exprs, ExprAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAstList> exprs_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API TryStmtAst final : public StmtAst
{
public:
    using Self = TryStmtAst;

    TryStmtAst()
        : StmtAst(Kind::TryStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)
    NAMED_AST_LIST_PARAM(Catch, catchs, StmtAst)
    NAMED_AST_PARAM(Final, final, StmtAst)

    SourceLoc keyLoc_;
    std::unique_ptr<StmtAst> stmt_;
    std::unique_ptr<StmtAstList> catchs_;
    std::unique_ptr<StmtAst> final_;
};

class UAISO_API CatchClauseStmtAst final : public StmtAst
{
public:
    using Self = CatchClauseStmtAst;

    CatchClauseStmtAst()
        : StmtAst(Kind::CatchClauseStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Decl, decl, DeclAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAst> decl_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API FinallyClauseStmtAst final : public StmtAst
{
public:
    using Self = FinallyClauseStmtAst;

    FinallyClauseStmtAst()
        : StmtAst(Kind::FinallyClauseStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

/*!
 * \brief The SelectiveStmtAst class
 */
class UAISO_API SelectiveStmtAst final : public StmtAst
{
public:
    using Self = SelectiveStmtAst;

    SelectiveStmtAst()
        : StmtAst(Kind::SelectiveStmt)
    {
        INIT_VARIETY(SelectiveVariety::Unknown);
    }

    APPLY_VARIETY(SelectiveVariety)

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(IfStmt, ifStmt, StmtAst)
    NAMED_LOC_PARAM(OtherKey, otherKey)
    NAMED_AST_PARAM(ElseStmt, elseStmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> ifStmt_;
    SourceLoc otherKeyLoc_;
    std::unique_ptr<StmtAst> elseStmt_;
};

class UAISO_API ErrorStmtAst final : public StmtAst
{
public:
    using Self = ErrorStmtAst;

    ErrorStmtAst()
        : StmtAst(Kind::ErrorStmt)
    {}

    NAMED_LOC_PARAM(Error, error)

    SourceLoc errorLoc_;
};

class UAISO_API AsyncStmtAst final : public StmtAst
{
public:
    using Self = AsyncStmtAst;

    AsyncStmtAst()
        : StmtAst(Kind::AsyncStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_AST_PARAM(Expr, expr, ExprAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc terminLoc_;
};

class UAISO_API DeferredStmtAst final : public StmtAst
{
public:
    using Self = DeferredStmtAst;

    DeferredStmtAst()
        : StmtAst(Kind::DeferredStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API FallthroughStmtAst final : public StmtAst
{
public:
    using Self = FallthroughStmtAst;

    FallthroughStmtAst()
        : StmtAst(Kind::FallthroughStmt)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API YieldStmtAst final : public StmtAst
{
public:
    using Self = YieldStmtAst;

    YieldStmtAst()
        : StmtAst(Kind::YieldStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_AST_PARAM(Expr, expr, ExprAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc terminLoc_;
};

class UAISO_API EvalStmtAst final : public StmtAst
{
public:
    using Self = EvalStmtAst;

    EvalStmtAst()
        : StmtAst(Kind::EvalStmt)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_AST_PARAM(Expr, expr, ExprAst)

    SourceLoc keyLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc terminLoc_;
};

} // namespace uaiso

#endif
