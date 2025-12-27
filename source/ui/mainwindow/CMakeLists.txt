# ./source/ui/mainwindow/CMakeLists.txt

# MainWindow component library for nsbaci.
# Defines the main application window widget.

# nsbaci_mainwindow_library

    add_library(nsbaci_mainwindow_library STATIC
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        codeeditor.cpp
        codeeditor.h
    )

# Include path

    target_include_directories(nsbaci_mainwindow_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_mainwindow_library PUBLIC
        config_compiler_flags_library
        nsbaci_qt_library
    )
