#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "data.h"

void parse(void);
void consume(NonTerminal);
int match(Terminal);
bool matchAny(Terminal*, int);

NonTerminal top_nonterminal;
Terminal currTerm;
Symbol*** parseTable;

void parse() {
	top_nonterminal = firstNonTerminal();
	currTerm = nextTerminal();
	parseTable = parseTableGen();
	consume(top_nonterminal);
}

void consume(NonTerminal term) {
	switch(term) {
		case NT_E:
			switch(currTerm) {
				case T_LPAREN:
				case T_F:
					consume(NT_P);
					match(T_LPAREN);
					consume(NT_E);
					match(T_RPAREN);
					break;
				case T_V:
					match(T_V);
					consume(NT_T);
					break;
			}
			break;
		case NT_P:
			switch(currTerm) {
				case T_F:
					match(T_F);
					break;
				case T_LPAREN:
					break;
			}
			break;
		case NT_T:
			switch(currTerm) {
				case T_RPAREN:
					break;
				case T_PLUS:
					match(T_PLUS);
					consume(NT_E);
			}
			break;
	}
}

void synch(NonTerminal nt) {
	Terminal *synchSet;
	int len;
	switch(nt) {
		case NT_E:
			synchSet = (Terminal[]){T_EOF, T_RPAREN, NT_P};
			len = 3;
			break;
		case NT_P:
			synchSet = (Terminal[]){T_EOF, T_LPAREN};
			len = 2;
			break;
		case NT_T:
			synchSet = (Terminal[]){T_EOF, T_RPAREN, NT_P};
			len = 3;
			break;
	}
	while(true) {
		for(int i = 0; i < sizeof(synchSet)/sizeof(*synchSet); i++) {
			if(currTerm = synchSet[i]) {
				return;
			}
		}
		currTerm = nextTerminal();
	}
}

int match(Terminal terminal) {
	if(currTerm == terminal) {
		printf("Matched %d\n", currTerm);
		currTerm = nextTerminal();
		return true;
	}
	return false;
}

int main(int argv, char** argc) {
	parse();
	return 0;
}
