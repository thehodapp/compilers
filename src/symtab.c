#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include "symtab.h"

void initTable(char*);
SymbolTable* createNode(char*);
void printTable(SymbolTable*, int);

SymbolTable *root;
SymbolTable *frontier;
bool shouldTurn;

void initTable(char* word) {
	root = frontier = createNode(word);
	shouldTurn = true;
}

SymbolTable* createNode(char* word) {
	SymbolTable* new = malloc(sizeof(SymbolTable));
	new->entry = malloc(sizeof(SymbolTableEntry));
	new->entry->word = malloc(strlen(word)*sizeof(char));
	strcpy(new->entry->word, word);
	return new;
}

SymbolTableEntry* checkSymbolTable(char* word) {
	if(!frontier) return NULL;

	SymbolTable* pTab = frontier;
	while(true) {
		if(!pTab->entry) {
			return NULL;
		}
		if(!strcmp(word, pTab->entry->word)) {
			return pTab->entry;
		}
		if(pTab->prev) {
			pTab = pTab->prev;
		} else if(pTab->parent) {
			pTab = pTab->parent;
		} else break;
	}
	return NULL;
}

void addVariable(char* word) {
	if(!frontier) fprintf(stderr, "Adding variable before entering procedure!\n");

	SymbolTable* new = createNode(word);

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

void enterProcedure(char* word) {
	if(!frontier) {
		initTable(word);
	} else {
		//add sibling
		SymbolTable* new = createNode(word);
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
