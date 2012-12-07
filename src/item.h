#include "symtab.h"
typedef struct item {
	struct foo {
		int count;
		char* proc;
		Type type;
	} in;
	char *lexeme;
	Type type;
	bool error;
	bool errHere;
} Item;
