/**
 * @file controller.h
 * @brief Controller class declaration for nsbaci.
 *
 * This module defines the central Controller class that serves as the main
 * coordination layer between the user interface and the backend services.
 * The Controller implements the Model-View-Controller (MVC) pattern, acting
 * as the mediator that handles user actions from the UI, delegates work to
 * appropriate services, and communicates results back to the view layer.
 *
 * The Controller manages the complete lifecycle of program execution including:
 * - File operations (save/load source files)
 * - Compilation workflow (source code to p-code instructions)
 * - Runtime execution control (run, step, pause, reset)
 * - Thread scheduling and monitoring
 * - Variable state tracking and display updates
 * - Input/output handling between runtime and UI
 *
 * @author Nicolás Serrano García
 * @copyright Copyright (c) 2025 Nicolás Serrano García. Licensed under the MIT
 * License.
 */

#ifndef NSBACI_CONTROLLER_H
#define NSBACI_CONTROLLER_H

#include <QObject>
#include <QTimer>

#include "compilerService.h"
#include "compilerTypes.h"
#include "drawingService.h"
#include "fileService.h"
#include "fileTypes.h"
#include "runtimeService.h"
#include "runtimeTypes.h"
#include "runtimeView.h"
#include "uiError.h"

/**
 * @namespace nsbaci
 * @brief Root namespace for the nsbaci application.
 *
 * Contains all components of the BACI concurrent programming interpreter
 * including the controller, services, UI components, and type definitions.
 */
namespace nsbaci {

/**
 * @class Controller
 * @brief Central coordinator between UI and backend services.
 *
 * The Controller class is a QObject that serves as the main application
 * controller, implementing the MVC pattern. It receives user actions through
 * Qt slots, processes them using the specific backend services, and
 * communicates results back to the UI through Qt signals.
 *
 * The controller owns instances of all required services:
 * - FileService: Handles file system operations
 * - CompilerService: Compiles NsBaci source code to p-code
 * - RuntimeService: Executes compiled programs with thread scheduling
 * - DrawingService: Not yet implemented, but will be used as graphical API in
 * the future
 *
 * Execution modes supported:
 * - Single-step execution: Execute one instruction at a time
 * - Continuous execution: Run program with timer-based batching
 * - Thread-specific stepping: Execute a single thread's instruction
 *
 * @note The controller uses a QTimer for continuous execution to maintain
 *       UI responsiveness during long-running programs.
 */
class Controller : public QObject {
  Q_OBJECT

 public:
  /**
   * @brief Constructs the Controller with all required services.
   *
   * Takes ownership of the provided services via move semantics. Initializes
   * the internal QTimer used for continuous execution mode.
   *
   * @param f FileService instance for file operations.
   * @param c CompilerService instance for compilation.
   * @param r RuntimeService instance for program execution.
   * @param d DrawingService instance for graphical output.
   * @param parent Optional parent QObject for Qt memory management.
   */
  explicit Controller(nsbaci::services::FileService&& f,
                      nsbaci::services::CompilerService&& c,
                      nsbaci::services::RuntimeService&& r,
                      nsbaci::services::DrawingService&& d,
                      QObject* parent = nullptr);

  /**
   * @brief Default destructor.
   *
   * The QTimer is automatically cleaned up through Qt's parent-child system.
   */
  ~Controller() = default;

 signals:
  /**
   * @brief Emitted when a file save operation fails.
   * @param errors List of errors describing what went wrong.
   */
  void saveFailed(std::vector<UIError> errors);

  /**
   * @brief Emitted when a file save operation succeeds.
   */
  void saveSucceeded();

  /**
   * @brief Emitted when a file load operation fails.
   * @param errors List of errors describing what went wrong.
   */
  void loadFailed(std::vector<UIError> errors);

  /**
   * @brief Emitted when a file load operation succeeds.
   * @param contents The loaded file contents as a QString.
   */
  void loadSucceeded(const QString& contents);

  /**
   * @brief Emitted when compilation fails.
   * @param errors List of compilation errors with line/column information.
   */
  void compileFailed(std::vector<UIError> errors);

  /**
   * @brief Emitted when compilation succeeds.
   *
   * After this signal, the compiled program is ready to be loaded into
   * the runtime via onRunRequested().
   */
  void compileSucceeded();

  /**
   * @brief Emitted when a program is loaded and ready for execution.
   * @param programName The name of the loaded program.
   */
  void runStarted(const QString& programName);

  /**
   * @brief Emitted when the runtime execution state changes.
   * @param running True if the program is currently executing continuously.
   * @param halted True if the program has terminated (reached Halt
   * instruction).
   */
  void runtimeStateChanged(bool running, bool halted);

