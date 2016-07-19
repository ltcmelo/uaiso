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

#include "Parsing/Diagnostic.h"
#include "Ast/Ast.h"
#include "Ast/AstLocator.h"
#include "Common/Assert.h"
#include <unordered_set>

using namespace uaiso;

namespace std {

template <> struct hash<Diagnostic>
{
    size_t operator()(const Diagnostic& info) const
    {
        return std::hash<size_t>()(info.code());
    }
};

} // namespace std

    /*--- Diagnostic codes ---*/

const Diagnostic::Code Diagnostic::VariableNotInitialized = 0;
const Diagnostic::Code Diagnostic::UnknownEnvironmentName = 1;
const Diagnostic::Code Diagnostic::MultipleVisibilityAttributes = 2;
const Diagnostic::Code Diagnostic::DuplicateTypeQualifier = 3;
const Diagnostic::Code Diagnostic::MultipleStorageSpecifiers = 4;
const Diagnostic::Code Diagnostic::MultipleLinkageSpecifiers = 5;
const Diagnostic::Code Diagnostic::UnexpectedDeclarationAttribute = 6;
const Diagnostic::Code Diagnostic::UnexpectedTypeQualifier = 7;
const Diagnostic::Code Diagnostic::IncompatibleTypeQualifiers = 8;
const Diagnostic::Code Diagnostic::IncompatibleDeclarationAttributes = 9;
const Diagnostic::Code Diagnostic::DuplicateDeclarationAttribute = 10;
const Diagnostic::Code Diagnostic::UnexpectedVisibilityAttribute = 11;
const Diagnostic::Code Diagnostic::UnexpectedDirectionAttribute = 12;
const Diagnostic::Code Diagnostic::MultipleDirectionAttributes = 13;
const Diagnostic::Code Diagnostic::UnexpectedEvaluationStrategyAttribute = 14;
const Diagnostic::Code Diagnostic::MultipleEvaluationStrategyAttributes = 15;
const Diagnostic::Code Diagnostic::UnexpectedStorageClassAttribute = 16;
const Diagnostic::Code Diagnostic::UnexpectedLinkageAttribute = 17;
const Diagnostic::Code Diagnostic::ExpressionOfUnexpectedType = 19;
const Diagnostic::Code Diagnostic::InitializersDoNotMatchDeclarations = 20;
const Diagnostic::Code Diagnostic::UnresolvedModule = 21;
const Diagnostic::Code Diagnostic::ModuleNameDoesNotMatchFileName = 22;
const Diagnostic::Code Diagnostic::PackageNameDoesNotMatchDirName = 23;
const Diagnostic::Code Diagnostic::MultipleInitializers = 24;
const Diagnostic::Code Diagnostic::NumericValueExpected = 25;
const Diagnostic::Code Diagnostic::IntegerValueExpected = 26;
const Diagnostic::Code Diagnostic::ArrayValueExpected = 27;
const Diagnostic::Code Diagnostic::IncompatibleTypes = 28;
const Diagnostic::Code Diagnostic::BooleanValueExpected = 29;
const Diagnostic::Code Diagnostic::StringValueExpected = 31;
const Diagnostic::Code Diagnostic::UnknownType = 32;
const Diagnostic::Code Diagnostic::ExtraInitializer = 33;
const Diagnostic::Code Diagnostic::IncompatibleAssignment = 34;
const Diagnostic::Code Diagnostic::IncompatibleInitialization = 35;
const Diagnostic::Code Diagnostic::IncompatibleComparison = 36;
const Diagnostic::Code Diagnostic::CircularType = 37;
const Diagnostic::Code Diagnostic::IncompatibleCondition = 38;
const Diagnostic::Code Diagnostic::UndeclaredIdentifier = 39;
const Diagnostic::Code Diagnostic::ValueWillBeTruncated = 40;
const Diagnostic::Code Diagnostic::ValueMayLoosePrecision = 41;
const Diagnostic::Code Diagnostic::IdentifierExpected = 42;
const Diagnostic::Code Diagnostic::UnexpectedToken = 43;
const Diagnostic::Code Diagnostic::SubrangeValueExpected = 44;
const Diagnostic::Code Diagnostic::UnknownEscapeSequence = 45;
const Diagnostic::Code Diagnostic::UnterminatedString = 46;
const Diagnostic::Code Diagnostic::InvalidBinaryDigit = 47;
const Diagnostic::Code Diagnostic::InvalidOctalDigit = 48;
const Diagnostic::Code Diagnostic::InvalidHexDigit = 49;
const Diagnostic::Code Diagnostic::InvalidFloatSuffix = 50;
const Diagnostic::Code Diagnostic::InvalidReferenceToSelf = 51;
const Diagnostic::Code Diagnostic::UnexpectedName = 52;
const Diagnostic::Code Diagnostic::UnmatchedStringJoining = 53;
const Diagnostic::Code Diagnostic::InvalidOperator = 54;

