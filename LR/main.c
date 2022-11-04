#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "parser.h"

int main(int argc,char **argv) {
    if(argc <= 1){
        fprintf(stderr,"error: no input file!\n");
        exit(-1);
    }

    FILE *fp = fopen(argv[1],"r");
    if(fp == NULL){
        fprintf(stderr,"error: can not open input file!\n");
        exit(-1);
    }

    init_action_table();
    parse(fp);

    return 0;
}
