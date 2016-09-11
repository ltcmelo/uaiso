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

#ifndef UAISO_ASTDECLARATION_H__
#define UAISO_ASTDECLARATION_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Ast/AstBase.h"
#include "Semantic/SymbolFwd.h"
#include "Semantic/Environment.h"

namespace uaiso {

class UAISO_API DeclAst : public Ast
{
public:
    using Ast::Ast;
};

class UAISO_API ErrorDeclAst final : public DeclAst
{
public:
    AST_CLASS(Error, Decl)

    ErrorDeclAst()
        : DeclAst(Kind::ErrorDecl)
    {}

    NAMED_LOC_PARAM(Error, error)

    SourceLoc errorLoc_;
};

class UAISO_API EmptyDeclAst final : public DeclAst
{
public:
    AST_CLASS(Empty, Decl)

    EmptyDeclAst()
        : DeclAst(Kind::EmptyDecl)
    {}

    NAMED_LOC_PARAM(Key, key)

    SourceLoc keyLoc_;
};

class UAISO_API VarDeclAst : public DeclAst
{
public:
    AST_CLASS(Var, Decl)

    VarDeclAst()
        : DeclAst(Kind::VarDecl)
        , sym_(nullptr)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM__BASE__(AssignLoc)
    NAMED_AST_PARAM__BASE__(Init, ExprAst)

    virtual bool hasInit() const { return false; }
    virtual ExprAst* init() const { return nullptr; }

    std::unique_ptr<NameAst> name_;

    Var* sym_;
};

/*
 * Helper type that "injects" an initializer.
 */
struct VarInit__
{
    static bool checkInitializer__() { return true; }

    void setAssignLoc__(const SourceLoc& loc) { assignLoc_ = loc; }
    const SourceLoc& assignLoc__() const { return assignLoc_; }
    void setInit__(ExprAst* expr) { init_.reset(expr); }
    ExprAst* init__() const { return init_.get(); }

    SourceLoc assignLoc_;
    std::unique_ptr<ExprAst> init_;
};

struct VarInit__Empty__
{
    static bool checkInitializer__() { return false; }
    void setAssignLoc__(const SourceLoc&) {}
    const SourceLoc& assignLoc__() const { return kEmptyLoc; }
    void setInit__(ExprAst*) {}
    ExprAst* init__() const { return nullptr; }
};

/*!
 * A parameterized subclass of VarDeclAst that avoids bloating the AST
 * memory size with members that are commonly absent. It works in combination
 * with the "member" injection helpers.
 */
template <class InitT = VarInit__Empty__>
class VarDeclAst__ final
        : public VarDeclAst
        , private InitT
{
public:
    using Self = VarDeclAst__<InitT>;
    using VarDeclAst::VarDeclAst;

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM__(AssignLoc, InitT)
    NAMED_AST_PARAM__(Init, InitT, ExprAst)

    bool hasInit() const override { return InitT::checkInitializer__(); }
    ExprAst* init() const override { return InitT::init__(); }
};

/*!
 * \brief The VarGroupDeclAst class
 *
 * A group of variables declared together. If a type specifier is supplied,
 * it's assumed to apply to all variables. If not, it's accepted that each
 * variable will have a corresponding initializer.
 */
class UAISO_API VarGroupDeclAst : public DeclAst
{
public:
    AST_CLASS(VarGroup, Decl)

    VarGroupDeclAst()
        : DeclAst(Kind::VarGroupDecl)
    {
        bit_.alloc = static_cast<char>(AllocScheme::Unknown);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_LOC_PARAM__BASE__(AssignLoc)
    NAMED_AST_LIST_PARAM__BASE__(Init, ExprAst)

    void setAllocScheme(AllocScheme v)
    {
        bit_.alloc = static_cast<char>(v);
    }

    AllocScheme allocScheme() const { return AllocScheme(bit_.alloc); }

    virtual bool hasInits() const { return false; }
    virtual ExprAstList* inits() const { return nullptr; }

    SourceLoc keyLoc_;
    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<DeclAstList> decls_;
    SourceLoc terminLoc_;
};

struct VarGroupInits__
{
    static bool checkInits__() { return true; }

