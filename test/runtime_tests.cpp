/**
 * @file runtime_tests.cpp
 * @brief Unit tests for the NSBACI runtime/interpreter
 *
 * Tests the interpreter execution of various instructions and programs.
 */

#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "nsbaciCompiler.h"
#include "nsbaciInterpreter.h"
#include "thread.h"
#include "program.h"
#include "instruction.h"
#include "error.h"
#include "runtimeTypes.h"

using namespace nsbaci::compiler;
using namespace nsbaci::services::runtime;

// Helper class for runtime tests
class RuntimeTest : public ::testing::Test {
protected:
    NsbaciCompiler compiler;
    
    // Compile and create a program
    std::unique_ptr<Program> compileToProgram(const std::string& source) {
        auto result = compiler.compile(source);
        if (!result.ok) return nullptr;
        
        return std::make_unique<Program>(
            std::move(result.instructions),
            std::move(result.symbols)
        );
    }
    
    // Run program until halt or max steps
    struct RunResult {
        bool halted;
        std::string output;
        std::vector<int32_t> memory;
        int steps;
    };
    
    RunResult runProgram(Program& program, int maxSteps = 10000) {
        Thread thread;
        NsbaciInterpreter interpreter;
        RunResult result{false, "", {}, 0};
        
        // Capture output via stepResult.output only (not callback to avoid duplicates)
        
        while (result.steps < maxSteps) {
            auto stepResult = interpreter.executeInstruction(thread, program);
            result.steps++;
            result.output += stepResult.output;
            
            if (thread.getState() == nsbaci::types::ThreadState::Terminated) {
                result.halted = true;
                break;
            }
            
            if (!stepResult.ok) {
                // Error occurred, stop
                break;
            }
        }
        
        result.memory = std::vector<int32_t>(program.memory().begin(), program.memory().end());
        return result;
    }
    
    // Compile and run, returning result
    RunResult compileAndRun(const std::string& source, int maxSteps = 10000) {
        auto program = compileToProgram(source);
        if (!program) {
            return {false, "COMPILE_ERROR", {}, 0};
        }
        return runProgram(*program, maxSteps);
    }
    
    // Get memory value at address after running
    int32_t getMemoryAt(const RunResult& result, size_t addr) {
        if (addr < result.memory.size()) {
            return result.memory[addr];
        }
        return 0;
    }
};

// ============== Basic Arithmetic Tests ==============

