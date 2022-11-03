#include <stdio.h>
#include <stdlib.h>

#include "lex.h"

/*
E -> TE_
E_ -> +TE_
    | -TE_
    | epsilon
T -> FT_
T_ ->  *FT_
     | /FT_
     | epsilon
F -> (E)
    | NUM
*/

int E_(int v);
int T();
int T_(int v);
int F();

int yyval;
int pos;
int start_pos;

int token;

void match(int ch){
	if(token == ch){
		token = yylex();
		return;
	}
	printf("%d : error: expect:%c got:%c\n",start_pos,ch,token);
	token = yylex();
}

int E(){
	printf("E->TE'\n");
	return E_(T());
}

int E_(int v){
	if(token == '+'){
		token = yylex();
		printf("E'->+TE'\n");
		return E_(v + T());
	}else if(token == '-'){
		token = yylex();
		printf("E'->-TE'\n");
		return E_(v - T());
	}else{
		printf("E'->epsilon\n");
		return v;
	}
}

int T(){
	printf("T->FT'\n");
	return T_(F());
}

int T_(int v){
	if(token == '*'){
		token = yylex();
		printf("T'->*FT'\n");
		return T_(v * F());
	}else if(token == '/'){
		token = yylex();
		printf("T'->/FT'\n");
		return T_(v / F());
	}else{
		printf("T'->epsilon");
		return v;
	}
}

int F(){
	int v;
	if(token == '('){
		token = yylex();
		v = E();
		match(')');
		printf("F->(E)\n");
		return v;
	}
	v = yyval;
	match(NUM);
	printf("F->NUM\n");
	return v;
}

int main(){
	token = yylex();


	printf("= %d\n",E());
	

	return 0;
}