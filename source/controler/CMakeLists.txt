# ./source/controler/CMakeLists.txt

# Controler component library for nsbaci.
# Defines the control layer.

# nsbaci_controler_library

    add_library(nsbaci_controler_library STATIC
        controler.cpp
        controler.h
    )

# Include path

    target_include_directories(nsbaci_controler_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_controler_library PUBLIC
        config_compiler_flags_library
        nsbaci_error_library
    )
