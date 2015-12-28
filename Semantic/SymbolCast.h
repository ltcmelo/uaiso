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
#define Alias_ConstCast(T) const_cast<uaiso::Alias*>(ConstAlias_Cast(T))

#define ConstBaseRecord_Cast(T) static_cast<const BaseRecord*>(T)
#define BaseRecord_Cast(T) static_cast<uaiso::BaseRecord*>(T)
#define BaseRecord_ConstCast(T) const_cast<uaiso::BaseRecord*>(ConstBaseRecord_Cast(T))

#define ConstBlock_Cast(T) static_cast<const Block*>(T)
#define Block_Cast(T) static_cast<uaiso::Block*>(T)
#define Block_ConstCast(T) const_cast<uaiso::Block*>(ConstBlock_Cast(T))

// Name would conflict with AST's decl cast.
#define ConstDeclSymbol_Cast(T) static_cast<const Decl*>(T)
#define DeclSymbol_Cast(T) static_cast<uaiso::Decl*>(T)
#define DeclSymbol_ConstCast(T) const_cast<uaiso::Decl*>(ConstDeclSymbol_Cast(T))

#define ConstEnum_Cast(T) static_cast<const Enum*>(T)
#define Enum_Cast(T) static_cast<uaiso::Enum*>(T)
#define Enum_ConstCast(T) const_cast<uaiso::Enum*>(ConstEnum_Cast(T))

#define ConstFunc_Cast(T) static_cast<const Func*>(T)
#define Func_Cast(T) static_cast<uaiso::Func*>(T)
#define Func_ConstCast(T) const_cast<uaiso::Func*>(ConstFunc_Cast(T))

#define ConstParam_Cast(T) static_cast<const Param*>(T)
#define Param_Cast(T) static_cast<uaiso::Param*>(T)
#define Param_ConstCast(T) const_cast<uaiso::Param*>(ConstParam_Cast(T))

#define ConstRecord_Cast(T) static_cast<const Record*>(T)
#define Record_Cast(T) static_cast<uaiso::Record*>(T)
#define Record_ConstCast(T) const_cast<uaiso::Record*>(ConstRecord_Cast(T))

#define ConstTypeDecl_Cast(T) static_cast<const TypeDecl*>(T)
#define TypeDecl_Cast(T) static_cast<uaiso::TypeDecl*>(T)
#define TypeDecl_ConstCast(T) const_cast<uaiso::TypeDecl*>(ConstTypeDecl_Cast(T))

#define ConstValueDecl_Cast(T) static_cast<const ValueDecl*>(T)
#define ValueDecl_Cast(T) static_cast<uaiso::ValueDecl*>(T)
#define ValueDecl_ConstCast(T) const_cast<uaiso::ValueDecl*>(ConstValueDecl_Cast(T))

#define ConstVar_Cast(T) static_cast<const Var*>(T)
#define Var_Cast(T) static_cast<uaiso::Var*>(T)
#define Var_ConstCast(T) const_cast<uaiso::Var*>(ConstVar_Cast(T))

#endif
