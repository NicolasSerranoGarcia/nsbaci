# ./source/controller/CMakeLists.txt

# Controller component library for nsbaci.
# Defines the control layer.

# nsbaci_controller_library

    add_library(nsbaci_controller_library STATIC
        controller.cpp
        controller.h
    )

# Include path

    target_include_directories(nsbaci_controller_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_controller_library PUBLIC
        config_compiler_flags_library
        nsbaci_qt_library
        nsbaci_error_library
        nsbaci_uierror_library
        runtimeView
        nsbaci_compilerInstruction_library
        
        nsbaci_services_library
    )
