%skeleton "lalr1.cc"
%language "c++"
%require "3.2"

%define api.namespace {nsbaci::compiler}
%define api.parser.class {Parser}
%define api.value.type variant
%define parse.error verbose
%locations

%code requires {
  #include <string>
  #include <memory>
  #include <vector>
  #include <unordered_map>
  #include <stack>
  #include "instruction.h"
  #include "error.h"
  namespace nsbaci::compiler { class Lexer; }

  namespace nsbaci::compiler {
    enum class VarType { Int, Bool, Char, Void };

    struct Symbol {
      std::string name;
      VarType type;
      uint32_t address;
      bool isConst;
      int scopeLevel;
      bool isParam = false;  // True if this is a function parameter
    };

    struct FunctionInfo {
      std::string name;
      VarType returnType;
      std::vector<std::pair<std::string, VarType>> params;
      uint32_t address;  // Instruction address where function starts
    };

    struct SymbolTable {
      std::unordered_map<std::string, Symbol> symbols;
      std::unordered_map<std::string, FunctionInfo> functions;
      uint32_t nextAddress = 0;
      int currentScope = 0;

      bool declare(const std::string& name, VarType type, bool isConst = false, bool isParam = false) {
        if (symbols.count(name) && symbols[name].scopeLevel == currentScope) {
          return false; // Already declared in this scope
        }
        symbols[name] = {name, type, nextAddress++, isConst, currentScope, isParam};
        return true;
      }

      bool declareFunction(const std::string& name, VarType returnType, 
                          const std::vector<std::pair<std::string, VarType>>& params, uint32_t addr) {
        if (functions.count(name)) {
          return false; // Already declared
        }
        functions[name] = {name, returnType, params, addr};
        return true;
      }

      FunctionInfo* lookupFunction(const std::string& name) {
        auto it = functions.find(name);
        return it != functions.end() ? &it->second : nullptr;
      }

      Symbol* lookup(const std::string& name) {
        auto it = symbols.find(name);
        return it != symbols.end() ? &it->second : nullptr;
      }

      void enterScope() { currentScope++; }
      void exitScope() {
        // Remove symbols declared in this scope to allow shadowing in sibling scopes
        for (auto it = symbols.begin(); it != symbols.end(); ) {
          if (it->second.scopeLevel == currentScope) {
            it = symbols.erase(it);
          } else {
            ++it;
          }
        }
        currentScope--;
      }
    };

    // Helper to emit instructions
    inline void emit(InstructionStream& is, Opcode op) {
      is.emplace_back(op);
    }
    inline void emit(InstructionStream& is, Opcode op, int32_t arg) {
      is.emplace_back(op, arg);
    }
    inline void emit(InstructionStream& is, Opcode op, uint32_t arg) {
      is.emplace_back(op, arg);
    }
    inline void emit(InstructionStream& is, Opcode op, uint32_t arg1, int32_t arg2) {
      is.emplace_back(op, arg1, arg2);
    }
    inline void emit(InstructionStream& is, Opcode op, const std::string& arg) {
      is.emplace_back(op, arg);
    }

    // For patching jumps
    inline size_t emitJump(InstructionStream& is, Opcode op) {
      size_t addr = is.size();
      is.emplace_back(op, int32_t(0)); // Placeholder
      return addr;
    }
    inline void patchJump(InstructionStream& is, size_t addr) {
      is[addr].operand1 = int32_t(is.size());
    }
    inline void patchJump(InstructionStream& is, size_t addr, size_t target) {
      is[addr].operand1 = int32_t(target);
    }

    // Helper to create compilation errors with proper payload
    inline void emitError(std::vector<nsbaci::Error>& errors, const std::string& msg, int line, int column) {
      nsbaci::Error err;
      err.basic.severity = nsbaci::types::ErrSeverity::Error;
      err.basic.message = msg;
      err.basic.type = nsbaci::types::ErrType::compilationError;
      err.payload = nsbaci::types::CompileError{line, column};
      errors.push_back(std::move(err));
    }
  }
}

%code {
  #include "lexer.h"
  #define yylex lexer.yylex

  // Global symbol table for parsing - passed implicitly
  static nsbaci::compiler::SymbolTable symtab;

  // Break/continue stack for loops
  static std::stack<std::vector<size_t>> breakStack;
  static std::stack<size_t> continueStack;

  // For-loop specific: track pending continue jumps that need to go to update section
  static std::stack<std::vector<size_t>> forContinueStack;
  static std::stack<bool> isForLoopStack;  // Track if current loop is a for-loop

  // For-loop update instruction buffer and marker
  static std::stack<nsbaci::compiler::InstructionStream> forUpdateBuffer;
  static std::stack<size_t> forUpdateMarker;

  // Function parsing helpers
  static std::string currentFunctionName;
  static nsbaci::compiler::VarType currentFunctionReturnType;
  static std::vector<std::pair<std::string, nsbaci::compiler::VarType>> currentFunctionParams;
  static std::stack<int> argCountStack;  // Stack for nested function calls
  
  // For frame-based local variable management
  static uint32_t currentFunctionFrameStart;
  static int32_t currentFunctionFrameSize;
  
  // Cobegin/coend: track block start addresses and jump-over addresses
  static std::vector<size_t> cobeginBlockStarts;  // Start address of each block
  static size_t cobeginJumpOverAddr;              // Address of jump that skips all blocks
}

