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

#include "Semantic/Type.h"
#include "Semantic/Environment.h"
#include "Semantic/Precision.h"
#include "Semantic/Signedness.h"
#include "Semantic/Symbol.h"
#include "Semantic/TypeCast.h"
#include "Semantic/TypeQuals.h"
#include "Common/Assert.h"
#include <algorithm>
#include <utility>
#include <vector>

using namespace uaiso;

struct uaiso::Type::TypeImpl
{
    virtual ~TypeImpl()
    {}

    TypeImpl(Kind kind)
        : bits_(0)
    {
        bit_.kind_ = static_cast<char>(kind);
    }

    struct BitFields
    {
        uint32_t kind_ : 5;
        uint32_t typeQuals_ : 8;
        uint32_t variety_ : 4;
        uint32_t signedness_ : 4;
        uint32_t precision_ : 4;
    };
    union
    {
        BitFields bit_;
        uint32_t bits_;
    };
};

Type::Type(Kind kind)
    : impl_(new TypeImpl(kind))
{}

Type::Type(TypeImpl* impl)
    : impl_(impl)
{}

Type::~Type()
{}

Type::Kind Type::kind() const
{
    return Kind(impl_->bit_.kind_);
}

void Type::setTypeQuals(TypeQualFlags flags)
{
    impl_->bit_.typeQuals_ = flags;
}

TypeQualFlags Type::typeQuals() const
{
    return TypeQualFlags(impl_->bit_.typeQuals_);
}

template <class TypeT, class... ArgT>
TypeT* Type::trivialClone(ArgT&&... args) const
{
    auto ty = new TypeT(std::forward<ArgT>(args)...);
    ty->impl_->bits_ = impl_->bits_;
    return ty;
}

    //--- VoidType ---//

VoidType* VoidType::clone() const
{
    return trivialClone<VoidType>();
}

    //--- BoolType ---//

BoolType::BoolType()
    : Type(Kind::Bool)
{}

BoolType* BoolType::clone() const
{
    return trivialClone<BoolType>();
}

    //--- InferredType ---//

InferredType *InferredType::clone() const
{
    return trivialClone<InferredType>();
}

    //--- IntType ---//

IntType::IntType()
    : Type(Kind::Int)
{
    impl_->bit_.signedness_ = static_cast<char>(Signedness::Signed);
    impl_->bit_.precision_ = static_cast<char>(Precision::BitSize32);
}

IntType::IntType(Signedness s, Precision p)
    : Type(Kind::Int)
{
    impl_->bit_.signedness_ = static_cast<char>(s);
    impl_->bit_.precision_ = static_cast<char>(p);
}

IntType* IntType::clone() const
{
    return trivialClone<IntType>(signedness(), precision());
}

void IntType::setSignedness(Signedness s)
{
    impl_->bit_.signedness_ = static_cast<char>(s);
}

Signedness IntType::signedness() const
{
    return Signedness(impl_->bit_.signedness_);
}

void IntType::setPrecision(Precision p)
{
    impl_->bit_.precision_ = static_cast<char>(p);
}

Precision IntType::precision() const
{
    return Precision(impl_->bit_.precision_);
}

    //--- FloatType ---//

FloatType::FloatType()
    : Type(Kind::Float)
{
    impl_->bit_.precision_ = static_cast<char>(Precision::BitSize64);
}

FloatType::FloatType(Precision p)
    : Type(Kind::Float)
{
    impl_->bit_.precision_ = static_cast<char>(p);
}

FloatType* FloatType::clone() const
{
    return trivialClone<FloatType>(precision());
}

void FloatType::setPrecision(Precision p)
{
    impl_->bit_.precision_ = static_cast<char>(p);
}

Precision FloatType::precision() const
{
    return Precision(impl_->bit_.precision_);
}

    //--- StrType ---//

StrType::StrType()
    : Type(Kind::Str)
{}

StrType* StrType::clone() const
{
    return trivialClone<StrType>();
}

    //--- ElaborateType ---//

struct uaiso::ElaborateType::ElaborateTypeImpl : Type::TypeImpl
{
    ElaborateTypeImpl(const Ident* name)
        : TypeImpl(Kind::Elaborate)
        , name_(name)
    {}

