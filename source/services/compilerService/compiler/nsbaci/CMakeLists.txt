# ./source/services/compilerService/compiler/nsbaci/CMakeLists.txt

# NsbaciCompiler component library for nsbaci.
# Concrete implementation of the Compiler interface using flex/bison.

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Find FlexLexer.h include directory
# On MSYS2/MinGW, FlexLexer.h is in /usr/include (MSYS layer)
if(WIN32)
    # Check common MSYS2 paths
    if(EXISTS "C:/msys64/usr/include/FlexLexer.h")
        set(FLEX_INCLUDE_DIR "C:/msys64/usr/include")
    elseif(EXISTS "/usr/include/FlexLexer.h")
        set(FLEX_INCLUDE_DIR "/usr/include")
    endif()
else()
    find_path(FLEX_INCLUDE_DIR FlexLexer.h)
endif()

message(STATUS "FLEX_INCLUDE_DIR: ${FLEX_INCLUDE_DIR}")

# Generate parser and lexer

    set(PARSER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

    BISON_TARGET(NsbaciParser
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.y
        ${PARSER_OUTPUT_DIR}/parser.tab.cc
        DEFINES_FILE ${PARSER_OUTPUT_DIR}/parser.tab.hh
    )

    FLEX_TARGET(NsbaciLexer
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
        ${PARSER_OUTPUT_DIR}/lex.yy.cc
    )

    ADD_FLEX_BISON_DEPENDENCY(NsbaciLexer NsbaciParser)

# nsbaci_nsbaciCompiler_library

    add_library(nsbaci_nsbaciCompiler_library STATIC
        nsbaciCompiler.cpp
        nsbaciCompiler.h
        lexer.h
        ${BISON_NsbaciParser_OUTPUTS}
        ${FLEX_NsbaciLexer_OUTPUTS}
    )

# Include paths

    target_include_directories(nsbaci_nsbaciCompiler_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${PARSER_OUTPUT_DIR}
        ${FLEX_INCLUDE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_nsbaciCompiler_library PUBLIC
        config_compiler_flags_library
        nsbaci_baseResult_library
        nsbaci_compilerInstruction_library
    )

# Subdirectories

    add_subdirectory(tools)