    void setAssignLoc__(const SourceLoc& loc) { assignLoc_ = loc; }
    const SourceLoc& assignLoc__() const { return assignLoc_; }
    void setInits__(ExprAstList* expr) { inits_.reset(expr); }
    ExprAstList* inits__() const { return inits_.get(); }

    SourceLoc assignLoc_;
    std::unique_ptr<ExprAstList> inits_;
};

struct VarGroupInits__Empty
{
    static bool checkInits__() { return false; }
    void setAssignLoc__(const SourceLoc&) {}
    const SourceLoc& assignLoc__() const { return kEmptyLoc; }
    void setInits__(ExprAstList*) {}
    ExprAstList* inits__() const { return nullptr; }
};

/*!
 * A parameterized subclass of VarGroupDeclAst that avoids bloating the AST
 * memory size with members that are commonly absent. It works in combination
 * with the "member" injection helpers.
 */
template <class InitsT = VarGroupInits__Empty>
class VarGroupDeclAst__ final
        : public VarGroupDeclAst
        , public InitsT
{
public:
    using Self = VarGroupDeclAst__<InitsT>;
    using VarGroupDeclAst::VarGroupDeclAst;

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_LOC_PARAM__(AssignLoc, InitsT)
    NAMED_AST_LIST_PARAM__(Init, InitsT, ExprAst)

    bool hasInits() const override { return InitsT::checkInits__(); }
    ExprAstList* inits() const override { return InitsT::inits__(); }
};

/*!
 * \brief The VarTagDeclAst class
 *
 * A declaration with a tag, as Go's fields.
 */
class UAISO_API VarTagDeclAst final : public VarGroupDeclAst
{
public:
    AST_CLASS(VarTag, Decl)
    using VarGroupDeclAst::VarGroupDeclAst;

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_AST_PARAM(Tag, tag, ExprAst)

    std::unique_ptr<ExprAst> tag_;
};

class UAISO_API ParamClauseDeclAst final : public DeclAst
{
public:
    AST_CLASS(ParamClause, Decl)

    ParamClauseDeclAst()
        : DeclAst(Kind::ParamClauseDecl)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_LOC_PARAM(Variadic, variadic)
    NAMED_LOC_PARAM(RDelim, rDelim)

    bool isVariadic() const; // If has variadic loc or if it's variadic typesafe
    bool isVariadicTypesafe() const;

    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> decls_;
    SourceLoc variadicLoc_;
    SourceLoc rDelimLoc_;
};

/*!
 * \brief The ParamGroupDeclAst class
 *
 * A group of parameters of the same type declared together.
 */
class UAISO_API ParamGroupDeclAst final : public DeclAst
{
public:
    AST_CLASS(ParamGroup, Decl)

    ParamGroupDeclAst()
        : DeclAst(Kind::ParamGroupDecl)
    {}

    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)

    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<DeclAstList> decls_;
};

class UAISO_API ParamDeclAst : public DeclAst
{
public:
    AST_CLASS(Param, Decl)

    ParamDeclAst()
        : DeclAst(Kind::ParamDecl)
        , sym_(nullptr)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM__BASE__(VariadicLoc)
    NAMED_LOC_PARAM__BASE__(AssignLoc)
    NAMED_AST_PARAM__BASE__(DefaultArg, ExprAst)

    virtual bool hasDefaultArg() const { return false; }
    virtual ExprAst* defaultArg() const { return nullptr; }

    virtual bool isVariadic() const { return false; }
    virtual const SourceLoc& variadicLoc() const { return kEmptyLoc; }

    std::unique_ptr<NameAst> name_;

    Param* sym_;
};

struct ParamDefaultArg__
{
    static bool checkDefaultArg__() { return true; }

