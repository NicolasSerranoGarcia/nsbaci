# ./source/serviceFactories/fileServiceFactory/CMakeLists.txt

# FileServiceFactory component library for nsbaci.
# Factory for creating FileService instances.

# nsbaci_fileServiceFactory_library

    add_library(nsbaci_fileServiceFactory_library STATIC
        fileServiceFactory.cpp
        fileServiceFactory.h
    )

# Include path

    target_include_directories(nsbaci_fileServiceFactory_library PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

# Dependencies

    target_link_libraries(nsbaci_fileServiceFactory_library PUBLIC
        config_compiler_flags_library
        nsbaci_fileService_library
    )
