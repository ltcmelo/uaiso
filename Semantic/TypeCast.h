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

#ifndef UAISO_TYPECAST_H__
#define UAISO_TYPECAST_H__

#define ConstArrayType_Cast(T) static_cast<const uaiso::ArrayType*>(T)
#define ArrayType_Cast(T) static_cast<uaiso::ArrayType*>(T)
#define ArrayType_ConstCast(T) const_cast<uaiso::ArrayType*>(ArrayType_Cast(T))

#define ConstBoolType_Cast(T) static_cast<const uaiso::BoolType*>(T)
#define BoolType_Cast(T) static_cast<uaiso::BoolType*>(T)
#define BoolType_ConstCast(T) const_cast<uaiso::BoolType*>(BoolType_Cast(T))

#define ConstChanType_Cast(T) static_cast<const uaiso::ChanType*>(T)
#define ChanType_Cast(T) static_cast<uaiso::ChanType*>(T)
#define ChanType_ConstCast(T) const_cast<uaiso::ChanType*>(ChanType_Cast(T))

#define ConstElaborateType_Cast(T) static_cast<const uaiso::ElaborateType*>(T)
#define ElaborateType_Cast(T) static_cast<uaiso::ElaborateType*>(T)
#define ElaborateType_ConstCast(T) const_cast<uaiso::ElaborateType*>(ConstElaborateType_Cast(T))

#define ConstEnumType_Cast(T) static_cast<const uaiso::EnumType*>(T)
#define EnumType_Cast(T) static_cast<uaiso::EnumType*>(T)
#define EnumType_ConstCast(T) const_cast<uaiso::EnumType*>(EnumType_Cast(T))

#define ConstEnumType_Cast(T) static_cast<const uaiso::EnumType*>(T)
#define EnumType_Cast(T) static_cast<uaiso::EnumType*>(T)
#define EnumType_ConstCast(T) const_cast<uaiso::EnumType*>(EnumType_Cast(T))

#define ConstFloatType_Cast(T) static_cast<const uaiso::FloatType*>(T)
#define FloatType_Cast(T) static_cast<uaiso::FloatType*>(T)
#define FloatType_ConstCast(T) const_cast<uaiso::FloatType*>(FloatType_Cast(T))

#define ConstFuncType_Cast(T) static_cast<const uaiso::FuncType*>(T)
#define FuncType_Cast(T) static_cast<uaiso::FuncType*>(T)
#define FuncType_ConstCast(T) const_cast<uaiso::FuncType*>(FuncType_Cast(T))

#define ConstIntType_Cast(T) static_cast<const uaiso::IntType*>(T)
#define IntType_Cast(T) static_cast<uaiso::IntType*>(T)
#define IntType_ConstCast(T) const_cast<uaiso::IntType*>(IntType_Cast(T))

#define ConstOpaqueType_Cast(T) static_cast<const uaiso::OpaqueType*>(T)
#define OpaqueType_Cast(T) static_cast<uaiso::OpaqueType*>(T)
#define OpaqueType_ConstCast(T) const_cast<uaiso::OpaqueType*>(OpaqueType_Cast(T))

#define ConstPtrType_Cast(T) static_cast<const uaiso::PtrType*>(T)
#define PtrType_Cast(T) static_cast<uaiso::PtrType*>(T)
#define PtrType_ConstCast(T) const_cast<uaiso::PtrType*>(PtrType_Cast(T))

#define ConstRecordType_Cast(T) static_cast<const uaiso::RecordType*>(T)
#define RecordType_Cast(T) static_cast<uaiso::RecordType*>(T)
#define RecordType_ConstCast(T) const_cast<uaiso::RecordType*>(RecordType_Cast(T))

#define ConstStrType_Cast(T) static_cast<const uaiso::StrType*>(T)
#define StrType_Cast(T) static_cast<StrType*>(T)
#define StrType_ConstCast(T) const_cast<StrType*>(StrType_Cast(T))

#define ConstSubrangeType_Cast(T) static_cast<const uaiso::SubrangeType*>(T)
#define SubrangeType_Cast(T) static_cast<uaiso::SubrangeType*>(T)
#define SubrangeType_ConstCast(T) const_cast<uaiso::SubrangeType*>(SubrangeType_Cast(T))

#define ConstVoidType_Cast(T) static_cast<const uaiso::VoidType*>(T)
#define VoidType_Cast(T) static_cast<uaiso::VoidType*>(T)
#define VoidType_ConstCast(T) const_cast<uaiso::VoidType*>(VoidType_Cast(T))


#endif
