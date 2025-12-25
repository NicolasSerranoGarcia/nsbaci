# ./source/CMakeLists.txt

# Source library configuration for nsbaci.
# Defines the nsbaci_library static library containing the main application logic.

# nsbaci_library

    add_library(nsbaci_library STATIC
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        dialog.h
        dialog.cpp
        dialog.ui
    )

# Include path

    target_include_directories(nsbaci_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_library PUBLIC
        config_compiler_flags_library
        nsbaci_qt_library
    )
