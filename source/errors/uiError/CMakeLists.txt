# ./source/errors/uiError/CMakeLists.txt

# UI Error component library for nsbaci.
# Defines UI-ready error representations for display.

# nsbaci_uierror_library

    add_library(nsbaci_uierror_library STATIC
        uiError.cpp
        uiError.h
    )

# Include path

    target_include_directories(nsbaci_uierror_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_uierror_library PUBLIC
        config_compiler_flags_library
        nsbaci_qt_library
        
        nsbaci_types_library
        nsbaci_error_library
    )
