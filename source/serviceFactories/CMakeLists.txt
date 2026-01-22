# ./source/serviceFactories/CMakeLists.txt

# ServiceFactories components configuration for nsbaci.
# Aggregates all service factory libraries into a single interface library.

# Sub-components

    add_subdirectory(compilerServiceFactory)
    add_subdirectory(drawingServiceFactory)
    add_subdirectory(fileServiceFactory)
    add_subdirectory(runtimeServiceFactory)

# nsbaci_serviceFactories_library

    add_library(nsbaci_serviceFactories_library INTERFACE)

# Include path

    target_include_directories(nsbaci_serviceFactories_library INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_serviceFactories_library INTERFACE
        nsbaci_compilerServiceFactory_library
        nsbaci_drawingServiceFactory_library
        nsbaci_fileServiceFactory_library
        nsbaci_runtimeServiceFactory_library
    )
