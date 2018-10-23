%option noyywrap nodefault yylineno case-insensitive
%{
#include <stdio.h>
int linenum;
%}

%%

\.[a-zA-Z_]+            { printf("Directive %s\n", yytext); }
[a-zA-Z_][a-zA-Z0-9_]+  { printf("Identifier %s\n", yytext); }
[-+]?0[Xx][0-9A-Za-z]+  { printf("Hex number %s\n", yytext); }
[-+]?[0-9]+             { printf("Number %s\n", yytext); }
[\+\-\*\/]              { printf("Operator: %s\n", yytext); }
[\(\)]                  { printf("Klammer %s\n", yytext); }
\n                      { linenum++; }
[//|;].*                { printf("Comment\n"); }
\".+\"                  { printf("String literal %s\n", yytext); }
.                       { printf("%s\n", yytext); }
%%

int main(int argc, char **argv) {
    while (yylex() != 0);
    return 0;
}
