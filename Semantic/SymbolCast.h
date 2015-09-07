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

#ifndef UAISO_SYMBOLCAST_H
#define UAISO_SYMBOLCAST_H

#define ConstAlias_Cast(T) static_cast<const Alias*>(T)
#define Alias_Cast(T) static_cast<uaiso::Alias*>(T)
#define Alias_ConstCast(T) const_cast<uaiso::Alias*>(Alias_Cast(T))

#define ConstBaseRecord_Cast(T) static_cast<const BaseRecord*>(T)
#define BaseRecord_Cast(T) static_cast<uaiso::BaseRecord*>(T)
#define BaseRecord_ConstCast(T) const_cast<uaiso::BaseRecord*>(BaseRecord_Cast(T))

#define ConstBlock_Cast(T) static_cast<const Block*>(T)
#define Block_Cast(T) static_cast<uaiso::Block*>(T)
#define Block_ConstCast(T) const_cast<uaiso::Block*>(Block_Cast(T))

#define ConstDeclSymbol_Cast(T) static_cast<const DeclSymbol*>(T)
#define DeclSymbol_Cast(T) static_cast<uaiso::DeclSymbol*>(T)
#define DeclSymbol_ConstCast(T) const_cast<uaiso::DeclSymbol*>(DeclSymbol_Cast(T))

#define ConstEnum_Cast(T) static_cast<const Enum*>(T)
#define Enum_Cast(T) static_cast<uaiso::Enum*>(T)
#define Enum_ConstCast(T) const_cast<uaiso::Enum*>(Enum_Cast(T))

#define ConstFunc_Cast(T) static_cast<const Func*>(T)
#define Func_Cast(T) static_cast<uaiso::Func*>(T)
#define Func_ConstCast(T) const_cast<uaiso::Func*>(Func_Cast(T))

#define ConstParam_Cast(T) static_cast<const Param*>(T)
#define Param_Cast(T) static_cast<uaiso::Param*>(T)
#define Param_ConstCast(T) const_cast<uaiso::Param*>(Param_Cast(T))

#define ConstRecord_Cast(T) static_cast<const Record*>(T)
#define Record_Cast(T) static_cast<uaiso::Record*>(T)
#define Record_ConstCast(T) const_cast<uaiso::Record*>(ConstRecord_Cast(T))

#define ConstTypeSymbol_Cast(T) static_cast<const TypeSymbol*>(T)
#define TypeSymbol_Cast(T) static_cast<uaiso::TypeSymbol*>(T)
#define TypeSymbol_ConstCast(T) const_cast<uaiso::TypeSymbol*>(TypeSymbol_Cast(T))

#define ConstValueSymbol_Cast(T) static_cast<const ValueSymbol*>(T)
#define ValueSymbol_Cast(T) static_cast<uaiso::ValueSymbol*>(T)
#define ValueSymbol_ConstCast(T) const_cast<uaiso::ValueSymbol*>(ValueSymbol_Cast(T))

#define ConstVar_Cast(T) static_cast<const Var*>(T)
#define Var_Cast(T) static_cast<uaiso::Var*>(T)
#define Var_ConstCast(T) const_cast<uaiso::Var*>(Var_Cast(T))

#endif
