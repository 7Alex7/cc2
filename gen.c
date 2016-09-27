//parse and gen

#include "cc.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

int *emit, *e;

void gen_init(void) {
	static int is_init = 0;
	if(!is_init) {
		emit = e = (int*)malloc(MAXSIZE * sizeof(int));
		is_init = 1;
	}
}

void gen(char *str) {
	//into code
	*e++ = AG; int *_main = e++;
	*e++ = PUSH; *e++ = AX;
	*e++ = SET; *e++ = AX; int *_exit = e++;
	*e++ = PUSH; *e++ = AX;
	*e++ = CALL; *e++ = 0;//*e++ = JMP; int *_main = e++;
	*_exit = e - emit; *e++ = EXIT;
	
	//parse and gen
	tokensrc(str);
	next();
	while(strcmp(tks, "") || tki != -1) {
		declare(GLO);
		next();
	}
	*_main = getid("main") -> offset;
}