%{
#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
%}

DIGIT [0-9]
WS    [ \t\r]
%%

{DIGIT}+    {start_pos = pos;pos += yyleng;yyval = strtol(yytext,NULL,0);return NUM;}
{WS}  {pos += yyleng;}
\n    {pos += yyleng;return END;}
.   {start_pos = pos;pos += yyleng; yyval = yytext[0]; return yytext[0];}
<<EOF>> {return EOF;}

%%
int yywrap(){
    return 1;
}