namespace uaiso {

class DiagnosticTable
{
public:
    DiagnosticTable()
    {}

    const Diagnostic& find(size_t code) const
    {
        // The hash specialization for Diagnostic only considers the code.
        return *values_.find(Diagnostic(code, "", Severity::Error));
    }

    std::unordered_set<Diagnostic> values_
    {
        { Diagnostic::VariableNotInitialized,
                    "uninitialized variable", Severity::Warning },
        { Diagnostic::UnknownEnvironmentName,
                    "unknown environment name", Severity::Error },
        { Diagnostic::MultipleVisibilityAttributes,
                    "multiple access specifiers", Severity::Error },
        { Diagnostic::DuplicateTypeQualifier,
                    "duplicate type qualifier", Severity::Error },
        { Diagnostic::MultipleStorageSpecifiers,
                    "multiple linkage specifiers", Severity::Error },
        { Diagnostic::MultipleLinkageSpecifiers,
                    "multiple linkage specifiers", Severity::Error },
        { Diagnostic::UnexpectedDeclarationAttribute,
                    "unexpected attribute", Severity::Error },
        { Diagnostic::UnexpectedTypeQualifier,
                    "unexpected type qualifier", Severity::Error },
        { Diagnostic::IncompatibleTypeQualifiers,
                    "incompatible type qualifiers", Severity::Error },
        { Diagnostic::IncompatibleDeclarationAttributes,
                    "incompatible declaration attributes", Severity::Error },
        { Diagnostic::DuplicateDeclarationAttribute,
                    "duplicate declaraiton attribute", Severity::Error },
        { Diagnostic::UnexpectedVisibilityAttribute,
                    "unexpected visibility attribute", Severity::Error },
        { Diagnostic::UnexpectedDirectionAttribute,
                    "unexpected direction attribute", Severity::Error },
        { Diagnostic::MultipleDirectionAttributes,
                    "multiple direction attribute", Severity::Error },
        { Diagnostic::UnexpectedEvaluationStrategyAttribute,
                    "unexpected evaluation strategy attribute", Severity::Error },
        { Diagnostic::MultipleEvaluationStrategyAttributes,
                    "multiple evaluation strategy attribute", Severity::Error },
        { Diagnostic::UnexpectedStorageClassAttribute,
                    "unexpected storage class attribute", Severity::Error },
        { Diagnostic::UnexpectedLinkageAttribute,
                    "unexpected linkage attribute", Severity::Error },
        { Diagnostic::ExpressionOfUnexpectedType,
                    "expression of unexpected type", Severity::Error },
        { Diagnostic::InitializersDoNotMatchDeclarations,
                    "initializers do not match declarations", Severity::Warning },
        { Diagnostic::UnresolvedModule,
                    "unresolved module", Severity::Error },
        { Diagnostic::ModuleNameDoesNotMatchFileName,
                    "module name does not match file name", Severity::Warning },
        { Diagnostic::PackageNameDoesNotMatchDirName,
                    "package name does not match dir name", Severity::Warning },
        { Diagnostic::MultipleInitializers,
                    "multiple initializers", Severity::Error },
        { Diagnostic::NumericValueExpected,
                    "numeric type expected", Severity::Error },
        { Diagnostic::ArrayValueExpected,
                    "array value expected", Severity::Error },
        { Diagnostic::IncompatibleTypes,
                    "incompatible types", Severity::Error },
        { Diagnostic::BooleanValueExpected,
                    "boolean value expected", Severity::Error },
        { Diagnostic::IntegerValueExpected,
                    "integer value expected", Severity::Error },
        { Diagnostic::StringValueExpected,
                    "string value expected", Severity::Error },
        { Diagnostic::UnknownType,
                    "unknown type", Severity::Error },
        { Diagnostic::ExtraInitializer,
                    "extra initializer", Severity::Error },
        { Diagnostic::IncompatibleAssignment,
                    "incompatible assignment", Severity::Error },
        { Diagnostic::IncompatibleInitialization,
                    "incompatible initialization", Severity::Error },
        { Diagnostic::IncompatibleComparison,
                    "incompatible comparison", Severity::Error },
        { Diagnostic::CircularType,
                    "circular type", Severity::Error },
        { Diagnostic::IncompatibleCondition,
                    "incompatible condition", Severity::Error },
        { Diagnostic::UndeclaredIdentifier,
                    "undeclared identifier", Severity::Error },
        { Diagnostic::ValueWillBeTruncated,
                    "value will be truncated", Severity::Warning },
        { Diagnostic::ValueMayLoosePrecision,
                    "value may loose precision", Severity::Warning },
        { Diagnostic::IdentifierExpected,
                    "identifier expected", Severity::Error },
        { Diagnostic::UnexpectedToken,
                    "syntax error, unexpected token", Severity::Error },
        { Diagnostic::SubrangeValueExpected,
                    "subrange value expected", Severity::Error },
        { Diagnostic::UnknownEscapeSequence,
                    "unknown escape sequence", Severity::Error },
        { Diagnostic::UnterminatedString,
                    "unterminated string", Severity::Error },
        { Diagnostic::InvalidBinaryDigit,
                    "invalid binary digit", Severity::Error },
        { Diagnostic::InvalidOctalDigit,
                    "invalid octal digit", Severity::Error },
        { Diagnostic::InvalidHexDigit,
                    "invalid hex digit", Severity::Error },
        { Diagnostic::InvalidFloatSuffix,
                    "invalid float suffix", Severity::Error },
        { Diagnostic::InvalidReferenceToSelf,
                    "invalid reference to self", Severity::Error },
        { Diagnostic::UnexpectedName,
                    "unexpected name", Severity::Error },
        { Diagnostic::UnmatchedStringJoining,
                    "unmatched string joining", Severity::Error },
        { Diagnostic::InvalidOperator,
                    "invalid operator", Severity::Error },
    };
};

} // namespace uaiso

