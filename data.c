#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#define X(a) a, 

enum Symbol {
	SYMBS
};
#undef X
// data
int cursor = 0;
Terminal nextTerminal() {
	Terminal terms[] = {T_PERIOD};
	int len = sizeof(terms)/sizeof(terms[0]);
	if(cursor < len) {
		return terms[cursor++];
	} else {
		return terms[len-1];
	}
}
