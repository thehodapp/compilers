void setup(NonTerminal*, (Terminal*)(*myLexTerminal)());
void parse(void);
void consume(NonTerminal*);
int match(Terminal*);
bool matchAny(Terminal*, int);

(Terminal*)(*lexTerminal)(FILE*);
NonTerminal* top_nonterminal = NT_PROGRAM;

void setup(NonTerminal* myTop_nonterminal, (Terminal*)(*myLexTerminal)()) {
	lexTerminal = myLexTerminal;
	top_nonterminal = myTop_nonterminal;
}

void parse() {
	consume(top_nonterminal);
}

void consume(NonTerminal* nonterminal) {
	Terminal* term = tokens[0];
	Symbol** toConsume = parseTable[nonterminal][term];
	for(int i = 0; i < parseTableLengths[nonterminal][term]; i++) {
		Symbol* symb = toConsume[i];
		if(symb->isTerminal) {
			if(!match(tokens, symb)) {
				Symbol** synchs = synchSets[nonterminal];
				matchAny(synchs, synchSetLengths[nonterminal]);
				break;
			}
		} else {
			consume(tokens, symb);
		}
		toConsume = toConsume->next;
	}
}

int match(Terminal* terminal) {
	Terminal* nextTerm = (*lexTerminal)();
	return nextTerm->type == terminal->type;
}

bool matchAny(Terminal** terms, int length) {
	while(true) {
		Terminal* nextTerm = (*lexTerminal)();
		for(int i = 0; i < length; i++) {
			if(nextTerm->type == terminal->type) {
				return;
			}
		}
	}
}

int main(char** argc, int argv) {
	parse();
}
