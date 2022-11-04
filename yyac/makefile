calc : lex.yy.c y.tab.c y.tab.h
	gcc -o calc lex.yy.c y.tab.c

lex.yy.c : calc.l
	flex calc.l

y.tab.c y.tab.h : calc.y
	bison -yd calc.y

clean:
	rm calc lex.yy.c y.tab.* *.output