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

#ifndef UAISO_SANITIZER_H__
#define UAISO_SANITIZER_H__

#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Parsing/Token.h"

namespace uaiso {

class UAISO_API Sanitizer
{
public:
    virtual ~Sanitizer();

    virtual bool moduleMatchesFile(const std::string& fullFileName,
                                   const std::string& moduleName) const;

    virtual bool packageMatchesDir(const std::string& fullFileName,
                                   const std::string& packageName) const;

    virtual bool mayMergeImportEnv(const Ident* name) const;

    virtual bool allowAnonymous(Symbol::Kind symKind) const;

    virtual bool validateVisibility(Symbol::Kind symKind, Token tk) const;

    virtual bool validateDirection(Symbol::Kind symKind, Token tk) const;

    virtual bool validateEvalStrategy(Symbol::Kind symKind, Token tk) const;

    virtual bool validateLinkage(Symbol::Kind symKind, Token tk) const;

    virtual bool validateStorageClass(Symbol::Kind symKind, Token tk) const;

    virtual bool validateDeclAttr(Symbol::Kind symKind, Token tk) const;
    virtual bool checkDeclAttrCoherence(const Decl*, Token tk) const;

    virtual bool validateTypeQual(Type::Kind typeKind, Token tk) const;
    virtual bool checkTypeQualCoherence(const Type*, Token tk) const;
};

    /*--- Utility ---*/

bool isFileNameOK(const std::string& moduleName,
                  const std::string& extension,
                  const std::string& fullFileName);

} // namespace uaiso

#endif
