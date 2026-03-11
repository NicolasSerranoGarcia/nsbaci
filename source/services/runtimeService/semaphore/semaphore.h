/**
 * @file semaphore.h
 * @brief Semaphore implementation for process synchronization.
 */

#ifndef NSBACI_RUNTIME_SEMAPHORE_H
#define NSBACI_RUNTIME_SEMAPHORE_H

#include <cstdint>
#include <unordered_map>

#include "compilerTypes.h"
#include "thread.h"

namespace nsbaci::services::runtime {

/**
 * @class Semaphore
 * @brief Counting semaphore for process synchronization.
 *
 * Created at runtime when StoreSemaphore instruction executes.
 */
class Semaphore {
 public:
  explicit Semaphore(int32_t initialCount = 0) : count(initialCount) {}

  /// @brief P operation (wait/decrement)
  /// @param currentThread The thread attempting to wait
  /// @return true if thread can proceed, false if it must block
  bool wait(nsbaci::types::ThreadID currentThread);

  /// @brief V operation (signal/increment)
  /// @return ThreadID to wake, or 0 if none waiting
  nsbaci::types::ThreadID signal();

  /// @brief Get current count (for debugging)
  int32_t getCount() const { return count; }

  /// @brief Check if any threads are blocked
  bool hasWaiting() const { return !blocked.empty(); }

 private:
  std::queue<nsbaci::types::ThreadID> blocked;  ///< Queue of blocked threads
  int32_t count;                                ///< Current semaphore count
};

}  // namespace nsbaci::services::runtime

namespace nsbaci::types {

/**
 * @brief Maps memory addresses to semaphores.
 *
 * - Compile time: Compiler emits StoreSemaphore instructions with addresses
 * - Runtime: Executing StoreSemaphore adds entry to this table
 * - Wait/Signal: Look up semaphore by address
 */
using SemaphoreTable =
    std::unordered_map<MemoryAddr, nsbaci::services::runtime::Semaphore>;

}  // namespace nsbaci::types

#endif  // NSBACI_RUNTIME_SEMAPHORE_H