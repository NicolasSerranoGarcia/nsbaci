# ./source/ui/preferencesDialog/CMakeLists.txt

# PreferencesDialog component library for nsbaci.
# Provides the application settings dialog.

# nsbaci_preferencesdialog_library

    add_library(nsbaci_preferencesdialog_library STATIC
        preferencesDialog.cpp
        preferencesDialog.h
    )

# Include path

    target_include_directories(nsbaci_preferencesdialog_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_preferencesdialog_library PUBLIC
        config_compiler_flags_library
        nsbaci_qt_library
    )
