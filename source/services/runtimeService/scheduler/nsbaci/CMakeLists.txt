# ./source/services/runtimeService/scheduler/nsbaci/CMakeLists.txt

# NsbaciScheduler component library for nsbaci runtime service.
# Concrete implementation of the Scheduler interface for BACI.

# nsbaci_nsbaciScheduler_library

    add_library(nsbaci_nsbaciScheduler_library STATIC
        nsbaciScheduler.cpp
        nsbaciScheduler.h
    )

# Include path

    target_include_directories(nsbaci_nsbaciScheduler_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_nsbaciScheduler_library PUBLIC
        config_compiler_flags_library
        nsbaci_scheduler_library
    )
