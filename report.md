

# 语法分析实验报告

## 实验环境

本次实现语法分析程序使用环境如下

- 操作系统:macOS Big Sur 11.6.8.

- c语言编译器:Apple clang version 13.0.0 (clang-1300.0.29.30).
- flex:flex 2.6.4 Apple(flex-34).
- bison:bison (GNU Bison) 2.3.

所有程序均在本机上运行通过。

---

## 实验任务

本次实验要求编写程序实现对算数表达式的语法分析，使用表达式文法如下

```c
E->E+T       
  |E-T       
  |T        
T->T*F       
  |T/F       
  |F         
F->(E)      
  |num       
```

要求在对算数表达式进行分析时依次输出所采用的产生式。

---

## 自顶向下的分析

### 递归下降

#### 代码实现思路

为了实现实用的(不带回溯)递归下降，需要采用预测分析方法。

为了知道选择哪一个产生式来进行分析，首先需要对原文发进行改造，需要提取左公因子并且消除左递归，原文法存在左递归，消除左递归后的文法如下

```c
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
```

有了改造后的文法容易依据文法写出递归下降程序，伪代码如下

```c
void E(){
  print "E->TE'"
  T()
  E_()
}

void E_(){
  if token == '+' then // 选择表达式 E_->+TE_
    print "E'->+TE'"
    get next token
    T()
  	E_()
   else if token == '-' then // 选择表达式 E_->-TE_
     print "E'->-TE'"
     get next token
     T()
  	 E_()
  else // 选择 E_->epsilon
    print "E'->epsilon" 
}

void T(){
  F()
  T_()
}

void T_(){
  if token == '*' then // 选择表达式 T_->*FT_
    print "T'->*FT_"
    get next token
    F()
  	T_()
   else if token == '-' then // 选择表达式 T_->/FT_
     print "T'->/FT_"
     get next token
     F()
  	 T_()
  else // 选择 T_->epsilon
    print "T'->epsilon" 
}

void F(){
  if token == '(' then
    print "F->(E)"
    get next token
    E()
    check if token == ')' , if not ,report an error,if token == ')' ,get next token
  else
    check if token == NUM
    if token == NUM ,get next token and print "F->num"
    if not ,report an error
}

int main(){
  get first token
  
  E()   // 进行语法分析
  
  return 0;
}
```

由上述伪代码可知，对每一个非终结符可以按照产生式来写对应的函数，每个函数分析处理一个非终结符，通过这种方法，可以很容易手动实现语法分析程序。

上面的伪代码可以进行语法分析并且输出用到的产生式，但我仍希望在对表达式分析完毕后可以同时计算出表达式的运算结果，为达到这个效果，修改代码如下

```c
int E(){  // 返回表达式求值结果
  int v = T();
  return E_(v);
}

int E_(int v){ // 由于表达式求值是左结合的，该函数需要知道表达式左边已有的计算结果，即参数v。返回表达式计算结果。
  if token == '+' then
    get next token
    v += T()
    return E_(v);
  else if token == '-' then
    get next token
    v -= T()
    return E_(v)
  else // 表达式计算结束了
    return v;
}

int T(){
  ...  // 返回一串只有* /的表达式计算结果 写法和E()类似
}

int T_(int v){
  ... // 和E_()写法类似
}

int F(){
  int v;
  if token == '(' then
    get next token
    v = E()
    check if token == ')' , if not ,report an error,if token == ')' ,get next token and return v
  else
    check if token == NUM
    if token == NUM ,get next token and return the number value
    if not ,report an error
}
```

通过边分析边依据token 来进行相应的计算，最终可以在完成分析后得到表达式的值。上述伪代码的具体实现见`recur_desc/main.c`文件。

---

#### 词法分析

上一小节的语法分析代码是以词法分析为基础的，为实现表达式语法解析至少应该要有一个能识别数字和字符并返回对应标记的词法分析器。为了简单考虑，我用`flex`实现了一个简单的词法分析器，主要代码如下(见`recur_desc/lex.l`文件)

```c
...

DIGIT [0-9]
WS    [ \t\r]
%%

{DIGIT}+    {start_pos = pos;pos += yyleng;yyval = strtol(yytext,NULL,0);return NUM;} // 识别整数并计算整数的值
{WS}  {pos += yyleng;}  // 跳过空白符
\n    {start_pos = pos;pos += yyleng;return END;}  // 程序以换行作为表达式结束标志
.   {start_pos = pos;pos += yyleng; yyval = yytext[0]; return yytext[0];} // 单个字符直接作为一个token
<<EOF>> {return EOF;}

...
```

上面代码中的`NUM`、`END`为自定义的宏，`start_pos`为每一个标记在一行中的位置(列数)，`pos`为当前词法分析器下一个输入字符所在位置(列数)，这两个变量用于在出现语法错误时定位错误位置。该词法分析器要配合头文件`lex.h`使用(见`recur_desc/lex.h`)，头文件内容如下

```c
#ifndef LR_LEX_H
#define LR_LEX_H

#define NUM 256
#define END 0

extern int yyval;

extern int start_pos;
extern int pos;

extern int yylex();


#endif //LR_LEX_H
```

使用`flex`产生`yy.lex.c`后主程序只需要使用`yylex`即可得到一个个标记。

---

#### 程序构建及测试

递归下降方法源程序详见`recur_desc`文件夹，文件夹结构如下

```bash
.
├── lex.h
├── lex.l
└── main.c
```

其中`main.c`为递归下降分析主程序，`lex.l`为`flex`输入文件，`lex.h`为词法分析相关头文件，构造可执行程序只需要在终端输入如下命令即可

```bash
flex lex.h     # 生成词法分析程序源码 lex.yy.c
gcc -o expr main.c lex.yy.c  # 生成可执行文件 expr
```

执行一些测试样例如下

```bash
>>1 + 2 + 3
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->NUM
T'->epsilon
E'->epsilon
= 6
```

可以看到正确输出了使用的产生式并计算出了正确结果，下面尝试使用一个较为复杂的表达式进行测试

```bash
>>3 * (3+ 9)*(1+(2+3)) - 27 / 3
E->TE'
T->FT'
F->NUM
T'->*FT'
F->(E)
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->NUM
T'->epsilon
E'->epsilon
T'->*FT'
F->(E)
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->(E)
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->NUM
T'->epsilon
E'->epsilon
T'->epsilon
E'->epsilon
T'->epsilon
E'->-TE'
T->FT'
F->NUM
T'->/FT'
F->NUM
T'->epsilon
E'->epsilon
= 207
```

可以看到最后计算结果正确无误。尝试几个带有语法错误的输入

```bash
>>1 + 2 +
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
8 : error: expect:NUM got:END
```

可以看到程序在检测到第八列(输入是1 + 2 +)出现了语法错误，换一个例子尝试

```bash
>>1 + (2 +4 * 5
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->(E)
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->NUM
T'->*FT'
F->NUM
T'->epsilon
E'->epsilon
14 : error: expect:) got:END
```

可以看到语法分析器在第14列报了一个错误，再换一个示例尝试

```bash
>>1 + 2 & （3 + 4）
E->TE'
T->FT'
F->NUM
T'->epsilon
E'->+TE'
T->FT'
F->NUM
T'->epsilon
E'->epsilon
7 :syntax error: an expression followed by illegal token
```

可以看到程序检测到第7列出现了一个错误。

---

### LL(1)语法分析程序

