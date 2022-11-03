//
// Created by 谢卫凯 on 2022/10/22.
//

#ifndef LR_LEX_H
#define LR_LEX_H

#define NUM 256
#define END 0

extern int yyval;

extern int start_pos;
extern int pos;

extern int yylex();


#endif //LR_LEX_H
