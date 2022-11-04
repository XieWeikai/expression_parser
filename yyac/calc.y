%{
#include <stdio.h>

int yylex();

void yyerror(const char* msg) {printf("%s !\n",msg);}
%}

%token NUM

%left '+' '-'
%left '*' '/'

%%

S   :   E '\n'        { printf("ans = %d\n", $1); return 0;}
    ;

E   :   E '+' E         { $$ = $1 + $3; printf("use E->E+E to reduce\n");}
    |   E '-' E         { $$ = $1 - $3; printf("use E->E-E to reduce\n");}
    |   E '*' E         { $$ = $1 * $3; printf("use E->E*E to reduce\n");}
    |   E '/' E         { $$ = $1 / $3; printf("use E->E/E to reduce\n");}
    |   NUM           { $$ = $1; printf("use E->NUM to reduce\n");}
    |   '(' E ')'       { $$ = $2; printf("use E->(E) to reduce\n");}
    ;

%%

int main() {
    return yyparse();
}