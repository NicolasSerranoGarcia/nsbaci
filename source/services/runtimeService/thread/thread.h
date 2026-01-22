/**
 * @file thread.h
 * @brief Thread class declaration for nsbaci runtime service.
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_SERVICES_RUNTIME_THREAD_H
#define NSBACI_SERVICES_RUNTIME_THREAD_H

#include <cstdint>
#include <queue>
#include <vector>

#include "runtimeTypes.h"

/**
 * @namespace nsbaci::services::runtime
 * @brief Runtime services namespace for nsbaci.
 */
namespace nsbaci::services::runtime {

/**
 * @class Thread
 * @brief Represents a thread in the runtime service.
 *
 * Each thread has its own stack, program counter, and execution state.
 */
class Thread {
 public:
  Thread()
      : id(nextThreadId++),
        state(nsbaci::types::ThreadState::Ready),
        priority(0),
        pc(0),
        bp(0),
        sp(0) {}
  ~Thread() = default;

  /**
   * @brief Gets the thread ID.
   * @return The unique identifier of this thread.
   */
  nsbaci::types::ThreadID getId() const;

  /**
   * @brief Gets the current state of the thread.
   * @return The current ThreadState.
   */
  nsbaci::types::ThreadState getState() const;

  /**
   * @brief Sets the state of the thread.
   * @param newState The new state to set.
   */
  void setState(nsbaci::types::ThreadState newState);

  /**
   * @brief Gets the priority of the thread.
   * @return The current Priority level.
   */
  nsbaci::types::Priority getPriority() const;

  /**
   * @brief Sets the priority of the thread.
   * @param newPriority The new priority level to set.
   */
  void setPriority(nsbaci::types::Priority newPriority);

  // ============== Stack Operations ==============

  /**
   * @brief Push a value onto the thread's stack.
   */
  void push(int32_t value);

  /**
   * @brief Pop a value from the thread's stack.
   */
  int32_t pop();

  /**
   * @brief Peek at the top of the stack without removing.
   */
  int32_t top() const;

  // ============== Program Counter ==============

  /**
   * @brief Get the program counter.
   */
  uint32_t getPC() const { return pc; }

  /**
   * @brief Set the program counter.
   */
  void setPC(uint32_t addr) { pc = addr; }

  /**
   * @brief Increment the program counter.
   */
  void advancePC() { ++pc; }

  // ============== Base/Stack Pointers ==============

  uint32_t getBP() const { return bp; }
  void setBP(uint32_t addr) { bp = addr; }

  uint32_t getSP() const { return sp; }
  void setSP(uint32_t addr) { sp = addr; }

 private:
  nsbaci::types::ThreadID id;
  nsbaci::types::ThreadState state;
  nsbaci::types::Priority priority;

  // Program counter - index into instruction stream
  uint32_t pc;
  // Base pointer - for stack frames
  uint32_t bp;
  // Stack pointer
  uint32_t sp;

  // Thread-local stack
  std::vector<int32_t> stack;

  static nsbaci::types::ThreadID nextThreadId;

  // friend the scheduler
};

}  // namespace nsbaci::services::runtime

/**
 * @namespace nsbaci::types
 * @brief Type definitions namespace for nsbaci.
 */
namespace nsbaci::types {

/// @brief Queue of threads for scheduler operations
using ThreadQueue = std::queue<nsbaci::services::runtime::Thread>;

}  // namespace nsbaci::types

#endif  // NSBACI_SERVICES_RUNTIME_THREAD_H