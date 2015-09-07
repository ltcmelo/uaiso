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
        uint64_t direction_ : 4;
        uint64_t evaluation_ : 4;
        uint64_t kind_ : 5;
        uint64_t linkage_ : 4;
        uint64_t storage_ : 4;
        uint64_t visibility_ : 4;
        uint64_t auto_ : 1;
        uint64_t declAttrs_ : 10;
    };
    union
    {
        BitFields bit_;
        uint64_t bits_;
    };
    SourceLoc sourceLoc_;
};

Symbol::Symbol(Kind kind)
    : impl_(new SymbolImpl(kind))
{}

Symbol::Symbol(Symbol::SymbolImpl* impl)
    : impl_(impl)
{}

Symbol::~Symbol()
{}

Symbol::Kind Symbol::kind() const
{
    return Symbol::Kind(impl_->bit_.kind_);
}

void Symbol::setSourceLoc(const SourceLoc& loc)
{
    impl_->sourceLoc_ = loc;
}

const SourceLoc& Symbol::sourceLoc() const
{
    return impl_->sourceLoc_;
}

    //--- DeclSymbol ---//

struct uaiso::DeclSymbol::DeclSymbolImpl : Symbol::SymbolImpl
{
    DeclSymbolImpl(const Ident* name, Symbol::Kind kind)
        : SymbolImpl(kind)
        , name_(name)
    {}

    const Ident* name_;
};

DEF_PIMPL_CAST(DeclSymbol)

DeclSymbol::DeclSymbol(DeclSymbol::DeclSymbolImpl* impl)
    : Symbol(impl)
{}

void DeclSymbol::setVisibility(Visibility visibility)
{
    impl_->bit_.visibility_ = static_cast<char>(visibility);
}

DeclSymbol::Visibility DeclSymbol::visibility() const
{
    return Visibility(impl_->bit_.visibility_);
}

void DeclSymbol::setStorage(DeclSymbol::Storage store)
{
    impl_->bit_.storage_ = static_cast<char>(store);
}

DeclSymbol::Storage DeclSymbol::storage() const
{
    return DeclSymbol::Storage(impl_->bit_.storage_);
}

void DeclSymbol::setLinkage(DeclSymbol::Linkage link)
{
    impl_->bit_.linkage_ = static_cast<char>(link);
}

DeclSymbol::Linkage DeclSymbol::linkage() const
{
    return DeclSymbol::Linkage(impl_->bit_.linkage_);
}

const Ident* DeclSymbol::name() const
{
    return P_CAST->name_;
}

bool DeclSymbol::isAnonymous() const
{
    return P_CAST->name_ == &kNullIdent;
}

void DeclSymbol::markAsAuto()
{
    impl_->bit_.auto_ = true;
}

bool DeclSymbol::isMarkedAuto() const
{
    return impl_->bit_.auto_;
}

void DeclSymbol::setDeclAttrs(DeclAttrFlags flags)
{
    impl_->bit_.declAttrs_ = flags;
}

DeclAttrFlags DeclSymbol::declAttrs() const
{
    return DeclAttrFlags(impl_->bit_.declAttrs_);
}

    //--- TypeSymbol ---//

struct uaiso::TypeSymbol::TypeSymbolImpl : DeclSymbol::DeclSymbolImpl
{
    using DeclSymbolImpl::DeclSymbolImpl;

    std::unique_ptr<Type> ty_;
};

DEF_PIMPL_CAST(TypeSymbol)

void TypeSymbol::setType(std::unique_ptr<Type> ty)
{
    P_CAST->ty_ = std::move(ty);
}

const Type* TypeSymbol::type() const
{
    return P_CAST->ty_.get();
}

    //--- ValueSymbol ---//

struct uaiso::ValueSymbol::ValueSymbolImpl : DeclSymbol::DeclSymbolImpl
{
    using DeclSymbolImpl::DeclSymbolImpl;

    std::shared_ptr<Type> valueType_;
};

DEF_PIMPL_CAST(ValueSymbol)

void ValueSymbol::setValueType(std::unique_ptr<Type> ty)
{
    P_CAST->valueType_ = std::move(ty);
}

const Type* ValueSymbol::valueType() const
{
    return P_CAST->valueType_.get();
}

    //--- Alias ---//

Alias::Alias(const Ident *name)
    : TypeSymbol(new TypeSymbolImpl(name, Kind::Alias))
{}

    //--- Placeholder ---//

struct uaiso::Placeholder::PlaceholderImpl : TypeSymbol::TypeSymbolImpl
{
    using TypeSymbolImpl::TypeSymbolImpl;

    TypeSymbol* actual_ { nullptr };
};

DEF_PIMPL_CAST(Placeholder)

Placeholder::Placeholder(const Ident *name)
    : TypeSymbol(new TypeSymbolImpl(name, Kind::Placeholder))
{}

