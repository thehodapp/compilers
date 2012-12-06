#ifndef SYMTAB_H
#define SYMTAB_H
typedef struct {
	int type;
	char *word;
} SymbolTableEntry;

typedef struct symbtab {
	SymbolTableEntry* entry;
	struct symbtab* parent;
	struct symbtab* child;
	struct symbtab* prev;
	struct symbtab* next;
} SymbolTable;

#endif
