#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "symtab.h"

extern void semerr(char*);

void initTable(char*);
SymbolTable* createNode(char*);
void printTable(SymbolTable*, int);

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

SymbolTableEntry* checkSymbolTable(char* word, bool local) {
	if(!frontier) return NULL;

	SymbolTable *limit;
	if(local && shouldTurn) limit = frontier;
	else if(local && frontier->parent) limit = frontier->parent;
	else limit = root;


	SymbolTable* pTab = frontier;
	while(pTab && pTab != limit) {
		if(!strcmp(word, pTab->entry->word)) {
			return pTab->entry;
		}
		if(pTab->prev) {
			pTab = pTab->prev;
		} else if(pTab->parent) {
			pTab = pTab->parent;
		}
	}
	return NULL;
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
		initTable(word);
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
