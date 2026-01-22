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
    };

    struct SymbolTable {
      std::unordered_map<std::string, Symbol> symbols;
      uint32_t nextAddress = 0;
      int currentScope = 0;

      bool declare(const std::string& name, VarType type, bool isConst = false) {
        if (symbols.count(name) && symbols[name].scopeLevel == currentScope) {
          return false; // Already declared in this scope
        }
        symbols[name] = {name, type, nextAddress++, isConst, currentScope};
        return true;
      }

      Symbol* lookup(const std::string& name) {
        auto it = symbols.find(name);
        return it != symbols.end() ? &it->second : nullptr;
      }

      void enterScope() { currentScope++; }
      void exitScope() {
        // Keep all symbols for runtime display - just decrement scope level
        // Note: This means scoped variables remain visible in debugger
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
  | program statement
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
  | ';'  /* empty statement */
  ;

declaration:
    type_spec IDENT ';'
    {
      if (!symtab.declare($2, $1)) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Variable '" + $2 + "' already declared";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      }
      // Initialize to 0
      emit(instructions, Opcode::PushLiteral, int32_t(0));
      emit(instructions, Opcode::Store, symtab.lookup($2)->address);
    }
  | type_spec IDENT '=' expr ';'
    {
      if (!symtab.declare($2, $1)) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Variable '" + $2 + "' already declared";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      }
      // expr already on stack, store it
      emit(instructions, Opcode::Store, symtab.lookup($2)->address);
    }
  | CONST type_spec IDENT '=' expr ';'
    {
      if (!symtab.declare($3, $2, true)) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Constant '" + $3 + "' already declared";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
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
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Undeclared variable '" + $1 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else if (sym->isConst) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Cannot assign to constant '" + $1 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else {
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT '[' expr ']' '=' expr
    {
      // Array element assignment
      Symbol* sym = symtab.lookup($1);
      if (!sym) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Undeclared array '" + $1 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else {
        // Stack: [index, value] -> compute address and store
        // Push base address, add index, then store indirect
        emit(instructions, Opcode::PushLiteral, int32_t(sym->address));
        emit(instructions, Opcode::Add);  // base + index
        emit(instructions, Opcode::StoreKeep);  // Store value at computed address
      }
    }
  | IDENT PLUS_ASSIGN expr
    {
      Symbol* sym = symtab.lookup($1);
      if (!sym) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Undeclared variable '" + $1 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::Add);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT MINUS_ASSIGN expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::Sub);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT MULT_ASSIGN expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::Mult);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT DIV_ASSIGN expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
        emit(instructions, Opcode::Div);
        emit(instructions, Opcode::Store, sym->address);
      }
    }
  | IDENT MOD_ASSIGN expr
    {
      Symbol* sym = symtab.lookup($1);
      if (sym) {
        emit(instructions, Opcode::LoadValue, sym->address);
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
    }
  ;

do_while_stmt:
    DO
    {
      breakStack.push({});
      continueStack.push(instructions.size());
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
    }
  ;

for_stmt:
    FOR '(' for_init ';'
    {
      breakStack.push({});
      continueStack.push(instructions.size());  // Condition start
    }
    expr ';'
    {
      size_t exitJump = emitJump(instructions, Opcode::JumpZero);
      breakStack.top().push_back(exitJump);
    }
    for_update ')' statement
    {
      // Jump back to condition check
      emit(instructions, Opcode::Jump, int32_t(continueStack.top()));
      // Patch all breaks to here
      for (size_t addr : breakStack.top()) {
        patchJump(instructions, addr);
      }
      breakStack.pop();
      continueStack.pop();
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
      if (sym) emit(instructions, Opcode::Store, sym->address);
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
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "'break' outside of loop";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else {
        breakStack.top().push_back(emitJump(instructions, Opcode::Jump));
      }
    }
  ;

continue_stmt:
    CONTINUE ';'
    {
      if (continueStack.empty()) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "'continue' outside of loop";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else {
        emit(instructions, Opcode::Jump, int32_t(continueStack.top()));
      }
    }
  ;

return_stmt:
    RETURN ';'
    {
      emit(instructions, Opcode::Halt);
    }
  | RETURN expr ';'
    {
      // Return value is on stack - for now just halt
      emit(instructions, Opcode::Halt);
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
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Undeclared variable '" + $2 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else if (sym->isConst) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Cannot read into constant '" + $2 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
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
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Undeclared variable '" + $3 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else if (sym->isConst) {
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Cannot read into constant '" + $3 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
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
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Undeclared variable '" + $1 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
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
        nsbaci::Error err;
        err.basic.severity = nsbaci::types::ErrSeverity::Error;
        err.basic.message = "Undeclared array '" + $1 + "'";
        err.basic.type = nsbaci::types::ErrType::compilationError;
        errors.push_back(std::move(err));
      } else {
        emit(instructions, Opcode::PushLiteral, int32_t(sym->address));
        emit(instructions, Opcode::Add);
        emit(instructions, Opcode::LoadIndirect);
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
