%{
#include <stdio.h>
#include <stdarg.h>
#include "d16asm.h"
extern int yylex();
%}

%union {
	long l;
	char *s;
}

%token <l> NUMBER
%token <s> IDENTIFIER LITERAL LABEL
%token EOL ORG EQU DW DROP JMP CALL RET


%type<s> keyword

%start S

%%


S:
 | S line
;

line: EOL
| line EOL
| line keyword
| line NUMBER
| LABEL line
;


keyword: DROP { printf("%s\n", $$); }
| JMP { printf("%s\n", $$); }
| CALL { printf("%s\n", $$); }
| RET { printf("%s\n", $$); }
;


/*
line: EOL
| directive EOL
| expression line
| keyword line
| LABEL line
;

directive: ORG NUMBER EOL { printf("directive is ORG %ld\n", $2); }
| EQU IDENTIFIER
;

expression: NUMBER
| expression '+' expression
| expression '-' expression
| expression '*' expression
| expression '/' expression
| '(' expression ')'
;
*/
%%

void yyerror(char *s, ...) {
    va_list ap;
    va_start(ap, s);
    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}

int main( int argc, char **argv ) {
    yyparse();
    return 0;
}