    void setAssignLoc__(const SourceLoc& loc) { assignLoc_ = loc; }
    const SourceLoc& assignLoc__() const { return assignLoc_; }
    void setDefaultArg__(ExprAst* expr) { defArg_.reset(expr); }
    ExprAst* defaultArg__() const { return defArg_.get(); }

    SourceLoc assignLoc_;
    std::unique_ptr<ExprAst> defArg_;
};

struct ParamDefaultArg__Empty__
{
    static bool checkDefaultArg__() { return false; }
    void setAssignLoc__(const SourceLoc&) {}
    const SourceLoc& assignLoc__() const { return kEmptyLoc; }
    void setDefaultArg__(ExprAst*) {}
    ExprAst* defaultArg__() const { return nullptr; }
};

struct ParamVariadic__
{
    static bool checkVariadic__() { return true; }

    void setVariadicLoc__(const SourceLoc& loc) { variadicLoc_ = loc; }
    const SourceLoc& variadicLoc__() const { return variadicLoc_; }

    SourceLoc variadicLoc_;
};

struct ParamVariadic__Empty__
{
    static bool checkVariadic__() { return false; }
    void setVariadicLoc__(const SourceLoc&) {}
    const SourceLoc& variadicLoc__() const { return kEmptyLoc; }
};

/*!
 * A parameterized subclass of ParamDeclAst that avoids bloating the AST
 * memory size with members that are commonly absent. It works in combination
 * with the "member" injection helpers.
 */
template <class VariadicT = ParamVariadic__Empty__,
          class DefaultArgT = ParamDefaultArg__Empty__>
class ParamDeclAst__ final
        : public ParamDeclAst
        , private VariadicT
        , private DefaultArgT
{
public:
    using Self = ParamDeclAst__<VariadicT, DefaultArgT>;
    using ParamDeclAst::ParamDeclAst;

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM__(VariadicLoc, VariadicT)
    NAMED_LOC_PARAM__(AssignLoc, DefaultArgT)
    NAMED_AST_PARAM__(DefaultArg, DefaultArgT, ExprAst)

    bool hasDefaultArg() const override { return DefaultArgT::checkDefaultArg__(); }
    ExprAst* defaultArg() const override { return DefaultArgT::defaultArg__(); }

    bool isVariadic() const override { return VariadicT::checkVariadic__(); }
    const SourceLoc& variadicLoc() const override { return VariadicT::variadicLoc__(); }
};

class UAISO_API TemplateParamClauseDeclAst final : public DeclAst
{
public:
    AST_CLASS(TemplateParamClause, Decl)

    TemplateParamClauseDeclAst()
        : DeclAst(Kind::TemplateParamClauseDecl)
    {}

    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> decls_;
    SourceLoc rDelimLoc_;
};

class UAISO_API TemplateParamDeclAst : public DeclAst
{
public:
    AST_CLASS(TemplateParam, Decl)

    TemplateParamDeclAst()
        : DeclAst(Kind::TemplateParamDecl)
    {}

    NAMED_LOC_PARAM(Pack, pack)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM__BASE__(AssignLoc)
    NAMED_AST_PARAM__BASE__(DefaultArg, Ast)
    NAMED_LOC_PARAM__BASE__(BindLoc)
    NAMED_AST_PARAM__BASE__(Specialization, Ast)

    virtual bool hasDefaultArg() const { return false; }
    virtual Ast* defaultArg() const { return nullptr; }

    virtual bool hasSpecialization() const { return false; }
    virtual Ast* specialization() const { return nullptr; }

    bool isNonType() const { return spec_.get() != nullptr; }

    bool isTuple() const { return !packLoc_.isEmpty(); }

    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<NameAst> name_;
    SourceLoc packLoc_;
};

/*
 * Helper type that "injects" a default arg.
 */
struct TemplateParamDefaultArg__
{
    static bool checkDefaultArg__() { return true; }

