#include<stdbool.h>
#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum {
	NONE = 0,

	PGNAME,
	PGPARM,
	PROCNAME,

	REAL,

	INT,
} StandardType;

typedef struct {
	StandardType st_type;
	bool isArray;
	bool isParam;
	int low;
	int high;
} Type;

int typeSize(Type);
Type makeParameterType(Type);
Type unParameterType(Type);
#define unPP unParameterType

char* typeToString(Type);
bool typeEqual(Type, Type);

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
void printTable(FILE*, SymbolTable*, int, int);
void addVariable(char*, Type);
void enterProcedure(char*, Type);
void exitProcedure(void);
SymbolTableEntry* nthParamOfProc(char*, int);
#endif
