#ifndef SYMTAB_H
#define SYMTAB_H
typedef struct {
	int type;
	char *word;
} SymbolTableEntry;

typedef struct symtab {
	SymbolTableEntry* entry;
	struct symtab* parent;
	struct symtab* lchild;
	struct symtab* prev;
	struct symtab* next;
} SymbolTable;

SymbolTableEntry* checkSymbolTable(char*);
void addVariable(char*);
void enterProcedure(char*);
void exitProcedure(void);
#endif
