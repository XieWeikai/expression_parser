//
// Created by 谢卫凯 on 2022/10/22.
//

#ifndef LR_PARSER_H
#define LR_PARSER_H

#define MAX_BODY_SIZE 100

#define SHIFT 0
#define REDUCE 1
#define GOTO 2
#define ACC 3
#define ERROR 4

typedef struct {
    int head;
    int body[MAX_BODY_SIZE];
    int size;
}production;

typedef struct {
    int type;  // SHIFT , REDUCE , GOTO , ACC or ERROR
    int num;   // state number, production number , state number , not meaningful , not meaningful
}action;


int print_prod(int ind);
void init_action_table();
int parse(FILE *f);

#endif //LR_PARSER_H
