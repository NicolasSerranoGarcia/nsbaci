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

"//"[^\n]* { /* single-line comment */ yylloc->step(); }
"/*"([^*]|\*+[^*/])*\*+"/" { /* multi-line comment */ yylloc->step(); }

"int"       { return nsbaci::compiler::Parser::token::INT; }
"bool"      { return nsbaci::compiler::Parser::token::BOOL; }
"char"      { return nsbaci::compiler::Parser::token::CHAR; }
"void"      { return nsbaci::compiler::Parser::token::VOID; }
"const"     { return nsbaci::compiler::Parser::token::CONST; }
"if"        { return nsbaci::compiler::Parser::token::IF; }
"else"      { return nsbaci::compiler::Parser::token::ELSE; }
"while"     { return nsbaci::compiler::Parser::token::WHILE; }
"do"        { return nsbaci::compiler::Parser::token::DO; }
"for"       { return nsbaci::compiler::Parser::token::FOR; }
"break"     { return nsbaci::compiler::Parser::token::BREAK; }
"continue"  { return nsbaci::compiler::Parser::token::CONTINUE; }
"return"    { return nsbaci::compiler::Parser::token::RETURN; }
"true"      { return nsbaci::compiler::Parser::token::TRUE_LIT; }
"false"     { return nsbaci::compiler::Parser::token::FALSE_LIT; }
"cout"      { return nsbaci::compiler::Parser::token::COUT; }
"cin"       { return nsbaci::compiler::Parser::token::CIN; }
"endl"      { return nsbaci::compiler::Parser::token::ENDL; }
"semaphore" { return nsbaci::compiler::Parser::token::SEMAPHORE; }
"cobegin"   { return nsbaci::compiler::Parser::token::COBEGIN; }
"coend"     { return nsbaci::compiler::Parser::token::COEND; }
"p"         { return nsbaci::compiler::Parser::token::P_OP; }
"v"         { return nsbaci::compiler::Parser::token::V_OP; }

"canvas"        { return nsbaci::compiler::Parser::token::CANVAS; }
"setColor"      { return nsbaci::compiler::Parser::token::DRAW_SET_COLOR; }
"setLineWidth"  { return nsbaci::compiler::Parser::token::DRAW_SET_LINE_WIDTH; }
"drawCircle"    { return nsbaci::compiler::Parser::token::DRAW_CIRCLE; }
"drawRect"      { return nsbaci::compiler::Parser::token::DRAW_RECT; }
"drawTriangle"  { return nsbaci::compiler::Parser::token::DRAW_TRIANGLE; }
"drawLine"      { return nsbaci::compiler::Parser::token::DRAW_LINE; }
"drawEllipse"   { return nsbaci::compiler::Parser::token::DRAW_ELLIPSE; }
"drawPixel"     { return nsbaci::compiler::Parser::token::DRAW_PIXEL; }
"drawText"      { return nsbaci::compiler::Parser::token::DRAW_TEXT; }
"fillCircle"    { return nsbaci::compiler::Parser::token::FILL_CIRCLE; }
"fillRect"      { return nsbaci::compiler::Parser::token::FILL_RECT; }
"fillTriangle"  { return nsbaci::compiler::Parser::token::FILL_TRIANGLE; }
"fillEllipse"   { return nsbaci::compiler::Parser::token::FILL_ELLIPSE; }
"clearCanvas"   { return nsbaci::compiler::Parser::token::DRAW_CLEAR; }
"refreshCanvas" { return nsbaci::compiler::Parser::token::DRAW_REFRESH; }

"RED"        { return nsbaci::compiler::Parser::token::COLOR_RED; }
"GREEN"      { return nsbaci::compiler::Parser::token::COLOR_GREEN; }
"BLUE"       { return nsbaci::compiler::Parser::token::COLOR_BLUE; }
"WHITE"      { return nsbaci::compiler::Parser::token::COLOR_WHITE; }
"BLACK"      { return nsbaci::compiler::Parser::token::COLOR_BLACK; }
"YELLOW"     { return nsbaci::compiler::Parser::token::COLOR_YELLOW; }
"CYAN"       { return nsbaci::compiler::Parser::token::COLOR_CYAN; }
"MAGENTA"    { return nsbaci::compiler::Parser::token::COLOR_MAGENTA; }
"ORANGE"     { return nsbaci::compiler::Parser::token::COLOR_ORANGE; }
"PINK"       { return nsbaci::compiler::Parser::token::COLOR_PINK; }
"PURPLE"     { return nsbaci::compiler::Parser::token::COLOR_PURPLE; }
"GRAY"       { return nsbaci::compiler::Parser::token::COLOR_GRAY; }

