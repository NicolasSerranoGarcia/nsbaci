# ./source/services/compilerService/compiler/CMakeLists.txt

# Abstract Compiler component library for nsbaci.

# Subdirectories

    add_subdirectory(instruction)
    add_subdirectory(nsbaci)

# nsbaci_compiler_library

    add_library(nsbaci_compiler_library STATIC
        compiler.h
    )

# Include path

    target_include_directories(nsbaci_compiler_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_compiler_library PUBLIC
        config_compiler_flags_library
        nsbaci_baseResult_library
        nsbaci_compilerInstruction_library
        nsbaci_nsbaciCompiler_library
    )
