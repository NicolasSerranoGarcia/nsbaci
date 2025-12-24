if(GENERATE_DOCS)
    message(STATUS "(nbaci) Generating docs is ENABLED")
    message(STATUS "(nbaci) Doxygen version short: ${PROJECT_VERSION_SHORT}")

    include(FetchContent)
    FetchContent_Declare(
        doxygen-awesome-css
        URL https://github.com/jothepro/doxygen-awesome-css/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(doxygen-awesome-css)

    # Save the location the files were cloned into
    # This allows us to get the path to doxygen-awesome.css
    FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

    message(STATUS "(nbaci) AWESOME_CSS_DIR = ${AWESOME_CSS_DIR}")

    # Generate the Doxyfile
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    message(STATUS "(nbaci) Doc files have been written to ${CMAKE_SOURCE_DIR}/docs")
else()
    message(STATUS "(nbaci) Generating docs is DISABLED")
endif()