    void setAssignLoc__(const SourceLoc& loc) { assignLoc_ = loc; }
    const SourceLoc& assignLoc__() const { return assignLoc_; }
    void setDefaultArg__(Ast* ast) { defaultArg_.reset(ast); }
    Ast* defaultArg__() const { return defaultArg_.get(); }

    SourceLoc assignLoc_;
    std::unique_ptr<Ast> defaultArg_;
};

struct TemplateParamDefaultArg__Empty__
{
    static bool checkDefaultArg__() { return false; }
    void setAssignLoc__(const SourceLoc&) {}
    const SourceLoc& assignLoc__() const { return kEmptyLoc; }
    void setDefaultArg__(Ast*) {}
    Ast* defaultArg__() const { return nullptr; }
};

/*
 * Helper type that "injects" a specialization.
 */
struct TemplateParamSpecialization__
{
    static bool checkSpecialization__() { return true; }

    void setBindLoc__(const SourceLoc& loc) { bindLoc_ = loc; }
    const SourceLoc& bindLoc__() const { return bindLoc_; }
    void setSpecialization__(Ast* ast) { ast_.reset(ast); }
    Ast* specialization__() const { return ast_.get(); }

    SourceLoc bindLoc_;
    std::unique_ptr<Ast> ast_;
};

struct TemplateParamSpecialization__Empty__
{
    static bool checkSpecialization__() { return false; }
    void setBindLoc__(const SourceLoc&) {}
    const SourceLoc& bindLoc__() const { return kEmptyLoc; }
    void setSpecialization__(Ast*) {}
    Ast* specialization__() const { return nullptr; }
};

template <class DefaultArgT = TemplateParamDefaultArg__Empty__,
          class SpecializationT = TemplateParamSpecialization__Empty__>
class TemplateParamDeclAst__
        : public TemplateParamDeclAst
        , private DefaultArgT
        , private SpecializationT
{
public:
    using Self = TemplateParamDeclAst__<DefaultArgT, SpecializationT>;
    using TemplateParamDeclAst::TemplateParamDeclAst;

    NAMED_LOC_PARAM(Pack, pack)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM__(AssignLoc, DefaultArgT)
    NAMED_AST_PARAM__(DefaultArg, DefaultArgT, Ast)
    NAMED_LOC_PARAM__(BindLoc, SpecializationT)
    NAMED_AST_PARAM__(Specialization, SpecializationT, Ast)

    virtual bool hasDefaultArg() const { return DefaultArgT::checkDefaultArg__(); }
    virtual Ast* defaultArg() const { return DefaultArgT::defaultArg__(); }

    virtual bool hasSpecialization() const { return SpecializationT::checkSpecialization__(); }
    virtual Ast* specialization() const { return SpecializationT::specialization__(); }
};

class UAISO_API TemplateParamAliasDeclAst final : public TemplateParamDeclAst
{
public:
    AST_CLASS(TemplateParamAlias, Decl)
    using TemplateParamDeclAst::TemplateParamDeclAst;

    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Alias, alias)

    SourceLoc aliasLoc_;
};

class UAISO_API TemplateParamThisDeclAst final : public TemplateParamDeclAst
{
public:
    AST_CLASS(TemplateParamThis, Decl)
    using TemplateParamDeclAst::TemplateParamDeclAst;

    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Thiss, thiss)

    SourceLoc thissLoc_;
};

class UAISO_API FuncDeclAst : public DeclAst
{
public:
    AST_CLASS(Func, Decl)
    VARIETY_AST(FuncVariety)

    FuncDeclAst()
        : DeclAst(Kind::FuncDecl)
        , sym_(nullptr)
    {
        INIT_VARIETY(FuncVariety::Unknown);
    }

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    virtual bool hasExplicitReceiver() const { return false; }

    std::unique_ptr<NameAst> name_;
    std::unique_ptr<SpecAst> spec_;
    std::unique_ptr<StmtAst> stmt_;

    Func* sym_;
};

