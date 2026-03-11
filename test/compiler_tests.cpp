/**
 * @file compiler_tests.cpp
 * @brief Unit tests for the NSBACI compiler
 *
 * Tests the lexer and parser functionality, ensuring correct compilation
 * of various language constructs.
 */

#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

#include "error.h"
#include "instruction.h"
#include "nsbaciCompiler.h"

using namespace nsbaci::compiler;

// Helper class for compiler tests
class CompilerTest : public ::testing::Test {
 protected:
  NsbaciCompiler compiler;

  // Compile and return success status
  bool compile(const std::string& source) {
    auto result = compiler.compile(source);
    return result.ok;
  }

  // Compile and get instructions
  std::pair<bool, InstructionStream> compileAndGetInstructions(
      const std::string& source) {
    auto result = compiler.compile(source);
    return {result.ok, result.instructions};
  }

  // Compile and get errors
  std::vector<nsbaci::Error> compileAndGetErrors(const std::string& source) {
    auto result = compiler.compile(source);
    return result.errors;
  }

  // Check if instruction stream contains opcode
  bool hasOpcode(const InstructionStream& instrs, Opcode op) {
    for (const auto& instr : instrs) {
      if (instr.opcode == op) return true;
    }
    return false;
  }

  // Count occurrences of opcode
  int countOpcode(const InstructionStream& instrs, Opcode op) {
    int count = 0;
    for (const auto& instr : instrs) {
      if (instr.opcode == op) count++;
    }
    return count;
  }
};

// ============== Variable Declaration Tests ==============

TEST_F(CompilerTest, DeclareIntVariable) { EXPECT_TRUE(compile("int x = 5;")); }

TEST_F(CompilerTest, DeclareBoolVariable) {
  EXPECT_TRUE(compile("bool flag = true;"));
}

TEST_F(CompilerTest, DeclareCharVariable) {
  EXPECT_TRUE(compile("char c = 'a';"));
}

TEST_F(CompilerTest, DeclareConstant) {
  EXPECT_TRUE(compile("const int MAX = 100;"));
}

TEST_F(CompilerTest, DeclareMultipleVariables) {
  EXPECT_TRUE(compile("int a = 1; int b = 2; int c = 3;"));
}

TEST_F(CompilerTest, UndeclaredVariableError) {
  EXPECT_FALSE(compile("x = 5;"));
  auto errors = compileAndGetErrors("x = 5;");
  EXPECT_FALSE(errors.empty());
}

TEST_F(CompilerTest, ConstantReassignmentError) {
  EXPECT_FALSE(compile("const int X = 5; X = 10;"));
}

// ============== Array Declaration Tests ==============

TEST_F(CompilerTest, DeclareIntArray) { EXPECT_TRUE(compile("int arr[10];")); }

TEST_F(CompilerTest, DeclareArrayAndAccess) {
  auto [ok, instrs] = compileAndGetInstructions("int arr[5]; arr[0] = 10;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::StoreIndirect));
}

TEST_F(CompilerTest, ArrayAccessWithExpression) {
  EXPECT_TRUE(compile("int arr[10]; int i = 2; arr[i] = 5;"));
}

TEST_F(CompilerTest, ArrayAccessReadAndWrite) {
  EXPECT_TRUE(compile("int arr[10]; arr[0] = 5; int x = arr[0];"));
}

// ============== Arithmetic Expression Tests ==============

TEST_F(CompilerTest, Addition) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 5 + 3;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Add));
}

TEST_F(CompilerTest, Subtraction) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 10 - 3;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Sub));
}

TEST_F(CompilerTest, Multiplication) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 4 * 5;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Mult));
}

TEST_F(CompilerTest, Division) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 20 / 4;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Div));
}

TEST_F(CompilerTest, Modulo) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 17 % 5;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Mod));
}

TEST_F(CompilerTest, UnaryNegation) {
  auto [ok, instrs] = compileAndGetInstructions("int x = -5;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Negate));
}

TEST_F(CompilerTest, ComplexExpression) {
  EXPECT_TRUE(compile("int x = (5 + 3) * 2 - 10 / 2;"));
}

// ============== Compound Assignment Tests ==============

TEST_F(CompilerTest, PlusAssign) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 5; x += 3;");
  EXPECT_TRUE(ok);
  // Should have LoadValue (to get x), Add, Store
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Add));
}

