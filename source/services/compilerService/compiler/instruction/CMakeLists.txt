# ./source/services/compilerService/compiler/instruction/CMakeLists.txt

# Instruction component library for nsbaci compiler.

# Subdirectories

    add_subdirectory(instructionSet)

# nsbaci_compilerInstruction_library

    add_library(nsbaci_compilerInstruction_library STATIC
        instruction.h
        instruction.cpp
    )

# Include path

    target_include_directories(nsbaci_compilerInstruction_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_compilerInstruction_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
        nsbaci_compilerInstructionSet_library
    )
