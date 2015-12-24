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

#ifndef UAISO_TYPE_H__
#define UAISO_TYPE_H__

#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Ast/AstVariety.h"
#include "Semantic/TypeCast.h"
#include "Semantic/TypeQuals.h"

namespace uaiso {

class BaseRecord;
class Environment;
class Ident;
enum class Precision : char;
enum class Signedness : char;

/*!
 * \brief The Type class
 */
class UAISO_API Type
{
public:
    virtual ~Type();

    /*!
     * \brief The Kind enum
     */
    enum class Kind : char
    {
        Array,
        Bool,
        Chan,
        Elaborate,
        Enum,
        Float,
        Func,
        Inferred,
        Int,
        Ptr,
        Record,
        Str,
        Subrange,
        Void
    };

    Kind kind() const;

    void setTypeQuals(TypeQualFlags flags);
    TypeQualFlags typeQuals() const;

    virtual Type* clone() const = 0;

protected:
    Type(Kind kind);

    template <class TypeT, class... ArgT>
    TypeT* trivialClone(ArgT&&...) const;

    DECL_PIMPL(Type)
    DECL_PIMPL_BASE(Type)
};

/*!
 * \brief The VoidType class
 */
class UAISO_API VoidType final : public Type
{
public:
    VoidType() : Type(Kind::Void)
    {}

    VoidType* clone() const override;
};

/*!
 * \brief The BoolType class
 */
class UAISO_API BoolType final : public Type
{
public:
    BoolType();

    BoolType* clone() const override;
};

/*!
 * \brief The FloatType class
 */
class UAISO_API FloatType final : public Type
{
public:
    FloatType();
    FloatType(Precision);

    void setPrecision(Precision);
    Precision precision() const;

    FloatType* clone() const override;

private:
    DECL_PIMPL_CAST(FloatType)
};

/*!
 * \brief The IntType class
 */
class UAISO_API IntType final : public Type
{
public:
    IntType();
    IntType(Signedness s, Precision p);

    void setSignedness(Signedness);
    Signedness signedness() const;

    void setPrecision(Precision);
    Precision precision() const;

    IntType* clone() const override;
};

/*!
 * \brief The StrType class
 *
 * An string builtin, it might be a type on its own or an aliased type.
 */
class UAISO_API StrType final : public Type
{
public:
    StrType();

    StrType* clone() const override;
};

/*!
 * \brief The InferredType class
 *
 * Indicates a type to be inferred from an expression.
 */
class UAISO_API InferredType final : public Type
{
public:
    InferredType() : Type(Type::Kind::Inferred)
    {}

    InferredType* clone() const override;
};

/*!
 * \brief The ElaborateType class
 */
class UAISO_API ElaborateType final : public Type
{
public:
    ElaborateType(const Ident* name);
    ~ElaborateType();

    const Ident* name() const;

    ElaborateType* clone() const override;

    bool isResolved() const;

    const Type* canonicalType() const;

private:
    DECL_PIMPL_CAST(ElaborateType)
    friend class TypeChecker;
    void setCanonical(std::unique_ptr<Type> type);
};

/*!
 * \brief The EnumType class
 *
 * The type of an enumeration contains an environment of named elements,
 * whose values are typically ordered.
 */
class UAISO_API EnumType final : public Type
{
public:
    EnumType();
    ~EnumType();

    void setEnv(Environment env);
    Environment env() const;

    EnumType* clone() const override;

private:
    DECL_PIMPL_CAST(EnumType)
};

class UAISO_API OpaqueType : public Type
{
public:
    /*!
     * \brief baseType
     * \return
     */
    const Type* baseType() const;

protected:
    OpaqueType(std::unique_ptr<Type> baseType, Kind kind);

    DECL_PIMPL_CAST(OpaqueType)
    DECL_PIMPL_BASE(OpaqueType)
};

/*!
 * \brief The ArrayType class
 */
class UAISO_API ArrayType final : public OpaqueType
{
public:
    ArrayType(std::unique_ptr<Type> baseType);
    ~ArrayType();

    void setVariety(ArrayVariety variety);
    ArrayVariety variety() const;

    void setKeyType(std::unique_ptr<Type> type);
    const Type* keyType() const;

    ArrayType* clone() const override;

private:
    DECL_PIMPL_CAST(ArrayType)
};

/*!
 * \brief The PtrType class
 */
class UAISO_API PtrType final : public OpaqueType
{
public:
    PtrType(std::unique_ptr<Type> baseType);

    PtrType* clone() const override;
};

/*!
 * \brief The SubrangeType class
 *
 * A subrange type is similar to an enumeration type, but it simply denotes
 * a subset of a base type. In fact, one might have a subrange type of an
 * enumeration type.
 */
class UAISO_API SubrangeType final : public OpaqueType
{
public:
    SubrangeType(std::unique_ptr<Type> baseType);

    SubrangeType* clone() const override;
};

/*!
 * \brief The ChanType class
 *
 * A channel type, as in Go.
 */
class UAISO_API ChanType final : public OpaqueType
{
public:
    ChanType(std::unique_ptr<Type> baseType);

    void setVariety(ChanVariety variety);
    ChanVariety variety() const;

    ChanType* clone() const override;
};

class UAISO_API RecordType final : public Type
{
public:
    RecordType();
    ~RecordType();

    void setVariety(RecordVariety variety);
    RecordVariety variety() const;

    void setEnv(Environment env);
    Environment env() const;

    void addBase(std::unique_ptr<BaseRecord> base);

    RecordType* clone() const override;

private:
    DECL_PIMPL_CAST(RecordType)
};

class UAISO_API FuncType final : public Type
{
public:
    FuncType();
    ~FuncType();

    void setReturnType(std::unique_ptr<Type> type);
    const Type* returnType() const;

    FuncType* clone() const override;

private:
    DECL_PIMPL_CAST(FuncType)
};

    //--- Utility ---//

/*!
 * \brief isNumType
 * \return
 */
bool isNumType(Type::Kind);

/*!
 * \brief isEnvType
 * \return
 *
 * Return whether the type has its own environment.
 */
std::pair<bool, Environment> isEnvType(const Type*);

} // namespace uaiso

#endif
