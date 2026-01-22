# ./source/serviceFactories/runtimeServiceFactory/CMakeLists.txt

# RuntimeServiceFactory component library for nsbaci.
# Factory for creating RuntimeService instances.

# nsbaci_runtimeServiceFactory_library

    add_library(nsbaci_runtimeServiceFactory_library STATIC
        runtimeServiceFactory.cpp
        runtimeServiceFactory.h
    )

# Include path

    target_include_directories(nsbaci_runtimeServiceFactory_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_runtimeServiceFactory_library PUBLIC
        config_compiler_flags_library
        nsbaci_runtimeService_library
        nsbaci_nsbaciInterpreter_library
        nsbaci_nsbaciScheduler_library
    )