/*!
 * \brief The FuncRecvDeclAst class
 */
class UAISO_API FuncRecvDeclAst final : public FuncDeclAst
{
public:
    AST_CLASS(FuncRecv, Decl)
    using FuncDeclAst::FuncDeclAst;

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)
    NAMED_AST_PARAM(Recv, recv, DeclAst)

    bool hasExplicitReceiver() const override { return true; }

    std::unique_ptr<DeclAst> recv_; // Go's receivers.
};

/*!
 * \brief The TemplateDeclAst class
 *
 * A template declaration.
 */
class UAISO_API TemplateDeclAst final : public DeclAst
{
public:
    AST_CLASS(Template, Decl)

    TemplateDeclAst()
        : DeclAst(Kind::TemplateDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    // TODO: Incomplete...
};

/*!
 * \brief The BlockDeclAst class
 *
 * A declaration grouping, doesn't introduce a new environment (as a BlockStmtAst).
 */
class UAISO_API BlockDeclAst final : public DeclAst
{
public:
    AST_CLASS(Block, Decl)

    BlockDeclAst()
        : DeclAst(Kind::BlockDecl)
    {}

    NAMED_AST_LIST_PARAM(Attr, attrs, AttrAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<AttrAstList> attrs_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> decls_;
    SourceLoc rDelimLoc_;
};

class UAISO_API ConstraintDeclAst final : public DeclAst
{
public:
    AST_CLASS(Constraint, Decl)

    ConstraintDeclAst()
        : DeclAst(Kind::ConstraintDecl)
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

class UAISO_API AliasDeclAst final : public DeclAst
{
public:
    AST_CLASS(Alias, Decl)

    AliasDeclAst()
        : DeclAst(Kind::AliasDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Eq, eq)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc eqLoc_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc terminLoc_;
};

/*!
 * \brief The PackageDeclAst class
 *
 * A package declaration, as in Go. The name of the package typically
 * determines a directory in which modules (normally as files) of this
 * package can be found.
 */
class UAISO_API PackageDeclAst final : public DeclAst
{
public:
    AST_CLASS(Package, Decl)

    PackageDeclAst()
        : DeclAst(Kind::PackageDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc terminLoc_;

};

/*!
 * \brief The ModuleDeclAst class
 *
 * A module declaration, as in D. The name of a module typically maps to a
 * source-file, which might belog to a particular package, identified by
 * is owning directory.
 */
class UAISO_API ModuleDeclAst final : public DeclAst
{
public:
    AST_CLASS(Module, Decl)

    ModuleDeclAst()
        : DeclAst(Kind::ModuleDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_AST_PARAM(Expot, expot, DeclAst) // Exported names.
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    std::unique_ptr<DeclAst> expot_;
    SourceLoc terminLoc_;
};

class UAISO_API ImportGroupDeclAst : public DeclAst
{
public:
    AST_CLASS(ImportGroup, Decl)

    ImportGroupDeclAst()
        : DeclAst(Kind::ImportGroupDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Hint, hint)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_LOC_PARAM(Termin, termin)
    NAMED_AST_LIST_PARAM(Module, modules, DeclAst)

    SourceLoc keyLoc_;
    SourceLoc hintLoc_;
    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
    SourceLoc terminLoc_;
    std::unique_ptr<DeclAstList> modules_;
};

class UAISO_API ImportDeclAst : public DeclAst
{
public:
    AST_CLASS(Import, Decl)

    ImportDeclAst()
        : DeclAst(Kind::ImportDecl)
    {}

    NAMED_AST_PARAM(Mode, mode, NameAst)
    NAMED_AST_PARAM(Target, target, ExprAst)
    NAMED_LOC_PARAM(As, as)
    NAMED_AST_PARAM(LocalName, localName, NameAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Selection, selections, DeclAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    std::unique_ptr<NameAst> mode_;             //!< Qualified or unqualified.
    std::unique_ptr<ExprAst> target_;           //!< What is to be imported.
    SourceLoc asLoc_;                           //!< Local name indicator.
    std::unique_ptr<NameAst> localName_;        //!< The local name.
    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> selections_;   //!< Selected item names.
    SourceLoc rDelimLoc_;
};

class UAISO_API ImportSelectionDeclAst : public DeclAst
{
public:
    AST_CLASS(ImportSelection, Decl)

