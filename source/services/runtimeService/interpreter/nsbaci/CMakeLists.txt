# ./source/services/runtimeService/interpreter/nsbaci/CMakeLists.txt

# NsbaciInterpreter component library for nsbaci runtime service.
# Concrete implementation of the Interpreter interface for BACI.

# nsbaci_nsbaciInterpreter_library

    add_library(nsbaci_nsbaciInterpreter_library STATIC
        nsbaciInterpreter.cpp
        nsbaciInterpreter.h
    )

# Include path

    target_include_directories(nsbaci_nsbaciInterpreter_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_nsbaciInterpreter_library PUBLIC
        config_compiler_flags_library
        nsbaci_interpreter_library
    )
