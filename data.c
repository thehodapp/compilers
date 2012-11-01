#include <stdio.h>
#include <stdlib.h>
#include "data.h"

// data
NonTerminal firstNonTerminal() {
	return NT_E;
}
int cursor = 0;
Terminal nextTerminal() {
	Terminal terms[] = {T_F, T_LPAREN, T_V, T_PLUS, T_V, T_RPAREN, T_PLUS, T_V, T_EOF};
	int len = sizeof(terms)/sizeof(terms[0]);
	if(cursor < len) {
		return terms[cursor++];
	} else {
		return terms[len-1];
	}
}

bool isTerminal(Symbol s) {
	return 0 <= s && s < cTerminals;
}

bool isNonTerminal(Symbol s) {
	return cTerminals <= s && s < cTerminals + cNonTerminals;
}
