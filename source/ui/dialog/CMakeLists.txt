# ./source/ui/dialog/CMakeLists.txt

# Dialog component library for nsbaci.
# Defines the dialog widget for user interactions.

# nsbaci_dialog_library

    add_library(nsbaci_dialog_library STATIC
        dialog.cpp
        dialog.h
        dialog.ui
    )

# Include path

    target_include_directories(nsbaci_dialog_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_dialog_library PUBLIC
        config_compiler_flags_library
        nsbaci_qt_library
    )
