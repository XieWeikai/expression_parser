#include <stdio.h>
#include <stdlib.h>

#include "lex.h"
#include "parser.h"

int main(int argc,char **argv) {
    if(argc <= 1) {
        fprintf(stderr, "no input file!\n");
        exit(-1);
    }

    FILE *input = fopen(argv[1],"r");
    if(input == NULL){
        fprintf(stderr, "failed to open file %s\n",argv[1]);
        exit(-1);
    }

    char s1[100],s2[100];
    int terminal[] = {NUM,'+','*','(',')',0};

    construct_table();

    printf("***************************\n");
    parse(input);

    return 0;
}
