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

#include "Semantic/CompletionProposer.h"
#include "Semantic/Builtin.h"
#include "Semantic/Environment.h"
#include "Semantic/Program.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Semantic/TypeResolver.h"
#include "Ast/Ast.h"
#include "Ast/AstVisitor.h"
#include "Common/Assert.h"
#include "Common/Trace__.h"
#include "Parsing/Factory.h"
#include "Parsing/Lexeme.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/Lang.h"
#include <iostream>
#include <stack>
#include <vector>

#define ENSURE_ANNOTATED_SYMBOL \
    UAISO_ASSERT(ast->sym_, return Abort, "no symbol annotated")
#define ENSURE_NONEMPTY_AST_STACK \
    UAISO_ASSERT(!asts_.empty(), return Abort)

#define TRACE_NAME "Completion"

using namespace uaiso;

namespace {

class CompletionContext final : public AstVisitor<CompletionContext>
{
public:
    using Base = AstVisitor<CompletionContext>;

    CompletionContext(const Lang* lang)
        : lang_(lang)
    {}

    bool analyse(const ProgramAst* progAst,
                 const LexemeMap* lexs,
                 Environment env)
    {
        UAISO_ASSERT(lexs, return false);

        lexs_ = lexs;
        env_ = env;

        auto result = traverseProgram(progAst, this, lang_);

        return result == Abort;
    }

    Environment env_;
    const LexemeMap* lexs_ { nullptr };
    std::stack<Ast*> asts_;
    size_t collectName_ { 0 }; // Collect names only when it matters.
    std::vector<const Ident*> name_;
    const Lang* lang_ { nullptr };

    VisitResult visitCompletionName(CompletionNameAst*)
    {
        // Visit only until completion name is found.
        return Abort;
    }

    VisitResult visitSimpleName(SimpleNameAst* ast)
    {
        if (collectName_) {
            auto name = lexs_->findAt<Ident>(ast->nameLoc_.fileName_,
                                                ast->nameLoc_.lineCol());
            name_.push_back(name);
        }
        return Continue;
    }

    //--- Expressions ---//

    VisitResult traverseIdentExpr(IdentExprAst* ast)
    {
        asts_.push(ast);
        VIS_CALL(Base::traverseIdentExpr(ast));
        ENSURE_NONEMPTY_AST_STACK;
        asts_.pop();

        return Continue;
    }

    VisitResult traverseMemberAccessExpr(MemberAccessExprAst* ast)
    {
        asts_.push(ast);

        if (!collectName_)
            name_.clear();
        ++collectName_;
        if (ast->isExpr())
            VIS_CALL(traverseExpr(Expr_Cast(ast->exprOrSpec_.get())));
        else
            VIS_CALL(traverseSpec(Spec_Cast(ast->exprOrSpec_.get())));
        UAISO_ASSERT(collectName_ > 0, return Abort);
        --collectName_;

        VIS_CALL(traverseName(ast->name_.get()));
        ENSURE_NONEMPTY_AST_STACK;
        asts_.pop();

        return Continue;
    }

    //--- Specifiers ---//

    VisitResult traverseNamedSpec(NamedSpecAst* ast)
    {
        asts_.push(ast);

        if (!collectName_)
            name_.clear();
        ++collectName_;
        VIS_CALL(Base::traverseNamedSpec(ast));
        UAISO_ASSERT(collectName_ > 0, return Abort);
        --collectName_;

        ENSURE_NONEMPTY_AST_STACK;
        asts_.pop();

        return Continue;
    }

    // Track the current environment.

    VisitResult traverseRecordDecl(RecordDeclAst* ast)
    {
        ENSURE_ANNOTATED_SYMBOL;
        env_ = ast->sym_->type()->env();
        VIS_CALL(Base::traverseRecordDecl(ast));
        env_ = env_.outerEnv();
        return Continue;
    }

    VisitResult traverseFuncDecl(FuncDeclAst* ast)
    {
        if (lang_->hasFuncLevelScope()) {
            ENSURE_ANNOTATED_SYMBOL;
            env_ = ast->sym_->env();
            VIS_CALL(Base::traverseFuncDecl(ast));
            env_ = env_.outerEnv();
        } else {
            VIS_CALL(Base::traverseFuncDecl(ast));
        }
        return Continue;
    }

    VisitResult traverseBlockStmt(BlockStmtAst* ast)
    {
        // The environment of a block stmt might be the shared with its
        // parent. If that's the case, it's been entered already.
        if (env_ == ast->env_
                || !lang_->hasBlockLevelScope()) {
            VIS_CALL(Base::traverseBlockStmt(ast));
        } else {
            env_ = ast->env_;
            VIS_CALL(Base::traverseBlockStmt(ast));
            env_ = env_.outerEnv();
        }
        return Continue;
    }
};

} // anonymous

struct uaiso::CompletionProposer::CompletionProposerImpl
{
    CompletionProposerImpl(Factory* factory)
        : lang_(factory->makeLang())
        , builtin_(factory->makeBuiltin())
        , resolver_(factory)
    {}

    Symbols& typeDecls(Environment env, Symbols& syms, const Ident* ident)
    {
        auto sym = env.searchTypeDecl(ident);
        if (!sym) {
            DEBUG_TRACE("symbol is unknown\n");
            return syms;
        }

        bool hasEnv;
        std::tie(hasEnv, env) = envForType(sym->type(), env);
        if (hasEnv) {
            auto extraSyms = env.listDecls();
            std::copy(extraSyms.begin(), extraSyms.end(), std::back_inserter(syms));
        }

        return syms;
    }

