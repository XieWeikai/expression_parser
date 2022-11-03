%{
#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
%}

DIGIT [0-9]
WS    [ \t\n\r]
%%

{DIGIT}+    {start_pos = pos;pos += yyleng;yyval = strtol(yytext,NULL,0);return NUM;}
{WS}  {pos += yyleng;}
.   {start_pos = pos;pos += yyleng; yyval = yytext[0]; return yytext[0];}
<<EOF>> {return END;}

%%
int yywrap(){
    return 1;
}