    ImportSelectionDeclAst()
        : DeclAst(Kind::ImportSelectionDecl)
    {}

    NAMED_AST_PARAM(ActualName, actualName, NameAst)
    NAMED_LOC_PARAM(As, as)
    NAMED_AST_PARAM(AlternateName, alternateName, NameAst)

    std::unique_ptr<NameAst> actualName_;
    SourceLoc asLoc_;
    std::unique_ptr<NameAst> alternateName_;
};

class UAISO_API ExportDeclAst final : public DeclAst
{
public:
    AST_CLASS(Export, Decl)

    ExportDeclAst()
        : DeclAst(Kind::ExportDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Selection, selections, DeclAst)
    NAMED_LOC_PARAM(RDelim, rDelim)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> selections_;
    SourceLoc rDelimLoc_;
};

class UAISO_API ExportSelectionDeclAst : public DeclAst
{
public:
    AST_CLASS(ExportSelection, Decl)

    ExportSelectionDeclAst()
        : DeclAst(Kind::ExportSelectionDecl)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)

    std::unique_ptr<NameAst> name_;
};

/*!
 * \brief The SectionDeclAst class
 *
 * A section declaration, as in Go (for vars, const, types).
 */
class UAISO_API SectionDeclAst final : public DeclAst
{
public:
    AST_CLASS(Section, Decl)
    VARIETY_AST(SectionVariety)

    SectionDeclAst()
        : DeclAst(Kind::SectionDecl)
    {
        INIT_VARIETY(SectionVariety::Unknown);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_LOC_PARAM(Termin, termin)

    bool isEmpty() const;

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> decls_;
    SourceLoc rDelimLoc_;
    SourceLoc terminLoc_;
};

class UAISO_API StaticAssertDeclAst final : public DeclAst
{
public:
    AST_CLASS(StaticAssert, Decl)