    const Ident* name_;
    std::unique_ptr<Type> canonical_;
};

DEF_PIMPL_CAST(ElaborateType)

ElaborateType* ElaborateType::clone() const
{
    return trivialClone<ElaborateType>(name());
}

ElaborateType::ElaborateType(const Ident *name)
    : Type(new ElaborateTypeImpl(name))
{}

ElaborateType::~ElaborateType()
{}

const Ident* ElaborateType::name() const
{
    return P_CAST->name_;
}

const Type* ElaborateType::canonicalType() const
{
    return P_CAST->canonical_.get();
}

bool ElaborateType::isResolved() const
{
    return P_CAST->canonical_.get();
}

void ElaborateType::resolveType(std::unique_ptr<Type> ty)
{
    P_CAST->canonical_ = std::move(ty);
}

    //--- EnumType ---//

struct uaiso::EnumType::EnumTypeImpl : Type::TypeImpl
{
    using TypeImpl::TypeImpl;

    Environment env_;
};

DEF_PIMPL_CAST(EnumType)

EnumType::EnumType()
    : Type(new EnumTypeImpl(Type::Kind::Enum))
{}

EnumType::~EnumType()
{}

EnumType* EnumType::clone() const
{
    auto ty = trivialClone<EnumType>();
    ty->P_CAST->env_ = P_CAST->env_;
    return ty;
}

void EnumType::setEnv(Environment env)
{
    P_CAST->env_ = env;
}

Environment EnumType::env() const
{
    return P_CAST->env_;
}

    //--- RecordType ---//

struct uaiso::RecordType::RecordTypeImpl : Type::TypeImpl
{
    using TypeImpl::TypeImpl;

    Environment env_;
    std::vector<std::unique_ptr<BaseRecord>> bases_;
};

DEF_PIMPL_CAST(RecordType)

RecordType* RecordType::clone() const
{
    auto ty = trivialClone<RecordType>();
    ty->P_CAST->env_ = P_CAST->env_;
    for (auto& base : P_CAST->bases_)
        ty->P_CAST->bases_.emplace_back(new BaseRecord(base->name()));
    return ty;
}

RecordType::RecordType()
    : Type(new RecordTypeImpl(Kind::Record))
{}

RecordType::~RecordType()
{}

void RecordType::addBase(std::unique_ptr<BaseRecord> base)
{
    P_CAST->bases_.push_back(std::move(base));
}

std::vector<const BaseRecord*> RecordType::bases() const
{
    std::vector<const BaseRecord*> v;
    std::for_each(P_CAST->bases_.begin(), P_CAST->bases_.end(),
                  [&v](const std::unique_ptr<BaseRecord>& base) {
        v.push_back(base.get());
    });
    return v;
}

void RecordType::setVariety(RecordVariety variety)
{
    impl_->bit_.variety_ = static_cast<char>(variety);
}

RecordVariety RecordType::variety() const
{
    return RecordVariety(impl_->bit_.variety_);
}

void RecordType::setEnv(Environment env)
{
    P_CAST->env_ = env;
}

Environment RecordType::env() const
{
    return P_CAST->env_;
}

    //--- OpaqueType ---//

struct OpaqueType::OpaqueTypeImpl : Type::TypeImpl
{
    OpaqueTypeImpl(std::unique_ptr<Type> baseType, Type::Kind kind)
        : TypeImpl(kind)
        , baseType_(std::move(baseType))
    {}

    std::unique_ptr<Type> baseType_;
};

DEF_PIMPL_CAST(OpaqueType)

OpaqueType::OpaqueType(std::unique_ptr<Type> baseType, Type::Kind kind)
    : Type(new OpaqueTypeImpl(std::move(baseType), kind))
{}

OpaqueType::OpaqueType(OpaqueTypeImpl* impl)
    : Type(impl)
{}

const Type *OpaqueType::baseType() const
{
    return P_CAST->baseType_.get();
}

    //--- PtrType ---//

