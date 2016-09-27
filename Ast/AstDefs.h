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

#ifndef UAISO_ASTDEFS_H__
#define UAISO_ASTDEFS_H__

#define AST_MIXIN(GEN_CODE_FOR) \
    NAME_AST_MIXIN(GEN_CODE_FOR) \
    SPEC_AST_MIXIN(GEN_CODE_FOR) \
    ATTR_AST_MIXIN(GEN_CODE_FOR) \
    DECL_AST_MIXIN(GEN_CODE_FOR) \
    EXPR_AST_MIXIN(GEN_CODE_FOR) \
    STMT_AST_MIXIN(GEN_CODE_FOR)

#define NAME_AST_MIXIN(GEN_CODE_FOR) \
    GEN_CODE_FOR(Completion, Name) \
    GEN_CODE_FOR(Error, Name) \
    GEN_CODE_FOR(Gen, Name) \
    GEN_CODE_FOR(Nested, Name) \
    GEN_CODE_FOR(Punc, Name) \
    GEN_CODE_FOR(Simple, Name) \
    GEN_CODE_FOR(Special, Name) \
    GEN_CODE_FOR(TemplateInst, Name)

#define SPEC_AST_MIXIN(GEN_CODE_FOR) \
    GEN_CODE_FOR(Array, OpaqueSpec) \
    GEN_CODE_FOR(Builtin, Spec) \
    GEN_CODE_FOR(Chan, OpaqueSpec) \
    GEN_CODE_FOR(Decorated, Spec) \
    GEN_CODE_FOR(Opaque, Spec) \
    GEN_CODE_FOR(Func, Spec) \
    GEN_CODE_FOR(Inferred, Spec) \
    GEN_CODE_FOR(Named, Spec) \
    GEN_CODE_FOR(Ptr, OpaqueSpec) \
    GEN_CODE_FOR(Record, Spec) \
    GEN_CODE_FOR(Typeof, Spec) \
    GEN_CODE_FOR(Void, Spec)

#define ATTR_AST_MIXIN(GEN_CODE_FOR) \
    GEN_CODE_FOR(Annot, Attr) \
    GEN_CODE_FOR(Auto, Attr) \
    GEN_CODE_FOR(Codegen, Attr) \
    GEN_CODE_FOR(Decl, Attr) \
    GEN_CODE_FOR(EvalStrategy, Attr) \
    GEN_CODE_FOR(Linkage, Attr) \
    GEN_CODE_FOR(ParamDir, Attr) \
    GEN_CODE_FOR(StorageClass, Attr) \
    GEN_CODE_FOR(TypeQual, Attr) \
    GEN_CODE_FOR(Visibility, Attr)

#define DECL_AST_MIXIN(GEN_CODE_FOR) \
    GEN_CODE_FOR(Alias, Decl) \
    GEN_CODE_FOR(AsPat, Decl) \
    GEN_CODE_FOR(Base, Decl) \
    GEN_CODE_FOR(Block, Decl) \
    GEN_CODE_FOR(Constraint, Decl) \
    GEN_CODE_FOR(Debug, Decl) \
    GEN_CODE_FOR(DestructPat, Decl) \
    GEN_CODE_FOR(Empty, Decl) \
    GEN_CODE_FOR(Enum, Decl) \
    GEN_CODE_FOR(EnumMember, Decl) \
    GEN_CODE_FOR(Error, Decl) \
    GEN_CODE_FOR(Export, Decl) \
    GEN_CODE_FOR(ExportSelection, Decl) \
    GEN_CODE_FOR(Func, Decl) \
    GEN_CODE_FOR(Forward, Decl) \
    GEN_CODE_FOR(Import, Decl) \
    GEN_CODE_FOR(ImportGroup, Decl) \
    GEN_CODE_FOR(ImportSelection, Decl) \
    GEN_CODE_FOR(Invariant, Decl) \
    GEN_CODE_FOR(IrrefutPat, Decl) \
    GEN_CODE_FOR(ListPat, Decl) \
    GEN_CODE_FOR(Module, Decl) \
    GEN_CODE_FOR(Package, Decl) \
    GEN_CODE_FOR(Param, Decl) \
    GEN_CODE_FOR(ParamGroup, Decl) \
    GEN_CODE_FOR(ParamClause, Decl) \
    GEN_CODE_FOR(Pat, Decl) \
    GEN_CODE_FOR(PatBind, Decl) \
    GEN_CODE_FOR(Record, Decl) \
    GEN_CODE_FOR(Section, Decl) \
    GEN_CODE_FOR(Selective, Decl) \
    GEN_CODE_FOR(StaticAssert, Decl) \
    GEN_CODE_FOR(Template, Decl) \
    GEN_CODE_FOR(TemplateParam, Decl) \
    GEN_CODE_FOR(TemplateParamClause, Decl) \
    GEN_CODE_FOR(TuplePat, Decl) \
    GEN_CODE_FOR(TrivialPat, Decl) \
    GEN_CODE_FOR(UnitTest, Decl) \
    GEN_CODE_FOR(WildCardPat, Decl) \
    GEN_CODE_FOR(WrappedPat, Decl) \
    GEN_CODE_FOR(Var, Decl) \
    GEN_CODE_FOR(VarGroup, Decl) \
    GEN_CODE_FOR(Version, Decl)