TEST_F(CompilerTest, MinusAssign) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 10; x -= 3;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Sub));
}

TEST_F(CompilerTest, MultiplyAssign) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 5; x *= 2;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Mult));
}

TEST_F(CompilerTest, DivideAssign) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 20; x /= 4;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Div));
}

TEST_F(CompilerTest, ModuloAssign) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 17; x %= 5;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Mod));
}

// ============== Comparison Tests ==============

TEST_F(CompilerTest, EqualityTest) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = (5 == 5);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::TestEQ));
}

TEST_F(CompilerTest, InequalityTest) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = (5 != 3);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::TestNE));
}

TEST_F(CompilerTest, LessThanTest) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = (3 < 5);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::TestLT));
}

TEST_F(CompilerTest, LessEqualTest) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = (3 <= 5);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::TestLE));
}

TEST_F(CompilerTest, GreaterThanTest) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = (5 > 3);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::TestGT));
}

TEST_F(CompilerTest, GreaterEqualTest) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = (5 >= 3);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::TestGE));
}

// ============== Logical Operator Tests ==============

TEST_F(CompilerTest, LogicalAnd) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = true && false;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::And));
}

TEST_F(CompilerTest, LogicalOr) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = true || false;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Or));
}

TEST_F(CompilerTest, LogicalNot) {
  auto [ok, instrs] = compileAndGetInstructions("bool b = !true;");
  EXPECT_TRUE(ok);
  // Logical NOT is implemented as comparison with 0
  EXPECT_TRUE(hasOpcode(instrs, Opcode::TestEQ));
}

// ============== Control Flow Tests ==============

TEST_F(CompilerTest, IfStatement) {
  auto [ok, instrs] = compileAndGetInstructions("if (true) { int x = 1; }");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::JumpZero));
}

TEST_F(CompilerTest, IfElseStatement) {
  auto [ok, instrs] = compileAndGetInstructions(
      "int x = 0; if (true) { x = 1; } else { x = 2; }");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::JumpZero));
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Jump));
}

TEST_F(CompilerTest, WhileLoop) {
  auto [ok, instrs] =
      compileAndGetInstructions("int i = 0; while (i < 5) { i++; }");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::JumpZero));
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Jump));
}

TEST_F(CompilerTest, DoWhileLoop) {
  auto [ok, instrs] =
      compileAndGetInstructions("int i = 0; do { i++; } while (i < 5);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::JumpZero));
}

TEST_F(CompilerTest, ForLoop) {
  auto [ok, instrs] =
      compileAndGetInstructions("for (int i = 0; i < 10; i++) { int x = i; }");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::JumpZero));
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Jump));
}

TEST_F(CompilerTest, ForLoopWithExistingVariable) {
  EXPECT_TRUE(compile("int i = 0; for (i = 0; i < 5; i++) { int x = i; }"));
}

TEST_F(CompilerTest, BreakStatement) {
  auto [ok, instrs] = compileAndGetInstructions("while (true) { break; }");
  EXPECT_TRUE(ok);
  // Break should generate a Jump
  EXPECT_GE(countOpcode(instrs, Opcode::Jump), 2);
}

TEST_F(CompilerTest, ContinueStatement) {
  auto [ok, instrs] =
      compileAndGetInstructions("int i = 0; while (i < 10) { i++; continue; }");
  EXPECT_TRUE(ok);
  EXPECT_GE(countOpcode(instrs, Opcode::Jump), 2);
}

TEST_F(CompilerTest, BreakOutsideLoopError) { EXPECT_FALSE(compile("break;")); }

TEST_F(CompilerTest, ContinueOutsideLoopError) {
  EXPECT_FALSE(compile("continue;"));
}

TEST_F(CompilerTest, NestedLoops) {
  EXPECT_TRUE(
      compile("for (int i = 0; i < 3; i++) {"
              "  for (int j = 0; j < 3; j++) {"
              "    int x = i * j;"
              "  }"
              "}"));
}