  /**
   * @brief Emitted when thread information needs to be updated in the UI.
   * @param threads Current state of all threads including PC, state, and
   * current instruction.
   */
  void threadsUpdated(const std::vector<nsbaci::ui::ThreadInfo>& threads);

  /**
   * @brief Emitted when variable information needs to be updated in the UI.
   * @param variables Current values of all program variables.
   */
  void variablesUpdated(const std::vector<nsbaci::ui::VariableInfo>& variables);

  /**
   * @brief Emitted when the runtime produces output (cout, writeln, etc.).
   * @param output The output string to display in the console.
   */
  void outputReceived(const QString& output);

  /**
   * @brief Emitted when the runtime needs user input (cin, read, etc.).
   * @param prompt The prompt message to display to the user.
   */
  void inputRequested(const QString& prompt);

 public slots:
  /**
   * @brief Handles a request to save source code to a file.
   * @param file The target file path.
   * @param contents The source code content to save.
   */
  void onSaveRequested(nsbaci::types::File file, nsbaci::types::Text contents);

  /**
   * @brief Handles a request to open and load a source file.
   * @param file The file path to load.
   */
  void onOpenRequested(nsbaci::types::File file);

  /**
   * @brief Handles a request to compile source code.
   *
   * Compiles the provided source code into p-code instructions. On success,
   * the instructions are stored in the CompilerService ready to be loaded
   * into the runtime.
   *
   * @param contents The BACI source code to compile.
   */
  void onCompileRequested(nsbaci::types::Text contents);

  /**
   * @brief Handles a request to load and prepare a compiled program for
   * execution.
   *
   * Takes the compiled instructions and symbol table from the CompilerService
   * and loads them into the RuntimeService. Sets up the output callback for
   * forwarding runtime output to the UI.
   */
  void onRunRequested();

  /**
   * @brief Executes a single instruction across any ready thread.
   *
   * The scheduler picks the next thread to run and executes one instruction.
   * Updates the UI with new thread and variable states.
   */
  void onStepRequested();

  /**
   * @brief Executes a single instruction on a specific thread.
   * @param threadId The ID of the thread to step.
   */
  void onStepThreadRequested(nsbaci::types::ThreadID threadId);

  /**
   * @brief Starts or resumes continuous execution mode.
   *
   * Begins timer-based execution where batches of instructions are executed
   * periodically, allowing the UI to remain responsive.
   */
  void onRunContinueRequested();

  /**
   * @brief Pauses continuous execution.
   *
   * Stops the execution timer but preserves program state for later resumption.
   */
  void onPauseRequested();

  /**
   * @brief Resets the runtime to initial state.
   *
   * Clears all thread state and memory but keeps the loaded program.
   * The program can be re-run from the beginning.
   */
  void onResetRequested();

  /**
   * @brief Stops execution and unloads the program.
   *
   * Completely stops the runtime and marks no program as loaded.
   */
  void onStopRequested();

  /**
   * @brief Provides user input to the runtime.
   *
   * Called when the user enters input in response to an inputRequested signal.
   * If the program was running continuously before the input request, execution
   * is automatically resumed.
   *
   * @param input The user-provided input string.
   */
  void onInputProvided(const QString& input);

 private:
  /**
   * @brief Updates the UI with current thread and variable states.
   *
   * Gathers information from the runtime and emits threadsUpdated and
   * variablesUpdated signals.
   */
  void updateRuntimeDisplay();

  /**
   * @brief Collects current thread information from the runtime.
   * @return Vector of ThreadInfo structures for UI display.
   */
  std::vector<nsbaci::ui::ThreadInfo> gatherThreadInfo();

  /**
   * @brief Collects current variable values from program memory.
   * @return Vector of VariableInfo structures for UI display.
   */
  std::vector<nsbaci::ui::VariableInfo> gatherVariableInfo();

  /**
   * @brief Executes a batch of instructions during continuous execution.
   *
   * Called by the QTimer during continuous execution mode. Executes multiple
   * instructions per timer tick to improve performance while still yielding
   * to the event loop for UI responsiveness.
   */
  void runBatch();

  nsbaci::services::FileService
      fileService;  ///< Service for file I/O operations.
  nsbaci::services::CompilerService
      compilerService;  ///< Service for BACI compilation.
  nsbaci::services::RuntimeService
      runtimeService;  ///< Service for program execution.
  nsbaci::services::DrawingService
      drawingService;  ///< Service for graphical output.

  QString currentProgramName;  ///< Name of the currently loaded program.
  bool programLoaded = false;  ///< True if a program is loaded and ready.
  bool isRunning = false;      ///< True when continuous execution is active.
  bool wasRunningBeforeInput =
      false;  ///< Tracks if execution should resume after input.
  QTimer* runTimer = nullptr;  ///< Timer for continuous execution batching.
};

}  // namespace nsbaci

#endif  // NSBACI_CONTROLLER_H
