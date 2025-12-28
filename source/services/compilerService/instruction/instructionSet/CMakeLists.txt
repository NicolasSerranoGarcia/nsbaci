# ./source/services/runtimeService/instruction/InstructionSet/CMakeLists.txt

# InstructionSet interface library for nsbaci runtime service.

# nsbaci_instructionSet_library

    add_library(nsbaci_instructionSet_library INTERFACE)

# Include path

    target_include_directories(nsbaci_instructionSet_library INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
