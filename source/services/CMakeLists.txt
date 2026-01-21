# ./source/services/CMakeLists.txt

# Services components configuration for nsbaci.
# Aggregates all service libraries into a single interface library.

# Sub-components

    add_subdirectory(baseResult)

    add_subdirectory(compilerService)
    add_subdirectory(fileService)
    add_subdirectory(runtimeService)
    add_subdirectory(drawingService)

# nsbaci_services_library

    add_library(nsbaci_services_library INTERFACE)

# Dependencies

    target_link_libraries(nsbaci_services_library INTERFACE
        nsbaci_baseResult_library
        nsbaci_compilerService_library
        nsbaci_fileService_library
        nsbaci_runtimeService_library
        nsbaci_drawingService_library
    )
