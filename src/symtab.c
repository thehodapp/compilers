#include<stdlib.h>
#include<string.h>
#include "symtab.h"
SymbolTable *tab;
SymbolTableEntry* checkSymbolTable(char* word) {
	if(!tab) {
		tab = malloc(sizeof(SymbolTable));
		tab->entry = NULL;
	}

	SymbolTable* pTab = tab;
	while(1) {
		if(!pTab->entry) {
			tab->entry = malloc(sizeof(SymbolTableEntry));
			tab->entry->word = malloc(strlen(word)+1);
			strcpy(tab->entry->word, word);
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
	tab->next = malloc(sizeof(SymbolTable));
	tab->next->entry = malloc(sizeof(SymbolTableEntry));
	tab->next->prev = tab;
	tab->next->entry->word = malloc(strlen(word)+1);
	strcpy(tab->next->entry->word, word);
	tab = tab->next;
	return tab->entry;
}