    StaticAssertDeclAst()
        : DeclAst(Kind::StaticAssertDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(MDelim, mDelim)
    NAMED_AST_PARAM(Message, message, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc mDelimLoc_;
    std::unique_ptr<ExprAst> message_;
    SourceLoc rDelimLoc_;
    SourceLoc terminLoc_;
};

/*!
 * \brief The VersionDeclAst class
 */
class UAISO_API VersionDeclAst final : public DeclAst
{
public:
    AST_CLASS(Version, Decl)

    VersionDeclAst()
        : DeclAst(Kind::VersionDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    SourceLoc delimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc terminLoc_;
};

/*!
 * \brief The DebugDeclAst class
 */
class UAISO_API DebugDeclAst final : public DeclAst
{
public:
    AST_CLASS(Debug, Decl)

    DebugDeclAst()
        : DeclAst(Kind::DebugDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(Delim, delim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    SourceLoc delimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc terminLoc_;
};

class UAISO_API InvariantDeclAst final : public DeclAst
{
public:
    AST_CLASS(Invariant, Decl)

    InvariantDeclAst()
        : DeclAst(Kind::InvariantDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

class UAISO_API UnitTestDeclAst final : public DeclAst
{
public:
    AST_CLASS(UnitTest, Decl)

    UnitTestDeclAst()
        : DeclAst(Kind::UnitTestDecl)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Stmt, stmt, StmtAst)

    SourceLoc keyLoc_;
    std::unique_ptr<StmtAst> stmt_;
};

/*!
 * \brief The SelectiveDeclAst class
 *
 * For conditional compilation and the like.
 */
class UAISO_API SelectiveDeclAst final : public DeclAst
{
public:
    AST_CLASS(Selective, Decl)
    VARIETY_AST(SelectiveVariety)

    SelectiveDeclAst()
        : DeclAst(Kind::SelectiveDecl)
    {
        INIT_VARIETY(SelectiveVariety::Unknown);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_PARAM(Expr, expr, ExprAst)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_AST_PARAM(IfDecl, ifDecl, DeclAst)
    NAMED_LOC_PARAM(OtherKey, otherKey)
    NAMED_AST_PARAM(ElseDecl, elseDecl, DeclAst)

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
    std::unique_ptr<DeclAst> ifDecl_;
    SourceLoc otherKeyLoc_;
    std::unique_ptr<DeclAst> elseDecl_;
};

class UAISO_API EnumDeclAst final : public DeclAst
{
public:
    AST_CLASS(Enum, Decl)

    EnumDeclAst()
        : DeclAst(Kind::EnumDecl)
        , sym_(nullptr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Sep, sep)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(LDelim, lDelim)
    NAMED_AST_LIST_PARAM(Decl, decls, DeclAst)
    NAMED_LOC_PARAM(RDelim, rDelim)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc sepLoc_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<DeclAstList> decls_;
    SourceLoc rDelimLoc_;
    SourceLoc terminLoc_;

    Enum* sym_;
};

class UAISO_API EnumMemberDeclAst final : public DeclAst
{
public:
    AST_CLASS(EnumMember, Decl)

    EnumMemberDeclAst()
        : DeclAst(Kind::EnumMemberDecl)
        , sym_(nullptr)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Assign, assign)
    NAMED_AST_PARAM(Init, init, ExprAst)

    bool hasInit() const { return init_.get(); }

    std::unique_ptr<NameAst> name_;
    SourceLoc assignLoc_;
    std::unique_ptr<ExprAst> init_;

    EnumItem* sym_;
};

class UAISO_API RecordDeclAst final : public DeclAst
{
public:
    AST_CLASS(Record, Decl)

    RecordDeclAst()
        : DeclAst(Kind::RecordDecl)
        , sym_(nullptr)
    {}

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_AST_PARAM(Spec, spec, SpecAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    std::unique_ptr<SpecAst> spec_;
    SourceLoc terminLoc_;

    Record* sym_;
};

/*!
 * \brief The BaseDeclAst class
 *
 * A base class or interface declaration.
 */
class UAISO_API BaseDeclAst final : public DeclAst
{
public:
    AST_CLASS(Base, Decl)

    BaseDeclAst()
        : DeclAst(Kind::BaseDecl)
    {}

    NAMED_AST_PARAM(Name, name, NameAst)

    std::unique_ptr<NameAst> name_;
};

class UAISO_API ForwardDeclAst final : public DeclAst
{
public:
    AST_CLASS(Forward, Decl)
    VARIETY_AST(RecordVariety)

    ForwardDeclAst()
        : DeclAst(Kind::ForwardDecl)
    {
        INIT_VARIETY(RecordVariety::Struct);
    }

    NAMED_LOC_PARAM(Key, key)
    NAMED_AST_PARAM(Name, name, NameAst)
    NAMED_LOC_PARAM(Termin, termin)

    SourceLoc keyLoc_;
    std::unique_ptr<NameAst> name_;
    SourceLoc terminLoc_;
};

/*!
 * \brief The PatBindDeclAst class
 *
 * A pattern binding.
 */
class UAISO_API PatBindDeclAst final : public DeclAst
{
public:
    AST_CLASS(PatBind, Decl)

    PatBindDeclAst()
        : DeclAst(Kind::PatBindDecl)
    {}

    NAMED_AST_PARAM(Pat, pat, ExprAst)
    NAMED_LOC_PARAM(Eq, eq)
    NAMED_AST_PARAM(Bind, bind, ExprAst)

    std::unique_ptr<ExprAst> pat_;
    SourceLoc eqLoc_;
    std::unique_ptr<ExprAst> bind_;
};

} // namespace uaiso

#endif