%parse-param { nsbaci::compiler::Lexer& lexer }
%parse-param { nsbaci::compiler::InstructionStream& instructions }
%parse-param { std::vector<nsbaci::Error>& errors }
%parse-param { nsbaci::compiler::SymbolTable& outSymbols }

// Tokens
%token <std::string> IDENT
%token <std::string> STRING_LIT
%token <int> NUMBER
%token <char> CHAR_LIT

// Type keywords
%token INT BOOL CHAR VOID CONST

// Control flow
%token IF ELSE WHILE DO FOR BREAK CONTINUE RETURN

// I/O
%token COUT CIN ENDL
%token SHL SHR

// Concurrency
%token SEMAPHORE COBEGIN COEND P_OP V_OP

// Drawing
%token CANVAS DRAW_SET_COLOR DRAW_SET_LINE_WIDTH
%token DRAW_CIRCLE DRAW_RECT DRAW_TRIANGLE DRAW_LINE DRAW_ELLIPSE
%token DRAW_PIXEL DRAW_TEXT DRAW_CLEAR DRAW_REFRESH
%token FILL_CIRCLE FILL_RECT FILL_TRIANGLE FILL_ELLIPSE

// Color macros
%token COLOR_RED COLOR_GREEN COLOR_BLUE COLOR_WHITE COLOR_BLACK
%token COLOR_YELLOW COLOR_CYAN COLOR_MAGENTA COLOR_ORANGE
%token COLOR_PINK COLOR_PURPLE COLOR_GRAY COLOR_BROWN

// Position macros
%token POS_CENTER POS_TOP_LEFT POS_TOP_CENTER POS_TOP_RIGHT
%token POS_CENTER_LEFT POS_CENTER_RIGHT POS_BOTTOM_LEFT
%token POS_BOTTOM_CENTER POS_BOTTOM_RIGHT

// Literals
%token TRUE_LIT FALSE_LIT

// Operators
%token EQ NE LE GE AND OR
%token INC DEC
%token PLUS_ASSIGN MINUS_ASSIGN MULT_ASSIGN DIV_ASSIGN MOD_ASSIGN

// Precedence (lowest to highest)
%right '='
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right UNARY '!'
%left INC DEC

// Types for non-terminals
%type <nsbaci::compiler::VarType> type_spec
%type <size_t> if_head

%%

start:
    program
    {
      // Ensure program ends with a Halt
      emit(instructions, Opcode::Halt);
      // Copy internal symbol table to output parameter
      outSymbols = symtab;
    }
  ;

program:
    %empty
    {
      // Initialize symbol table
      symtab = nsbaci::compiler::SymbolTable{};
    }
  | program top_level_item
  ;

top_level_item:
    statement
  | function_def
  ;

statement:
    declaration
  | assignment_stmt ';'
  | block
  | if_stmt
  | while_stmt
  | do_while_stmt
  | for_stmt
  | break_stmt
  | continue_stmt
  | return_stmt
  | cout_stmt ';'
  | cin_stmt ';'
  | expr_stmt
  | cobegin_stmt
  | p_stmt ';'
  | v_stmt ';'
  | draw_stmt ';'
  | ';'  /* empty statement */
  ;

declaration:
    type_spec IDENT ';'
    {
      if (!symtab.declare($2, $1)) {
        emitError(errors, "Variable '" + $2 + "' already declared", @2.begin.line, @2.begin.column);
      }
      // Initialize to 0
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::Store, symtab.lookup($2)->address);
    }
  | type_spec IDENT '=' expr ';'
    {
      if (!symtab.declare($2, $1)) {
        emitError(errors, "Variable '" + $2 + "' already declared", @2.begin.line, @2.begin.column);
      }
      // expr already on stack, store it
      emit(instructions, Opcode::Store, symtab.lookup($2)->address);
    }
  | SEMAPHORE IDENT '=' expr ';'
    {
      // Semaphore declaration with initial value
      if (!symtab.declare($2, VarType::Int)) {
        emitError(errors, "Semaphore '" + $2 + "' already declared", @2.begin.line, @2.begin.column);
      }
      // Initialize semaphore with value from expr (already on stack)
      emit(instructions, Opcode::StoreSemaphore, symtab.lookup($2)->address);
    }
  | SEMAPHORE IDENT ';'
    {
      // Semaphore declaration with default value 0
      if (!symtab.declare($2, VarType::Int)) {
        emitError(errors, "Semaphore '" + $2 + "' already declared", @2.begin.line, @2.begin.column);
      }
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::StoreSemaphore, symtab.lookup($2)->address);
    }
  | CONST type_spec IDENT '=' expr ';'
    {
      if (!symtab.declare($3, $2, true)) {
        emitError(errors, "Constant '" + $3 + "' already declared", @3.begin.line, @3.begin.column);
      }
      emit(instructions, Opcode::Store, symtab.lookup($3)->address);
    }
  | type_spec IDENT '[' NUMBER ']' ';'
    {
      // Array declaration - reserve space
      for (int i = 0; i < $4; i++) {
        std::string elemName = $2 + "[" + std::to_string(i) + "]";
        symtab.declare(elemName, $1);
        emit(instructions, Opcode::PushLiteral, int32_t(0));
        emit(instructions, Opcode::Store, symtab.lookup(elemName)->address);
      }
      // Store base address in symbol table under array name
      std::string baseName = $2 + "[0]";
      Symbol* base = symtab.lookup(baseName);
      if (base) {
        symtab.symbols[$2] = {$2, $1, base->address, false, symtab.currentScope};
      }
    }
  ;

