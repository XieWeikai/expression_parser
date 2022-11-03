#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int pos = 1;
int start_pos;

int token;

void match(int ch){
	if(token == ch){
		token = yylex();
		return;
	}

	static char expect[100];
	static char token_str[100];
	if(ch == NUM)
		strcpy(expect,"NUM");
	else{
		expect[0] = ch;
		expect[1] = 0;
	}

	if(token == NUM)
		strcpy(token_str,"NUM");
	else if(token == END)
		strcpy(token_str,"END");
	else{
		token_str[0] = token;
		token_str[1] = 0;
	}

	printf("%d : error: expect:%s got:%s\n",start_pos,expect,token_str);
	exit(-1);
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
		printf("T'->epsilon\n");
		return v;
	}
}

int F(){
	int v;
	if(token == '('){
		token = yylex();
		printf("F->(E)\n");
		v = E();
		match(')');
		return v;
	}
	v = yyval;
	match(NUM);
	printf("F->NUM\n");
	return v;
}

int main(){
	
	printf(">>");
	token = yylex();

	int v = E();

	if(token == END)
		printf("= %d\n",v);
	else
		printf("%d :syntax error: an expression followed by illegal token\n",start_pos);
	

	return 0;
}