"CENTER"        { return nsbaci::compiler::Parser::token::POS_CENTER; }
"TOP_LEFT"      { return nsbaci::compiler::Parser::token::POS_TOP_LEFT; }
"TOP_CENTER"    { return nsbaci::compiler::Parser::token::POS_TOP_CENTER; }
"TOP_RIGHT"     { return nsbaci::compiler::Parser::token::POS_TOP_RIGHT; }
"CENTER_LEFT"   { return nsbaci::compiler::Parser::token::POS_CENTER_LEFT; }
"CENTER_RIGHT"  { return nsbaci::compiler::Parser::token::POS_CENTER_RIGHT; }
"BOTTOM_LEFT"   { return nsbaci::compiler::Parser::token::POS_BOTTOM_LEFT; }
"BOTTOM_CENTER" { return nsbaci::compiler::Parser::token::POS_BOTTOM_CENTER; }
"BOTTOM_RIGHT"  { return nsbaci::compiler::Parser::token::POS_BOTTOM_RIGHT; }

[0-9]+ {
    yylval->emplace<int>(std::stoi(yytext));
    return nsbaci::compiler::Parser::token::NUMBER;
}

\"([^\"\\\n]|\\.)*\" {
    // String literal - remove quotes
    std::string s(yytext + 1, yyleng - 2);
    // Process escape sequences
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            switch (s[i + 1]) {
                case 'n': result += '\n'; ++i; break;
                case 't': result += '\t'; ++i; break;
                case '\\': result += '\\'; ++i; break;
                case '"': result += '"'; ++i; break;
                default: result += s[i]; break;
            }
        } else {
            result += s[i];
        }
    }
    yylval->emplace<std::string>(result);
    return nsbaci::compiler::Parser::token::STRING_LIT;
}

\'.\' {
    yylval->emplace<char>(yytext[1]);
    return nsbaci::compiler::Parser::token::CHAR_LIT;
}

\'\\n\' { yylval->emplace<char>('\n'); return nsbaci::compiler::Parser::token::CHAR_LIT; }
\'\\t\' { yylval->emplace<char>('\t'); return nsbaci::compiler::Parser::token::CHAR_LIT; }
\'\\0\' { yylval->emplace<char>('\0'); return nsbaci::compiler::Parser::token::CHAR_LIT; }
\'\\\\\' { yylval->emplace<char>('\\'); return nsbaci::compiler::Parser::token::CHAR_LIT; }
\'\\\'\' { yylval->emplace<char>('\''); return nsbaci::compiler::Parser::token::CHAR_LIT; }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval->emplace<std::string>(yytext);
    return nsbaci::compiler::Parser::token::IDENT;
}

"=="  return nsbaci::compiler::Parser::token::EQ;
"!="  return nsbaci::compiler::Parser::token::NE;
"<="  return nsbaci::compiler::Parser::token::LE;
">="  return nsbaci::compiler::Parser::token::GE;
"&&"  return nsbaci::compiler::Parser::token::AND;
"||"  return nsbaci::compiler::Parser::token::OR;
"++"  return nsbaci::compiler::Parser::token::INC;
"--"  return nsbaci::compiler::Parser::token::DEC;
"+="  return nsbaci::compiler::Parser::token::PLUS_ASSIGN;
"-="  return nsbaci::compiler::Parser::token::MINUS_ASSIGN;
"*="  return nsbaci::compiler::Parser::token::MULT_ASSIGN;
"/="  return nsbaci::compiler::Parser::token::DIV_ASSIGN;
"%="  return nsbaci::compiler::Parser::token::MOD_ASSIGN;
"<<"  return nsbaci::compiler::Parser::token::SHL;
">>"  return nsbaci::compiler::Parser::token::SHR;

"="   return '=';
"+"   return '+';
"-"   return '-';
"*"   return '*';
"/"   return '/';
"%"   return '%';
"<"   return '<';
">"   return '>';
"!"   return '!';
";"   return ';';
","   return ',';
"("   return '(';
")"   return ')';
"{"   return '{';
"}"   return '}';
"["   return '[';
"]"   return ']';

. {
    return nsbaci::compiler::Parser::token::YYerror;
}

%%
