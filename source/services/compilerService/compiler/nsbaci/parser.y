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
  #include "instruction.h"
  namespace nsbaci::compiler { class Lexer; }
}

%code {
  #include "lexer.h"
  #define yylex lexer.yylex
}

%parse-param { nsbaci::compiler::Lexer& lexer }
%parse-param { nsbaci::compiler::InstructionStream& instructions }

%token <std::string> IDENT
%token <int> NUMBER
%token INT

%left '+'

%%

program:
    program statement
  | statement
  ;

statement:
    IDENT '=' expr ';'           /* x = 3 + 4; */
  | INT IDENT '=' expr ';'       /* int x = 3 + 4; */
  ;

expr:
    expr '+' expr
  | NUMBER
  | IDENT
  ;

%%

void nsbaci::compiler::Parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << loc << ": " << msg << std::endl;
}
