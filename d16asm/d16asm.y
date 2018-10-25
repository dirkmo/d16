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


%start S

%%


S:
 | S line
 ;

line: EOL
 | keyword
 | NUMBER { printf("%ld\n", $1); }
 | LABEL
 ;


keyword: DROP { printf("DROP\n"); }
 | JMP { printf("JMP\n"); }
 | CALL { printf("CALL\n"); }
 | RET { printf("RET\n"); }
 ;


/*
directive: ORG NUMBER EOL { printf("directive is ORG %ld\n", $2); }
 ;
equ: EQU IDENTIFIER equparams EOL
 ;

equparams: NUMBER
         | LITERAL
         | NUMBER ',' equparams
         | LITERAL ',' equparams
         ;
line: EOL
| directive EOL
| expression line
| keyword line
| LABEL line
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

