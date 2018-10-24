%option noyywrap nodefault yylineno case-insensitive
%{
#include <stdlib.h>
#include "d16asm.tab.h"
%}

%%

\.org              { return ORG; }
\.equ              { return EQU; }
\.dw               { return DW; }
drop               { return DROP; }
jmp                { return JMP; }
call               { return CALL; }
ret                { return RET; }
^[A-Z_][A-Z0-9_]+: { yylval.s = strdup(yytext); return LABEL; }
[A-Z_][A-Z0-9_]+   { yylval.s = strdup(yytext); return IDENTIFIER; }
[-+]?0x[0-9A-Z]+   { yylval.l = strtol(yytext, NULL, 16); return NUMBER; }
[-+]?[0-9]+        { yylval.l = strtol(yytext, NULL, 10); return NUMBER; }
[\-\+\*\/\(\),]    { return yytext[0]; }
\n                 { return EOL; }
\/\/.*
;.*\n
\".*\"             { yylval.s = strdup(yytext); return LITERAL; }
[ \t]
.                  { printf("Unknown: '%s' (%d)\n", yytext, yytext[0]); }
%%

