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

#include "Semantic/SymbolCollector.h"
#include "Semantic/Program.h"
#include "Semantic/Symbol.h"
#include "Ast/AstLocator.h"
#include "Ast/AstVisitor.h"
#include "Common/Assert.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/Syntax.h"
#include <iterator>
#include <unordered_set>

#define ENSURE_ANNOTATED_SYMBOL \
    UAISO_ASSERT(ast->sym_, return Abort, "symbol not annotated")

#define ENSURE_NAMED_SYMBOL \
    UAISO_ASSERT(ast->sym_->name(), return Abort, "symbol unnamed")

#define ENSURE_VALID_TYPE_SYMBOL \
    ENSURE_ANNOTATED_SYMBOL; \
    ENSURE_NAMED_SYMBOL; \
    UAISO_ASSERT(env_.lookUpType(ast->sym_->name()), return Abort, "type not found")

using namespace uaiso;

struct uaiso::SymbolCollector::SymbolCollectorImpl
{
    SymbolCollectorImpl(Factory* factory)
        : locator_(factory->makeAstLocator())
        , syntax_(factory->makeSyntax())
    {}

    //! Language-specific AST locator.
    std::unique_ptr<const AstLocator> locator_;

    //! Language-specific syntax.
    std::unique_ptr<const Syntax> syntax_;
};

SymbolCollector::SymbolCollector(Factory* factory)
    : P(new SymbolCollectorImpl(factory))
{}

SymbolCollector::~SymbolCollector()
{}

namespace {

class SymbolDefVisitor final : public AstVisitor<SymbolDefVisitor>
{
public:
    SymbolDefVisitor(const AstLocator* locator)
        : locator_(locator)
    {}

    const AstLocator* locator_;
    std::vector<SymbolCollector::MentionInfo> refs_;

    template <class AstT>
    VisitResult collectDef(AstT* ast)
    {
        ENSURE_ANNOTATED_SYMBOL;
        refs_.push_back(std::make_tuple(SymbolCollector::Mention::Def,
                                        ast->sym_,
                                        fullLoc(ast->name_.get(), locator_)));
        return Continue;
    }

    //--- Declarations ---//

    VisitResult visitEnumDecl(EnumDeclAst* ast) { return collectDef(ast); }
    VisitResult visitEnumMemberDecl(EnumMemberDeclAst* ast) { return collectDef(ast); }
    VisitResult visitFuncDecl(FuncDeclAst* ast) { return collectDef(ast); }
    VisitResult visitRecordDecl(RecordDeclAst* ast) { return collectDef(ast); }
    VisitResult visitVarDecl(VarDeclAst* ast) { return collectDef(ast); }
};

class SymbolUseVisitor final : public AstVisitor<SymbolUseVisitor>
{
public:
    SymbolUseVisitor(Environment env,
                     const AstLocator* locator,
                     const Syntax* syntax,
                     const LexemeMap* lexemes,
                     const std::vector<SymbolCollector::MentionInfo>& defs)
        : env_(env)
        , locator_(locator)
        , syntax_(syntax)
        , lexemes_(lexemes)
    {
        // We want only uses that are not defs.
        for (const auto& def : defs)
            known_.insert(std::get<2>(def).lineCol());
    }

    using Base = AstVisitor<SymbolUseVisitor>;

    Environment env_;
    const AstLocator* locator_;
    const Syntax* syntax_;
    const LexemeMap* lexemes_;
    std::vector<SymbolCollector::MentionInfo> refs_;
    std::unordered_set<LineCol> known_;

    //--- Declarations ---//

    VisitResult traverseEnumDecl(EnumDeclAst* ast)
    {
        ENSURE_VALID_TYPE_SYMBOL;
        env_ = ast->sym_->type()->env();
        VIS_CALL(Base::traverseEnumDecl(ast));
        env_ = env_.outerEnv();
        return Continue;
    }

    VisitResult traverseRecordDecl(RecordDeclAst* ast)
    {
        ENSURE_VALID_TYPE_SYMBOL;
        env_ = ast->sym_->type()->env();
        VIS_CALL(Base::traverseRecordDecl(ast));
        env_ = env_.outerEnv();
        return Continue;
    }

    //--- Statements ---//

    VisitResult traverseBlockStmt(BlockStmtAst* ast)
    {
        // The environment of a block stmt might be the shared with its
        // parent. If that's the case, it's been entered already.
        if (env_ == ast->env_
                || !syntax_->hasBlockLevelScope()) {
            VIS_CALL(Base::traverseBlockStmt(ast));
        } else {
            env_ = ast->env_;
            VIS_CALL(Base::traverseBlockStmt(ast));
            env_ = env_.outerEnv();
        }
        return Continue;
    }

    //--- Names ---//

    VisitResult visitSimpleName(SimpleNameAst* ast)
    {
        const DeclSymbol* sym = lookUpValue(ast, env_, lexemes_);
        if (!sym) {
            sym = lookUpType(ast, env_, lexemes_);
            if (!sym)
                return Continue;
        }

        const SourceLoc& loc = fullLoc(ast, locator_);
        if (known_.count(loc.lineCol()) == 0) {
            refs_.push_back(std::make_tuple(SymbolCollector::Mention::Use,
                                            sym, loc));
        }

        return Continue;
    }
};

} // anonymous

template <class VisitorT>
std::vector<SymbolCollector::MentionInfo>
SymbolCollector::collectCore(ProgramAst* progAst, VisitorT& vis)
{
    using Refs = std::vector<MentionInfo>;

    UAISO_ASSERT(progAst, return Refs());
    UAISO_ASSERT(progAst->program_, return Refs());

    traverseProgram(progAst, &vis, P->syntax_.get());

    return std::move(vis.refs_);
}

std::vector<SymbolCollector::MentionInfo>
SymbolCollector::collectDefs(ProgramAst* progAst)
{
    SymbolDefVisitor vis(P->locator_.get());
    return collectCore(progAst, vis);
}

std::vector<SymbolCollector::MentionInfo>
SymbolCollector::collect(ProgramAst* progAst, const LexemeMap* lexemes)
{
    auto refs = collectDefs(progAst);

    SymbolUseVisitor vis(progAst->program_->env(), P->locator_.get(),
                         P->syntax_.get(), lexemes, refs);
    auto uses = collectCore(progAst, vis);

    refs.reserve(refs.size() + uses.size());
    std::copy(std::make_move_iterator(uses.begin()),
              std::make_move_iterator(uses.end()),
              std::back_inserter(refs));

    return std::move(refs);
}
