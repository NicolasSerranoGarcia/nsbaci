# ./source/ui/CMakeLists.txt

# UI components configuration for nsbaci.
# Aggregates all user interface widget libraries into a single interface library.

# Sub-components

    add_subdirectory(dialog)
    add_subdirectory(mainwindow)

# nsbaci_ui_library

    add_library(nsbaci_ui_library INTERFACE)

# Dependencies

    target_link_libraries(nsbaci_ui_library INTERFACE
        nsbaci_dialog_library
        nsbaci_mainwindow_library
    )