// ============== Function Declaration Tests ==============

TEST_F(CompilerTest, VoidFunctionNoParams) {
  auto [ok, instrs] = compileAndGetInstructions("void foo() { return; }");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::ShortReturn));
}

TEST_F(CompilerTest, IntFunctionWithParams) {
  auto [ok, instrs] =
      compileAndGetInstructions("int add(int a, int b) { return a + b; }");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::ExitFunction));
  EXPECT_TRUE(hasOpcode(instrs, Opcode::EnterFrame));
}

TEST_F(CompilerTest, FunctionCall) {
  auto [ok, instrs] = compileAndGetInstructions(
      "int add(int a, int b) { return a + b; }"
      "int x = add(5, 3);");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Call));
}

TEST_F(CompilerTest, RecursiveFunction) {
  EXPECT_TRUE(
      compile("int factorial(int n) {"
              "  if (n <= 1) { return 1; }"
              "  return n * factorial(n - 1);"
              "}"));
}

TEST_F(CompilerTest, UndeclaredFunctionError) {
  EXPECT_FALSE(compile("int x = foo(5);"));
}

TEST_F(CompilerTest, WrongArgumentCountError) {
  EXPECT_FALSE(
      compile("int add(int a, int b) { return a + b; }"
              "int x = add(5);"));
}

TEST_F(CompilerTest, NestedFunctionCalls) {
  EXPECT_TRUE(
      compile("int add(int a, int b) { return a + b; }"
              "int mult(int a, int b) { return a * b; }"
              "int x = add(mult(2, 3), mult(4, 5));"));
}

// ============== I/O Tests ==============

TEST_F(CompilerTest, CoutInteger) {
  auto [ok, instrs] = compileAndGetInstructions("cout << 42;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Write));
}

TEST_F(CompilerTest, CoutString) {
  auto [ok, instrs] = compileAndGetInstructions("cout << \"Hello\";");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::WriteRawString));
}

TEST_F(CompilerTest, CoutEndl) {
  auto [ok, instrs] = compileAndGetInstructions("cout << endl;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Writeln));
}

TEST_F(CompilerTest, CoutChained) {
  EXPECT_TRUE(compile("cout << \"x = \" << 5 << endl;"));
}

TEST_F(CompilerTest, CinInteger) {
  auto [ok, instrs] = compileAndGetInstructions("int x; cin >> x;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Read));
}

// ============== Increment/Decrement Tests ==============

TEST_F(CompilerTest, PostIncrement) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 5; x++;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Add));
}

TEST_F(CompilerTest, PostDecrement) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 5; x--;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Sub));
}

// ============== Edge Cases ==============

TEST_F(CompilerTest, EmptyProgram) { EXPECT_TRUE(compile("")); }

TEST_F(CompilerTest, OnlyComments) {
  EXPECT_TRUE(compile("// This is a comment\n// Another comment"));
}

TEST_F(CompilerTest, MultiLineComments) {
  EXPECT_TRUE(compile("/* Multi-line\n   comment */"));
}

TEST_F(CompilerTest, ZeroLiteral) { EXPECT_TRUE(compile("int x = 0;")); }

TEST_F(CompilerTest, NegativeLiteral) { EXPECT_TRUE(compile("int x = -42;")); }

TEST_F(CompilerTest, CharLiteral) { EXPECT_TRUE(compile("char c = 'Z';")); }

TEST_F(CompilerTest, BoolLiterals) {
  EXPECT_TRUE(compile("bool t = true; bool f = false;"));
}

TEST_F(CompilerTest, DeeplyNestedExpressions) {
  EXPECT_TRUE(compile("int x = ((((1 + 2) * 3) - 4) / 5);"));
}

TEST_F(CompilerTest, LongIdentifier) {
  EXPECT_TRUE(compile("int very_long_variable_name_that_is_valid = 42;"));
}

TEST_F(CompilerTest, MultipleStatementsSameLine) {
  EXPECT_TRUE(compile("int a = 1; int b = 2; int c = 3;"));
}

// ============== Halt/Return Tests ==============

