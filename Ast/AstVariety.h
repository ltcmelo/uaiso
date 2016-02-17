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

#ifndef UAISO_ASTVARIETY_H__
#define UAISO_ASTVARIETY_H__

#define APPLY_VARIETY(VARIETY) \
    Self* setVariety(VARIETY v) \
    { \
        bit_.variety = static_cast<char>(v); \
        return this; \
    } \
    VARIETY variety() const { return VARIETY(bit_.variety); }

#define INIT_VARIETY(VARIETY_VALUE) \
    do { \
        bit_.variety = static_cast<char>(VARIETY_VALUE); \
    } while (0)

namespace uaiso {

/*!
 * \brief The NumLitVariety enum
 */
enum class NumLitVariety : char
{
    Unknown,
    IntegerFormat,
    FloatFormat,
    DoubleFormat,
    ReadlFormat
};

/*!
 * \brief The ChanVariety enum
 */
enum class ChanVariety : char
{
    Unknown,
    Sender,
    Receiver,
    Bidirectional
};

/*!
 * \brief The RecordVariety enum
 */
enum class RecordVariety : char
{
    Unknown,
    Union,
    Struct,
    Class,
    Interface
};

/*!
 * \brief The SelectiveVariety enum
 */
enum class SelectiveVariety : char
{
    Unknown,
    Debug,
    StaticIf,
    Version,
};

/*!
 * \brief The SectionVariety enum
 */
enum class SectionVariety : char
{
    Unknown,
    Vars,
    Types
};

/*!
 * \brief The AssignVariety enum
 */
enum class AssignVariety : char
{
    Unknow,
    Basic,
    ByAdd,
    BySub,
    ByMul,
    ByDiv,
    ByMod,
    ByShift,
    ByAnd,
    ByOr,
    ByXor,
};

/*!
 * \brief The FuncVariety enum
 */
enum class FuncVariety : char
{
    Unknown,
    Regular,
    Method,
    Constructor,
    Destructor
};

/*!
 * \brief The ArrayVariety enum
 */
enum class ArrayVariety : char
{
    Unknown,
    Plain,
    Associative,
};


/*!
 * \brief The AllocScheme enum
 */
enum class AllocScheme : char
{
    Unknown,
    Dynamic,
    CompileTime,
};

} // namespace uaiso

#endif