void Placeholder::setActual(TypeSymbol* sym)
{
    P_CAST->actual_ = sym;
}

const TypeSymbol* Placeholder::actual() const
{
    return P_CAST->actual_;
}

    //--- Func ---//

struct uaiso::Func::FuncImpl : TypeSymbol::TypeSymbolImpl
{
    using TypeSymbolImpl::TypeSymbolImpl;

    Environment env_;
};

DEF_PIMPL_CAST(Func)

Func::Func(const Ident *name)
    : TypeSymbol(new FuncImpl(name, Kind::Func))
{}

Func::~Func()
{}

void Func::setType(std::unique_ptr<FuncType> ty)
{
    TypeSymbol::setType(std::unique_ptr<Type>(ty.release()));
}

const FuncType *Func::type() const
{
    return ConstFuncType_Cast(TypeSymbol::type());
}

void Func::setEnv(Environment env)
{
    P_CAST->env_ = env;
}

Environment Func::env() const
{
    return P_CAST->env_;
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

    //--- Record ---//

struct Record::RecordImpl : TypeSymbol::TypeSymbolImpl
{
    using TypeSymbolImpl::TypeSymbolImpl;

    std::vector<std::unique_ptr<BaseRecord>> bases_;
};

DEF_PIMPL_CAST(Record)

Record::Record(const Ident* name)
    : TypeSymbol(new RecordImpl(name, Kind::Record))
{}

Record::~Record()
{}

void Record::addBase(std::unique_ptr<BaseRecord> base)
{
    P_CAST->bases_.push_back(std::move(base));
}

void Record::setType(std::unique_ptr<RecordType> type)
{
    TypeSymbol::setType(std::unique_ptr<Type>(type.release()));
}

const RecordType *Record::type() const
{
    return ConstRecordType_Cast(TypeSymbol::type());
}

    //--- Enum ---//

struct Enum::EnumImpl : TypeSymbol::TypeSymbolImpl
{
    using TypeSymbolImpl::TypeSymbolImpl;

    std::unique_ptr<Type> baseType_;
};

DEF_PIMPL_CAST(Enum)

Enum::Enum(const Ident* name)
    : TypeSymbol(new EnumImpl(name, Kind::Enum))
{}

Enum::~Enum()
{}

void Enum::setType(std::unique_ptr<EnumType> ty)
{
    TypeSymbol::setType(std::unique_ptr<Type>(ty.release()));
}

const EnumType *Enum::type() const
{
    return ConstEnumType_Cast(TypeSymbol::type());
}

void Enum::setUnderlyingType(std::unique_ptr<Type> ty)
{
    P_CAST->baseType_ = std::move(ty);
}

const Type* Enum::underlyingType() const
{
    return P_CAST->baseType_.get();
}

    //--- BaseRecord ---//

BaseRecord::BaseRecord(const Ident* name)
    : DeclSymbol(new DeclSymbolImpl(name, Kind::BaseRecord))
{}

    //--- Param ---//

struct uaiso::Param::ParamImpl : ValueSymbol::ValueSymbolImpl
{
    using ValueSymbolImpl::ValueSymbolImpl;
};

DEF_PIMPL_CAST(Param)

Param::Param()
    : ValueSymbol(new ParamImpl(&kNullIdent, Kind::Param))
{}

Param::Param(const Ident* name)
    : ValueSymbol(new ParamImpl(name, Kind::Param))
{}

Param::~Param()
{}

void Param::setDirection(Param::Direction dir)
{
    impl_->bit_.direction_ = static_cast<char>(dir);
}

Param::Direction Param::direction() const
{
    return Param::Direction(impl_->bit_.direction_);
}

void Param::setEvalStrategy(Param::EvalStrategy eval)
{
    impl_->bit_.evaluation_ = static_cast<char>(eval);
}

Param::EvalStrategy Param::evalStrategy() const
{
    return Param::EvalStrategy(impl_->bit_.evaluation_);
}

    //--- Var ---//

struct uaiso::Var::VarImpl : ValueSymbol::ValueSymbolImpl
{
    using ValueSymbolImpl::ValueSymbolImpl;
};

DEF_PIMPL_CAST(Var)

Var::Var(const Ident* name)
    : ValueSymbol(new ValueSymbolImpl(name, Kind::Var))
{}

Var::~Var()
{}

    //--- EnumItem ---//

EnumItem::EnumItem(const Ident* name)
    : ValueSymbol(new ValueSymbolImpl(name, Kind::EnumItem))
{}

    //--- Utility functions ---//

namespace uaiso {

bool isDeclSymbol(const Symbol* symbol)
{
    switch (symbol->kind()) {
    case Symbol::Kind::Import:
        return false;
    default:
        return true;
    }
}

bool isTypeSymbol(const Symbol *symbol)
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

bool isValueSymbol(const Symbol *symbol)
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
