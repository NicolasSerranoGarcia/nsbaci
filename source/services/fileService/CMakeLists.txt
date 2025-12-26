# ./source/services/fileService/CMakeLists.txt

# FileService component library for nsbaci.

# nsbaci_fileService_library

    add_library(nsbaci_fileService_library STATIC
        fileService.cpp
        fileService.h
    )

# Include path

    target_include_directories(nsbaci_fileService_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_fileService_library PUBLIC
        config_compiler_flags_library
    )
