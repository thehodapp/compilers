#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "symtab.h"

extern void semerr(char*);

void initTable(char*, Type);
SymbolTable* createNode(char*, Type);

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

SymbolTableEntry* nthParamOfProc(char* word, int n) {
	SymbolTable* proc = findNode(word, false);
	SymbolTable* pParam = proc;
	for(int i = 0; i <= n; i++) {
		if(i) pParam = pParam->next;
		else pParam = pParam->lchild;
		if(!pParam || !isParameterType(pParam->entry->type))
			return NULL;
	}
	return pParam->entry;
}
	

void printTable(SymbolTable *base, int depth) {
	if(!base || !base->entry) return;
	for(int i = 0; i < depth; i++) putchar(' ');
	printf("%s %s\n", base->entry->word, typeToString(base->entry->type));
	printTable(base->lchild, depth + 1);
	printTable(base->next, depth);
}

// type functions
bool isArrayType(Type t) {
	return t == AINT || t == AREAL || t == PPAINT || t == PPAREAL;
}

Type makeArrayType(Type t) {
	switch(t) {
		case REAL:
		case PPREAL:
			return AREAL;
		case INT:
		case PPINT:
			return AINT;
		default: return t;
	}
}

Type unArrayType(Type t) {
	switch(t) {
		case AREAL:
		case PPAREAL:
			return REAL;
		case AINT:
		case PPAINT:
			return INT;
		default: return t;
	}
}

bool isParameterType(Type t) {
	return t == PPINT || t == PPREAL || t == PPAINT || t == PPAREAL;
}

Type makeParameterType(Type t) {
	switch(t) {
		case INT: return PPINT;
		case REAL: return PPREAL;
		case AINT: return PPAINT;
		case AREAL: return PPAREAL;
		default: return t;
	}
}

Type unParameterType(Type t) {
	switch(t) {
		case PPINT: return INT;
		case PPREAL: return REAL;
		case PPAINT: return AINT;
		case PPAREAL: return AREAL;
		default: return t;
	}
}

char *typeToString(Type t) {
	switch(t) {
	case NONE: return "";
	case PGNAME: return "(program name)";
	case PGPARM: return "(program param)";
	case PROCNAME: return "(procedure name)";
	case INT: return "(int)";
	case AINT: return "(array of int)";
	case PPINT: return "(int, proc param)";
	case PPAINT: return "(array of int, proc param)";
	case REAL: return "(real)";
	case PPREAL: return "(real, proc param)";
	case AREAL: return "(array of real)";
	case PPAREAL: return "(array of real, proc param)";
	default: return "FIXMEFIXME";
	}
}