TEST_F(CompilerTest, ReturnInMain) {
  auto [ok, instrs] = compileAndGetInstructions("return;");
  EXPECT_TRUE(ok);
  EXPECT_TRUE(hasOpcode(instrs, Opcode::Halt));
}

TEST_F(CompilerTest, ExplicitHalt) {
  auto [ok, instrs] = compileAndGetInstructions("int x = 5;");
  EXPECT_TRUE(ok);
  // Program should end with Halt
  if (!instrs.empty()) {
    EXPECT_EQ(instrs.back().opcode, Opcode::Halt);
  }
}

// ============== Scope Tests ==============

TEST_F(CompilerTest, SiblingBlocksSameVariableName) {
  // Variables with the same name in sibling scopes should be allowed
  std::string source = R"(
        { int i = 1; }
        { int i = 2; }
        { int i = 3; }
    )";
  EXPECT_TRUE(compile(source));
}

TEST_F(CompilerTest, CobeginBlocksSameVariableName) {
  // Each cobegin block should have its own scope
  std::string source = R"(
        cobegin {
            int i = 0;
            int temp = 1;
        }
        {
            int i = 0;
            int temp = 2;
        }
        {
            int i = 0;
            int temp = 3;
        } coend
    )";
  EXPECT_TRUE(compile(source));
}

TEST_F(CompilerTest, NestedScopesShadowing) {
  // Inner scope can shadow outer scope variables
  std::string source = R"(
        int x = 1;
        {
            int x = 2;
            {
                int x = 3;
            }
        }
    )";
  EXPECT_TRUE(compile(source));
}

TEST_F(CompilerTest, SameScopeRedeclarationError) {
  // Same variable name in same scope should error
  EXPECT_FALSE(compile("int x = 1; int x = 2;"));
  auto errors = compileAndGetErrors("int x = 1; int x = 2;");
  EXPECT_FALSE(errors.empty());
  EXPECT_TRUE(errors[0].basic.message.find("already declared") !=
              std::string::npos);
}

TEST_F(CompilerTest, LoopVariableScopeIsolation) {
  // Loop variables in separate loops should be independent
  std::string source = R"(
        for (int i = 0; i < 5; i++) { }
        for (int i = 0; i < 5; i++) { }
    )";
  EXPECT_TRUE(compile(source));
}

// ============== Error Payload Tests ==============

TEST_F(CompilerTest, ErrorHasCompileErrorPayload) {
  // Compilation errors should have CompileError payload type
  auto errors = compileAndGetErrors("undeclared_var = 5;");
  ASSERT_FALSE(errors.empty());

  // Check that payload is CompileError type (index 2 in variant)
  EXPECT_TRUE(
      std::holds_alternative<nsbaci::types::CompileError>(errors[0].payload));
}

TEST_F(CompilerTest, ErrorHasLineInfo) {
  // Errors should have line information
  std::string source = "int x = 1;\nundeclared = 5;";
  auto errors = compileAndGetErrors(source);
  ASSERT_FALSE(errors.empty());

  auto* compileErr =
      std::get_if<nsbaci::types::CompileError>(&errors[0].payload);
  ASSERT_NE(compileErr, nullptr);
  EXPECT_GT(compileErr->line, 0);  // Line should be positive
}

TEST_F(CompilerTest, ErrorHasColumnInfo) {
  // Errors should have column information
  auto errors =
      compileAndGetErrors("     undeclared = 5;");  // 5 spaces before error
  ASSERT_FALSE(errors.empty());

  auto* compileErr =
      std::get_if<nsbaci::types::CompileError>(&errors[0].payload);
  ASSERT_NE(compileErr, nullptr);
  EXPECT_GT(compileErr->column, 0);  // Column should be positive
}

TEST_F(CompilerTest, MultipleErrorsHavePayloads) {
  // Multiple errors should all have proper payloads
  std::string source = R"(
        a = 1;
        b = 2;
        c = 3;
    )";
  auto errors = compileAndGetErrors(source);

  for (const auto& err : errors) {
    EXPECT_TRUE(
        std::holds_alternative<nsbaci::types::CompileError>(err.payload));
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
