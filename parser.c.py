import sys
from firstfollow import *
from massage import *

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
	#print "%s %s %s" % (l, r, first(r))
	for symb in first(r):
		if symb != '':
			pt[l][symb].append(r)
		else:
			for symb in fol[l]:
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
#include "data.h"
"""
for v in Vh:
	hCode += '#define %s __LINE__\n' % v
for t in Th:
	if t == '': continue
	hCode += '#define %s __LINE__\n' % t
cCode += """#define T_EOF __LINE__
void parse(void);
void consume(NonTerminal);
int match(Terminal);

Terminal currTerm;

void parse() {
	NonTerminal top_nonterminal = firstNonTerminal();
	currTerm = nextTerminal();
	consume(top_nonterminal);
}

void consume(NonTerminal term) {
	switch(term) {
"""
for v in Vh:
	cCode += '\t\tcase %s:\n' % v
	cCode += '\t\t\tswitch(currTerm) {\n'
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
					cCode += '\t\t\t\t\tmatch(%s);\n' % symb
			cCode += '\t\t\t\t\tbreak;\n'
			rulesGenerated.add(tuple(pt[v][t][0]))
	cCode += '\t\t\t}\n'
	cCode += '\t\t\tbreak;\n'
cCode += """
	}
}

void synch(NonTerminal nt) {
	Terminal *synchSet;
	int len;
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
		cCode += '\t\t\tsynchSet = (Terminal[]){%s};\n' % ', '.join(ssv)
		cCode += '\t\t\tlen = %d;\n' % len(ssv)
		cCode += '\t\t\tbreak;\n'
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
		printf("Matched %d\\n", currTerm);
		currTerm = nextTerminal();
		return true;
	}
	return false;
}

int main(int argv, char** argc) {
	parse();
	return 0;
}
"""
print cCode
print
print hCode