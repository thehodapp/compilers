import sys
from firstfollow import *
from massage import *

H = (Vh, Th, Sh, Ph)
fir = firsts()
fol = follows()

#construct table
pt = defaultdict(lambda: defaultdict(list))

for (l,r) in Ph:
	# construct first(r)
	firstR = set()
	if r[0] in Th:
		firstR = set(r[0])
	elif r[0] in Vh:
		firstR = fir[r[0]]
	elif r[0] == '':
		firstR = set([''])
	for symb in firstR:
		print "symb = %s" % repr(symb)
		if symb in Th:
			parseTable[l][symb].append(r)
		if symb == '':
			for symb in fol[l]:
				parseTable[l][symb].append(r)

for v in Vh:
	for t in Th:
		if parseTable[v] and parseTable[l][t]:
			if len(parseTable[v][t]) > 1:
				print 'WTF'
				print '\n'.join(', '.join(l) for l in parseTable[v][t])
				sys.exit()

cCode = """
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
"""
for v in Vh:
	cCode += 'case %s:\n' % v
	cCode += 'switch(currTerm) {\n'
	for t in Th:
		cCode += 'case %s:\n' % t
		if pt[v][t] and pt[v][t][0]:
			for symb in pt[v][t][0]:
				if symb in Vh:
					cCode += 'consume(%s);\n' % symb
				elif symb in Th:
					cCode += 'match(%s);\n' % symb
		cCode += 'break;\n'
	cCode += '}'
	cCode += 'break;\n'
cCode += """
	}
}

void synch(NonTerminal nt) {
	Terminal *synchSet;
	int len;
	switch(nt) {
"""
for v in Vh:
	synch = set(['T_EOF']) | fol[v]
	cCode += 'case %s:\n' % v
	cCode += 'synchSet = (Terminal[]){%s};\n' % ', '.join(synch)
	cCode += 'len = %d;\n' % len(synch)
	cCode += 'break;\n'
cCode += """
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
