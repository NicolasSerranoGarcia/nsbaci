# ./source/services/runtimeService/CMakeLists.txt

# RuntimeService component library for nsbaci.

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
        nsbaci_types_library
        nsbaci_error_library
    )
