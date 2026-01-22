/**
 * @file baseResult.h
 * @brief Base result class declaration for nsbaci services.
 *
 * This module defines the BaseResult struct that serves as the foundational
 * result type for all service operations in the nsbaci application. It
 * implements a simple success/failure pattern with associated error
 * information, providing a consistent interface for error handling across all
 * services.
 *
 * The BaseResult pattern ensures that:
 * - All service operations return a result that can be checked for success
 * - Failed operations always provide descriptive error information
 * - Results can be efficiently moved without copying error data
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_BASERESULT_H
#define NSBACI_SERVICES_BASERESULT_H

#include <vector>

#include "error.h"

/**
 * @namespace nsbaci
 * @brief Main namespace for the nsbaci interpreter application.
 */
namespace nsbaci {

/**
 * @struct BaseResult
 * @brief Base result structure for all service operations.
 *
 * BaseResult provides a common interface for service operation results,
 * encapsulating success/failure status and any associated error information.
 * All service-specific result types should inherit from this base to ensure
 * consistent error handling patterns throughout the application.
 *
 * @invariant If ok is false, errors vector contains at least one error.
 * @invariant If ok is true, errors vector is empty.
 *
 * Usage example:
 * @code
 * BaseResult result = someService.doOperation();
 * if (!result.ok) {
 *   for (const auto& err : result.errors) {
 *     handleError(err);
 *   }
 * }
 * @endcode
 */
struct BaseResult {
  /**
   * @brief Default constructor creates a successful result.
   */
  BaseResult() : ok(true) {}

  /**
   * @brief Constructs a result from a vector of errors.
   *
   * If the error vector is empty, the result is considered successful.
   *
   * @param errs Vector of errors encountered during the operation.
   */
  explicit BaseResult(std::vector<nsbaci::Error> errs)
      : ok(errs.empty()), errors(std::move(errs)) {}

  /**
   * @brief Constructs a failed result from a single error.
   * @param error The error that caused the operation to fail.
   */
  explicit BaseResult(nsbaci::Error error)
      : ok(false), errors({std::move(error)}) {}

  BaseResult(BaseResult&&) noexcept = default;
  BaseResult& operator=(BaseResult&&) noexcept = default;

  BaseResult(const BaseResult&) = default;
  BaseResult& operator=(const BaseResult&) = default;

  bool ok;  ///< True if the operation succeeded.
  std::vector<nsbaci::Error>
      errors;  ///< Errors encountered (empty if ok is true).
};

}  // namespace nsbaci

#endif  // NSBACI_SERVICES_BASERESULT_H