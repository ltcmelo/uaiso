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

//--------------------------//
//--- The UaiSo! Project ---//
//--------------------------//

#include "Semantic/Symbol.h"
#include "Semantic/DeclAttrs.h"
#include "Semantic/Environment.h"
#include "Semantic/SymbolCast.h"
#include "Semantic/Type.h"
#include "Semantic/TypeCast.h"
#include "Ast/AstVariety.h"
#include "Parsing/Lexeme.h"
#include <vector>

using namespace uaiso;

struct uaiso::Symbol::SymbolImpl
{
    virtual ~SymbolImpl()
    {}

    SymbolImpl(Kind kind)
        : bits_(0)
    {
        bit_.kind_ = static_cast<char>(kind);
    }

    struct BitFields
    {
        uint64_t direction_     : 4;
        uint64_t evaluation_    : 4;
        uint64_t kind_          : 5;
        uint64_t linkage_       : 4;
        uint64_t storage_       : 4;
        uint64_t visibility_    : 4;
        uint64_t auto_          : 1;
        uint64_t declAttrs_     : 10;
        uint64_t builtin_       : 1;
        uint64_t fake_          : 1;
    };
    union
    {
        BitFields bit_;
        uint64_t bits_;
    };
    SourceLoc sourceLoc_;
};

Symbol::Symbol(Kind kind)
    : P(new SymbolImpl(kind))
{}

Symbol::Symbol(Symbol::SymbolImpl* impl)
    : P(impl)
{}

Symbol::~Symbol()
{}

Symbol::Kind Symbol::kind() const
{
    return Symbol::Kind(P->bit_.kind_);
}

void Symbol::setSourceLoc(const SourceLoc& loc)
{
    P->sourceLoc_ = loc;
}

const SourceLoc& Symbol::sourceLoc() const
{
    return P->sourceLoc_;
}

void Symbol::setIsBuiltin(bool isBuiltin)
{
    P->bit_.builtin_ = isBuiltin;
}

bool Symbol::isBuiltin() const
{
    return P->bit_.builtin_;
}

void Symbol::setIsFake(bool isFake)
{
    P->bit_.fake_ = isFake;
}

bool Symbol::isFake() const
{
    return P->bit_.fake_;
}

template <class SymbolT, class... ArgT>
SymbolT* Symbol::trivialClone(ArgT&&... args) const
{
    auto sym = new SymbolT(std::forward<ArgT>(args)...);
    sym->P->bits_ = P->bits_;
    return sym;
}

    //--- Decl ---//

struct uaiso::Decl::DeclImpl : Symbol::SymbolImpl
{
    DeclImpl(const Ident* name, Symbol::Kind kind)
        : SymbolImpl(kind)
        , name_(name)
    {}

    const Ident* name_;
};

DEF_PIMPL_CAST(Decl)

Decl::Decl(Decl::DeclImpl* impl)
    : Symbol(impl)
{}

void Decl::setVisibility(Visibility visibility)
{
    P->bit_.visibility_ = static_cast<char>(visibility);
}

Decl::Visibility Decl::visibility() const
{
    return Visibility(P->bit_.visibility_);
}

void Decl::setStorage(Decl::Storage store)
{
    P->bit_.storage_ = static_cast<char>(store);
}

Decl::Storage Decl::storage() const
{
    return Decl::Storage(P->bit_.storage_);
}

void Decl::setLinkage(Decl::Linkage link)
{
    P->bit_.linkage_ = static_cast<char>(link);
}

Decl::Linkage Decl::linkage() const
{
    return Decl::Linkage(P->bit_.linkage_);
}

const Ident* Decl::name() const
{
    return P_CAST->name_;
}

bool Decl::isAnonymous() const
{
    return P_CAST->name_ == &kNullIdent;
}

void Decl::markAsAuto()
{
    P->bit_.auto_ = true;
}

bool Decl::isMarkedAuto() const
{
    return P->bit_.auto_;
}

void Decl::setDeclAttrs(DeclAttrFlags flags)
{
    P->bit_.declAttrs_ = flags;
}

DeclAttrFlags Decl::declAttrs() const
{
    return DeclAttrFlags(P->bit_.declAttrs_);
}

    //--- TypeDecl ---//

struct uaiso::TypeDecl::TypeDeclImpl : Decl::DeclImpl
{
    using DeclImpl::DeclImpl;

    std::unique_ptr<Type> ty_;
};

DEF_PIMPL_CAST(TypeDecl)

void TypeDecl::setType(std::unique_ptr<Type> ty)
{
    P_CAST->ty_ = std::move(ty);
}

const Type* TypeDecl::type() const
{
    return P_CAST->ty_.get();
}

    //--- ValueDecl ---//

struct uaiso::ValueDecl::ValueDeclImpl : Decl::DeclImpl
{
    using DeclImpl::DeclImpl;

    std::shared_ptr<Type> valueTy_;
};

DEF_PIMPL_CAST(ValueDecl)

