# ./cmake/test.cmake

if(BUILD_TESTS)
    message(STATUS "(nsbaci) Building tests is ENABLED")

    # --- Manage tests from ctest utility

        include(CTest)
        enable_testing()

    # --- Fetch google test

        include(FetchContent)

        # Avoid overriding the parent project's compiler/linker settings on Windows.
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )

        # Keep the behavior consistent with the previous CPM options.
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

        FetchContent_MakeAvailable(googletest)

        include(GoogleTest)

        find_package(Threads REQUIRED)

    # --- Create an interface library for google_benchmarks dependencies ---

        add_library(google_test_library INTERFACE)

        target_link_libraries(google_test_library INTERFACE
            Threads::Threads
            gtest_main
        )

    # --- Include all the benchmarks

        add_subdirectory(${CMAKE_SOURCE_DIR}/test)

else()
    message(STATUS "(nsbaci) Building tests is DISABLED")
endif()

