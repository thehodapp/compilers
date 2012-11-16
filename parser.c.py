import sys
from firstfollow import *
from massage import *

Vh, Th = sorted(Vh), sorted(Th)
H = (Vh, Th, Sh, Ph)
fir = firsts()
fol = follows()

#construct table
pt = defaultdict(lambda: defaultdict(list))

def first(w):
	if w == []:
		return set([''])
	if w[0] in Th:
		return set([w[0]])
	if w[0] in Vh:
		if '' in fir[w[0]]:
			return (fir[w[0]] - set([''])) | first(w[1:])
		else:
			return fir[w[0]]

for (l,r) in Ph:
	for symb in first(r):
		if symb != '':
			pt[l][symb].append(r)
		else:
			for symb in fol[l]:
				if l.startswith('NT_STATEMENT1') and symb == 'T_ELSE':
					# special-case: dangling else ambiguity
					pass
				else:
					pt[l][symb].append(r)

for v in Vh:
	for t in Th:
		if pt[v] and pt[l][t]:
			if len(pt[v][t]) > 1:
				print 'WTF'
				print v, t, pt[v][t]
				sys.exit()

hCode = ""
cCode = """
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "p1/src/types.h"
#include "p1/src/machines.h"

int depth = 0;
typedef MachineResult Terminal;
typedef enum {
"""
for v in Vh:
	cCode += "\t%s," % v
cCode += """} NonTerminal;

char* ntToString(NonTerminal nt) {
	switch(nt) {
"""
for v in Vh:
	cCode += "case %s: return \"%s\"; " % (v,v)
cCode += """}
}
void parse(void);
void consume(NonTerminal);
int match(int);
void synch(NonTerminal);
Terminal nextTerminal(void);

Terminal currTerm;
FILE *fSrc;
char sLine[90];
char *psLine;

void parse() {
"""
cCode += "\tNonTerminal top_nonterminal = %s;" % Sh
cCode += """
	currTerm = nextTerminal();
	consume(top_nonterminal);
}

void consume(NonTerminal term) {
	for(int i = 0; i < depth; i++) printf(" ");
	printf("%s\\n", ntToString(term));
	depth++;
	switch(term) {
"""
for v in Vh:
	cCode += '\t\tcase %s:\n' % v
	cCode += '\t\t\tswitch(currTerm.type) {\n'
	rulesGenerated = set()
	for t in Th:
		if t and pt[v][t] and pt[v][t][0] and tuple(pt[v][t][0]) not in rulesGenerated:
			cCode += '\t\t\t\tcase %s:\n' % t
			for otherT in Th:
				if pt[v][t] == pt[v][otherT] and otherT != t:
					cCode += '\t\t\t\tcase %s:\n' % otherT
			for symb in pt[v][t][0]:
				if symb in Vh:
					cCode += '\t\t\t\t\tconsume(%s);\n' % symb
				elif symb in Th and symb != '':
					cCode += '\t\t\t\t\tif(!match(%s)) {synch(term); break;}\n' % symb
			cCode += '\t\t\t\t\tbreak;\n'
			rulesGenerated.add(tuple(pt[v][t][0]))
	cCode += '\t\t\t\tdefault:\n'
	cCode += '\t\t\t\t\tsynch(term);\n'
	cCode += '\t\t\t\t\tbreak;\n'
	cCode += '\t\t\t}\n'
	cCode += '\t\t\tbreak;\n'
cCode += """
	}
	depth--;
}

void synch(NonTerminal nt) {
	printf("Entering synch mode for non-terminal %s\\n", ntToString(nt));
	int *synchSet;
	switch(nt) {
"""
varsGenerated = set()
synch = {}
for v in Vh:
	synch[v] = set(['T_EOF']) | fol[v]
for v in synch:
	if v not in varsGenerated:
		ssv = synch[v]
		generatingVars = [v for (v,s) in synch.items() if synch[v] == ssv]
		for gv in generatingVars:
			cCode += '\t\tcase %s:\n' % gv
			varsGenerated.add(gv)
		cCode += '\t\t\tsynchSet = (int[]){%s};\n' % ', '.join(ssv)
		cCode += '\t\t\tbreak;\n'
cCode += """
	}
	while(true) {
		for(int i = 0; i < sizeof(synchSet)/sizeof(*synchSet); i++) {
			if(currTerm.type == synchSet[i]) {
				return;
			}
		}
		printf("Throwing away \\"%s\\" (%s)\\n", currTerm.lexeme, convertConstantToString(currTerm.type));
		currTerm = nextTerminal();
	}
}

int match(int termtype) {
	if(currTerm.type == termtype) {
		for(int i = 0; i < depth; i++) printf(" ");
		printf("%s\\n", convertConstantToString(currTerm.type));
		for(int i = 0; i < depth+1; i++) printf(" ");
		printf("\\"%s\\"\\n", currTerm.lexeme);
		currTerm = nextTerminal();
		return true;
	}
	return false;
}

int main(int argv, char** argc) {
	char *sfSrc = argc[1];
	fSrc = fopen(sfSrc, "r");
	machinesInit("p1/data/reserved-words.txt");
	parse();
	return 0;
}

Terminal nextTerminal() {
	if(!psLine) psLine = sLine;
	if(!*psLine) {
		fgets(sLine, sizeof(sLine), fSrc);
		if(feof(fSrc)) {
			sLine[0] = EOF;
			sLine[1] = 0;
		}
		psLine = sLine;
	}
	MachineResult res = identifyToken(psLine);
	psLine = res.newString;
	if(res.type == T_WS) {
		return nextTerminal();
	}
	return res;
}
"""
print cCode