type_spec:
    INT   { $$ = nsbaci::compiler::VarType::Int; }
  | BOOL  { $$ = nsbaci::compiler::VarType::Bool; }
  | CHAR  { $$ = nsbaci::compiler::VarType::Char; }
  | VOID  { $$ = nsbaci::compiler::VarType::Void; }
  ;

assignment_stmt:
    IDENT '=' expr
    {
      Symbol* sym = symtab.lookup($1);
      if (!sym) {
        emitError(errors, "Undeclared variable '" + $1 + "'", @1.begin.line, @1.begin.column);
      } else if (sym->isConst) {
        emitError(errors, "Cannot assign to constant '" + $1 + "'", @1.begin.line, @1.begin.column);
      } else {
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT '[' expr ']' '=' expr
    {
      // Array element assignment
      // Stack after parsing: [index, value]
      Symbol* sym = symtab.lookup($1);
      if (!sym) {
        emitError(errors, "Undeclared array '" + $1 + "'", @1.begin.line, @1.begin.column);
      } else {
        // Stack: [index, value]
        // Swap to get: [value, index]
        emit(instructions, Opcode::Swap);
        // Push base address: [value, index, base]
        emit(instructions, Opcode::PushLiteral, int32_t(sym->address));
        // Add base+index: [value, address]
        emit(instructions, Opcode::Add);
        // StoreIndirect pops address then value
        emit(instructions, Opcode::StoreIndirect);
      }
    }
  | IDENT PLUS_ASSIGN
    {
      // Load variable before expr is evaluated
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
      }
    }
    expr
    {
      Symbol* sym = symtab.lookup($1);
      if (!sym) {
        emitError(errors, "Undeclared variable '" + $1 + "'", @1.begin.line, @1.begin.column);
      } else {
        // Stack: [var, expr] -> var + expr
        emit(instructions, Opcode::Add);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT MINUS_ASSIGN
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
      }
    }
    expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        // Stack: [var, expr] -> var - expr
        emit(instructions, Opcode::Sub);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT MULT_ASSIGN
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
      }
    }
    expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        // Stack: [var, expr] -> var * expr
        emit(instructions, Opcode::Mult);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT DIV_ASSIGN
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
      }
    }
    expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        // Stack: [var, expr] -> var / expr
        emit(instructions, Opcode::Div);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT MOD_ASSIGN
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
      }
    }
    expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        // Stack: [var, expr] -> var % expr
        emit(instructions, Opcode::Mod);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  ;

block:
    '{' { symtab.enterScope(); } stmt_list '}' { symtab.exitScope(); }
  ;

stmt_list:
    %empty
  | stmt_list statement
  ;

if_head:
    IF '(' expr ')'
    {
      $$ = emitJump(instructions, Opcode::JumpZero);
    }
  ;

if_stmt:
    if_head statement
    {
      patchJump(instructions, $1);
    }
  | if_head statement ELSE
    {
      size_t skipElse = emitJump(instructions, Opcode::Jump);
      patchJump(instructions, $1);
      $1 = skipElse;  // Reuse for patching after else
    }
    statement
    {
      patchJump(instructions, $1);
    }
  ;

while_stmt:
    WHILE
    {
      breakStack.push({});
      isForLoopStack.push(false);  // This is a while-loop
    }
    '('
    {
      continueStack.push(instructions.size());  // Mark condition start
    }
    expr ')'
    {
      size_t exitJump = emitJump(instructions, Opcode::JumpZero);
      breakStack.top().push_back(exitJump);
    }
    statement
    {
      // Jump back to condition
      emit(instructions, Opcode::Jump, int32_t(continueStack.top()));
      // Patch all breaks
      for (size_t addr : breakStack.top()) {
        patchJump(instructions, addr);
      }
      breakStack.pop();
      continueStack.pop();
      isForLoopStack.pop();
    }
  ;

do_while_stmt:
    DO
    {
      breakStack.push({});
      continueStack.push(instructions.size());
      isForLoopStack.push(false);  // This is a do-while-loop
    }
    statement WHILE '(' expr ')' ';'
    {
      // Jump back if true
      emit(instructions, Opcode::JumpZero, int32_t(instructions.size() + 2));
      emit(instructions, Opcode::Jump, int32_t(continueStack.top()));
      // Patch breaks
      for (size_t addr : breakStack.top()) {
        patchJump(instructions, addr);
      }
      breakStack.pop();
      continueStack.pop();
      isForLoopStack.pop();
    }
  ;

