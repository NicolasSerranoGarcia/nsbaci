# ./CMakeLists.txt

# CMake config

    # --- CMake ---

        cmake_minimum_required(VERSION 3.20...3.25)

        set(CMAKE_CXX_STANDARD 23)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)
        set(CMAKE_CXX_EXTENSIONS OFF)

    # --- Project ---

        file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_STRING)
        project(
            nsbaci
            VERSION ${PROJECT_VERSION_STRING}
            DESCRIPTION "nsbaci — Learn concurrency in C++"
            HOMEPAGE_URL "https://github.com/NicolasSerranoGarcia/nsbaci"
            LANGUAGES CXX
        )

        # --- Some tools prefer the Major.Minor verison of SEMVER ---

        set(PROJECT_VERSION_SHORT "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

    # --- Build output ---

        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# config_compiler_flags_library library

    # --- Compiler flags ---

    add_library(config_compiler_flags_library INTERFACE)
    target_compile_features(config_compiler_flags_library INTERFACE cxx_std_23)

    # --- GCC/Clang flags ---

    target_compile_options(config_compiler_flags_library INTERFACE
        "$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang,ARMClang,LCC>:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>"
        "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W3>"
    )

    # --- Link flags ---

    if(WIN32 AND MINGW)
        target_link_options(config_compiler_flags_library INTERFACE
            -static
            -static-libgcc
            -static-libstdc++
        )
    endif()

# Executable

    # --- nsbaci ---

    add_executable(${PROJECT_NAME} source/main.cpp)

    # --- Resources file (currently just for the windows icon) ---

    if(WIN32)
        enable_language(RC)
        target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/resources.rc)
    endif()

    target_link_libraries(${PROJECT_NAME} PRIVATE config_compiler_flags_library)

    # --- global preprocessor flag available when coding ---

    add_compile_definitions(NSBACI_VERSION="${PROJECT_VERSION}")

# qt

		include(${CMAKE_SOURCE_DIR}/cmake/qt.cmake)

		# --- Link it with the executable ---

		target_link_libraries(${PROJECT_NAME} PRIVATE nsbaci_qt_library)

		qt_add_resources(nsbaci_qt_resources
			${CMAKE_SOURCE_DIR}/qt_resources.qrc
		)

		target_sources(${PROJECT_NAME} PRIVATE
			${nsbaci_qt_resources}
		)

# Sub-libraries

add_subdirectory(${CMAKE_SOURCE_DIR}/source)

# Link project libraries

target_link_libraries(${PROJECT_NAME} PRIVATE
    nsbaci_library
)

# Auxiliary modules

    include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/doxygen.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/appImage.cmake)

# Set build type

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING
            "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel." FORCE)
    endif()
    message(STATUS "(nsbaci) Build type: ${CMAKE_BUILD_TYPE}")

    if(WIN32)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE)
            target_link_options(${PROJECT_NAME} PRIVATE -mconsole)
        elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
            target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
            target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
        endif()
    endif()

# Installation

    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/install")
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# CPack

    set(CPACK_GENERATOR "ZIP")

    include(InstallRequiredSystemLibraries)
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VENDOR "Nicolás Serrano García")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} helps you learn concurrency.")
    set(CPACK_PACKAGE_CONTACT "serranogarcianicolas@gmail.com")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
    set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
    include(CPack)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)