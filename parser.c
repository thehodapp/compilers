#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "data.h"

void parse(void);
void consume(NonTerminal);
int match(Terminal);
bool matchAny(Terminal*, int);


Terminal(*lexTerminal)();
NonTerminal top_nonterminal;
Terminal currTerm;
Symbol*** parseTable;

void setup(NonTerminal myTop_nonterminal, Terminal(*myLexTerminal)()) {
	lexTerminal = myLexTerminal;
	top_nonterminal = myTop_nonterminal;
	currTerm = myLexTerminal();
	parseTable = parseTableGen();
}

void parse() {
	consume(top_nonterminal);
}

void consume(NonTerminal nonterminal) {
	Symbol* toConsume = parseTable[nonterminal][currTerm];
	int len = parseTableLengths[nonterminal][currTerm];
	printf("%d has %d children\n", nonterminal, len);
	if(!toConsume || len < 0) {
		printf("ERROR\n");
		printf("nonterminal = %d, currTerm = %d\n", nonterminal, currTerm);
		exit(1);
	}
	for(int i = 0; i < len; i++) {
		printf("\ti = %d\n", i);
		Symbol symb = toConsume[i];
		if(symb < cTerminals) {
			match(symb);
		} else {
			consume(symb);
		}
	}
	printf("Finished consuming %d\n", nonterminal);
}

int match(Terminal terminal) {
	if(currTerm == terminal) {
		printf("Matched %d\n", currTerm);
		currTerm = lexTerminal();
		return true;
	}
	return false;
}

bool matchAny(Terminal* terms, int length) {
	printf("Entering synch mode.\n");
	while(true) {
		for(int i = 0; i < length; i++) {
			if(currTerm == terms[i]) {
				return true;
			}
		}
		currTerm = (*lexTerminal)();
	}
}

Terminal getTerminal(void);
int main(int argv, char** argc) {
	setup(NT_E, &getTerminal);
	parse();
	return 0;
}

int cursor = 0;
Terminal getTerminal() {
	Terminal terms[] = {T_F, T_LPAREN, T_V, T_PLUS, T_V, T_RPAREN, T_PLUS, T_V, T_EOF};
	int len = sizeof(terms)/sizeof(terms[0]);
	if(cursor < len) {
		return terms[cursor++];
	} else {
		return terms[len-1];
	}
}
