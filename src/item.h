#ifndef ITEM_H
#define ITEM_H
#include "symtab.h"
typedef struct item {
	struct foo {
		int count;
		char* proc;
		Type type;
	} in;
	
	Type type;
	char *lexeme;
	bool error;
	bool errHere;
} Item;

Item* createItem(void);
#endif
