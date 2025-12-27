# ./source/ui/errorDialogFactory/CMakeLists.txt

# ErrorDialogFactory component library for nsbaci.
# Provides factory for converting UIErrors to dialog widgets.
# Private to the UI layer.

# nsbaci_errordialogfactory_library

    add_library(nsbaci_errordialogfactory_library STATIC
        errorDialogFactory.cpp
        errorDialogFactory.h
    )

# Include path

    target_include_directories(nsbaci_errordialogfactory_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_errordialogfactory_library PUBLIC
        nsbaci_qt_library
        nsbaci_uierror_library
    )
