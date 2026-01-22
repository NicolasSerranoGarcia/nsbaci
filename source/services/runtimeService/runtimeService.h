/**
 * @file runtimeService.h
 * @brief RuntimeService class declaration for nsbaci.
 *
 * This module defines the RuntimeService class which serves as the main
 * interface for executing compiled nsbaci programs. It controls the
 * interpreter and scheduler components to provide program
 * execution with support for stepping, continuous running, and status info
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_RUNTIMESERVICE_H
#define NSBACI_RUNTIMESERVICE_H

#include <memory>

#include "baseResult.h"
#include "interpreter.h"
#include "program.h"
#include "scheduler.h"

/**
 * @namespace nsbaci::services
 * @brief Services namespace containing all backend service implementations.
 */
namespace nsbaci::services {

/**
 * @struct RuntimeResult
 * @brief Result of a runtime operation (step, run, etc.).
 */
struct RuntimeResult : nsbaci::BaseResult {
  /**
   * @brief Default constructor creates a successful result.
   */
  RuntimeResult() : BaseResult() {}

  /**
   * @brief Constructs a result from a vector of errors.
   * @param errs Vector of runtime errors.
   */
  explicit RuntimeResult(std::vector<nsbaci::Error> errs)
      : BaseResult(std::move(errs)) {}

  /**
   * @brief Constructs a failed result from a single error.
   * @param error The runtime error that occurred.
   */
  explicit RuntimeResult(nsbaci::Error error) : BaseResult(std::move(error)) {}

  RuntimeResult(RuntimeResult&&) noexcept = default;
  RuntimeResult& operator=(RuntimeResult&&) noexcept = default;
  RuntimeResult(const RuntimeResult&) = default;
  RuntimeResult& operator=(const RuntimeResult&) = default;

  bool halted = false;      ///< True if program has terminated
  bool needsInput = false;  ///< True if waiting for user input
  std::string inputPrompt;  ///< Prompt to show for input
  std::string output;       ///< Output produced by this step
};

/**
 * @enum RuntimeState
 * @brief Possible states of the runtime service.
 *
 * Represents the lifecycle states of program execution:
 * - Idle: Initial state, no program loaded or execution completed
 * - Running: Active execution in progress
 * - Paused: Execution suspended, can be resumed or stepped
 * - Halted: Program has finished (reached Halt instruction)
 */
enum class RuntimeState {
  Idle,     ///< No program loaded or ready to start.
  Running,  ///< Program is actively executing.
  Paused,   ///< Execution paused, can step or continue.
  Halted    ///< Program has finished execution.
};

/**
 * @class RuntimeService
 * @brief Service that manages program execution.
 *
 * The service is move-only to ensure single ownership of the interpreter
 * and scheduler components.
 *
 * Usage example:
 * @code
 * RuntimeService rs(std::make_unique<NsbaciInterpreter>(),
 *                   std::make_unique<NsbaciScheduler>());
 * rs.loadProgram(std::move(compiledProgram));
 * rs.setOutputCallback([](const std::string& out) { std::cout << out; });
 *
 * while (!rs.isHalted()) {
 *   auto result = rs.step();
 *   if (result.needsInput) {
 *     rs.provideInput(getUserInput());
 *   }
 * }
 * @endcode
 */
class RuntimeService {
 public:
  /**
   * @brief Default constructor creates an uninitialized service.
   *
   * A service created this way must have its interpreter and scheduler
   * set before use, typically via move assignment from a factory-created
   * instance.
   */
  RuntimeService() = default;

  /**
   * @brief Constructs a RuntimeService with interpreter and scheduler.
   * @param i Unique pointer to the interpreter implementation.
   * @param s Unique pointer to the scheduler implementation.
   */
  RuntimeService(std::unique_ptr<runtime::Interpreter> i,
                 std::unique_ptr<runtime::Scheduler> s);

