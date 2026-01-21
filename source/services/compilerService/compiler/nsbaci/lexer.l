%option c++
%option noyywrap
%option nodefault
%option yyclass="nsbaci::compiler::Lexer"

%{
#include "lexer.h"
#include "parser.tab.hh"

#undef YY_DECL
#define YY_DECL int nsbaci::compiler::Lexer::yylex(nsbaci::compiler::Parser::semantic_type* yylval, nsbaci::compiler::Parser::location_type* yylloc)

#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%

%{
    yylloc->step();
%}

[ \t\r]+ { yylloc->step(); }
\n+ { yylloc->lines(yyleng); yylloc->step(); }

"int" { return nsbaci::compiler::Parser::token::INT; }

[0-9]+ {
    yylval->emplace<int>(std::stoi(yytext));
    return nsbaci::compiler::Parser::token::NUMBER;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval->emplace<std::string>(yytext);
    return nsbaci::compiler::Parser::token::IDENT;
}

"=" return '=';
"+" return '+';
";" return ';';

. {
    std::cerr << "Unexpected char: " << yytext << "\n";
    return nsbaci::compiler::Parser::token::YYerror;
}

%%
