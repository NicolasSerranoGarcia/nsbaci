# ./source/ui/userGuideDialog/CMakeLists.txt

# UserGuideDialog component library for nsbaci.
# Provides the integrated User Guide dialog with code examples.

# nsbaci_userguidedialog_library

    add_library(nsbaci_userguidedialog_library STATIC
        userGuideDialog.cpp
        userGuideDialog.h
    )

# Include path

    target_include_directories(nsbaci_userguidedialog_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_userguidedialog_library PUBLIC
        config_compiler_flags_library
        nsbaci_qt_library
    )
