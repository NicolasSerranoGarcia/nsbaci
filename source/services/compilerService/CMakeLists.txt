# ./source/services/compilerService/CMakeLists.txt

# CompilerService component library for nsbaci.
# Defines the compilation-related service.

# nsbaci_compilerService_library

    add_library(nsbaci_compilerService_library STATIC
        compilerService.cpp
        compilerService.h
    )

# Include path

    target_include_directories(nsbaci_compilerService_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_compilerService_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
        nsbaci_error_library
    )
