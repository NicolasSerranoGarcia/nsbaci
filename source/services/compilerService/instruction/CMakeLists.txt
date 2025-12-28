# ./source/services/compilerService/instruction/CMakeLists.txt

# Instruction component library for nsbaci compiler service.

# Subdirectories

    add_subdirectory(instructionSet)

# nsbaci_instruction_library

    add_library(nsbaci_instruction_library STATIC
        instruction.h
        instruction.cpp
    )

# Include path

    target_include_directories(nsbaci_instruction_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_instruction_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
        nsbaci_instructionSet_library
    )
