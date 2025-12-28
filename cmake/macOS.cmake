# ./cmake/macOS.cmake

# macOS bundle configuration module for nsbaci.
# This module handles the creation of macOS .app bundles.

if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    return()
endif()

if(APPLE)
    message(STATUS "(nsbaci) Configuring macOS bundle")

    # --- Bundle configuration ---

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_SHORT}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.nicolasserranogarcia.nsbaci"
        MACOSX_BUNDLE_COPYRIGHT "© 2025 Nicolas Serrano Garcia"
        MACOSX_BUNDLE_INFO_STRING "nsbaci — Learn concurrency in C++"
    )

    # --- Icon file (if exists) ---

    set(MACOS_ICON_FILE "${CMAKE_SOURCE_DIR}/assets/nsbaci.icns")
    if(EXISTS "${MACOS_ICON_FILE}")
        set_source_files_properties(${MACOS_ICON_FILE} PROPERTIES
            MACOSX_PACKAGE_LOCATION "Resources"
        )
        target_sources(${PROJECT_NAME} PRIVATE ${MACOS_ICON_FILE})
        set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_ICON_FILE "nsbaci.icns"
        )
        message(STATUS "(nsbaci) Using macOS icon: ${MACOS_ICON_FILE}")
    else()
        message(STATUS "(nsbaci) No .icns icon found at ${MACOS_ICON_FILE}")
    endif()

    # --- High DPI support ---

    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/cmake/Info.plist.in"
    )

endif()
