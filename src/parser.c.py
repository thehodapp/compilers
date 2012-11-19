import sys
from firstfollow import *
from massage import *
from table import *

hCode = ""
cCode = """
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../p1/src/types.h"
#include "../p1/src/machines.h"

int depth = 0;
typedef MachineResult Terminal;
typedef enum {
"""
for v in Vh:
	cCode += "\t%s,\n" % v
cCode += """} NonTerminal;

char* ntToString(NonTerminal nt) {
	switch(nt) {
"""
for v in Vh:
	cCode += "\t\tcase %s: return \"%s\";\n" % (v,v)
cCode += """\t}
}
void parse(void);
void consume(NonTerminal);
int match(int, NonTerminal);
void synch(NonTerminal);
void synerr(int*, int, Terminal);
void lexerr(Terminal);
Terminal nextTerminal(void);

Terminal currTerm;
FILE *fSrc, *fTree, *fList;
char sLine[90] = {0};
char *psLine;
int cLine;
int cColumn;

void parse() {
"""
cCode += "\tNonTerminal top_nonterminal = %s;" % Sh
cCode += """
	currTerm = nextTerminal();
	consume(top_nonterminal);
}

void consume(NonTerminal nt) {
	for(int i = 0; i < depth; i++) fprintf(fTree, " ");
	fprintf(fTree, "%s\\n", ntToString(nt));
	depth++;
	switch(nt) {
"""
for v in Vh:
	expectedVars = set()
	labelName = '%s_synch' % v.lower()
	labelUsed = False
	cCode += '\t\tcase %s:\n' % v
	cCode += '\t\t\tswitch(currTerm.type) {\n'
	rulesGenerated = set()
	for t in Th:
		if t and pt[v][t] and pt[v][t][0] and tuple(pt[v][t][0]) not in rulesGenerated:
			cCode += '\t\t\t\tcase %s:\n' % t
			expectedVars.add(t)
			for otherT in Th:
				if pt[v][t] == pt[v][otherT] and otherT != t:
					cCode += '\t\t\t\tcase %s:\n' % otherT
					expectedVars.add(otherT)
			for symb in pt[v][t][0]:
				if symb in Vh:
					cCode += '\t\t\t\t\tconsume(%s);\n' % symb
				elif symb in Th and symb != '':
					cCode += '\t\t\t\t\tif(!match(%s, nt)) goto %s;\n' % (symb, labelName)
					labelUsed = True
			cCode += '\t\t\t\t\tbreak;\n'
			rulesGenerated.add(tuple(pt[v][t][0]))
	cCode += '\t\t\t\tdefault:\n'
	cCode += '\t\t\t\t\tsynerr((int[]){%s}, %d, currTerm);\n' % (', '.join(expectedVars), len(expectedVars))
	if labelUsed: cCode += '\t\t\t\t%s:\n' % labelName
	cCode += '\t\t\t\t\tsynch(nt);\n'
	cCode += '\t\t\t\t\tbreak;\n'
	cCode += '\t\t\t}\n'
	cCode += '\t\t\tbreak;\n'
cCode += """
	}
	depth--;
}

void synch(NonTerminal nt) {
	int *synchSet;
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
		cCode += '\t\t\tsynchSet = (int[]){%s}; len = %d;\n' % (', '.join(ssv), len(ssv))
		cCode += '\t\t\tbreak;\n'
cCode += """
	}
	while(true) {
		for(int i = 0; i < len; i++) {
			if(currTerm.type == synchSet[i]) {
				char* lexPrint = currTerm.lexeme;
				if(currTerm.type == T_EOF) {
					fprintf(stderr, "Reached end-of-file; terminating\\n");
					exit(1);
				} else {
					fprintf(stderr, "Synchronized on term \\"%s\\" (%s)\\n", lexPrint, convertConstantToString(currTerm.type));
					return;
				}
			}
		}
		currTerm = nextTerminal();
	}
}

int match(int termtype, NonTerminal nt) {
	if(currTerm.type == termtype) {
		for(int i = 0; i < depth; i++) fprintf(fTree, " ");
		fprintf(fTree, "%s\\n", convertConstantToString(currTerm.type));
		for(int i = 0; i < depth+1; i++) fprintf(fTree, " ");
		fprintf(fTree, "\\"%s\\"\\n", currTerm.lexeme);
		currTerm = nextTerminal();
		return true;
	}
	synerr((int[]){termtype}, 1, currTerm);
	return false;
}

void synerr(int *expected, int expLen, Terminal encountered) {
	char *str = " one of";
	if(expLen == 1) str = "";
	fprintf(fList, "SYNERR, column %d: Unexpected %s; expected%s: ", cColumn, convertConstantToString(encountered.type), str);
	for(int i = 0; i < expLen - 1; i++) {
		fprintf(fList, "%s, ", convertConstantToString(expected[i]));
	}
	fprintf(fList, "%s\\n", convertConstantToString(expected[expLen-1]));
	fprintf(stderr, "SYNERR, line %d, column %d: Unexpected %s; expected%s: ", cLine, cColumn, convertConstantToString(encountered.type), str);
	for(int i = 0; i < expLen - 1; i++) {
		fprintf(stderr, "%s, ", convertConstantToString(expected[i]));
	}
	fprintf(stderr, "%s\\n", convertConstantToString(expected[expLen-1]));
}

int main(int argc, char** argv) {
	if(argc < 2) exit(2);
	char *sfSrc = argv[1];
	fSrc = fopen(sfSrc, "r");
	char sfTree[80];
	strcpy(sfTree, sfSrc);
	strcpy(sfTree + strlen(sfSrc) - 4, ".tree");
	fTree = fopen(sfTree, "w");
	char sfList[80];
	strcpy(sfList, sfSrc);
	strcpy(sfList + strlen(sfSrc) - 4, ".lst");
	fList = fopen(sfList, "w");

	cLine = 1;
	cColumn = 0;
	machinesInit("p1/data/reserved-words.txt");
	parse();
	fclose(fSrc);
	fclose(fTree);
	return 0;
}

Terminal nextTerminal() {
	if(!psLine) psLine = sLine;
	if(!*psLine) {
		fgets(sLine, sizeof(sLine), fSrc);
		fprintf(fList, "%d. %s", cLine, sLine);
		if(feof(fSrc)) {
			sLine[0] = EOF;
			sLine[1] = 0;
		}
		psLine = sLine;
		cLine++;
		cColumn = 1;
	}
	MachineResult res = identifyToken(psLine);
	cColumn += res.newString - psLine;
	psLine = res.newString;
	if(res.type == T_WS) {
		return nextTerminal();
	}
	if(res.type == T_LEXERR) {
		lexerr(res);
		return nextTerminal();
	}
	return res;
}

void lexerr(Terminal res) {
	fprintf(fList, "%s, column %d\\n", convertConstantToString(res.error), cColumn);
	fprintf(stderr, "%s, line %d, column %d\\n", convertConstantToString(res.error), cColumn, cLine);
}
"""
print cCode
