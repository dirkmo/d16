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
    | NUMBER { printf("Number %ld\n", $1); }
    | LABEL  { printf("Label %s\n", $1); }
    | IDENTIFIER { printf("Identifier: %s\n", $1); }
    | directive
    ;


keyword: DROP { printf("Keyword DROP\n"); }
       | JMP { printf("Keyword JMP\n"); }
       | CALL { printf("Keyword CALL\n"); }
       | RET { printf("Keyword RET\n"); }
       ;


directive: ORG NUMBER EOL { printf("directive is ORG %ld\n", $2); }
         | equ
         | dw
         ;


equ: EQU IDENTIFIER NUMBER EOL { printf("directive is EQU %s %ld\n", $2, $3); }
   | EQU IDENTIFIER LITERAL EOL { printf("directive is EQU %s %s\n", $2, $3); }
   ;


dw: DW dwdata { printf("directive is DW\n"); }
  ;


dwdata: NUMBER { printf("%ld ", $1); }
      | LITERAL { printf("%s ", $1); }
      | IDENTIFIER { printf("%s ", $1); }
      | dwdata ',' NUMBER { printf("%ld, ", $3); }
      | dwdata ',' LITERAL { printf("%s, ", $3); }
      | dwdata ',' IDENTIFIER { printf("%s ", $3); }
      ;

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