  /**
   * @brief Default destructor.
   */
  ~RuntimeService() = default;

  RuntimeService(const RuntimeService&) = delete;
  RuntimeService& operator=(const RuntimeService&) = delete;

  RuntimeService(RuntimeService&&) = default;
  RuntimeService& operator=(RuntimeService&&) = default;

  /**
   * @brief Loads a compiled program for execution.
   *
   * Initializes the runtime with the program's instructions, symbol table,
   * and memory. Creates the initial main thread and sets state to Paused
   * ready for execution.
   *
   * @param p The compiled program to load (which must be moved into the
   * service).
   */
  void loadProgram(runtime::Program&& p);

  /**
   * @brief Resets the runtime to initial state.
   *
   * Clears all threads, resets memory, and sets state to Idle.
   * The program must be reloaded before execution can continue.
   */
  void reset();

  /**
   * @brief Executes a single instruction for any ready thread.
   *
   * The scheduler picks the next thread to run and the interpreter
   * executes one instruction from that thread's current position.
   *
   * @return RuntimeResult with execution outcome, output, and I/O state.
   */
  RuntimeResult step();

  /**
   * @brief Executes a single instruction for a specific thread.
   *
   * Allows targeted debugging by stepping only the specified thread.
   *
   * @param threadId The ID of the thread to step.
   * @return RuntimeResult with execution outcome.
   */
  RuntimeResult stepThread(nsbaci::types::ThreadID threadId);

  /**
   * @brief Runs the program until halted, error, or step limit.
   *
   * Executes instructions continuously until:
   * - The program halts (reaches Halt instruction)
   * - An error occurs
   * - The maximum step count is reached
   * - Input is required
   *
   * @param maxSteps Maximum instructions to execute (0 = unlimited).
   * @return RuntimeResult with final execution state.
   */
  RuntimeResult run(size_t maxSteps = 0);

  /**
   * @brief Pauses continuous execution.
   *
   * Only affects state if currently Running; changes state to Paused.
   */
  void pause();

  /**
   * @brief Gets the current runtime state.
   * @return The current RuntimeState value.
   */
  RuntimeState getState() const;

  /**
   * @brief Checks if the program has finished execution.
   * @return True if state is Halted.
   */
  bool isHalted() const;

  /**
   * @brief Gets the number of active threads.
   * @return Count of threads in the scheduler.
   */
  size_t threadCount() const;

  /**
   * @brief Gets all threads from the scheduler.
   * @return Const reference to the threads vector for UI display.
   */
  const std::vector<runtime::Thread>& getThreads() const;

  /**
   * @brief Gets the loaded program.
   * @return Const reference to the program for accessing instructions and
   * memory.
   */
  const runtime::Program& getProgram() const;

  /**
   * @brief Provides input to the runtime.
   *
   * Called when the user provides input in response to a Read instruction.
   * The input is stored and will be consumed on the next execution step.
   *
   * @param input The user-provided input string.
   */
  void provideInput(const std::string& input);

  /**
   * @brief Checks if the runtime is waiting for user input.
   * @return True if a Read instruction is blocking execution.
   */
  bool isWaitingForInput() const;

  /**
   * @brief Sets the callback for output operations.
   *
   * The callback is invoked whenever the program produces output
   * (Write, Writeln, WriteRawString instructions).
   *
   * @param callback Function to call with output strings.
   */
  void setOutputCallback(runtime::OutputCallback callback);

 private:
  runtime::Program
      program;  ///< The loaded program with instructions and memory.
  std::unique_ptr<runtime::Interpreter>
      interpreter;  ///< Executes individual instructions.
  std::unique_ptr<runtime::Scheduler>
      scheduler;                            ///< Manages thread scheduling.
  RuntimeState state = RuntimeState::Idle;  ///< Current execution state.
};

}  // namespace nsbaci::services

#endif  // NSBACI_RUNTIMESERVICE_H
