# ./source/services/runtimeService/CMakeLists.txt

# RuntimeService component library for nsbaci.

# Subdirectories

    add_subdirectory(program)
    add_subdirectory(scheduler) # defines thread library
    add_subdirectory(interpreter)

# nsbaci_runtimeService_library

    add_library(nsbaci_runtimeService_library STATIC
        runtimeService.cpp
        runtimeService.h
    )

# Include path

    target_include_directories(nsbaci_runtimeService_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_runtimeService_library PUBLIC
        config_compiler_flags_library
        nsbaci_scheduler_library
        nsbaci_program_library
        nsbaci_interpreter_library
    )