for_stmt:
    FOR '(' for_init ';'
    {
      breakStack.push({});
      forUpdateBuffer.push({});  // Buffer for update code
      forContinueStack.push({});  // Track continues that need to jump to update
      isForLoopStack.push(true);  // This is a for-loop
      continueStack.push(instructions.size());  // Condition start (for reference)
    }
    expr ';'
    {
      size_t exitJump = emitJump(instructions, Opcode::JumpZero);
      breakStack.top().push_back(exitJump);
      forUpdateMarker.push(instructions.size());  // Mark start of update code
    }
    for_update ')'
    {
      // Move update instructions from main stream to buffer (preserving order)
      size_t marker = forUpdateMarker.top();
      for (size_t i = marker; i < instructions.size(); i++) {
        forUpdateBuffer.top().push_back(instructions[i]);
      }
      instructions.resize(marker);  // Remove the update instructions
      forUpdateMarker.pop();
    }
    statement
    {
      // Patch all continue jumps to point here (start of update code)
      for (size_t addr : forContinueStack.top()) {
        patchJump(instructions, addr);
      }
      forContinueStack.pop();
      // Emit the buffered update code AFTER the body
      for (const auto& instr : forUpdateBuffer.top()) {
        instructions.push_back(instr);
      }
      forUpdateBuffer.pop();
      // Jump back to condition check
      emit(instructions, Opcode::Jump, int32_t(continueStack.top()));
      // Patch all breaks to here
      for (size_t addr : breakStack.top()) {
        patchJump(instructions, addr);
      }
      breakStack.pop();
      continueStack.pop();
      isForLoopStack.pop();
    }
  ;

for_init:
    %empty
  | IDENT '=' expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | type_spec IDENT '=' expr
    {
      symtab.declare($2, $1);
      emit(instructions, Opcode::Store, symtab.lookup($2)->address);
    }
  ;

for_update:
    %empty
  | IDENT '=' expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT INC
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::PushLiteral, int32_t(1));
        emit(instructions, Opcode::Add);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT DEC
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::PushLiteral, int32_t(1));
        emit(instructions, Opcode::Sub);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  ;

break_stmt:
    BREAK ';'
    {
      if (breakStack.empty()) {
        emitError(errors, "'break' outside of loop", @1.begin.line, @1.begin.column);
      } else {
        breakStack.top().push_back(emitJump(instructions, Opcode::Jump));
      }
    }
  ;

continue_stmt:
    CONTINUE ';'
    {
      if (continueStack.empty()) {
        emitError(errors, "'continue' outside of loop", @1.begin.line, @1.begin.column);
      } else if (!isForLoopStack.empty() && isForLoopStack.top()) {
        // In a for-loop: emit forward jump, patch later to update section
        forContinueStack.top().push_back(emitJump(instructions, Opcode::Jump));
      } else {
        // In while/do-while: jump directly to condition
        emit(instructions, Opcode::Jump, int32_t(continueStack.top()));
      }
    }
  ;

return_stmt:
    RETURN ';'
    {
      // Check if we're in a function or main program
      if (currentFunctionName.empty()) {
        // In main program - halt
        emit(instructions, Opcode::Halt);
      } else {
        // In a function - restore frame and return to caller
        if (currentFunctionFrameSize > 0) {
          emit(instructions, Opcode::LeaveFrame, currentFunctionFrameStart, currentFunctionFrameSize);
        }
        emit(instructions, Opcode::ShortReturn);
      }
    }
  | RETURN expr ';'
    {
      // Return value is on stack
      if (currentFunctionName.empty()) {
        // In main program - halt (value discarded)
        emit(instructions, Opcode::Halt);
      } else {
        // In a function - restore frame and return with value on stack
        if (currentFunctionFrameSize > 0) {
          emit(instructions, Opcode::LeaveFrame, currentFunctionFrameStart, currentFunctionFrameSize);
        }
        emit(instructions, Opcode::ExitFunction);
      }
    }
  ;

cout_stmt:
    COUT cout_chain
  ;

cout_chain:
    SHL cout_item
  | cout_chain SHL cout_item
  ;

cout_item:
    expr
    {
      // Write the expression value
      emit(instructions, Opcode::Write);
    }
  | STRING_LIT
    {
      // Write string literal
      emit(instructions, Opcode::WriteRawString, $1);
    }
  | ENDL
    {
      // Write newline
      emit(instructions, Opcode::Writeln);
    }
  ;

cin_stmt:
    CIN cin_chain
  ;

