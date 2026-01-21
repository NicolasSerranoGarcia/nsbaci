# ./source/services/compilerService/compiler/instruction/instructionSet/CMakeLists.txt

# InstructionSet interface library for nsbaci compiler.
# This folder will contain concrete instruction implementations.

# nsbaci_compilerInstructionSet_library

    add_library(nsbaci_compilerInstructionSet_library INTERFACE)

# Include path

    target_include_directories(nsbaci_compilerInstructionSet_library INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
