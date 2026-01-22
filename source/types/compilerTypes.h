/**
 * @file compilerTypes.h
 * @brief Type definitions for compiler-related operations.
 *
 * This header provides type aliases used by the CompilerService
 * and other components that work with compilation.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_TYPES_COMPILERTYPES_H
#define NSBACI_TYPES_COMPILERTYPES_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

/// @brief Type alias for variable names
using VarName = std::string;

/// @brief Type alias for memory addresses
using MemoryAddr = uint32_t;

/// @brief Information about a variable/symbol
struct SymbolInfo {
  VarName name;
  MemoryAddr address;
  std::string type;  ///< "int", "bool", "char", "void", etc.
  bool isGlobal;
};

/// @brief Lookup table mapping variable names to their symbol info
using SymbolTable = std::unordered_map<VarName, SymbolInfo>;

}  // namespace nsbaci::types

#endif  // NSBACI_TYPES_COMPILERTYPES_H
