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

#include "Common/Util__.h"
#include "Parsing/Lexeme.h"
#include "Semantic/Sanitizer.h"
#include "Semantic/TypeQuals.h"
#include "StringUtils/string.hpp"
#include <vector>

using namespace uaiso;

Sanitizer::~Sanitizer()
{}

bool uaiso::Sanitizer::moduleMatchesFile(const std::string& fullFileName,
                                         const std::string& moduleName) const
{
    UAISO_UNUSED(fullFileName);
    UAISO_UNUSED(moduleName);

    return true;
}

bool uaiso::Sanitizer::packageMatchesDir(const std::string& fullFileName,
                                         const std::string& packageName) const
{
    UAISO_UNUSED(fullFileName);
    UAISO_UNUSED(packageName);

    return true;
}

bool Sanitizer::mayMergeImportEnv(const Ident* name) const
{
    return !name;
}

bool Sanitizer::validateVisibility(Symbol::Kind, Token) const
{
    return true;
}

bool Sanitizer::validateDirection(Symbol::Kind, Token) const
{
    return true;
}

bool Sanitizer::validateEvalStrategy(Symbol::Kind, Token) const
{
    return true;
}

bool Sanitizer::validateLinkage(Symbol::Kind, Token) const
{
    return true;
}

bool Sanitizer::validateStorageClass(Symbol::Kind, Token) const
{
    return true;
}

bool Sanitizer::validateDeclAttr(Symbol::Kind, Token) const
{
    return true;
}

bool Sanitizer::checkDeclAttrCoherence(const DeclSymbol*, Token) const
{
    return true;
}

bool Sanitizer::validateTypeQual(Type::Kind typeKind, Token) const
{
    return true;
}

bool Sanitizer::checkTypeQualCoherence(const Type*, Token) const
{
    return true;
}

bool Sanitizer::allowAnonymous(Symbol::Kind symKind) const
{
    return symKind == Symbol::Kind::Param;
}

namespace uaiso {

bool isFileNameOK(const std::string& moduleName,
                  const std::string& extension,
                  const std::string& fullFileName)
{
    std::vector<std::string> candidates;
    candidates.emplace_back(moduleName + extension);
    for (auto candidate : candidates) {
        if (str::iends_with(fullFileName, candidate))
            return true;
    }

    return false;
}

} // namespace uaiso
