# ./source/ui/drawingWidget/CMakeLists.txt

# DrawingWidget component library for nsbaci.
# Provides a Qt widget for canvas-based drawing operations.

# nsbaci_drawingWidget_library

    set(CMAKE_AUTOMOC ON)

    add_library(nsbaci_drawingWidget_library STATIC
        drawingWidget.cpp
        drawingWidget.h
    )

# Include path

    target_include_directories(nsbaci_drawingWidget_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_drawingWidget_library PUBLIC
        config_compiler_flags_library
        nsbaci_types_library
        nsbaci_qt_library
    )
