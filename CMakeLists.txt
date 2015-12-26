cmake_minimum_required(VERSION 2.8)

project(uaiso)

set(UAISO_CXX_FLAGS)
set(UAISO_CXX_FLAGS "${UAISO_CXX_FLAGS} -std=c++11 -g -Wno-deprecated-register")
set(UAISO_CXX_FLAGS "${UAISO_CXX_FLAGS} -fvisibility=hidden -DEXPORT_API")

set(SCRIPTS_PATH Scripts)
set(PARSING_PATH Parsing)
set(AST_PATH Ast)
set(EXTERNAL_PATH External)
set(COMMON_PATH Common)
set(SEMANTIC_PATH Semantic)
set(D_PARSER_PATH D)
set(GO_PARSER_PATH Go)
set(PY_PARSER_PATH Python)

# File generation.
add_custom_command(
    OUTPUT ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DLexer.h
           ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DLexer.cpp
           ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DParser.h
           ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DParser.cpp
           ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoLexer.h
           ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoLexer.cpp
           ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoParser.h
           ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoParser.cpp
           ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstCast.h
           ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Token.h
           ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/TokenName.cpp
           ${PROJECT_SOURCE_DIR}/TestDataAbsPath.txt
    COMMAND python ${PROJECT_SOURCE_DIR}/${SCRIPTS_PATH}/GenFlexBison.py
    COMMAND python ${PROJECT_SOURCE_DIR}/${SCRIPTS_PATH}/GenAstCast.py
    COMMAND python ${PROJECT_SOURCE_DIR}/${SCRIPTS_PATH}/GenTokens.py
    COMMAND python ${PROJECT_SOURCE_DIR}/${SCRIPTS_PATH}/ConfigTestDataPath.py
)

set(UAISO_TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/Main.cpp
    # Common
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/FileInfoTest.cpp
    # D
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DCompletionTest.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DIncrementalLexerTest.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DUnitTest.cpp
    # Go
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoBinderTest.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoCompletionTest.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoIncrementalLexerTest.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoTypeCheckerTest.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoUnitTest.cpp
    # Parsing
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/UnitTest.h
    # Python
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyBinderTest.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyCompletionTest.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyLexerTest.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyParserTest.cpp
    # Semantic
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/BinderTest.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/BinderTest.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/CompletionTest.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/CompletionTest.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/EnvironmentTest.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/TypeCheckerTest.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/TypeCheckerTest.h
)

set(UAISO_SOURCES
    # Ast
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstBase.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstDecl.cpp
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstDecl.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstDumper.cpp
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstDumper.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstExpr.cpp
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstExpr.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstList.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstLocator.cpp
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstLocator.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstName.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstSerializer.cpp
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstSerializer.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstSpec.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstSpec.cpp
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstStmt.h
    ${PROJECT_SOURCE_DIR}/${AST_PATH}/AstVisitor.h
    # Common
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Assert.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Config.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Error.cpp
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Error__.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/FileInfo.cpp
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/FileInfo.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Flag.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/LineCol.cpp
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/LineCol.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Pimpl.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Test.cpp
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Test.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Trace.cpp
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Trace__.h
    ${PROJECT_SOURCE_DIR}/${COMMON_PATH}/Util__.h
    # D
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DAstLocator.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DAstLocator.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DFactory.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DFactory.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DFlexBison.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DFlexBison__.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DIncrementalLexer.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DIncrementalLexer.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DLang.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DLang.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DLexer.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DLexer.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DParser.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DParser.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DParsingContext.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DParsingContext.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DSanitizer.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DSanitizer.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DTypeSystem.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DTypeSystem.h
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DUnit.cpp
    ${PROJECT_SOURCE_DIR}/${D_PARSER_PATH}/DUnit.h
    # Go
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoAstLocator.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoAstLocator.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoFactory.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoFactory.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoFlexBison.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoFlexBison__.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoIncrementalLexer.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoIncrementalLexer.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoLang.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoLang.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoLexer.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoLexer.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoParser.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoParser.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoParsingContext.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoParsingContext.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoSanitizer.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoSanitizer.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoTypeSystem.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoTypeSystem.h
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoUnit.cpp
    ${PROJECT_SOURCE_DIR}/${GO_PARSER_PATH}/GoUnit.h
    # Parsing
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/DataIndex.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Diagnostic.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Diagnostic.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Factory.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Factory.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/FlexBison.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/FlexBison__.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/IncrementalLexer.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/IncrementalLexer.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Lang.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Lang.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Lexeme.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Lexeme.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/LexemeMap.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Lexer.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Lexer.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Parser.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/ParsingContext.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/ParsingContext.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Phrasing.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Phrasing.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/SourceLoc.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Token.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/TokenMap.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/TokenName.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Unit.cpp
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Unit.h
    ${PROJECT_SOURCE_DIR}/${PARSING_PATH}/Unit__.h
    # Python
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyAstLocator.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyAstLocator.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyBuiltins.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/Pybuiltins.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyFactory.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyFactory.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyIncrementalLexer.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyIncrementalLexer.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyKeywords.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyKeywords.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyLang.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyLang.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyLexer.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyLexer.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyParser.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyParser.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PySanitizer.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PySanitizer.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyTypeSystem.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyTypeSystem.h
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyUnit.cpp
    ${PROJECT_SOURCE_DIR}/${PY_PARSER_PATH}/PyUnit.h
    # Semantic
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Binder.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Binder.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Builtins.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Builtins.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/CompletionProposer.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/CompletionProposer.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/DeclAttrs.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Environment.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Environment.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/ImportResolver.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/ImportResolver.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Manager.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Manager.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Program.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Program.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Sanitizer.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Sanitizer.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Snapshot.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Snapshot.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Symbol.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Symbol.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/SymbolCollector.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/SymbolCollector.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Type.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/Type.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/TypeChecker.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/TypeChecker.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/TypeQuals.h
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/TypeSystem.cpp
    ${PROJECT_SOURCE_DIR}/${SEMANTIC_PATH}/TypeSystem.h
)

foreach(file ${UAISO_TEST_SOURCES})
    set_source_files_properties(
        ${file} PROPERTIES
        COMPILE_FLAGS "${UAISO_CXX_FLAGS}"
    )
endforeach()

foreach(file ${UAISO_SOURCES})
    set_source_files_properties(
        ${file} PROPERTIES
        COMPILE_FLAGS "${UAISO_CXX_FLAGS}"
    )
endforeach()

include_directories(
    .
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/${EXTERNAL_PATH}
)

if(BUILD_STATIC)
    set(UAISO_LIB_TYPE STATIC)
    message(STATUS "Static lib")
else()
    set(UAISO_LIB_TYPE SHARED)
    message(STATUS "Shared lib")
endif()

set(UAISO_LIB UaiSoEngine)
add_library(${UAISO_LIB} ${UAISO_LIB_TYPE} ${UAISO_SOURCES})

set(UAISO_TEST UaiSoEngineTest)
add_executable(${UAISO_TEST} ${UAISO_TEST_SOURCES})

target_link_libraries(${UAISO_TEST} ${UAISO_LIB})
