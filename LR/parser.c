//
// Created by 谢卫凯 on 2022/10/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

#define STACK_SIZE 1024

#define E 257
#define T 258
#define F 259

//E->E+T       0
//E->E-T       1
//E->T         2
//T->T*F       3
//T->T/F       4
//T->F         5
//F->(E)       6
//F->num       7
// E:257 ; T:258 ; F:259 ;

int yyval;
int start_pos;
int pos = 1;

production prods[] = {
        {.head=E,.body={E,'+',T},.size=3},
        {.head=E,.body={E,'-',T},.size=3},
        {.head=E,.body={T},.size=1},
        {.head=T,.body={T,'*',F},.size=3},
        {.head=T,.body={T,'/',F},.size=3},
        {.head=T,.body={F},.size=1},
        {.head=F,.body={'(',E,')'},.size=3},
        {.head=F,.body={NUM},.size=1},
};

char *strs[] = {
        "NUM","E","T","F",
};

int result_stack[STACK_SIZE] = {0};
int state_stack[STACK_SIZE] = {0};
int top = -1;

// num 为非终结符编号或终结符编号
// 在str中存格式化后的字符串
static char *get_str(char *str,int num){
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
    if(ind >= 0 && ind <= 7){
        printf("%s->",get_str(str,prods[ind].head));
        for (int i = 0; i < prods[ind].size; i++)
            printf("%s", get_str(str,prods[ind].body[i]));
        printf("\n");
        return 0;
    }
    return -1;
}

action action_table[30][300];

// 辅助填写action表的函数
static void add_action(int i,int j,const char *s){
    int num;
    if(s[0] == 'R' || s[0] == 'r') {
        num = (int)strtol(s + 1,NULL,0);
        action_table[i][j].type = REDUCE;
        action_table[i][j].num = num;
        return;
    }
    if(s[0] == 'S' || s[0] == 's'){
        num = (int)strtol(s + 1,NULL,0);
        action_table[i][j].type = SHIFT;
        action_table[i][j].num = num;
        return;
    }

    if(strcmp(s,"ACC") == 0){
        action_table[i][j].type = ACC;
        return;
    }

    num = (int)strtol(s,NULL,0);
    action_table[i][j].type = GOTO;
    action_table[i][j].num = num;
}

static void syntax_error(int cur_state,int token){
    printf("error occur at column %d\n",start_pos);
    printf("expected: ");
    char expect[100];
    for(int i = 0;i < 300;i ++)
        if(action_table[cur_state][i].type != ERROR && i <= NUM)
            printf("%s ", get_str(expect,i));
    printf(", got: %s\n", get_str(expect,token));
}

// 使用某个产生式具有特定的求值方式
static void calc_res(int prod_num){
    switch (prod_num) {
        case 0: // E->E+T
            printf("%d + %d\n",result_stack[top],result_stack[top + 2]);
            result_stack[top] = result_stack[top] + result_stack[top + 2];
            break;
        case 1: // E->E-T
            printf("%d - %d\n",result_stack[top],result_stack[top + 2]);
            result_stack[top] = result_stack[top] - result_stack[top + 2];
            break;
        case 2: // E->T
            printf("%d\n",result_stack[top]);
            result_stack[top] = result_stack[top];
            break;
        case 3: // T->T*F
            printf("%d * %d\n",result_stack[top],result_stack[top + 2]);
            result_stack[top] = result_stack[top] * result_stack[top + 2];
            break;
        case 4: // T->T/F
            printf("%d / %d\n",result_stack[top],result_stack[top + 2]);
            result_stack[top] = result_stack[top] / result_stack[top + 2];
            break;
        case 5: // T->F
            printf("%d\n",result_stack[top]);
            result_stack[top] = result_stack[top];
            break;
        case 6: // F->(E)
            printf("(%d)\n",result_stack[top + 1]);
            result_stack[top] = result_stack[top + 1];
            break;
        case 7: // F->num
            printf("%d\n",result_stack[top]);
            result_stack[top] = result_stack[top];
            break;
        default:
            break;
    }
}

static void print_stack(){
    printf("state: ");
    for (int i = 0; i <= top; i++)
        printf("%d ",state_stack[i]);
    printf("\n");
    printf("res: ");
    for (int i = 0; i <= top; i++)
        printf("%d ",result_stack[i]);
    printf("\n");
}

int parse(FILE *f){
    extern FILE *yyin;
    yyin = f;
    int token = yylex();
    action *cur;
    int cur_state;
    int prod_num;
    while(1){
//        printf("----------------state:%d----------------\n",state_stack[top]);
//        printf(">>>%d(%c) %d\n",token,token,yyval);
        cur_state = state_stack[top];
        cur = &action_table[cur_state][token]; // 当前分析表表项
        if(cur->type == ERROR){ // 不进行错误恢复，直接返回-1
            syntax_error(cur_state,token);
            return -1;
        }

        if(cur->type == SHIFT){ // 移进
            state_stack[++top] = cur->num;
            result_stack[top] = yyval;
            token = yylex();
        }

        if(cur->type == REDUCE){ // 规约
            prod_num = cur->num;
            top -= prods[prod_num].size ;  // 根据产生式体长度弹出状态
            cur_state = state_stack[top];  // 更新现在栈顶状态
            cur = &action_table[cur_state][prods[prod_num].head]; // 看action表goto表项
            if(cur->type == ERROR){
                syntax_error(cur_state,token);
                return -1;
            }
            state_stack[++top] = cur->num; // 依据goto表项压入状态
//            printf("------------------------\n");
            calc_res(prod_num);  // 根据产生式计算结果
            print_prod(prod_num); // 打印规约使用的产生式
            printf("------------------------\n");
        }

        if(cur->type == ACC){
            printf("valid input !!!\n");
            printf("the result is: %d\n",result_stack[top]);
            return 0;
        }

//        print_stack();
//        printf("---------------------------------\n");
    }
    return -1;
}

