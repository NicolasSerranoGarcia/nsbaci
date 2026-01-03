# ./source/services/runtimeService/scheduler/CMakeLists.txt

# Scheduler component library for nsbaci runtime service.

# Subdirectories

    add_subdirectory(${CMAKE_SOURCE_DIR}/source/services/runtimeService/thread
        ${CMAKE_BINARY_DIR}/source/services/runtimeService/thread
    )

    add_subdirectory(nsbaci)

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
        nsbaci_thread_library
        nsbaci_nsbaciScheduler_library
    )