void ValueDecl::setValueType(std::unique_ptr<Type> ty)
{
    P_CAST->valueTy_ = std::move(ty);
}

const Type* ValueDecl::valueType() const
{
    return P_CAST->valueTy_.get();
}

    //--- Alias ---//

Alias::Alias(const Ident *name)
    : TypeDecl(new TypeDeclImpl(name, Kind::Alias))
{}

Alias* Alias::clone() const
{
    return trivialClone<Alias>(P_CAST->name_);
}

    //--- Placeholder ---//

struct uaiso::Placeholder::PlaceholderImpl : TypeDecl::TypeDeclImpl
{
    using TypeDeclImpl::TypeDeclImpl;

    TypeDecl* actual_ { nullptr };
};

DEF_PIMPL_CAST(Placeholder)

Placeholder::Placeholder(const Ident *name)
    : TypeDecl(new TypeDeclImpl(name, Kind::Placeholder))
{}

Placeholder *Placeholder::clone() const
{
    auto holder = trivialClone<Placeholder>(P_CAST->name_);
    holder->P_CAST->ty_ = std::unique_ptr<Type>(P_CAST->ty_->clone());
    return holder;
}

    //--- Func ---//

struct uaiso::Func::FuncImpl : TypeDecl::TypeDeclImpl
{
    using TypeDeclImpl::TypeDeclImpl;

    Environment env_;
};

DEF_PIMPL_CAST(Func)

Func::Func(const Ident *name)
    : TypeDecl(new FuncImpl(name, Kind::Func))
{}

Func::~Func()
{}

void Func::setType(std::unique_ptr<FuncType> ty)
{
    TypeDecl::setType(std::unique_ptr<Type>(ty.release()));
}

const FuncType *Func::type() const
{
    return ConstFuncType_Cast(TypeDecl::type());
}

void Func::setEnv(Environment env)
{
    P_CAST->env_ = env;
}

Environment Func::env() const
{
    return P_CAST->env_;
}

Func* Func::clone() const
{
    auto func = trivialClone<Func>(P_CAST->name_);
    func->P_CAST->ty_ = std::unique_ptr<Type>(P_CAST->ty_->clone());
    func->P_CAST->env_ = P_CAST->env_;
    return func;
}

    //--- Import ---//

struct Import::ImportImpl : Symbol::SymbolImpl
{
    ImportImpl(const std::string& originDir,
               const std::string& moduleName,
               const Ident* name,
               bool mergeEnv)
        : SymbolImpl(Symbol::Kind::Import)
        , originDir_(originDir)
        , moduleName_(moduleName)
        , localName_(name)
        , mergeEnv_(mergeEnv)
    {}

    const std::string originDir_;
    const std::string moduleName_;
    const Ident* localName_;
    bool mergeEnv_;
};

DEF_PIMPL_CAST(Import)

Import::Import(const std::string& originDir,
               const std::string& moduleName,
               const Ident* localName,
               bool mergeEnv)
    : Symbol(new ImportImpl(originDir, moduleName, localName, mergeEnv))
{}

const std::string& Import::moduleName() const
{
    return P_CAST->moduleName_;
}

const std::string& Import::originDir() const
{
    return P_CAST->originDir_;
}

const Ident* Import::localName() const
{
    return P_CAST->localName_;
}

bool Import::mergeEnv() const
{
    return P_CAST->mergeEnv_;
}

Import* Import::clone() const
{
    return trivialClone<Import>(P_CAST->originDir_, P_CAST->moduleName_,
                                P_CAST->localName_, P_CAST->mergeEnv_);
}

    //--- Namespace ---//

struct Namespace::NamespaceImpl : Symbol::SymbolImpl
{
    NamespaceImpl(const Ident* name)
        : SymbolImpl(Symbol::Kind::Namespace)
        , name_(name)
    {}

    const Ident* name_;
    Environment env_;
};

DEF_PIMPL_CAST(Namespace)

Namespace::Namespace()
    : Symbol(new NamespaceImpl(nullptr))
{}

Namespace::Namespace(const Ident* name)
    : Symbol(new NamespaceImpl(name))
{}

const Ident* Namespace::name() const
{
    return P_CAST->name_;
}

bool Namespace::isAnonymous() const
{
    return !P_CAST->name_;
}

void Namespace::setEnv(Environment env)
{
    P_CAST->env_ = env;
}

Environment Namespace::env() const
{
    return P_CAST->env_;
}

Namespace* Namespace::clone() const
{
    auto space = trivialClone<Namespace>(P_CAST->name_);
    space->P_CAST->env_ = P_CAST->env_;
    return space;
}

    //--- Record ---//

struct Record::RecordImpl : TypeDecl::TypeDeclImpl
{
    using TypeDeclImpl::TypeDeclImpl;
};

DEF_PIMPL_CAST(Record)

Record::Record(const Ident* name)
    : TypeDecl(new RecordImpl(name, Kind::Record))
{}