    Symbols& addBasicTypeDecls(const LexemeMap* lexs,
                               Environment env,
                               Symbols& syms,
                               Type::Kind kind)
    {
        auto ident = builtin_->basicTypeDeclName(const_cast<LexemeMap*>(lexs), kind);
        if (!ident)
            return syms;
        return typeDecls(env, syms, ident);
    }

    Symbols& addRootRecordDecls(const LexemeMap* lexs,
                                Environment env,
                                Symbols& syms)
    {
        auto ident = builtin_->rootTypeDeclName(const_cast<LexemeMap*>(lexs));
        UAISO_ASSERT(ident, return syms);
        return typeDecls(env, syms, ident);
    }

    //! Language-specific details.
    std::unique_ptr<const Lang> lang_;

    //! Language-specific builtin.
    std::unique_ptr<const Builtin> builtin_;

    //! Type resolver.
    TypeResolver resolver_;
};

CompletionProposer::CompletionProposer(Factory *factory)
    : P(new CompletionProposerImpl(factory))
{}

CompletionProposer::~CompletionProposer()
{}

CompletionProposer::Result
CompletionProposer::propose(ProgramAst* progAst, const LexemeMap* lexs)
{
    using Symbols = std::vector<const Decl*>;

    UAISO_ASSERT(progAst->program_,
                 return Result(Symbols(), CompletionAstNotFound));

    CompletionContext context(P->lang_.get());
    auto ok = context.analyse(progAst, lexs, progAst->program_->env());

    if (!ok) {
        DEBUG_TRACE("completion AST node not found\n");
        return Result(Symbols(), CompletionAstNotFound);
    }

    if (context.asts_.empty()) {
        DEBUG_TRACE("completion AST node has not bound context\n");
        return Result(Symbols() , UnboundCompletionAstContext);
    }

    auto topAst = context.asts_.top();
    auto env = context.env_;

    // When completing an identifier, simply list the environment.
    if (topAst->kind() == Ast::Kind::IdentExpr) {
        Symbols syms;
        while (true) {
            auto curSyms = env.listDecls();
            std::copy(curSyms.begin(), curSyms.end(), std::back_inserter(syms));
            if (env.isRootEnv())
                break;
            env = env.outerEnv();
        }
        return Result(syms, Success);
    }

    // Completing on a member access requires identifying the type or namespace.
    if (topAst->kind() == Ast::Kind::MemberAccessExpr
            || topAst->kind() == Ast::Kind::NamedSpec) {
        UAISO_ASSERT(!context.name_.empty(),
                     return Result(Symbols(), InternalError));
        const Type* ty = nullptr;
        for (auto ident : context.name_) {
            auto tySym = env.searchTypeDecl(ident);
            if (tySym) {
                ty = tySym->type();
            } else {
                auto valSym = env.searchValueDecl(ident);
                if (valSym) {
                    ty = valSym->valueType();
                } else {
                    auto spaceSym = env.fetchNamespace(ident);
                    if (!spaceSym) {
                        DEBUG_TRACE("symbol is unknown\n");
                        return Result(Symbols(), UnknownSymbol);
                    }

                    // Enter the namespace and start it over.
                    env = spaceSym->env();
                    continue;
                }
            }

            if (!ty) {
                DEBUG_TRACE("type is undefined\n");
                return Result(Symbols(), UndefinedType);
            }

            // If this is an elaborate type, try resolving it.
            if (ty->kind() == Type::Kind::Elaborate) {
                ty = std::get<0>(P->resolver_.resolve(ElaborateType_ConstCast(ty), env));
                if (!ty) {
                    DEBUG_TRACE("type is undefined\n");
                    return Result(Symbols(), UndefinedType);
                }
            }

            // If type is not yet inferred, type check the program first.
            if (ty->kind() == Type::Kind::Inferred) {
                DEBUG_TRACE("type has not yet been inferred\n");
                return Result(Symbols(), TypeNotYetInferred);
            }

            bool hasEnv;
            std::tie(hasEnv, env) = envForType(ty, env);
            if (!hasEnv) {
                if (!P->lang_->isPurelyOO()) {
                    DEBUG_TRACE("type has no associated environment\n");
                    return Result(Symbols(), InvalidType);
                }

                Symbols syms;
                P->addRootRecordDecls(lexs, env, syms);
                P->addBasicTypeDecls(lexs, env, syms, ty->kind());
                return Result(syms, Success);
            }
        }

        // If completing a namespace, there will be no type.
        if (!ty)
            return Result(env.listDecls(), Success);

        // Look into base classes.
        Symbols syms = env.listDecls();
        std::stack<const Type*> allTy;
        allTy.push(ty);
        while (!allTy.empty()) {
            const Type* curTy = allTy.top();
            allTy.pop();
            UAISO_ASSERT(curTy, return Result(Symbols(), InternalError));
            if (curTy->kind() != Type::Kind::Record)
                continue;

            const RecordType* recTy = ConstRecordType_Cast(curTy);
            for (auto base : recTy->bases()) {
                auto baseTySym = env.searchTypeDecl(base->name());
                if (!baseTySym)
                    continue;

                bool baseHasEnv;
                Environment baseEnv;
                std::tie(baseHasEnv, baseEnv) = envForType(baseTySym->type(), baseEnv);
                if (baseHasEnv) {
                    allTy.push(baseTySym->type());
                    auto extraSyms = baseEnv.listDecls();
                    std::copy(extraSyms.begin(), extraSyms.end(), std::back_inserter(syms));
                }
            }
        }

        if (P->lang_->isPurelyOO())
            P->addRootRecordDecls(lexs, env, syms);

        return Result(syms, Success);
    }

    DEBUG_TRACE("completion case not yet implemented\n");
    return Result(Symbols(), CaseNotImplemented);
}