void init_action_table(){
    state_stack[++top] = 0;

    for (int i = 0; i < 30; i++)
        for(int j = 0; j < 300; j++)
            action_table[i][j].type = ERROR;

    add_action(0,'(',"S4");
    add_action(0,NUM,"S5");
    add_action(0,E,"1");
    add_action(0,T,"2");
    add_action(0,F,"3");

    add_action(1,'+',"S6");
    add_action(1,'-',"S7");
    add_action(1,END,"ACC");

    add_action(2,'+',"R2");
    add_action(2,'-',"R2");
    add_action(2,'*',"S8");
    add_action(2,'/',"S9");
    add_action(2,END,"R2");

    add_action(3,'+',"R5");
    add_action(3,'-',"R5");
    add_action(3,'*',"R5");
    add_action(3,'/',"R5");
    add_action(3,END,"R5");

    add_action(4,'(',"S18");
    add_action(4,NUM,"S13");
    add_action(4,E,"10");
    add_action(4,T,"11");
    add_action(4,F,"12");

    add_action(5,'+',"R7");
    add_action(5,'-',"R7");
    add_action(5,'*',"R7");
    add_action(5,'/',"R7");
    add_action(5,END,"R7");

    add_action(6,'(',"S4");
    add_action(6,NUM,"S5");
    add_action(6,T,"14");
    add_action(6,F,"3");

    add_action(7,'(',"S4");
    add_action(7,NUM,"S5");
    add_action(7,T,"15");
    add_action(7,F,"3");

    add_action(8,'(',"S4");
    add_action(8,NUM,"S5");
    add_action(8,F,"16");

    add_action(9,'(',"S4");
    add_action(9,NUM,"S5");
    add_action(9,F,"17");

    add_action(10,')',"S19");
    add_action(10,'+',"S20");
    add_action(10,'-',"S21");

    add_action(11,')',"R2");
    add_action(11,'+',"R2");
    add_action(11,'-',"R2");
    add_action(11,'*',"S22");
    add_action(11,'/',"S23");

    add_action(12,')',"R5");
    add_action(12,'+',"R5");
    add_action(12,'-',"R5");
    add_action(12,'*',"R5");
    add_action(12,'/',"R5");

    add_action(13,')',"R7");
    add_action(13,'+',"R7");
    add_action(13,'-',"R7");
    add_action(13,'*',"R7");
    add_action(13,'/',"R7");

    add_action(14,'+',"R0");
    add_action(14,'-',"R0");
    add_action(14,'*',"S8");
    add_action(14,'/',"S9");
    add_action(14,END,"R0");

    add_action(15,'+',"R1");
    add_action(15,'-',"R1");
    add_action(15,'*',"S8");
    add_action(15,'/',"S9");
    add_action(15,END,"R1");

    add_action(16,'+',"R3");
    add_action(16,'-',"R3");
    add_action(16,'*',"R3");
    add_action(16,'/',"R3");
    add_action(16,END,"R3");

    add_action(17,'+',"R4");
    add_action(17,'-',"R4");
    add_action(17,'*',"R4");
    add_action(17,'/',"R4");
    add_action(17,END,"R4");

    add_action(18,'(',"S18");
    add_action(18,NUM,"S13");
    add_action(18,E,"24");
    add_action(18,T,"11");
    add_action(18,F,"12");

    add_action(19,'+',"R6");
    add_action(19,'-',"R6");
    add_action(19,'*',"R6");
    add_action(19,'/',"R6");
    add_action(19,END,"R6");

    add_action(20,'(',"S18");
    add_action(20,NUM,"S13");
    add_action(20,T,"25");
    add_action(20,F,"12");

    add_action(21,'(',"S18");
    add_action(21,NUM,"S13");
    add_action(21,T,"26");
    add_action(21,F,"12");

    add_action(22,'(',"S18");
    add_action(22,NUM,"S13");
    add_action(22,F,"27");

    add_action(23,'(',"S18");
    add_action(23,NUM,"S13");
    add_action(23,F,"28");

    add_action(24,')',"S29");
    add_action(24,'+',"S20");
    add_action(24,'-',"S21");

    add_action(25,')',"R0");
    add_action(25,'+',"R0");
    add_action(25,'-',"R0");
    add_action(25,'*',"S22");
    add_action(25,'/',"S23");

    add_action(26,')',"R1");
    add_action(26,'+',"R1");
    add_action(26,'-',"R1");
    add_action(26,'*',"S22");
    add_action(26,'/',"S23");

    add_action(27,')',"R3");
    add_action(27,'+',"R3");
    add_action(27,'-',"R3");
    add_action(27,'*',"R3");
    add_action(27,'/',"R3");

    add_action(28,')',"R4");
    add_action(28,'+',"R4");
    add_action(28,'-',"R4");
    add_action(28,'*',"R4");
    add_action(28,'/',"R4");

    add_action(29,')',"R6");
    add_action(29,'+',"R6");
    add_action(29,'-',"R6");
    add_action(29,'*',"R6");
    add_action(29,'/',"R6");
}
