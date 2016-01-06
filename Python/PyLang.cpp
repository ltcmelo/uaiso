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

#include "Python/PyLang.h"
#include "Python/PyKeywords.h"

using namespace uaiso;

bool PyLang::hasBlockLevelScope() const { return false; }

bool PyLang::hasExecutableRecord() const { return true; }

bool PyLang::hasNewlineAsTerminator() const { return true; }

bool PyLang::requiresReturnTypeInference() const { return true; }

PyLang::Structure PyLang::structure() const
{
    return StmtBased;
}

PyLang::ImportMechanism PyLang::importMechanism() const
{
    return PerModuleAndPackage;
}

std::string PyLang::sourceFileSuffix() const
{
    return ".py";
}

Token PyLang::classifyIdent(const char* spell, size_t length) const
{
    return PyKeywords::classify(spell, length);
}

bool PyLang::isStrLitQuote(char ch) const
{
    return ch == '"' || ch == '\'';
}

bool uaiso::PyLang::isPurelyOO() const
{
    return true;
}
