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

#include "Ast/Ast.h"
#include "Ast/AstVisitor.h"
#include "Ast/AstDumper.h"
#include "Common/Assert.h"
#include "Common/FileInfo.h"
#include "Common/Test.h"
#include "D/DIncrementalLexer.h"
#include "D/DSanitizer.h"
#include "D/DUnit.h"
#include "Go/GoIncrementalLexer.h"
#include "Go/GoSanitizer.h"
#include "Go/GoUnit.h"
#include "Haskell/HsLexer.h"
#include "Parsing/Factory.h"
#include "Parsing/LexemeMap.h"
#include "Parsing/TokenMap.h"
#include "Parsing/Unit.h"
#include "Python/PyLexer.h"
#include "Python/PyParser.h"
#include "Semantic/Binder.h"
#include "Semantic/CompletionProposer.h"
#include "Semantic/CompletionTest.h"
#include "Semantic/Environment.h"
#include "Semantic/ImportResolver.h"
#include "Semantic/Program.h"
#include "Semantic/Sanitizer.h"
#include "Semantic/Snapshot.h"
#include "Semantic/Symbol.h"
#include "Semantic/Type.h"
#include "Semantic/TypeChecker.h"
#include "StringUtils/string.hpp"
#include <cstring>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using namespace uaiso;

/*!
 * Read searchs paths to feed the tests. The absolute path of the test
 * data should have been configured (by a Python script) in the first build.
 */
std::vector<std::string> readSearchPaths()
{
    using Paths = std::vector<std::string>;

    std::ifstream ifs("TestDataAbsPath.txt");
    if (!ifs.is_open())
        return Paths();

    std::string path;
    if (!std::getline(ifs, path))
        return Paths();

    Paths paths;
    paths.push_back(path + "/Go/");
    paths.push_back(path + "/Python/");

    return paths;
}

CALL_CLASS_TEST(Binder)
CALL_CLASS_TEST(DIncrementalLexer)
CALL_CLASS_TEST(DUnit)
CALL_CLASS_TEST(CompletionProposer)
CALL_CLASS_TEST(Environment)
CALL_CLASS_TEST(FileInfo)
CALL_CLASS_TEST(GoIncrementalLexer)
CALL_CLASS_TEST(GoUnit)
CALL_CLASS_TEST(HsLexer)
CALL_CLASS_TEST(PyLexer)
CALL_CLASS_TEST(PyParser)
CALL_CLASS_TEST(TypeChecker)

class WorkflowTest : public Test
{
public:
    TEST_RUN(WorkflowTest, &WorkflowTest::testAll)

    WorkflowTest()
        : debug_(false)
        , singlePass_(false)
        , file_(nullptr)
    {}

    void testAll()
    {
        if (singlePass_) {
            testParse();
        } else {
           for (auto fileName : testFiles_) {
                fileName_ = fileName;
                testParse();
            }
        }
    }

    void testParse()
    {
        std::cout << fileName_ << std::endl;

        file_ = fopen(fileName_.c_str(), "r");
        if (!file_) {
            UAISO_FAIL_TEST("cannot open file");
            return;
        }

        std::unique_ptr<Factory> factory;
        if (str::ends_with(fileName_, ".d")) {
            factory = std::move(FactoryCreator::create(LangId::D));
        } else if (str::ends_with(fileName_, ".go")) {
            factory = std::move(FactoryCreator::create(LangId::Go));
        } else if (str::ends_with(fileName_, ".py")) {
            factory = std::move(FactoryCreator::create(LangId::Py));
        } else {
            UAISO_FAIL_TEST("unrecognized file suffix");
            return;
        }

        std::unique_ptr<Unit> unit(factory->makeUnit());
        unit->setFileName(fileName_);
        unit->assignInput(file_);
        unit->parse(&tokens_, &lexs_);
        fclose(file_);
        std::unique_ptr<DiagnosticReports> reports(unit->releaseReports());
        UAISO_EXPECT_INT_EQ(0, reports->size());
        UAISO_EXPECT_TRUE(unit->ast());
        ProgramAst* progAst = Program_Cast(unit->ast());

        Binder binder(factory.get());
        binder.setLexemes(&lexs_);
        binder.setTokens(&tokens_);
        binder.collectDiagnostics(reports.get());
        UAISO_EXPECT_INT_EQ(0, reports->size());
        std::unique_ptr<Program> prog(binder.bind(progAst, fileName_));
        UAISO_EXPECT_TRUE(prog);
        UAISO_EXPECT_FALSE(prog->env().isEmpty());

        TypeChecker typeChecker(factory.get());
        typeChecker.setLexemes(&lexs_);
        typeChecker.setTokens(&tokens_);
        typeChecker.collectDiagnostics(reports.get());
        UAISO_EXPECT_INT_EQ(0, reports->size());
        typeChecker.check(progAst);
    }

