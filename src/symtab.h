#include<stdbool.h>
#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum {
	NONE = 0,

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

Type makeArrayType(Type);
bool isArrayType(Type);
Type unArrayType(Type);

bool isParameterType(Type);
Type makeParameterType(Type);

char* typeToString(Type);

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
void printTable(SymbolTable*, int);
void addVariable(char*, Type);
void enterProcedure(char*, Type);
void exitProcedure(void);
SymbolTableEntry* nthParamOfProc(char*, int);
#endif
