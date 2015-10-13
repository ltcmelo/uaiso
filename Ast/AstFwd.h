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

#ifndef UAISO_ASTFWD_H__
#define UAISO_ASTFWD_H__

#include "Ast/AstDefs.h"

namespace uaiso {

class Ast;
class ProgramAst;
class TemplateArgAst;
class GeneratorAst;
class FilterAst;

    //--- Kinds ---//

class NameAst;
class AttrAst;
class SpecAst;
class DeclAst;
class ExprAst;
class StmtAst;

    //--- Nodes ---//

#define DECLARE_FWD(AST_NODE, AST_KIND) \
    class AST_NODE##AST_KIND##Ast; \

#define DECLARE_NAME_FWD(AST_NODE, UNUSED) \
    DECLARE_FWD(AST_NODE, Name)
#define DECLARE_SPEC_FWD(AST_NODE, UNUSED) \
    DECLARE_FWD(AST_NODE, Spec)
#define DECLARE_ATTR_FWD(AST_NODE, UNUSED) \
    DECLARE_FWD(AST_NODE, Attr)
#define DECLARE_DECL_FWD(AST_NODE, UNUSED) \
    DECLARE_FWD(AST_NODE, Decl)
#define DECLARE_EXPR_FWD(AST_NODE, UNUSED) \
    DECLARE_FWD(AST_NODE, Expr)
#define DECLARE_STMT_FWD(AST_NODE, UNUSED) \
    DECLARE_FWD(AST_NODE, Stmt)

NAME_AST_MIXIN(DECLARE_NAME_FWD)
SPEC_AST_MIXIN(DECLARE_SPEC_FWD)
ATTR_AST_MIXIN(DECLARE_ATTR_FWD)
DECL_AST_MIXIN(DECLARE_DECL_FWD)
EXPR_AST_MIXIN(DECLARE_EXPR_FWD)
STMT_AST_MIXIN(DECLARE_STMT_FWD)

#undef DECLARE_FWD
#undef DECLARE_NAME_FWD
#undef DECLARE_SPEC_FWD
#undef DECLARE_ATTR_FWD
#undef DECLARE_DECL_FWD
#undef DECLARE_EXPR_FWD
#undef DECLARE_STMT_FWD

} // namespace uaiso

#endif