cin_chain:
    SHR IDENT
    {
      Symbol* sym = symtab.lookup($2);
      if (!sym) {
        emitError(errors, "Undeclared variable '" + $2 + "'", @2.begin.line, @2.begin.column);
      } else if (sym->isConst) {
        emitError(errors, "Cannot read into constant '" + $2 + "'", @2.begin.line, @2.begin.column);
      } else {
        // Read value and store
        emit(instructions, Opcode::Read);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | cin_chain SHR IDENT
    {
      Symbol* sym = symtab.lookup($3);
      if (!sym) {
        emitError(errors, "Undeclared variable '" + $3 + "'", @3.begin.line, @3.begin.column);
      } else if (sym->isConst) {
        emitError(errors, "Cannot read into constant '" + $3 + "'", @3.begin.line, @3.begin.column);
      } else {
        emit(instructions, Opcode::Read);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  ;

expr_stmt:
    expr ';'
    {
      // Discard the expression result if not used
      // For now, keep it - expressions like i++ need special handling
    }
  ;

cobegin_stmt:
    COBEGIN
    {
      // Clear block tracking and emit jump to skip over all blocks
      cobeginBlockStarts.clear();
      cobeginJumpOverAddr = instructions.size();
      emit(instructions, Opcode::Jump, 0);  // Will be patched to jump after all blocks
    }
    cobegin_body COEND
    {
      // Patch the initial jump to skip all blocks - now points here
      size_t afterBlocks = instructions.size();
      instructions[cobeginJumpOverAddr].operand1 = static_cast<int32_t>(afterBlocks);
      
      // Emit Create instructions for each block
      for (size_t blockStart : cobeginBlockStarts) {
        emit(instructions, Opcode::Create, static_cast<int32_t>(blockStart));
      }
      
      // Emit Coend - waits for all spawned threads to finish
      emit(instructions, Opcode::Coend, static_cast<int32_t>(cobeginBlockStarts.size()));
      
      cobeginBlockStarts.clear();
    }
  ;

cobegin_body:
    cobegin_block
  | cobegin_body cobegin_block
  ;

cobegin_block:
    {
      // Record start address of this block
      cobeginBlockStarts.push_back(instructions.size());
    }
    block
    {
      // Each block ends with ThreadEnd to terminate the spawned thread
      emit(instructions, Opcode::ThreadEnd);
    }
  ;

p_stmt:
    P_OP '(' IDENT ')'
    {
      Symbol* sym = symtab.lookup($3);
      if (!sym) {
        emitError(errors, "Undeclared semaphore '" + $3 + "'", @3.begin.line, @3.begin.column);
      } else {
        // P operation (wait): push semaphore address and execute Wait
        emit(instructions, Opcode::LoadAddress, sym->address);
        emit(instructions, Opcode::Wait);
      }
    }
  ;

v_stmt:
    V_OP '(' IDENT ')'
    {
      Symbol* sym = symtab.lookup($3);
      if (!sym) {
        emitError(errors, "Undeclared semaphore '" + $3 + "'", @3.begin.line, @3.begin.column);
      } else {
        // V operation (signal): push semaphore address and execute Signal
        emit(instructions, Opcode::LoadAddress, sym->address);
        emit(instructions, Opcode::Signal);
      }
    }
  ;

// ============== Drawing Statements ==============

draw_stmt:
    draw_set_color_stmt
  | draw_set_line_width_stmt
  | draw_circle_stmt
  | draw_rect_stmt
  | draw_triangle_stmt
  | draw_line_stmt
  | draw_ellipse_stmt
  | draw_pixel_stmt
  | draw_text_stmt
  | draw_clear_stmt
  | draw_refresh_stmt
  | fill_circle_stmt
  | fill_rect_stmt
  | fill_triangle_stmt
  | fill_ellipse_stmt
  ;

// Point expression: {x, y} - pushes x then y onto stack
point_expr:
    '{' expr ',' expr '}'
    {
      // expr already pushed x, then y onto stack - nothing more to do
    }
  ;

draw_set_color_stmt:
    DRAW_SET_COLOR '(' expr ',' expr ',' expr ')'
    {
      // Stack has r, g, b - emit set color
      emit(instructions, Opcode::DrawSetColor);
    }
  | DRAW_SET_COLOR '(' expr ',' expr ',' expr ',' expr ')'
    {
      // Stack has r, g, b, a - emit set color with alpha
      emit(instructions, Opcode::DrawSetColorAlpha);
    }
  | DRAW_SET_COLOR '(' color_macro ')'
    {
      // Color macro pushes r, g, b onto stack
      emit(instructions, Opcode::DrawSetColor);
    }
  ;

color_macro:
    COLOR_RED
    {
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | COLOR_GREEN
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | COLOR_BLUE
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
    }
  | COLOR_WHITE
    {
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
    }
  | COLOR_BLACK
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | COLOR_YELLOW
    {
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | COLOR_CYAN
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
    }
  | COLOR_MAGENTA
    {
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(255));
    }
  | COLOR_ORANGE
    {
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(165));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | COLOR_PINK
    {
      emit(instructions, Opcode::PushLiteral, int32_t(255));
      emit(instructions, Opcode::PushLiteral, int32_t(192));
      emit(instructions, Opcode::PushLiteral, int32_t(203));
    }
  | COLOR_PURPLE
    {
      emit(instructions, Opcode::PushLiteral, int32_t(128));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(128));
    }
  | COLOR_GRAY
    {
      emit(instructions, Opcode::PushLiteral, int32_t(128));
      emit(instructions, Opcode::PushLiteral, int32_t(128));
      emit(instructions, Opcode::PushLiteral, int32_t(128));
    }
  | COLOR_BROWN
    {
      emit(instructions, Opcode::PushLiteral, int32_t(139));
      emit(instructions, Opcode::PushLiteral, int32_t(69));
      emit(instructions, Opcode::PushLiteral, int32_t(19));
    }
  ;

position_macro:
    POS_CENTER
    {
      // Center position - will be resolved at runtime
      emit(instructions, Opcode::PushLiteral, int32_t(400));  // Default center X
      emit(instructions, Opcode::PushLiteral, int32_t(300));  // Default center Y
    }
  | POS_TOP_LEFT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | POS_TOP_CENTER
    {
      emit(instructions, Opcode::PushLiteral, int32_t(400));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | POS_TOP_RIGHT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(800));
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | POS_CENTER_LEFT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(300));
    }
  | POS_CENTER_RIGHT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(800));
      emit(instructions, Opcode::PushLiteral, int32_t(300));
    }
  | POS_BOTTOM_LEFT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::PushLiteral, int32_t(600));
    }
  | POS_BOTTOM_CENTER
    {
      emit(instructions, Opcode::PushLiteral, int32_t(400));
      emit(instructions, Opcode::PushLiteral, int32_t(600));
    }
  | POS_BOTTOM_RIGHT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(800));
      emit(instructions, Opcode::PushLiteral, int32_t(600));
    }
  ;

