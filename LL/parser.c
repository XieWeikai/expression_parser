//
// Created by 谢卫凯 on 2022/10/23.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lex.h"

#define E 257
#define T 258
#define F 259
#define E_ 260
#define T_ 261

#define EPSILON 262

#define STACK_SIZE 1024

#define NUM_PRODUCTIONS 10

int yyval;
int start_pos;
int pos = 1;

// E->TE'         0
// E'->+TE'       1
// E'->-TE'       2
// E'->epsilon    3
// T->FT'         4
// T'->*FT'       5
// T'->/FT'       6
// T'->epsilon    7
// F->(E)         8
// F->num         9

int states[STACK_SIZE] = {END,E};
int top = 1;

production prods[] = {
        {.head = E,.body = {T,E_},.size = 2},
        {.head = E_,.body = {'+',T,E_},.size = 3},
        {.head = E_,.body = {'-',T,E_},.size = 3},
        {.head = E_,.body = {0},.size = 0},
        {.head = T,.body = {F,T_},.size = 2},
        {.head = T_,.body = {'*',F,T_},.size = 3},
        {.head = T_,.body = {'/',F,T_},.size = 3},
        {.head = T_,.body = {0},.size = 0},
        {.head = F,.body = {'(',E,')'},.size = 3},
        {.head = F,.body = {NUM},.size = 1},
};

int first_size[] = {2,2,2,3,3};
// FIRST(E) is in first[E - 257]
// the size is first_size[E - 257]
int first[][100] = {
        {'(',NUM}, // FIRST(E)
        {'(',NUM}, // FIRST(T)
        {'(',NUM}, // FIRST(F)
        {'+','-',EPSILON}, // FIRST(E')
        {'*','/',EPSILON}, // FIRST(T')
};

int follow_size[] = {2,4,6,2,4};
int follow[][100] = {
        {')',END}, // FOLLOW(E)
        {'+','-',')',END}, // FOLLOW(T)
        {'+','-',')','*','/',END}, // FOLLOW(F)
        {')',END}, // FOLLOW(E')
        {'+','-',')',END}, // FOLLOW(T')
};

char *strs[] = {
        "NUM","E","T","F","E'","T'","epsilon"
};

// num 为非终结符编号或终结符编号
// 在str中存格式化后的字符串
char *get_str(char *str,int num){
    if(num < 0)
        strcpy(str,"EOF");
    if(num == 0)
        strcpy(str,"END");

    if(num >= 256)
        strcpy(str,strs[num - 256]);
    if(num > 0 && num <= 255)
        sprintf(str,"%c",num);
    return str;
}

// 打印编号为ind 的产生式
int print_prod(int ind){
    char str[128];
    if(ind >= 0 && ind <= 9){
        printf("%s->",get_str(str,prods[ind].head));
        for (int i = 0; i < prods[ind].size; i++)
            printf("%s", get_str(str,prods[ind].body[i]));
        if(prods[ind].size == 0)
            printf("epsilon");
        printf("\n");
        return 0;
    }
    printf("(null)\n");
    return -1;
}

#define TABLE_SIZE 1000

int table[TABLE_SIZE][TABLE_SIZE]; // 预测分析表

// 计算FIRST(alpha)集合
// alpha是文法符号串 size是文法符号串的大小
// res存放FIRST(alpha)集合
// 返回FIRST(alpha)集合的大小
int FIRST(const int alpha[],int size,int res[]){
    int has_epsilon ;
    int i = 0;
    int s = 0;

    if(size <= 0){
        res[0] = EPSILON;
        return 1;
    }

    do{
        has_epsilon = 0;
        if(alpha[i] >= 0 && alpha[i] <= 256) // terminal symbol
            res[s++] = alpha[i];
        else{// non-terminal symbol
            for(int j = 0; j < first_size[alpha[i] - 257]; j ++){
                if(first[alpha[i] - 257][j] == EPSILON)
                    has_epsilon = 1;
                else
                    res[s ++] = first[alpha[i] - 257][j];
            }
        }
        i ++;
    } while (has_epsilon && i < size);
    if(has_epsilon)
        res[s ++] = EPSILON;
    return s;
}

static void print_set(int a[],int n){
    char str[100];
    for (int i = 0; i < n; i++)
        printf("%s ", get_str(str,a[i]));
    printf("\n");
}

void construct_table(){
    int f[100];
    int size;
    char tmp[100];

    for(int i = 0;i < TABLE_SIZE ;i ++)
        for(int j = 0; j < TABLE_SIZE;j ++)
            table[i][j] = -1 ; // ERROR

    for(int i = 0;i < NUM_PRODUCTIONS;i ++){
        printf("handling production:");
        print_prod(i);
        size = FIRST(prods[i].body,prods[i].size,f);
        printf("FIRST:");
        print_set(f,size);

        for(int j =0;j < size;j ++){
            if(f[j] != EPSILON) {
                printf("(%s,", get_str(tmp,prods[i].head));
                printf("%s)\n", get_str(tmp,f[j]));
                table[prods[i].head][f[j]] = i;
            }
            else{
                for(int k = 0;k < follow_size[prods[i].head - 257];k++) {
                    printf("(%s,", get_str(tmp,prods[i].head));
                    printf("%s)\n", get_str(tmp,follow[prods[i].head - 257][k]));
                    table[prods[i].head][follow[prods[i].head - 257][k]] = i;
                }
            }
        }
        printf("-------------------------------\n");
    }

}

void syntax_error(int symbol,int token){
    char str[100];
    printf("current non-terminal symbol is %s\n", get_str(str,symbol));
    printf("got: %s at column %d,have no item in prediction table\n", get_str(str,token),start_pos);
}

int parse(FILE *fp){
    extern FILE *yyin;
    yyin = fp;

    int token;
    token = yylex();
    int item;
    int X;
    char str[100];
    while(top >= 0){
        X = states[top];
        if(X >= 0 && X <= 256){ // terminal symbol
            if(X == token){
                -- top;
                printf("at column %d:got: %s \n", start_pos ,get_str(str,token));
                print_set(states,top+1);
                token = yylex();
            }else{
                printf("syntax_error:want :%s  ", get_str(str,X));
                printf("got:%s\n", get_str(str,token));
                return -1;
            }
        }else { // non-terminal symbol
            item = table[X][token];
            if (item == -1) {
                syntax_error(states[top], token);
                return -1;
            }
            -- top;
            for(int i = prods[item].size - 1;i >= 0;i--)
                states[++top] = prods[item].body[i];
            printf("at column %d:got: %s ,use ", start_pos ,get_str(str,token));
            print_prod(item);
            print_set(states,top+1);
        }
        printf("------------------------------\n");
    }

    printf("the input is an expression");

    return 0;
}
