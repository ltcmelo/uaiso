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

#ifndef UAISO_SYMBOL_H__
#define UAISO_SYMBOL_H__

#include "Common/Config.h"
#include "Common/Pimpl.h"
#include "Parsing/SourceLoc.h"
#include "Semantic/DeclAttrs.h"
#include "Semantic/TypeFwd.h"
#include "Semantic/SymbolCast.h"
#include <string>

namespace uaiso {

class DeclAttrs;
class Environment;
class Ident;
enum class RecordVariety : char;

class UAISO_API Symbol
{
public:
    virtual ~Symbol();

    enum class Kind : char
    {
        Alias,
        BaseRecord,
        Enum,
        EnumItem,
        Func,
        Import,
        Namespace,
        Param,
        Placeholder,
        Record,
        Var,
    };

    Kind kind() const;

    /*!
     * \brief setSourceLoc
     * \param loc
     *
     * Set the symbol's source location.
     */
    void setSourceLoc(const SourceLoc& loc);

    /*!
     * \brief sourceLoc
     * \return
     *
     * Return the symbol's source location.
     *
     * \warning A builtin symbol has an empty source location.
     */
    const SourceLoc& sourceLoc() const;

    /*!
     * \brief setIsBuiltin
     * \param isBuiltin
     *
     * Set whether the symbol is a builtin. This means the symbol doesn't
     * appear in a source file and therefore has no source location.
     */
    void setIsBuiltin(bool isBuiltin);

    /*!
     * \brief isBuiltin
     * \return
     *
     * Return whether the symbol is a builtin.
     */
    bool isBuiltin() const;

protected:
    Symbol(Symbol::Kind kind);

    DECL_PIMPL(Symbol)
    DECL_PIMPL_BASE(Symbol)
};

/*!
 * \brief The Import class
 *
 * Import is treated as a symbol, not evaluated when binding, and stored
 * for later processing.
 */
class UAISO_API Import final : public Symbol
{
public:
    /*!
     * \brief Import
     * \param originDir  - where the import was made
     * \param moduleName - module to import
     * \param localName  - name in the importing program
     * \param mergeEnv   - whether to merge the imported env
     */
    Import(const std::string& originDir,
           const std::string& moduleName,
           const Ident* localName,
           bool mergeEnv);

    const std::string& originDir() const;

    const std::string& moduleName() const;

    const Ident* localName() const;

    bool mergeEnv() const;

private:
    DECL_PIMPL_CAST(Import)
};

/*!
 * \brief The Namespace class
 */
class UAISO_API Namespace : public Symbol
{
public:
    Namespace();
    Namespace(const Ident* name);

    const Ident* name() const;

    bool isAnonymous() const;

    void setEnv(Environment env);
    Environment env() const;

private:
    DECL_PIMPL_CAST(Namespace)
};

/*!
 * \brief The DeclSymbol class
 */
class UAISO_API DeclSymbol : public Symbol
{
public:
    enum class Visibility : char
    {
        Unknown,
        Package,
        Private,
        Protected,
        Public
    };

    void setVisibility(Visibility access);

    Visibility visibility() const;

    enum class Storage : char
    {
        Unknown,
        Ref,
        Scope,
        Static,
        Thread
    };

    void setStorage(Storage store);
    Storage storage() const;

    enum class Linkage : char
    {
        Unknown,
        Extern,
        Intern
    };

    void setLinkage(Linkage link);
    Linkage linkage() const;

    const Ident* name() const;

    bool isAnonymous() const;

    /*!
     * \brief markAsAuto
     *
     * Interpretation of what auto means dependens on the derived class.
     */
    void markAsAuto();

    bool isMarkedAuto() const;

    void setDeclAttrs(DeclAttrFlags flags);
    DeclAttrFlags declAttrs() const;

protected:
    DECL_PIMPL_CAST(DeclSymbol)

    DeclSymbol(DeclSymbolImpl* impl);
};

class UAISO_API TypeSymbol : public DeclSymbol
{
public:
    /*!
     * \brief setType
     * \param ty
     */
    void setType(std::unique_ptr<Type> ty);

    /*!
     * \brief ty
     * \return
     */
    const Type* type() const;

protected:
    using DeclSymbol::DeclSymbol;

    DECL_PIMPL_CAST(TypeSymbol)
};

class UAISO_API ValueSymbol : public DeclSymbol
{
public:
    /*!
     * \brief setValueType
     * Set value type
     * \param ty
     */
    void setValueType(std::unique_ptr<Type> ty);

    /*!
     * \brief valueType
     * Get value type
     * \return
     */
    const Type* valueType() const;

protected:
    using DeclSymbol::DeclSymbol;

    DECL_PIMPL_CAST(ValueSymbol)
};

/*!
 * \brief The BaseRecord class
 */
class UAISO_API BaseRecord final : public DeclSymbol
{
public:
    BaseRecord(const Ident* name);
};

/*!
 * \brief The Placeholder class
 *
 * Name declaration for an incomplete type, such as forward declaration in C++.
 */
class UAISO_API Placeholder final : public TypeSymbol
{
public:
    Placeholder(const Ident* name);

    void setActual(TypeSymbol* sym);

    const TypeSymbol* actual() const;

private:
    DECL_PIMPL_CAST(Placeholder)
};

/*!
 * \brief The Func class
 */
class UAISO_API Func final : public TypeSymbol
{
public:
    Func(const Ident* name);
    ~Func();

    void setType(std::unique_ptr<FuncType> ty);
    const FuncType* type() const;

    void setEnv(Environment env);
    Environment env() const;

private:
    DECL_PIMPL_CAST(Func)
};

/*!
 * \brief The Alias class
 */
class UAISO_API Alias final : public TypeSymbol
{
public:
    Alias(const Ident* name);
};

/*!
 * \brief The Record class
 */
class UAISO_API Record final : public TypeSymbol
{
public:
    Record(const Ident* name);
    ~Record();

    void setType(std::unique_ptr<RecordType> type);
    const RecordType* type() const;

private:
    DECL_PIMPL_CAST(Record)
};

/*!
 * \brief The Enum class
 */
class UAISO_API Enum final : public TypeSymbol
{
public:
    Enum(const Ident* name);
    ~Enum();

    void setUnderlyingType(std::unique_ptr<Type> ty);
    const Type* underlyingType() const;

    void setType(std::unique_ptr<EnumType> ty);
    const EnumType* type() const;

private:
    DECL_PIMPL_CAST(Enum)
};

/*!
 * \brief The Param class
 */
class UAISO_API Param final : public ValueSymbol
{
public:
    Param();
    Param(const Ident* name);
    ~Param();

    enum class Direction : char
    {
        Unknown,
        In,
        InOut,
        Out
    };

    void setDirection(Direction dir);
    Direction direction() const;

    enum class EvalStrategy : char
    {
        Unknown,
        Eager,
        Lazy
    };

    void setEvalStrategy(EvalStrategy eval);
    EvalStrategy evalStrategy() const;

private:
    DECL_PIMPL_CAST(Param)
};

/*!
 * \brief The Var class
 */
class UAISO_API Var final : public ValueSymbol
{
public:
    Var(const Ident* name);
    ~Var();

private:
    DECL_PIMPL_CAST(Var)
};

/*!
 * \brief The EnumItem class
 */
class UAISO_API EnumItem final : public ValueSymbol
{
public:
    EnumItem(const Ident *name);
};

    /*--- Utility ---*/

bool isDeclSymbol(const Symbol* symbol);
bool isTypeSymbol(const Symbol* symbol);
bool isValueSymbol(const Symbol* symbol);

} // namespace uaiso

#endif