draw_set_line_width_stmt:
    DRAW_SET_LINE_WIDTH '(' expr ')'
    {
      emit(instructions, Opcode::DrawSetLineWidth);
    }
  ;

// ============== Draw (Outline) Statements ==============

draw_circle_stmt:
    DRAW_CIRCLE '(' expr ',' expr ',' expr ')'
    {
      // x, y, radius - outline only
      emit(instructions, Opcode::DrawCircle);
    }
  | DRAW_CIRCLE '(' point_expr ',' expr ')'
    {
      // {x, y}, radius - outline only
      emit(instructions, Opcode::DrawCircle);
    }
  | DRAW_CIRCLE '(' position_macro ',' expr ')'
    {
      // Position macro pushed x, y; then radius - outline only
      emit(instructions, Opcode::DrawCircle);
    }
  ;

draw_rect_stmt:
    DRAW_RECT '(' expr ',' expr ',' expr ',' expr ')'
    {
      // x, y, width, height - outline only
      emit(instructions, Opcode::DrawRectangle);
    }
  | DRAW_RECT '(' point_expr ',' expr ',' expr ')'
    {
      // {x, y}, width, height - outline only
      emit(instructions, Opcode::DrawRectangle);
    }
  ;

draw_triangle_stmt:
    DRAW_TRIANGLE '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
    {
      // x1, y1, x2, y2, x3, y3 - outline only
      emit(instructions, Opcode::DrawTriangle);
    }
  | DRAW_TRIANGLE '(' point_expr ',' point_expr ',' point_expr ')'
    {
      // {x1, y1}, {x2, y2}, {x3, y3} - outline only
      emit(instructions, Opcode::DrawTriangle);
    }
  ;

draw_line_stmt:
    DRAW_LINE '(' expr ',' expr ',' expr ',' expr ')'
    {
      // x1, y1, x2, y2
      emit(instructions, Opcode::DrawLine);
    }
  | DRAW_LINE '(' point_expr ',' point_expr ')'
    {
      // {x1, y1}, {x2, y2}
      emit(instructions, Opcode::DrawLine);
    }
  ;

draw_ellipse_stmt:
    DRAW_ELLIPSE '(' expr ',' expr ',' expr ',' expr ')'
    {
      // x, y, radiusX, radiusY - outline only
      emit(instructions, Opcode::DrawEllipse);
    }
  | DRAW_ELLIPSE '(' point_expr ',' expr ',' expr ')'
    {
      // {x, y}, radiusX, radiusY - outline only
      emit(instructions, Opcode::DrawEllipse);
    }
  ;

draw_pixel_stmt:
    DRAW_PIXEL '(' expr ',' expr ')'
    {
      // x, y
      emit(instructions, Opcode::DrawPixel);
    }
  | DRAW_PIXEL '(' point_expr ')'
    {
      // {x, y}
      emit(instructions, Opcode::DrawPixel);
    }
  ;

draw_text_stmt:
    DRAW_TEXT '(' expr ',' expr ',' STRING_LIT ')'
    {
      // x, y, text - default font size
      emit(instructions, Opcode::PushLiteral, int32_t(12));  // Default font size
      emit(instructions, Opcode::DrawText, $7);
    }
  | DRAW_TEXT '(' expr ',' expr ',' STRING_LIT ',' expr ')'
    {
      // x, y, text, fontSize
      emit(instructions, Opcode::DrawText, $7);
    }
  | DRAW_TEXT '(' point_expr ',' STRING_LIT ')'
    {
      // {x, y}, text - default font size
      emit(instructions, Opcode::PushLiteral, int32_t(12));  // Default font size
      emit(instructions, Opcode::DrawText, $5);
    }
  | DRAW_TEXT '(' point_expr ',' STRING_LIT ',' expr ')'
    {
      // {x, y}, text, fontSize
      emit(instructions, Opcode::DrawText, $5);
    }
  ;

// ============== Fill (Filled Shape) Statements ==============

fill_circle_stmt:
    FILL_CIRCLE '(' expr ',' expr ',' expr ')'
    {
      // x, y, radius - filled
      emit(instructions, Opcode::FillCircle);
    }
  | FILL_CIRCLE '(' point_expr ',' expr ')'
    {
      // {x, y}, radius - filled
      emit(instructions, Opcode::FillCircle);
    }
  | FILL_CIRCLE '(' position_macro ',' expr ')'
    {
      // Position macro pushed x, y; then radius - filled
      emit(instructions, Opcode::FillCircle);
    }
  ;

