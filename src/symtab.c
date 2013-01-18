#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "symtab.h"

extern void semerr(char*);

void initTable(char*, Type);
SymbolTable* createNode(char*, Type);
char* stTypeToString(StandardType);

SymbolTable *root;
SymbolTable *frontier;
bool shouldTurn;

void initTable(char* word, Type type) {
	root = frontier = createNode(word, type);
	shouldTurn = true;
}

SymbolTable* createNode(char* word, Type type) {
	SymbolTable* new = malloc(sizeof(SymbolTable));
	new->entry = malloc(sizeof(SymbolTableEntry));

	new->entry->type = type;
	new->entry->word = malloc(strlen(word)*sizeof(char));
	strcpy(new->entry->word, word);
	return new;
}

SymbolTable* findNode(char* word, bool local) {
	if(!frontier) return NULL;

	SymbolTable *limit;
	if(local && shouldTurn) limit = frontier;
	else if(local && frontier->parent) limit = frontier->parent;
	else limit = root;


	SymbolTable* pTab = frontier;
	while(pTab && pTab != limit) {
		if(!strcmp(word, pTab->entry->word)) {
			return pTab;
		}
		if(pTab->prev) {
			pTab = pTab->prev;
		} else if(pTab->parent) {
			pTab = pTab->parent;
		}
	}
	return NULL;
}

SymbolTableEntry* checkSymbolTable(char* word, bool local) {
	SymbolTable* node = findNode(word, local);
	if(node) return node->entry;
	else return NULL;
}

void addVariable(char* word, Type type) {
	if(!frontier) fprintf(stderr, "Adding variable before entering procedure!\n");
	if(checkSymbolTable(word, true)) semerr("Duplicate variable names.");

	SymbolTable* new = createNode(word, type);

	if(shouldTurn) {
		new->parent = frontier;
		frontier->lchild = new;
		shouldTurn = false;
	} else {
		//add sibling
		new->prev = frontier;
		new->parent = frontier->parent;
		frontier->next = new;
	}
	frontier = new;
}

void enterProcedure(char* word, Type type) {
	if(checkSymbolTable(word, false)) semerr("Duplicate procedure names.");
	if(!frontier) {
		initTable(word, type);
	} else {
		//add sibling
		SymbolTable* new = createNode(word, type);
		new->prev = frontier;
		new->parent = frontier->parent;
		frontier->next = new;

		frontier = new;
	}
	shouldTurn = true;
}

void exitProcedure() {
	if(!frontier) fprintf(stderr, "Error: Stack Underflow\n");
	//up to parent
	frontier = frontier->parent;

	shouldTurn = false;
}

// 1-indexed!
SymbolTableEntry* nthParamOfProc(char* word, int n) {
	SymbolTable* proc = findNode(word, false);
	if(proc == NULL) return NULL;
	SymbolTable* pParam = proc;
	pParam = proc->lchild;
	if(!pParam || !pParam->entry->type.isParam) return NULL;
	//step n-1 times
	for(int i = 1; i < n; i++) {
		pParam = pParam->next;
		if(!pParam || !pParam->entry->type.isParam)
			return NULL;
	}
	return pParam->entry;
}
	

void printTable(FILE* f, SymbolTable *base, int depth, int offset) {
	if(!base || !base->entry) return;
	for(int i = 0; i < depth; i++) fprintf(f, " ");
	if(typeSize(base->entry->type) > 0) fprintf(f, "%s %s offset %d\n", base->entry->word, typeToString(base->entry->type), offset);
	else fprintf(f, "%s\n", base->entry->word);
	printTable(f, base->lchild, depth + 1, 0);
	printTable(f, base->next, depth, offset + typeSize(base->entry->type));
}

int typeSize(Type t) {
	int stSize;
	if(t.st_type == INT) stSize = 4;
	else if(t.st_type == REAL) stSize = 8;
	else stSize = 0;
	
	if(t.isArray) return stSize * (t.high - t.low + 1);
	else return stSize;
}

Type makeParameterType(Type t) {
	Type ret = t;
	ret.isParam = true;
	return ret;
}

Type unParameterType(Type t) {
	Type ret = t;
	ret.isParam = false;
	return ret;
}

char* typeToString(Type t) {
	if(t.st_type == NONE) return "";
	char *res = malloc(100*sizeof(char));
	res[0] = '(';
	int i = 1;
	if(t.isArray)
		i += sprintf(res+i, "array [%d..%d] of ", t.low, t.high);
		
	i += sprintf(res+i, "%s", stTypeToString(t.st_type));

	if(t.isParam && t.st_type != PGPARM)
		i += sprintf(res+i, ", proc param");

	i += sprintf(res+i, ")");
	return res;
}

char *stTypeToString(StandardType t) {
	switch(t) {
	case NONE: return "none";
	case PGNAME: return "program name";
	case PGPARM: return "program param";
	case PROCNAME: return "procedure name";
	case REAL: return "real";
	case INT: return "int";
	}
	return "";
}

bool typeEqual(Type t1, Type t2) {
	if(t1.st_type != t2.st_type) return false;
	if(t1.isArray != t2.isArray) return false;
	if(t1.isArray) {
		if(t1.low != t2.low) return false;
		if(t1.high != t2.high) return false;
	}
	return true;
}
