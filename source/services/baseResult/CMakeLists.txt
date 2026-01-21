# ./source/services/baseResult/CMakeLists.txt

# Basic result for services library

# nsbaci_baseResult_library

    add_library(nsbaci_baseResult_library STATIC
        baseResult.h
    )

# Include path

    target_include_directories(nsbaci_baseResult_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_baseResult_library PUBLIC
        config_compiler_flags_library
        nsbaci_error_library
    )
