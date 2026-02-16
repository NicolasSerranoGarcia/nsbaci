# ./source/services/runtimeService/program/CMakeLists.txt

# Program component library for nsbaci runtime service.

# Subdirectories

    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../semaphore
        ${CMAKE_BINARY_DIR}/semaphore)

# nsbaci_program_library

    add_library(nsbaci_program_library STATIC
        program.cpp
        program.h
    )

# Include path

    target_include_directories(nsbaci_program_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_program_library PUBLIC
        config_compiler_flags_library
        nsbaci_compilerInstruction_library
        nsbaci_types_library
        nsbaci_semaphore_library
    )
