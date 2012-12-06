#include<stdbool.h>
#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum {
	PGNAME,
	PGPARM,
	PROCNAME,

	REAL,
	AREAL,
	PPREAL,
	PPAREAL,

	INT,
	AINT,
	PPINT,
	PPAINT,
} Type;

typedef struct {
	Type type;
	char *word;
} SymbolTableEntry;

typedef struct symtab {
	SymbolTableEntry* entry;
	struct symtab* parent;
	struct symtab* lchild;
	struct symtab* prev;
	struct symtab* next;
} SymbolTable;

SymbolTableEntry* checkSymbolTable(char*, bool);
void addVariable(char*);
void enterProcedure(char*);
void exitProcedure(void);
#endif
