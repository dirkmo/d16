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
%token <s> IDENTIFIER LITERAL
%token EOL ORG EQU DW DROP JMP CALL BRAZ BRANZ BRAN BRAP RET

%%

start: IDENTIFIER start { printf("identifier: %s\n", $1); }
     | directive start 
     | IDENTIFIER { printf("identifier: %s\n", $1); }
     | directive
     | EOL start
     | EOL
     ;

directive: ORG NUMBER { printf("directive is ORG %ld\n", $2); }


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

