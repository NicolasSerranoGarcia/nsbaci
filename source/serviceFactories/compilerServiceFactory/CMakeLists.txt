# ./source/serviceFactories/compilerServiceFactory/CMakeLists.txt

# CompilerServiceFactory component library for nsbaci.
# Factory for creating CompilerService instances.

# nsbaci_compilerServiceFactory_library

    add_library(nsbaci_compilerServiceFactory_library STATIC
        compilerServiceFactory.cpp
        compilerServiceFactory.h
    )

# Include path

    target_include_directories(nsbaci_compilerServiceFactory_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_compilerServiceFactory_library PUBLIC
        config_compiler_flags_library
        nsbaci_compilerService_library
    )
