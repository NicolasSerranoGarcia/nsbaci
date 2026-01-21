# ./source/errors/error/CMakeLists.txt

# Backend Error component library for nsbaci.
# Defines the core error handling structures and classes.

# nsbaci_error_library

    add_library(nsbaci_error_library STATIC
        error.h
    )

# Include path

    target_include_directories(nsbaci_error_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_error_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
    )
