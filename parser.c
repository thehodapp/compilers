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
		if(isTerminal(symb)) {
			match(symb);
		} else if (isNonTerminal(symb)) {
			consume(symb);
		} else {
			printf("DATA ERROR\n");
			printf("Encountered symbol %d, neither terminal nor nonterminal\n");
			exit(-2);
		}
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

bool matchAny(Terminal* terms, int length) {
	printf("Entering synch mode.\n");
	while(true) {
		for(int i = 0; i < length; i++) {
			if(currTerm == terms[i]) {
				return true;
			}
		}
		currTerm = nextTerminal();
	}
}

int main(int argv, char** argc) {
	parse();
	return 0;
}
