# ./source/services/runtimeService/interpreter/CMakeLists.txt

# Interpreter component library for nsbaci runtime service.

# Subdirectories

    add_subdirectory(nsbaci)

# nsbaci_interpreter_library

    add_library(nsbaci_interpreter_library STATIC
        interpreter.cpp
        interpreter.h
    )

# Include path

    target_include_directories(nsbaci_interpreter_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_interpreter_library PUBLIC
        config_compiler_flags_library
        nsbaci_baseResult_library
        nsbaci_program_library
        nsbaci_thread_library
        nsbaci_nsbaciInterpreter_library
    )