#define EXPR_AST_MIXIN(GEN_CODE_FOR) \
    GEN_CODE_FOR(Add, BinExpr) \
    GEN_CODE_FOR(AddrOf, UnaryExpr) \
    GEN_CODE_FOR(ArrayInit, Expr) \
    GEN_CODE_FOR(ArrayIndex, Expr) \
    GEN_CODE_FOR(ArrayLength, PriExpr) \
    GEN_CODE_FOR(ArraySlice, Expr) \
    GEN_CODE_FOR(Assert, Expr) \
    GEN_CODE_FOR(Assign, Expr) \
    GEN_CODE_FOR(Bin, Expr) \
    GEN_CODE_FOR(BitAnd, BinExpr) \
    GEN_CODE_FOR(BitComp, UnaryExpr) \
    GEN_CODE_FOR(BitOr, BinExpr) \
    GEN_CODE_FOR(BitXor, BinExpr) \
    GEN_CODE_FOR(BoolLit, PriExpr) \
    GEN_CODE_FOR(Call, Expr) \
    GEN_CODE_FOR(Cast, Expr) \
    GEN_CODE_FOR(Chan, UnaryExpr) \
    GEN_CODE_FOR(CharLit, PriExpr) \
    GEN_CODE_FOR(Comma, BinExpr) \
    GEN_CODE_FOR(Concat, BinExpr ) \
    GEN_CODE_FOR(Del, Expr) \
    GEN_CODE_FOR(Designate, Expr) \
    GEN_CODE_FOR(Div, BinExpr) \
    GEN_CODE_FOR(Error, Expr) \
    GEN_CODE_FOR(Eq, BinExpr) \
    GEN_CODE_FOR(FuncLit, PriExpr) \
    GEN_CODE_FOR(Ident, Expr) \
    GEN_CODE_FOR(In, BinExpr) \
    GEN_CODE_FOR(IncDec, UnaryExpr) \
    GEN_CODE_FOR(Is, BinExpr) \
    GEN_CODE_FOR(ListCompre, Expr) \
    GEN_CODE_FOR(LogicAnd, BinExpr) \
    GEN_CODE_FOR(LogicNot, UnaryExpr) \
    GEN_CODE_FOR(LogicOr, BinExpr) \
    GEN_CODE_FOR(Make, Expr) \
    GEN_CODE_FOR(MemberAccess, Expr) \
    GEN_CODE_FOR(Minus, UnaryExpr) \
    GEN_CODE_FOR(Mod, BinExpr) \
    GEN_CODE_FOR(Mul, BinExpr) \
    GEN_CODE_FOR(NestedNew, Expr) \
    GEN_CODE_FOR(New, Expr) \
    GEN_CODE_FOR(NullLit, PriExpr) \
    GEN_CODE_FOR(NumLit, PriExpr) \
    GEN_CODE_FOR(Plus, UnaryExpr) \
    GEN_CODE_FOR(Pri, Expr) \
    GEN_CODE_FOR(Print, Expr) \
    GEN_CODE_FOR(Power, BinExpr) \
    GEN_CODE_FOR(PtrDeref, UnaryExpr) \
    GEN_CODE_FOR(RecordLit, PriExpr) \
    GEN_CODE_FOR(RecordInit, Expr) \
    GEN_CODE_FOR(Rel, BinExpr) \
    GEN_CODE_FOR(Shift, BinExpr) \
    GEN_CODE_FOR(StrLit, PriExpr) \
    GEN_CODE_FOR(Sub, BinExpr) \
    GEN_CODE_FOR(Subrange, Expr) \
    GEN_CODE_FOR(Super, PriExpr) \
    GEN_CODE_FOR(Ter, Expr) \
    GEN_CODE_FOR(This, PriExpr) \
    GEN_CODE_FOR(TupleLit, Expr) \
    GEN_CODE_FOR(Typeid, Expr) \
    GEN_CODE_FOR(TypeAssert, Expr) \
    GEN_CODE_FOR(TypeQuery, PriExpr) \
    GEN_CODE_FOR(Unary, Expr) \
    GEN_CODE_FOR(Unpack, Expr) \
    GEN_CODE_FOR(Mixin, Expr) \
    GEN_CODE_FOR(Wrapped, Expr) \
    GEN_CODE_FOR(Yield, Expr)

