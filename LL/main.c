#include <stdio.h>

#include "lex.h"
#include "parser.h"

char *get_str(char *str,int num);
extern int table[1000][1000];

int main() {
    FILE *input = fopen("expression.txt","r");

    char s1[100],s2[100];
    int terminal[] = {NUM,'+','*','(',')',0};

    construct_table();
    for(int i = 257;i <= 261;i ++){
        for(int j = 0;j <= 5;j ++){
            get_str(s1,i);
            get_str(s2,terminal[j]);
            printf("(%s,%s)  :  ",s1,s2);
            print_prod(table[i][terminal[j]]);
        }
    }

    printf("***************************\n");
    parse(input);

    return 0;
}
