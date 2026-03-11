# ./source/ui/CMakeLists.txt

# UI components configuration for nsbaci.
# Aggregates all user interface widget libraries into a single interface library.

# Sub-components

    add_subdirectory(errorDialogFactory)
    add_subdirectory(mainwindow)
    add_subdirectory(runtimeView)
    add_subdirectory(drawingWidget)
    add_subdirectory(userGuideDialog)
    add_subdirectory(preferencesDialog)

# nsbaci_ui_library

    add_library(nsbaci_ui_library INTERFACE)

# Dependencies

    target_link_libraries(nsbaci_ui_library INTERFACE
        nsbaci_mainwindow_library
        runtimeView
        nsbaci_drawingWidget_library
        # nsbaci_errordialogfactory_library is already linked via mainwindow
    )
