compiler-final-proj: proj2c.h proj2c.c proj2c.tab.c
	gcc -Wall -Wextra -ggdb -O0 -std=gnu99 -g -o compiler-final-proj lex.yy.c proj2c.c proj2c.tab.c
	
proj2c.tab.c: proj2c.y lex.yy.c
	bison -d proj2c.y
	
lex.yy.c: proj2c.l
	flex proj2c.l 
	
proj2c.l:
	flex proj2c.l
