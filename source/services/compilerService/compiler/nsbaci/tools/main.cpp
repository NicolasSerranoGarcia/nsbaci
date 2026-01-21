/**
 * @file main.cpp
 * @brief CLI tool for testing the nsbaci compiler.
 *
 * This is a standalone command-line tool for testing the compiler
 * independently of the main application.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#include <iostream>

#include "nsbaciCompiler.h"

int main() {
  nsbaci::compiler::NsbaciCompiler compiler;
  auto result = compiler.compile(std::cin);

  if (result.ok) {
    std::cout << "Compilation successful!" << std::endl;
    std::cout << "Generated " << result.instructions.size() << " instructions."
              << std::endl;
    return 0;
  } else {
    std::cerr << "Compilation failed with " << result.errors.size()
              << " error(s)." << std::endl;
    return 1;
  }
}
