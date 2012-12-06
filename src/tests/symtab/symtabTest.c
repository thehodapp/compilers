
#include<stdio.h>
#include "../../symtab.h"

#define trace(x)
#define p(x) trace(x); enterProcedure(x)
#define v(x) trace(x); addVariable(x)
#define up(x) trace("up()"); exitProcedure(x)
extern SymbolTable *root;

void printTable(SymbolTable*, int);
int main() {
	p("test");
		v("input");
		v("output");
		v("a");
		p("p1");
			v("a");
			v("b");
			v("c");
			p("p2");
				v("d");
			up();
		up();
		p("p3");
			v("x");
		up();
		p("p4");
			v("z");
			v("q");
		up();
	up();

	printTable(root, 0);
}

void printTable(SymbolTable* pTab, int depth) {
	for(int i = 0; i < depth; i++) putchar(' ');
	printf("%s\n", pTab->entry->word);
	if(pTab->lchild) printTable(pTab->lchild, depth + 1);
	if(pTab->next) printTable(pTab->next, depth);
}

void semerr(char* msg) {
	fprintf(stderr, "SEMERR: %s\n", msg);
}
