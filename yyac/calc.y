%{
#include <stdio.h>

int yylex();

void yyerror(const char* msg) {}
%}

%token NUM

%left '+' '-'
%left '*' '/'

%%

S   :   S E '\n'        { printf("ans = %d\n", $2); }
    |   /* empty */     { /* empty */ }
    ;

E   :   E '+' E         { $$ = $1 + $3; printf("use E->E+E to reduse\n");}
    |   E '-' E         { $$ = $1 - $3; printf("use E->E-E to reduse\n");}
    |   E '*' E         { $$ = $1 * $3; printf("use E->E*E to reduse\n");}
    |   E '/' E         { $$ = $1 / $3; printf("use E->E/E to reduse\n");}
    |   NUM           { $$ = $1; printf("use E->NUM to reduse\n");}
    |   '(' E ')'       { $$ = $2; printf("use E->(E) to reduse\n");}
    ;

%%

int main() {
    return yyparse();
}