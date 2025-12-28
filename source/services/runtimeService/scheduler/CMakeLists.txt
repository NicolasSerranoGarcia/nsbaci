# ./source/services/runtimeService/scheduler/CMakeLists.txt

# Scheduler component library for nsbaci runtime service.

# nsbaci_scheduler_library

    add_library(nsbaci_scheduler_library STATIC
        scheduler.cpp
        scheduler.h
    )

# Include path

    target_include_directories(nsbaci_scheduler_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_scheduler_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
        nsbaci_thread_library
    )