#define STMT_AST_MIXIN(GEN_CODE_FOR) \
    GEN_CODE_FOR(Async, Stmt) \
    GEN_CODE_FOR(Block, Stmt) \
    GEN_CODE_FOR(Body, Stmt) \
    GEN_CODE_FOR(Break, Stmt) \
    GEN_CODE_FOR(CaseClause, Stmt) \
    GEN_CODE_FOR(CatchClause, Stmt) \
    GEN_CODE_FOR(Contract, Stmt) \
    GEN_CODE_FOR(Continue, Stmt) \
    GEN_CODE_FOR(Decl, Stmt) \
    GEN_CODE_FOR(DefaultClause, Stmt) \
    GEN_CODE_FOR(Deferred, Stmt) \
    GEN_CODE_FOR(DoWhile, Stmt) \
    GEN_CODE_FOR(Empty, Stmt) \
    GEN_CODE_FOR(Error, Stmt) \
    GEN_CODE_FOR(Eval, Stmt) \
    GEN_CODE_FOR(Expr, Stmt) \
    GEN_CODE_FOR(Fallthrough, Stmt) \
    GEN_CODE_FOR(FinallyClause, Stmt) \
    GEN_CODE_FOR(For, Stmt) \
    GEN_CODE_FOR(Foreach, Stmt) \
    GEN_CODE_FOR(Goto, Stmt) \
    GEN_CODE_FOR(If, Stmt) \
    GEN_CODE_FOR(In, Stmt) \
    GEN_CODE_FOR(Labeled, Stmt) \
    GEN_CODE_FOR(Out, Stmt) \
    GEN_CODE_FOR(Return, Stmt) \
    GEN_CODE_FOR(Selective, Stmt) \
    GEN_CODE_FOR(Synced, Stmt) \
    GEN_CODE_FOR(Switch, Stmt) \
    GEN_CODE_FOR(Throw, Stmt) \
    GEN_CODE_FOR(Try, Stmt) \
    GEN_CODE_FOR(TypeSwitch, Stmt) \
    GEN_CODE_FOR(While, Stmt) \
    GEN_CODE_FOR(With, Stmt) \
    GEN_CODE_FOR(Yield, Stmt)

#define MAKE_NAME_CASE(AST_NODE, UNUSED) MAKE_CASE(AST_NODE, Name)
#define MAKE_SPEC_CASE(AST_NODE, UNUSED) MAKE_CASE(AST_NODE, Spec)
#define MAKE_ATTR_CASE(AST_NODE, UNUSED) MAKE_CASE(AST_NODE, Attr)
#define MAKE_DECL_CASE(AST_NODE, UNUSED) MAKE_CASE(AST_NODE, Decl)
#define MAKE_EXPR_CASE(AST_NODE, UNUSED) MAKE_CASE(AST_NODE, Expr)
#define MAKE_STMT_CASE(AST_NODE, UNUSED) MAKE_CASE(AST_NODE, Stmt)

#endif
