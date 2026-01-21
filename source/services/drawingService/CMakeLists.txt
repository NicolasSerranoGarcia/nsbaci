# ./source/services/drawingService/CMakeLists.txt

# DrawingService component library for nsbaci.
# Provides an adapter for graphical output backends (CLI, OpenGL canvas, etc.)

# nsbaci_drawingService_library

    add_library(nsbaci_drawingService_library STATIC
        drawingService.cpp
        drawingService.h
    )

# Include path

    target_include_directories(nsbaci_drawingService_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_drawingService_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
        nsbaci_error_library
    )