fill_rect_stmt:
    FILL_RECT '(' expr ',' expr ',' expr ',' expr ')'
    {
      // x, y, width, height - filled
      emit(instructions, Opcode::FillRectangle);
    }
  | FILL_RECT '(' point_expr ',' expr ',' expr ')'
    {
      // {x, y}, width, height - filled
      emit(instructions, Opcode::FillRectangle);
    }
  ;

fill_triangle_stmt:
    FILL_TRIANGLE '(' expr ',' expr ',' expr ',' expr ',' expr ',' expr ')'
    {
      // x1, y1, x2, y2, x3, y3 - filled
      emit(instructions, Opcode::FillTriangle);
    }
  | FILL_TRIANGLE '(' point_expr ',' point_expr ',' point_expr ')'
    {
      // {x1, y1}, {x2, y2}, {x3, y3} - filled
      emit(instructions, Opcode::FillTriangle);
    }
  ;

fill_ellipse_stmt:
    FILL_ELLIPSE '(' expr ',' expr ',' expr ',' expr ')'
    {
      // x, y, radiusX, radiusY - filled
      emit(instructions, Opcode::FillEllipse);
    }
  | FILL_ELLIPSE '(' point_expr ',' expr ',' expr ')'
    {
      // {x, y}, radiusX, radiusY - filled
      emit(instructions, Opcode::FillEllipse);
    }
  ;

// ============== Canvas Operations ==============

draw_clear_stmt:
    DRAW_CLEAR '(' ')'
    {
      // Clear with white (default)
      emit(instructions, Opcode::PushLiteral, int32_t(255));  // R
      emit(instructions, Opcode::PushLiteral, int32_t(255));  // G
      emit(instructions, Opcode::PushLiteral, int32_t(255));  // B
      emit(instructions, Opcode::DrawSetColor);
      emit(instructions, Opcode::DrawClear);
    }
  | DRAW_CLEAR '(' expr ',' expr ',' expr ')'
    {
      // Clear with specific color (r, g, b on stack)
      emit(instructions, Opcode::DrawSetColor);
      emit(instructions, Opcode::DrawClear);
    }
  | DRAW_CLEAR '(' color_macro ')'
    {
      // Clear with color macro
      emit(instructions, Opcode::DrawSetColor);
      emit(instructions, Opcode::DrawClear);
    }
  ;

draw_refresh_stmt:
    DRAW_REFRESH '(' ')'
    {
      emit(instructions, Opcode::DrawRefresh);
    }
  ;

function_def:
    type_spec IDENT '('
    {
      // Save function info for later
      currentFunctionName = $2;
      currentFunctionReturnType = $1;
      currentFunctionParams.clear();
      symtab.enterScope();
    }
    param_list ')'
    {
      // Jump over function body (for top-level code)
      size_t jumpOver = emitJump(instructions, Opcode::Jump);
      
      // Record function start address
      uint32_t funcAddr = static_cast<uint32_t>(instructions.size());
      symtab.declareFunction(currentFunctionName, currentFunctionReturnType, 
                             currentFunctionParams, funcAddr);
      
      // Declare parameters as local variables and emit code to store them
      // Arguments are on stack in order [arg0, arg1, ...argN] with argN on top
      // So we need to pop in reverse order
      std::vector<uint32_t> paramAddresses;
      for (const auto& param : currentFunctionParams) {
        symtab.declare(param.first, param.second, false, true);
        paramAddresses.push_back(symtab.lookup(param.first)->address);
      }
      
      // Track frame info for LeaveFrame on return
      if (!paramAddresses.empty()) {
        currentFunctionFrameStart = paramAddresses[0];
        currentFunctionFrameSize = static_cast<int32_t>(paramAddresses.size());
        // Emit EnterFrame to save current values before overwriting
        emit(instructions, Opcode::EnterFrame, currentFunctionFrameStart, currentFunctionFrameSize);
      } else {
        currentFunctionFrameStart = 0;
        currentFunctionFrameSize = 0;
      }
      
      // Pop arguments from stack into parameter addresses (reverse order)
      for (auto it = paramAddresses.rbegin(); it != paramAddresses.rend(); ++it) {
        emit(instructions, Opcode::Store, *it);
      }
      
      // Store the jump address for patching after body
      breakStack.push({jumpOver});  // Reuse breakStack temporarily
    }
    function_body
    {
      // Emit implicit return for void functions
      if (currentFunctionReturnType == VarType::Void) {
        if (currentFunctionFrameSize > 0) {
          emit(instructions, Opcode::LeaveFrame, currentFunctionFrameStart, currentFunctionFrameSize);
        }
        emit(instructions, Opcode::ShortReturn);
      }
      
      // Patch the jump-over instruction
      if (!breakStack.empty() && !breakStack.top().empty()) {
        patchJump(instructions, breakStack.top()[0]);
        breakStack.pop();
      }
      
      // Reset function context
      currentFunctionName.clear();
      currentFunctionFrameStart = 0;
      currentFunctionFrameSize = 0;
      
      symtab.exitScope();
    }
  ;

param_list:
    %empty
  | param_decl
  | param_list ',' param_decl
  ;

param_decl:
    type_spec IDENT
    {
      currentFunctionParams.push_back({$2, $1});
    }
  ;