static const DiagnosticTable kDiagnosticTable;

    //--- Diagnostic ---//

Diagnostic::Diagnostic(Code code,
                       const std::string& desc,
                       Severity severity)
    : code_(code)
    , desc_(desc)
    , severity_(severity)
{}

    //--- DiagnosticReport ---//

DiagnosticReport::DiagnosticReport(Diagnostic::Code code,
                                   const SourceLoc &loc)
    : code_(code)
    , loc_(loc)
{}

Diagnostic DiagnosticReport::diagnostic() const
{
    return kDiagnosticTable.find(code_);
}

const SourceLoc& DiagnosticReport::sourceLoc() const
{
    return loc_;
}

    //--- DiagnosticReports ---//

void DiagnosticReports::add(const DiagnosticReport& report)
{
    reports_.push_back(report);
}

void DiagnosticReports::add(const Diagnostic::Code code, const SourceLoc& loc)
{
    add(DiagnosticReport(code, loc));
}

void DiagnosticReports::add(const Diagnostic::Code code,
                            Ast* ast,
                            const AstLocator* locator)
{
    add(DiagnosticReport(code, fullLoc(ast, locator)));
}

void DiagnosticReports::add(const Diagnostic::Code code,
                            Ast* ast,
                            const std::unique_ptr<const AstLocator>& locator)
{
    add(code, ast, locator.get());
}
