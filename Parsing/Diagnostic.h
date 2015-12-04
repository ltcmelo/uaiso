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

#ifndef UAISO_DIAGNOSTIC_H__
#define UAISO_DIAGNOSTIC_H__

#include "Ast/AstFwd.h"
#include "Common/Config.h"
#include "Parsing/Severity.h"
#include "Parsing/SourceLoc.h"
#include <iterator>
#include <string>
#include <vector>

namespace uaiso {

class AstLocator;

/*!
 * \brief The Diagnostic class
 *
 * A parsing diagnostic.
 */
class UAISO_API Diagnostic
{
public:
    using Code = size_t;

    Code code() const { return code_; }

    const std::string& desc() const { return desc_; }

    Severity severity() const { return severity_; }

    static const Code VariableNotInitialized;
    static const Code UnknownEnvironmentName;
    static const Code MultipleVisibilityAttributes;
    static const Code UnexpectedVisibilityAttribute;
    static const Code MultipleStorageSpecifiers;
    static const Code MultipleLinkageSpecifiers;
    static const Code DuplicateTypeQualifier;
    static const Code UnexpectedTypeQualifier;
    static const Code IncompatibleTypeQualifiers;
    static const Code DuplicateDeclarationAttribute;
    static const Code UnexpectedDeclarationAttribute;
    static const Code IncompatibleDeclarationAttributes;
    static const Code UnexpectedDirectionAttribute;
    static const Code MultipleDirectionAttributes;
    static const Code UnexpectedEvaluationStrategyAttribute;
    static const Code MultipleEvaluationStrategyAttributes;
    static const Code UnexpectedStorageClassAttribute;
    static const Code UnexpectedLinkageAttribute;
    static const Code ExpressionOfUnexpectedType;
    static const Code InitializersDoNotMatchDeclarations;
    static const Code UnresolvedModule;
    static const Code ModuleNameDoesNotMatchFileName;
    static const Code PackageNameDoesNotMatchDirName;
    static const Code MultipleInitializers;
    static const Code IntegerValueExpected;
    static const Code NumericValueExpected;
    static const Code ArrayValueExpected;
    static const Code BooleanValueExpected;
    static const Code StringValueExpected;
    static const Code UnknownType;
    static const Code ExtraInitializer;
    static const Code IncompatibleInitialization;
    static const Code IncompatibleAssignment;
    static const Code IncompatibleComparison;
    static const Code CircularType;
    static const Code IncompatibleTypes;
    static const Code IncompatibleCondition;
    static const Code UndeclaredIdentifier;
    static const Code ValueWillBeTruncated;
    static const Code ValueMayLoosePrecision;
    static const Code NameRequired;
    static const Code UnexpectedToken;
    static const Code SubrangeValueExpected;
    static const Code UnknownEscapeSequence;
    static const Code UnterminatedString;
    static const Code InvalidBinaryDigit;
    static const Code InvalidOctalDigit;
    static const Code InvalidHexDigit;
    static const Code InvalidFloatSuffix;
    static const Code InvalidReferenceToSelf;

private:
    friend class DiagnosticTable;

    Diagnostic(Code code,
               const std::string& desc,
               Severity severity);

    Code code_;
    std::string desc_;
    Severity severity_;
};

inline bool operator==(const Diagnostic& a, const Diagnostic& b)
{
    return a.code() == b.code();
}
inline bool operator!=(const Diagnostic& a, const Diagnostic& b)
{
    return !(a == b);
}

/*!
 * \brief The DiagnosticReport class
 *
 * A diagnostic report.
 */
class UAISO_API DiagnosticReport final
{
public:
    DiagnosticReport(Diagnostic::Code code, const SourceLoc& loc);

    Diagnostic diagnostic() const;

    const SourceLoc& sourceLoc() const;

private:
    Diagnostic::Code code_;
    SourceLoc loc_;
};

/*!
 * \brief The DiagnosticReports class
 */
class UAISO_API DiagnosticReports final
{
private:
    using Cont = std::vector<DiagnosticReport>;
    Cont reports_;

public:

    using Iterator = std::vector<DiagnosticReport>::const_iterator;

    Iterator begin() const { return reports_.begin(); }

    Iterator end() const { return reports_.end(); }

    Cont::size_type size() const { return reports_.size(); }

    void add(const DiagnosticReport& report);

    void add(const Diagnostic::Code code, const SourceLoc& loc);

    void add(const Diagnostic::Code code, Ast* ast, const AstLocator* locator);
};

} // namespace uaiso

#endif