Record::~Record()
{}

void Record::setType(std::unique_ptr<RecordType> type)
{
    TypeDecl::setType(std::unique_ptr<Type>(type.release()));
}

const RecordType *Record::type() const
{
    return ConstRecordType_Cast(TypeDecl::type());
}

Record* Record::clone() const
{
    auto rec = trivialClone<Record>(P_CAST->name_);
    rec->P_CAST->ty_ = std::unique_ptr<Type>(P_CAST->ty_->clone());
    return rec;
}

    //--- Enum ---//

struct Enum::EnumImpl : TypeDecl::TypeDeclImpl
{
    using TypeDeclImpl::TypeDeclImpl;

    std::unique_ptr<Type> underTy_;
};

DEF_PIMPL_CAST(Enum)

Enum::Enum(const Ident* name)
    : TypeDecl(new EnumImpl(name, Kind::Enum))
{}

Enum::~Enum()
{}

void Enum::setType(std::unique_ptr<EnumType> ty)
{
    TypeDecl::setType(std::unique_ptr<Type>(ty.release()));
}

const EnumType *Enum::type() const
{
    return ConstEnumType_Cast(TypeDecl::type());
}

Enum* Enum::clone() const
{
    auto enun = trivialClone<Enum>(P_CAST->name_);
    enun->P_CAST->ty_ = std::unique_ptr<Type>(P_CAST->ty_->clone());
    enun->P_CAST->underTy_ = std::unique_ptr<Type>(P_CAST->underTy_->clone());
    return enun;
}

void Enum::setUnderlyingType(std::unique_ptr<Type> ty)
{
    P_CAST->underTy_ = std::move(ty);
}

const Type* Enum::underlyingType() const
{
    return P_CAST->underTy_.get();
}

    //--- BaseRecord ---//

BaseRecord::BaseRecord(const Ident* name)
    : Decl(new DeclImpl(name, Kind::BaseRecord))
{}

BaseRecord *BaseRecord::clone() const
{
    return trivialClone<BaseRecord>(P_CAST->name_);
}

    //--- Param ---//

struct uaiso::Param::ParamImpl : ValueDecl::ValueDeclImpl
{
    using ValueDeclImpl::ValueDeclImpl;
};

DEF_PIMPL_CAST(Param)

Param::Param()
    : ValueDecl(new ParamImpl(&kNullIdent, Kind::Param))
{}

Param::Param(const Ident* name)
    : ValueDecl(new ParamImpl(name, Kind::Param))
{}

Param::~Param()
{}

void Param::setDirection(Param::Direction dir)
{
    P->bit_.direction_ = static_cast<char>(dir);
}

Param::Direction Param::direction() const
{
    return Param::Direction(P->bit_.direction_);
}

void Param::setEvalStrategy(Param::EvalStrategy eval)
{
    P->bit_.evaluation_ = static_cast<char>(eval);
}

Param::EvalStrategy Param::evalStrategy() const
{
    return Param::EvalStrategy(P->bit_.evaluation_);
}

Param* Param::clone() const
{
    auto param = trivialClone<Param>(P_CAST->name_);
    param->P_CAST->valueTy_ = std::unique_ptr<Type>(P_CAST->valueTy_->clone());
    return param;
}

    //--- Var ---//

struct uaiso::Var::VarImpl : ValueDecl::ValueDeclImpl
{
    using ValueDeclImpl::ValueDeclImpl;
};

DEF_PIMPL_CAST(Var)

Var::Var(const Ident* name)
    : ValueDecl(new ValueDeclImpl(name, Kind::Var))
{}

Var::~Var()
{}

Var* Var::clone() const
{
    auto var = trivialClone<Var>(P_CAST->name_);
    var->P_CAST->valueTy_ = std::unique_ptr<Type>(P_CAST->valueTy_->clone());
    return var;
}

    //--- EnumItem ---//

EnumItem::EnumItem(const Ident* name)
    : ValueDecl(new ValueDeclImpl(name, Kind::EnumItem))
{}

EnumItem* EnumItem::clone() const
{
    auto item = trivialClone<EnumItem>(P_CAST->name_);
    item->P_CAST->valueTy_ = std::unique_ptr<Type>(P_CAST->valueTy_->clone());
    return item;
}

    //--- Utility functions ---//

namespace uaiso {

bool isDecl(const Symbol* symbol)
{
    switch (symbol->kind()) {
    case Symbol::Kind::Import:
        return false;
    default:
        return true;
    }
}

bool isTypeDecl(const Symbol *symbol)
{
    switch (symbol->kind()) {
    case Symbol::Kind::Alias:
    case Symbol::Kind::Func:
    case Symbol::Kind::Record:
        return true;
    default:
        return false;
    }
}

bool isValueDecl(const Symbol *symbol)
{
    switch (symbol->kind()) {
    case Symbol::Kind::Param:
    case Symbol::Kind::Var:
        return true;
    default:
        return false;
    }
}

} // namespace uaiso