PtrType::PtrType(std::unique_ptr<Type> baseType)
    : OpaqueType(std::move(baseType), Type::Kind::Ptr)
{}

PtrType* PtrType::clone() const
{
    return trivialClone<PtrType>(std::unique_ptr<Type>(P_CAST->baseType_->clone()));
}

    //--- SubrangeType ---//

SubrangeType::SubrangeType(std::unique_ptr<Type> baseType)
    : OpaqueType(std::move(baseType), Type::Kind::Subrange)
{}

SubrangeType* SubrangeType::clone() const
{
    return trivialClone<SubrangeType>(std::unique_ptr<Type>(P_CAST->baseType_->clone()));
}

    //--- ChanType ---//

ChanType::ChanType(std::unique_ptr<Type> baseType)
    : OpaqueType(std::move(baseType), Type::Kind::Chan)
{}

ChanType* ChanType::clone() const
{
    return trivialClone<ChanType>(std::unique_ptr<Type>(P_CAST->baseType_->clone()));
}

void ChanType::setVariety(ChanVariety variety)
{
    impl_->bit_.variety_ = static_cast<char>(variety);
}

ChanVariety ChanType::variety() const
{
    return ChanVariety(impl_->bit_.variety_);
}

    //--- ArrayType ---//

struct uaiso::ArrayType::ArrayTypeImpl : OpaqueType::OpaqueTypeImpl
{
    using OpaqueTypeImpl::OpaqueTypeImpl;

    std::unique_ptr<Type> keyType_;
};

DEF_PIMPL_CAST(ArrayType)

ArrayType::ArrayType(std::unique_ptr<Type> baseType)
    : OpaqueType(new ArrayTypeImpl(std::move(baseType), Type::Kind::Array))
{}

ArrayType::~ArrayType()
{}

void ArrayType::setVariety(ArrayVariety variety)
{
    impl_->bit_.variety_ = static_cast<char>(variety);
}

ArrayVariety ArrayType::variety() const
{
    return ArrayVariety(impl_->bit_.variety_);
}

void ArrayType::setKeyType(std::unique_ptr<Type> ty)
{
    P_CAST->keyType_ = std::move(ty);
}

const Type* ArrayType::keyType() const
{
    return P_CAST->keyType_.get();
}

ArrayType* ArrayType::clone() const
{
    auto ty = trivialClone<ArrayType>(std::unique_ptr<Type>(P_CAST->baseType_->clone()));
    if (P_CAST->keyType_)
        ty->P_CAST->keyType_.reset(P_CAST->keyType_->clone());
    return ty;
}

    //--- FuncType ---//

struct uaiso::FuncType::FuncTypeImpl : Type::TypeImpl
{
    using TypeImpl::TypeImpl;

    std::unique_ptr<Type> returnType_;
    std::unique_ptr<Type> paramType_;
};

DEF_PIMPL_CAST(FuncType)

FuncType* FuncType::clone() const
{
    auto ty = trivialClone<FuncType>();
    if (P_CAST->returnType_)
        ty->P_CAST->returnType_.reset(P_CAST->returnType_->clone());
    if (P_CAST->paramType_)
        ty->P_CAST->paramType_.reset(P_CAST->paramType_->clone());
    return ty;
}

FuncType::FuncType()
    : Type(new FuncTypeImpl(Kind::Func))
{}

FuncType::~FuncType()
{}

void FuncType::setReturnType(std::unique_ptr<Type> type)
{
    P_CAST->returnType_ = std::move(type);
}

const Type* FuncType::returnType() const
{
    return P_CAST->returnType_.get();
}

namespace uaiso {

bool isNumType(Type::Kind kind)
{
    return kind == Type::Kind::Int
            || kind == Type::Kind::Float;
}

std::pair<bool, Environment> envForType(const Type* ty, Environment env)
{
    switch (ty->kind()) {
    case Type::Kind::Record:
        return std::make_pair(true, ConstRecordType_Cast(ty)->env());
    case Type::Kind::Enum:
        return std::make_pair(true, ConstEnumType_Cast(ty)->env());
    default:
        return std::make_pair(false, env);
    }
}

} // namespace uaiso
