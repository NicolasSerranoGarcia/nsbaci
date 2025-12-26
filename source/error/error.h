/**
 * @file error.h
 * @brief Error class declaration for nsbaci.
 *
 * This module defines the error handling structures and classes
 * used throughout the application for consistent error reporting.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_ERROR_H
#define NSBACI_ERROR_H

#include "errorTypes.h"

using namespace nsbaci::types;

/**
 * @namespace nsbaci
 * @brief Root namespace for the nsbaci application.
 */
namespace nsbaci {

/**
 * @class Error
 * @brief Represents an error with a message and optional code.
 */
class Error {
    
    public:
    Error() = default;
    ~Error() = default;
    ErrorBase basic;
    ErrorPayload payload;
};

}  // namespace nsbaci

#endif  // NSBACI_ERROR_H
