# ./cmake/user_guide.cmake

# User guide PDF generation module for nsbaci.
# Uses Pandoc to convert Markdown to a professional PDF.

if(GENERATE_USER_GUIDE)
    message(STATUS "(nsbaci) User guide generation is ENABLED")

    find_program(PANDOC_EXECUTABLE pandoc)
    
    if(NOT PANDOC_EXECUTABLE)
        message(WARNING "(nsbaci) Pandoc not found. User guide generation disabled.")
        message(STATUS "(nsbaci) Install pandoc: sudo apt install pandoc texlive-xetex")
    else()
        message(STATUS "(nsbaci) Found Pandoc: ${PANDOC_EXECUTABLE}")

        set(USER_GUIDE_SOURCE ${CMAKE_SOURCE_DIR}/docs/user_guide.md)
        set(USER_GUIDE_OUTPUT ${CMAKE_BINARY_DIR}/docs/nsbaci_user_guide.pdf)
        set(USER_GUIDE_METADATA_IN ${CMAKE_SOURCE_DIR}/docs/user_guide_meta.yaml.in)
        set(USER_GUIDE_METADATA ${CMAKE_BINARY_DIR}/docs/user_guide_meta.yaml)

        # Create output directory
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/docs)

        # Configure metadata file with version
        configure_file(${USER_GUIDE_METADATA_IN} ${USER_GUIDE_METADATA} @ONLY)

        # Add custom target for user guide
        add_custom_target(user_guide
            COMMAND ${PANDOC_EXECUTABLE}
                ${USER_GUIDE_SOURCE}
                --metadata-file=${USER_GUIDE_METADATA}
                --pdf-engine=xelatex
                --toc
                --toc-depth=2
                --number-sections
                --highlight-style=tango
                -V geometry:margin=1in
                -V fontsize=11pt
                -V documentclass=report
                -V colorlinks=true
                -V linkcolor=blue
                -V urlcolor=blue
                -o ${USER_GUIDE_OUTPUT}
            DEPENDS ${USER_GUIDE_SOURCE} ${USER_GUIDE_METADATA}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "(nsbaci) Generating user guide PDF..."
            VERBATIM
        )

        message(STATUS "(nsbaci) User guide will be written to ${USER_GUIDE_OUTPUT}")
    endif()
else()
    message(STATUS "(nsbaci) User guide generation is DISABLED")
endif()
