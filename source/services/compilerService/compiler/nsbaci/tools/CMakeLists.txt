# ./source/services/compilerService/compiler/nsbaci/tools/CMakeLists.txt

# CLI tool for testing the nsbaci compiler.

# nsbaci_compiler_cli executable

    add_executable(nsbaci_compiler_cli
        main.cpp
    )

# Dependencies

    target_link_libraries(nsbaci_compiler_cli PRIVATE
        config_compiler_flags_library
        nsbaci_nsbaciCompiler_library
    )

# Set output name

    set_target_properties(nsbaci_compiler_cli PROPERTIES
        OUTPUT_NAME "nsbaci-compiler"
    )