function_body:
    '{' stmt_list '}'
  ;

expr:
    NUMBER
    {
      emit(instructions, Opcode::PushLiteral, int32_t($1));
    }
  | CHAR_LIT
    {
      emit(instructions, Opcode::PushLiteral, int32_t($1));
    }
  | TRUE_LIT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(1));
    }
  | FALSE_LIT
    {
      emit(instructions, Opcode::PushLiteral, int32_t(0));
    }
  | IDENT
    {
      Symbol* sym = symtab.lookup($1);
      if (!sym) {
        emitError(errors, "Undeclared variable '" + $1 + "'", @1.begin.line, @1.begin.column);
        emit(instructions, Opcode::PushLiteral, int32_t(0)); // Push dummy
      } else {
        emit(instructions, Opcode::LoadValue, sym->address);
      }
    }
  | IDENT '[' expr ']'
    {
      // Array access: compute base + index, load indirect
      Symbol* sym = symtab.lookup($1);
      if (!sym) {
        emitError(errors, "Undeclared array '" + $1 + "'", @1.begin.line, @1.begin.column);
      } else {
        emit(instructions, Opcode::PushLiteral, int32_t(sym->address));
        emit(instructions, Opcode::Add);
        emit(instructions, Opcode::LoadIndirect);
      }
    }
  | IDENT '(' 
    {
      // Function call - push new arg count for nested calls
      argCountStack.push(0);
    }
    arg_list ')'
    {
      int argCount = argCountStack.top();
      argCountStack.pop();
      FunctionInfo* func = symtab.lookupFunction($1);
      if (!func) {
        emitError(errors, "Undeclared function '" + $1 + "'", @1.begin.line, @1.begin.column);
        emit(instructions, Opcode::PushLiteral, int32_t(0)); // Push dummy return value
      } else {
        if (argCount != static_cast<int>(func->params.size())) {
          emitError(errors, "Function '" + $1 + "' expects " + 
                              std::to_string(func->params.size()) + " arguments, got " + 
                              std::to_string(argCount), @1.begin.line, @1.begin.column);
        }
        // Call the function
        emit(instructions, Opcode::Call, int32_t(func->address));
      }
    }
  | '(' expr ')'
    { /* Expression already evaluated */ }
  | '-' expr %prec UNARY
    {
      emit(instructions, Opcode::Negate);
    }
  | '!' expr %prec UNARY
    {
      // Logical NOT: if 0 -> 1, else -> 0
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::TestEQ);
    }
  | expr '+' expr
    {
      emit(instructions, Opcode::Add);
    }
  | expr '-' expr
    {
      emit(instructions, Opcode::Sub);
    }
  | expr '*' expr
    {
      emit(instructions, Opcode::Mult);
    }
  | expr '/' expr
    {
      emit(instructions, Opcode::Div);
    }
  | expr '%' expr
    {
      emit(instructions, Opcode::Mod);
    }
  | expr '<' expr
    {
      emit(instructions, Opcode::TestLT);
    }
  | expr '>' expr
    {
      emit(instructions, Opcode::TestGT);
    }
  | expr LE expr
    {
      emit(instructions, Opcode::TestLE);
    }
  | expr GE expr
    {
      emit(instructions, Opcode::TestGE);
    }
  | expr EQ expr
    {
      emit(instructions, Opcode::TestEQ);
    }
  | expr NE expr
    {
      emit(instructions, Opcode::TestNE);
    }
  | expr AND expr
    {
      emit(instructions, Opcode::And);
    }
  | expr OR expr
    {
      emit(instructions, Opcode::Or);
    }
  | IDENT INC
    {
      // Post-increment: push old value, then increment
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);  // Push old value
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::PushLiteral, int32_t(1));
        emit(instructions, Opcode::Add);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT DEC
    {
      // Post-decrement: push old value, then decrement
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::PushLiteral, int32_t(1));
        emit(instructions, Opcode::Sub);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | INC IDENT
    {
      // Pre-increment: increment, then push new value
      Symbol* sym = symtab.lookup($2);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::PushLiteral, int32_t(1));
        emit(instructions, Opcode::Add);
        emit(instructions, Opcode::StoreKeep, sym->address);
      }
    }
  | DEC IDENT
    {
      // Pre-decrement: decrement, then push new value
      Symbol* sym = symtab.lookup($2);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::PushLiteral, int32_t(1));
        emit(instructions, Opcode::Sub);
        emit(instructions, Opcode::StoreKeep, sym->address);
      }
    }
  ;

arg_list:
    %empty
  | arg
  | arg_list ',' arg
  ;

arg:
    expr
    {
      if (!argCountStack.empty()) {
        argCountStack.top()++;
      }
    }
  ;

%%

void nsbaci::compiler::Parser::error(const location_type& loc, const std::string& msg) {
    nsbaci::Error err;
    err.basic.severity = nsbaci::types::ErrSeverity::Error;
    err.basic.message = std::to_string(loc.begin.line) + ":" + 
                        std::to_string(loc.begin.column) + ": " + msg;
    err.basic.type = nsbaci::types::ErrType::compilationError;
    err.payload = nsbaci::types::CompileError{loc.begin.line, loc.begin.column};
    errors.push_back(std::move(err));
}