    bool debug_;
    bool singlePass_;
    FILE* file_;
    std::string fileName_;
    LexemeMap lexs_;
    TokenMap tokens_;

    std::vector<std::string> testFiles_
    {
        //--- Go files ---//

        "TestData/Go/testGoTour_Append.go",
        "TestData/Go/testGoTour_Array.go",
        "TestData/Go/testGoTour_BasicTypes.go",
        "TestData/Go/testGoTour_BufferChannels.go",
        "TestData/Go/testGoTour_Channels.go",
        "TestData/Go/testGoTour_Constants.go",
        "TestData/Go/testGoTour_DefaultSelection.go",
        "TestData/Go/testGoTour_Defer.go",
        "TestData/Go/testGoTour_DeferMulti.go",
        "TestData/Go/testGoTour_Errors.go",
        "TestData/Go/testGoTour_ExportedNames.go",
        "TestData/Go/testGoTour_For.go",
        "TestData/Go/testGoTour_ForContinued.go",
        "TestData/Go/testGoTour_ForIsGosWhile.go",
        "TestData/Go/testGoTour_Forever.go",
        "TestData/Go/testGoTour_FunctionClosures.go",
        "TestData/Go/testGoTour_FunctionValues.go",
        "TestData/Go/testGoTour_Functions.go",
        "TestData/Go/testGoTour_Functions_Ext.go",
        "TestData/Go/testGoTour_FunctionsContinued.go",
        "TestData/Go/testGoTour_GoRoutines.go",
        "TestData/Go/testGoTour_If.go",
        "TestData/Go/testGoTour_IfAndElse.go",
        "TestData/Go/testGoTour_IfWithShortStatement.go",
        "TestData/Go/testGoTour_Imports.go",
        "TestData/Go/testGoTour_Interfaces.go",
        "TestData/Go/testGoTour_InterfacesSatisifiedImplicitly.go",
        "TestData/Go/testGoTour_MakingSlices.go",
        "TestData/Go/testGoTour_MapLiterals.go",
        "TestData/Go/testGoTour_MapLiteralsContinued.go",
        "TestData/Go/testGoTour_Maps.go",
        "TestData/Go/testGoTour_Methods.go",
        "TestData/Go/testGoTour_MethodsContinued.go",
        "TestData/Go/testGoTour_MethodsWithPointerReceivers.go",
        "TestData/Go/testGoTour_MultipleResults.go",
        "TestData/Go/testGoTour_MutatingMaps.go",
        "TestData/Go/testGoTour_NamedResults.go",
        "TestData/Go/testGoTour_NilSlices.go",
        "TestData/Go/testGoTour_NumericConstants.go",
        "TestData/Go/testGoTour_Packages.go",
        "TestData/Go/testGoTour_Pointers.go",
        "TestData/Go/testGoTour_Range.go",
        "TestData/Go/testGoTour_RangeAndClose.go",
        "TestData/Go/testGoTour_RangeContinued.go",
        "TestData/Go/testGoTour_Readers.go",
        "TestData/Go/testGoTour_Select.go",
        "TestData/Go/testGoTour_ShortVariableDeclarations.go",
        "TestData/Go/testGoTour_Slices.go",
        "TestData/Go/testGoTour_SlicingSlices.go",
        "TestData/Go/testGoTour_Stringer.go",
        "TestData/Go/testGoTour_StructFields.go",
        "TestData/Go/testGoTour_StructLiterals.go",
        "TestData/Go/testGoTour_StructPointers.go",
        "TestData/Go/testGoTour_Structs.go",
        "TestData/Go/testGoTour_Switch.go",
        "TestData/Go/testGoTour_SwitchNoCondition.go",
        "TestData/Go/testGoTour_TypeConversions.go",
        "TestData/Go/testGoTour_TypeInference.go",
        "TestData/Go/testGoTour_Variables.go",
        "TestData/Go/testGoTour_VariablesWithInitializers.go",
        "TestData/Go/testGoTour_WebServers.go",
        "TestData/Go/testGoTour_Zero.go",
        "TestData/Go/testMyOwn.go",
        "TestData/Go/testGoSource_runtime.go",
        "TestData/Go/Stdlib/fmt/export_test.go",
        "TestData/Go/Stdlib/fmt/fmt_test.go",
        "TestData/Go/Stdlib/fmt/format.go",
        "TestData/Go/Stdlib/fmt/print.go",
        "TestData/Go/Stdlib/fmt/scan.go",
        "TestData/Go/Stdlib/fmt/scan_test.go",
        "TestData/Go/Stdlib/fmt/stringer_test.go",
        "TestData/Go/Stdlib/math/abs.go",
        "TestData/Go/Stdlib/math/acosh.go",
        "TestData/Go/Stdlib/math/all_test.go",
        "TestData/Go/Stdlib/math/asin.go",
        "TestData/Go/Stdlib/math/asinh.go",
        "TestData/Go/Stdlib/math/atan.go",
        "TestData/Go/Stdlib/math/atan2.go",
        "TestData/Go/Stdlib/math/atanh.go",
        "TestData/Go/Stdlib/math/bits.go",
        "TestData/Go/Stdlib/math/cbrt.go",
        "TestData/Go/Stdlib/math/const.go",
        "TestData/Go/Stdlib/math/copysign.go",
        "TestData/Go/Stdlib/math/dim.go",
        "TestData/Go/Stdlib/math/erf.go",
        "TestData/Go/Stdlib/math/exp.go",
        "TestData/Go/Stdlib/math/expm1.go",
        "TestData/Go/Stdlib/math/export_test.go",
        "TestData/Go/Stdlib/math/floor.go",
        "TestData/Go/Stdlib/math/frexp.go",
        "TestData/Go/Stdlib/math/gamma.go",
        "TestData/Go/Stdlib/math/hypot.go",
        "TestData/Go/Stdlib/math/j0.go",
        "TestData/Go/Stdlib/math/j1.go",
        "TestData/Go/Stdlib/math/jn.go",
        "TestData/Go/Stdlib/math/ldexp.go",
        "TestData/Go/Stdlib/math/lgamma.go",
        "TestData/Go/Stdlib/math/log.go",
        "TestData/Go/Stdlib/math/log10.go",
        "TestData/Go/Stdlib/math/log1p.go",
        "TestData/Go/Stdlib/math/logb.go",
        "TestData/Go/Stdlib/math/mod.go",
        "TestData/Go/Stdlib/math/nextafter.go",
        "TestData/Go/Stdlib/math/pow.go",
        "TestData/Go/Stdlib/math/pow10.go",
        "TestData/Go/Stdlib/math/remainder.go",
        "TestData/Go/Stdlib/math/signbit.go",
        "TestData/Go/Stdlib/math/sin.go",
        "TestData/Go/Stdlib/math/sincos.go",
        "TestData/Go/Stdlib/math/sinh.go",
        "TestData/Go/Stdlib/math/sqrt.go",
        "TestData/Go/Stdlib/math/tan.go",
        "TestData/Go/Stdlib/math/tanh.go",
        "TestData/Go/Stdlib/math/unsafe.go",
        "TestData/Go/Stdlib/math/big/arith.go",
        "TestData/Go/Stdlib/math/big/arith_decl.go",
        "TestData/Go/Stdlib/math/big/arith_test.go",
        "TestData/Go/Stdlib/math/big/calibrate_test.go",
        "TestData/Go/Stdlib/math/big/example_test.go",
        "TestData/Go/Stdlib/math/big/gcd_test.go",
        "TestData/Go/Stdlib/math/big/hilbert_test.go",
        "TestData/Go/Stdlib/math/big/int.go",
        "TestData/Go/Stdlib/math/big/int_test.go",
        "TestData/Go/Stdlib/math/big/nat.go",
        "TestData/Go/Stdlib/math/big/nat_test.go",
        "TestData/Go/Stdlib/math/big/rat.go",
        "TestData/Go/Stdlib/math/big/rat_test.go",
        "TestData/Go/Stdlib/math/cmplx/abs.go",
        "TestData/Go/Stdlib/math/cmplx/asin.go",
        "TestData/Go/Stdlib/math/cmplx/cmath_test.go",
        "TestData/Go/Stdlib/math/cmplx/conj.go",
        "TestData/Go/Stdlib/math/cmplx/exp.go",
        "TestData/Go/Stdlib/math/cmplx/isinf.go",
        "TestData/Go/Stdlib/math/cmplx/isnan.go",
        "TestData/Go/Stdlib/math/cmplx/log.go",
        "TestData/Go/Stdlib/math/cmplx/phase.go",
        "TestData/Go/Stdlib/math/cmplx/pow.go",
        "TestData/Go/Stdlib/math/cmplx/sin.go",
        "TestData/Go/Stdlib/math/cmplx/sqrt.go",
        "TestData/Go/Stdlib/math/cmplx/tan.go",

        //--- D files ---//

        "TestData/D/test1.d",
        "TestData/D/test2.d",
        "TestData/D/test3.d",
        "TestData/D/test4.d",
        "TestData/D/test5.d",
        "TestData/D/test6.d",
        "TestData/D/test7.d",
        "TestData/D/test8.d",
        "TestData/D/test9.d",
        "TestData/D/widg.d",
        "TestData/D/testTDPL_1.d",
        "TestData/D/testTDPL_2.d",
        "TestData/D/testTDPL_3.d",
        "TestData/D/testTDPL_4.d",
        "TestData/D/testTDPL_5.d",
        "TestData/D/testTDPL_6.d",
        "TestData/D/testTDPL_7.d",
        "TestData/D/testTDPL_8.d",
        "TestData/D/testTDPL_9.d",
        "TestData/D/testTDPL_10.d",
        "TestData/D/testTDPL_11.d",
        "TestData/D/testTDPL_12.d",

        //--- Python files ---//

        "Scripts/GenAstCast.py",
        "Scripts/GenAstVisitor.py",
        "Scripts/GenFlexBison.py",
        "Scripts/GenTokens.py",
        "TestData/Python/Cookie.py",
        "TestData/Python/copy.py",
        "TestData/Python/fileinput.py",
        "TestData/Python/getopt.py",
        "TestData/Python/glob.py",
        "TestData/Python/gzip.py",
        "TestData/Python/HTMLParser.py",
        "TestData/Python/hashlib.py",
        "TestData/Python/io.py",
        "TestData/Python/os.py",
        "TestData/Python/popen2.py",
        "TestData/Python/random.py",
        "TestData/Python/re.py",
        "TestData/Python/StringIO.py",
        "TestData/Python/sets.py",
        "TestData/Python/ssl.py",
        "TestData/Python/string.py",
        "TestData/Python/urllib.py",
        "TestData/Python/xmllib.py",
        "TestData/Python/zipfile.py"
    };
};

int main(int argc, char* argv[])
{
    WorkflowTest workflowTest;
    if (argc > 1 && !strcmp(argv[1], "-d")) {
        workflowTest.debug_ = true;
        --argc;
        ++argv;
    }
    if (argc > 1) {
        workflowTest.singlePass_ = true;
        workflowTest.fileName_ = argv[1];
    }

//    workflowTest.run();

    if (!workflowTest.singlePass_) {
//        test_FileInfo();
//        test_Environment();
//        test_Binder();
//        test_TypeChecker();
//        test_CompletionProposer();
//        test_DIncrementalLexer();
//        test_DUnit();
//        test_GoIncrementalLexer();
//        test_GoUnit();
//        test_PyLexer();
//        test_PyParser();
        test_HsLexer();
    }

    Test::printStats();

    return 0;
}
