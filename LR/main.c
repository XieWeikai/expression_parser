#include <stdio.h>

#include "lex.h"
#include "parser.h"

int main() {
    FILE *fp = fopen("expression.txt","r");
    init_action_table();
    parse(fp);

    return 0;
}