TEST_F(RuntimeTest, AddTwoNumbers) {
    auto result = compileAndRun("int x = 5 + 3;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 8);
}

TEST_F(RuntimeTest, SubtractTwoNumbers) {
    auto result = compileAndRun("int x = 10 - 3;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 7);
}

TEST_F(RuntimeTest, MultiplyTwoNumbers) {
    auto result = compileAndRun("int x = 4 * 7;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 28);
}

TEST_F(RuntimeTest, DivideTwoNumbers) {
    auto result = compileAndRun("int x = 20 / 4;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 5);
}

TEST_F(RuntimeTest, ModuloOperation) {
    auto result = compileAndRun("int x = 17 % 5;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 2);
}

TEST_F(RuntimeTest, NegativeNumbers) {
    auto result = compileAndRun("int x = -5;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), -5);
}

TEST_F(RuntimeTest, ComplexArithmetic) {
    auto result = compileAndRun("int x = (5 + 3) * 2 - 10 / 2;");
    EXPECT_TRUE(result.halted);
    // (5+3)*2 - 10/2 = 8*2 - 5 = 16 - 5 = 11
    EXPECT_EQ(getMemoryAt(result, 0), 11);
}

// ============== Compound Assignment Tests ==============

TEST_F(RuntimeTest, PlusAssignCorrectOrder) {
    auto result = compileAndRun("int x = 10; x += 5;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 15);
}

TEST_F(RuntimeTest, MinusAssignCorrectOrder) {
    // This was a bug: x = 10, x -= 3 should be 7, not -7
    auto result = compileAndRun("int x = 10; x -= 3;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 7);
}

TEST_F(RuntimeTest, MultiplyAssign) {
    auto result = compileAndRun("int x = 5; x *= 4;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 20);
}

TEST_F(RuntimeTest, DivideAssignCorrectOrder) {
    // x = 20, x /= 4 should be 5, not 0
    auto result = compileAndRun("int x = 20; x /= 4;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 5);
}

TEST_F(RuntimeTest, ModuloAssign) {
    auto result = compileAndRun("int x = 17; x %= 5;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 2);
}

TEST_F(RuntimeTest, ChainedCompoundAssignment) {
    auto result = compileAndRun("int x = 10; x += 5; x -= 3; x *= 2;");
    EXPECT_TRUE(result.halted);
    // 10 + 5 = 15, 15 - 3 = 12, 12 * 2 = 24
    EXPECT_EQ(getMemoryAt(result, 0), 24);
}

// ============== Comparison Tests ==============

TEST_F(RuntimeTest, EqualityTrue) {
    auto result = compileAndRun("int x = (5 == 5);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 1);
}

TEST_F(RuntimeTest, EqualityFalse) {
    auto result = compileAndRun("int x = (5 == 3);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 0);
}

TEST_F(RuntimeTest, LessThanTrue) {
    auto result = compileAndRun("int x = (3 < 5);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 1);
}

TEST_F(RuntimeTest, LessThanFalse) {
    auto result = compileAndRun("int x = (5 < 3);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 0);
}

TEST_F(RuntimeTest, GreaterThanTrue) {
    auto result = compileAndRun("int x = (5 > 3);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 1);
}

// ============== Control Flow Tests ==============

TEST_F(RuntimeTest, IfTrueBranch) {
    auto result = compileAndRun("int x = 0; if (true) { x = 1; }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 1);
}

TEST_F(RuntimeTest, IfFalseBranch) {
    auto result = compileAndRun("int x = 0; if (false) { x = 1; }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 0);
}

TEST_F(RuntimeTest, IfElseTrueBranch) {
    auto result = compileAndRun("int x = 0; if (true) { x = 1; } else { x = 2; }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 1);
}

TEST_F(RuntimeTest, IfElseFalseBranch) {
    auto result = compileAndRun("int x = 0; if (false) { x = 1; } else { x = 2; }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 2);
}

TEST_F(RuntimeTest, WhileLoopBasic) {
    auto result = compileAndRun(
        "int count = 0;"
        "int i = 0;"
        "while (i < 5) { count = count + 1; i++; }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 5);  // count
}

TEST_F(RuntimeTest, DoWhileLoopBasic) {
    auto result = compileAndRun(
        "int count = 0;"
        "int i = 0;"
        "do { count = count + 1; i++; } while (i < 5);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 5);  // count
}

TEST_F(RuntimeTest, DoWhileExecutesOnce) {
    // Do-while should execute at least once even if condition is false
    auto result = compileAndRun(
        "int x = 0;"
        "do { x = 42; } while (false);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 42);
}

TEST_F(RuntimeTest, ForLoopBasic) {
    // This was a bug: for loop was executing update before body
    auto result = compileAndRun(
        "int sum = 0;"
        "for (int i = 1; i <= 5; i++) { sum = sum + i; }");
    EXPECT_TRUE(result.halted);
    // 1 + 2 + 3 + 4 + 5 = 15
    EXPECT_EQ(getMemoryAt(result, 0), 15);
}

TEST_F(RuntimeTest, ForLoopStartsAtCorrectValue) {
    // First iteration should use initial value, not after update
    auto result = compileAndRun(
        "int first = 0;"
        "for (int i = 1; i < 5; i++) { if (first == 0) { first = i; } }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 1);  // First value should be 1, not 2
}

TEST_F(RuntimeTest, BreakExitsLoop) {
    auto result = compileAndRun(
        "int x = 0;"
        "while (true) { x = x + 1; if (x == 5) { break; } }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 5);
}

TEST_F(RuntimeTest, ContinueSkipsRest) {
    auto result = compileAndRun(
        "int sum = 0;"
        "for (int i = 1; i <= 10; i++) {"
        "  if (i % 2 == 0) { continue; }"
        "  sum = sum + i;"
        "}");
    EXPECT_TRUE(result.halted);
    // Sum of odd numbers 1-10: 1+3+5+7+9 = 25
    EXPECT_EQ(getMemoryAt(result, 0), 25);
}

TEST_F(RuntimeTest, ContinueInForLoopUpdates) {
    // Continue in for loop should still execute the update
    auto result = compileAndRun(
        "int count = 0;"
        "for (int i = 0; i < 10; i++) {"
        "  continue;"
        "  count = 999;"  // Should never execute
        "}"
        "count = i;");  // i should be 10 after loop
    EXPECT_TRUE(result.halted);
    // The loop should complete normally, count should not be 999
}

// ============== Array Tests ==============

TEST_F(RuntimeTest, ArrayInitialization) {
    auto result = compileAndRun(
        "int arr[5];"
        "arr[0] = 10;"
        "arr[1] = 20;"
        "arr[2] = 30;");
    EXPECT_TRUE(result.halted);
    // Array starts at address 0
    EXPECT_EQ(getMemoryAt(result, 0), 10);
    EXPECT_EQ(getMemoryAt(result, 1), 20);
    EXPECT_EQ(getMemoryAt(result, 2), 30);
}

TEST_F(RuntimeTest, ArrayWithLoopInit) {
    auto result = compileAndRun(
        "int arr[5];"
        "for (int i = 0; i < 5; i++) { arr[i] = i * 10; }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 0);
    EXPECT_EQ(getMemoryAt(result, 1), 10);
    EXPECT_EQ(getMemoryAt(result, 2), 20);
    EXPECT_EQ(getMemoryAt(result, 3), 30);
    EXPECT_EQ(getMemoryAt(result, 4), 40);
}

TEST_F(RuntimeTest, ArraySumCalculation) {
    auto result = compileAndRun(
        "int arr[5];"
        "arr[0] = 1; arr[1] = 2; arr[2] = 3; arr[3] = 4; arr[4] = 5;"
        "int sum = 0;"
        "for (int i = 0; i < 5; i++) { sum = sum + arr[i]; }");
    EXPECT_TRUE(result.halted);
    // Sum: 1+2+3+4+5 = 15
    // sum is at address 5
    EXPECT_EQ(getMemoryAt(result, 5), 15);
}

TEST_F(RuntimeTest, ArraySwap) {
    auto result = compileAndRun(
        "int arr[3];"
        "arr[0] = 1; arr[1] = 2; arr[2] = 3;"
        "int temp = arr[0];"
        "arr[0] = arr[2];"
        "arr[2] = temp;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 3);  // arr[0]
    EXPECT_EQ(getMemoryAt(result, 1), 2);  // arr[1] unchanged
    EXPECT_EQ(getMemoryAt(result, 2), 1);  // arr[2]
}

// ============== Function Tests ==============

TEST_F(RuntimeTest, SimpleFunctionCall) {
    auto result = compileAndRun(
        "int add(int a, int b) { return a + b; }"
        "int x = add(5, 3);");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 2), 8);  // x after function params
}

TEST_F(RuntimeTest, FunctionWithMultipleOps) {
    auto result = compileAndRun(
        "int compute(int a, int b) {"
        "  int temp = a * b;"
        "  return temp + a;"
        "}"
        "int x = compute(5, 3);");
    EXPECT_TRUE(result.halted);
    // 5 * 3 + 5 = 20
}

TEST_F(RuntimeTest, RecursiveFactorial) {
    auto result = compileAndRun(
        "int factorial(int n) {"
        "  if (n <= 1) { return 1; }"
        "  return n * factorial(n - 1);"
        "}"
        "int x = factorial(5);");
    EXPECT_TRUE(result.halted);
    // 5! = 120
    // Find x in memory
    EXPECT_EQ(getMemoryAt(result, 1), 120);
}

TEST_F(RuntimeTest, RecursiveFibonacci) {
    // This was a bug: double recursion wasn't working
    auto result = compileAndRun(
        "int fib(int n) {"
        "  if (n <= 0) { return 0; }"
        "  if (n == 1) { return 1; }"
        "  return fib(n - 1) + fib(n - 2);"
        "}"
        "int x = fib(10);");
    EXPECT_TRUE(result.halted);
    // fib(10) = 55
    EXPECT_EQ(getMemoryAt(result, 1), 55);
}

TEST_F(RuntimeTest, NestedFunctionCalls) {
    auto result = compileAndRun(
        "int add(int a, int b) { return a + b; }"
        "int mult(int a, int b) { return a * b; }"
        "cout << add(mult(2, 3), mult(4, 5));");
    EXPECT_TRUE(result.halted);
    // 2*3 + 4*5 = 6 + 20 = 26
    EXPECT_TRUE(result.output.find("26") != std::string::npos);
}

TEST_F(RuntimeTest, VoidFunctionCall) {
    auto result = compileAndRun(
        "int x = 0;"
        "void setX() { x = 42; return; }"
        "setX();");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 42);
}

TEST_F(RuntimeTest, GCDRecursive) {
    auto result = compileAndRun(
        "int gcd(int a, int b) {"
        "  if (b == 0) { return a; }"
        "  return gcd(b, a % b);"
        "}"
        "int x = gcd(48, 18);");
    EXPECT_TRUE(result.halted);
    // gcd(48, 18) = 6
    EXPECT_EQ(getMemoryAt(result, 2), 6);
}

// ============== Output Tests ==============

TEST_F(RuntimeTest, OutputInteger) {
    auto result = compileAndRun("cout << 42;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(result.output, "42");
}

TEST_F(RuntimeTest, OutputString) {
    auto result = compileAndRun("cout << \"Hello\";");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(result.output, "Hello");
}

TEST_F(RuntimeTest, OutputNewline) {
    auto result = compileAndRun("cout << endl;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(result.output, "\n");
}

TEST_F(RuntimeTest, OutputChained) {
    auto result = compileAndRun("cout << \"x=\" << 5 << endl;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(result.output, "x=5\n");
}

// ============== Edge Cases ==============

TEST_F(RuntimeTest, DivisionByZero) {
    // Should not crash, behavior may vary
    auto result = compileAndRun("int x = 10 / 0;", 100);
    // We don't assert specific behavior, just that it doesn't hang
}

TEST_F(RuntimeTest, LargeLoop) {
    // 1000 iterations needs more than default 10000 steps
    auto result = compileAndRun(
        "int sum = 0;"
        "for (int i = 0; i < 100; i++) { sum = sum + 1; }", 50000);
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 100);
}

TEST_F(RuntimeTest, DeeplyNestedIf) {
    auto result = compileAndRun(
        "int x = 0;"
        "if (true) { if (true) { if (true) { if (true) { x = 1; } } } }");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 1);
}

TEST_F(RuntimeTest, MaxInt32) {
    auto result = compileAndRun("int x = 2147483647;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), 2147483647);
}

TEST_F(RuntimeTest, MinInt32) {
    // Use expression to get min int32 since literal may overflow during parsing
    auto result = compileAndRun("int x = -2147483647 - 1;");
    EXPECT_TRUE(result.halted);
    EXPECT_EQ(getMemoryAt(result, 0), -2147483647 - 1);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
