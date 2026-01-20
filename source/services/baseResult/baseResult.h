/**
 * @file baseResult.h
 * @brief Base result class declaration for nsbaci services.
 *
 * This module defines the BaseResult struct that serves as the base
 * for all service operation results.
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
 * @brief Main namespace for nsbaci.
 */
namespace nsbaci {

/**
 * @struct BaseResult
 * @brief Base result for all service operations.
 */
struct BaseResult {
  BaseResult() : ok(true) {}
  explicit BaseResult(std::vector<nsbaci::Error> errs)
      : ok(errs.empty()), errors(std::move(errs)) {}
  explicit BaseResult(nsbaci::Error error)
      : ok(false), errors({std::move(error)}) {}

  BaseResult(BaseResult&&) noexcept = default;
  BaseResult& operator=(BaseResult&&) noexcept = default;

  BaseResult(const BaseResult&) = default;
  BaseResult& operator=(const BaseResult&) = default;

  bool ok;
  std::vector<nsbaci::Error> errors;
};

}  // namespace nsbaci

#endif  // NSBACI_SERVICES_BASERESULT_H