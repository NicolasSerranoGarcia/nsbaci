# ./source/services/runtimeService/thread/CMakeLists.txt

# Thread component library for nsbaci runtime service.

# nsbaci_thread_library

    add_library(nsbaci_thread_library STATIC
        thread.cpp
        thread.h
    )

# Include path

    target_include_directories(nsbaci_thread_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_thread_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
    )
