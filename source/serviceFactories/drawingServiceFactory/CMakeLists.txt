# ./source/serviceFactories/drawingServiceFactory/CMakeLists.txt

# DrawingServiceFactory component library for nsbaci.
# Factory for creating DrawingService instances.

# nsbaci_drawingServiceFactory_library

    add_library(nsbaci_drawingServiceFactory_library STATIC
        drawingServiceFactory.cpp
        drawingServiceFactory.h
    )

# Include path

    target_include_directories(nsbaci_drawingServiceFactory_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_drawingServiceFactory_library PUBLIC
        config_compiler_flags_library
        nsbaci_drawingService_library
    )
