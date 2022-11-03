//
// Created by 谢卫凯 on 2022/10/23.
//

#ifndef LL_PARSER_H
#define LL_PARSER_H

#define MAX_BODY_SIZE 100

typedef struct {
    int head;
    int body[MAX_BODY_SIZE];
    int size;
}production;

int print_prod(int ind);

void construct_table();
int parse(FILE *fp);

#endif //LL_PARSER_H
