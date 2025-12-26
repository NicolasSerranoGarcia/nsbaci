# ./source/types/CMakeLists.txt

# Types component library for nsbaci.
# Defines centralized type definitions used across the project.

# nsbaci_types_library

    add_library(nsbaci_types_library INTERFACE)

# Include path

    target_include_directories(nsbaci_types_library INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
