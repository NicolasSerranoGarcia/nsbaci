# Test CMakeLists.txt

# Compiler tests
add_executable(nsbaci_compiler_tests
    compiler_tests.cpp
)

target_link_libraries(nsbaci_compiler_tests PRIVATE
    google_test_library
    nsbaci_nsbaciCompiler_library
    nsbaci_compilerInstruction_library
)

target_include_directories(nsbaci_compiler_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/source/services/compilerService/compiler/nsbaci
    ${CMAKE_SOURCE_DIR}/source/services/compilerService/compiler/instruction
    ${CMAKE_SOURCE_DIR}/source/errors/error
)

gtest_discover_tests(nsbaci_compiler_tests)

# Runtime tests
add_executable(nsbaci_runtime_tests
    runtime_tests.cpp
)

target_link_libraries(nsbaci_runtime_tests PRIVATE
    google_test_library
    nsbaci_nsbaciCompiler_library
    nsbaci_nsbaciInterpreter_library
    nsbaci_thread_library
    nsbaci_program_library
    nsbaci_compilerInstruction_library
)

target_include_directories(nsbaci_runtime_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/source/services/compilerService/compiler/nsbaci
    ${CMAKE_SOURCE_DIR}/source/services/compilerService/compiler/instruction
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/interpreter/nsbaci
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/thread
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/program
    ${CMAKE_SOURCE_DIR}/source/errors/error
)

gtest_discover_tests(nsbaci_runtime_tests)

# Integration tests (compile + run)
add_executable(nsbaci_integration_tests
    integration_tests.cpp
)

target_link_libraries(nsbaci_integration_tests PRIVATE
    google_test_library
    nsbaci_nsbaciCompiler_library
    nsbaci_nsbaciInterpreter_library
    nsbaci_nsbaciScheduler_library
    nsbaci_thread_library
    nsbaci_program_library
    nsbaci_scheduler_library
    nsbaci_compilerInstruction_library
)

target_include_directories(nsbaci_integration_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/source/services/compilerService/compiler/nsbaci
    ${CMAKE_SOURCE_DIR}/source/services/compilerService/compiler/instruction
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/interpreter/nsbaci
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/scheduler/nsbaci
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/scheduler
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/thread
    ${CMAKE_SOURCE_DIR}/source/services/runtimeService/program
    ${CMAKE_SOURCE_DIR}/source/errors/error
)

gtest_discover_tests(nsbaci_integration_tests)
