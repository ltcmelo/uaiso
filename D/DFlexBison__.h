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

///////////////////////////////////////////////////////
///                                                 ///
///         This is an INTERNAL header              ///
///                                                 ///
///   Do not include this header from outside the   ///
///   the uaiso lib or from any public API header   ///
///                                                 ///
///////////////////////////////////////////////////////

#ifndef UAISO_DFLEXBISON_H__
#define UAISO_DFLEXBISON_H__

#include "Ast/AstFwd.h"
#include "Ast/AstList.h"
#include "Parsing/FlexBison__.h"
#include "Parsing/ParsingContext.h"
#include "Parsing/SourceLoc.h"
#include <memory>
#include <utility>

namespace uaiso {

namespace detail {

struct FullParam : PartialParse
{
    FullParam(ParamClauseDeclAst* param,
                  TemplateParamClauseDeclAst* templateParam = nullptr)
        : param_(param), templateParam_(templateParam)
    {}

    ~FullParam();

    std::unique_ptr<ParamClauseDeclAst> param_;
    std::unique_ptr<TemplateParamClauseDeclAst> templateParam_;
};

struct RecordKeyVariety : PartialParse
{
    RecordKeyVariety(const SourceLoc& key, int variety)
        : keyLoc_(key), variety_(variety)
    {}

    SourceLoc keyLoc_;
    int variety_;
};

struct Condition : PartialParse
{
    Condition(const SourceLoc& key,
              const SourceLoc& lDelim,
              ExprAst* expr,
              const SourceLoc& rDelim)
        : keyLoc_(key), lDelimLoc_(lDelim), expr_(expr), rDelimLoc_(rDelim)
    {}

    SourceLoc keyLoc_;
    SourceLoc lDelimLoc_;
    std::unique_ptr<ExprAst> expr_;
    SourceLoc rDelimLoc_;
};

    //--- Action rules ---//

void actionProgram(ParsingContext* context, DeclAstList* decls);
void actionProgram(ParsingContext* context, const SourceLoc& locA, NameAstList* names,
                   const SourceLoc& locB, DeclAstList* decls);
DeclAst* actionVarGroupDecl(SpecAst* spec, DeclAstList* decls, const SourceLoc& locA);
DeclAst* actionVarGroupDecl(AttrAstList* attrs, DeclAstList* decls, const SourceLoc& locA);

} // namespace detail

} // namespace uaiso

#endif
