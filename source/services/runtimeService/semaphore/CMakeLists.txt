# ./source/services/runtimeService/program/CMakeLists.txt

# Program component library for nsbaci runtime service.

# nsbaci_semaphore_library

    add_library(nsbaci_semaphore_library STATIC
        semaphore.cpp
        semaphore.h
    )

# Include path

    target_include_directories(nsbaci_semaphore_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_semaphore_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
        nsbaci_thread_library
    )
