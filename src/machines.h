#include "types.h"

typedef struct {
	char* lexeme;
	char* newString;
	int type;
	union {
		int attribute;
		void* pointer;
	};
	int validToken;
	int error;
} MachineResult;


typedef struct {
	char* word;
} SymbolTableEntry;

typedef struct symbtab {
	SymbolTableEntry* entry;
	struct symbtab* next;
} SymbolTable;

SymbolTable *tab;
void machinesInit(char*);
MachineResult identifyToken(char* str);
typedef MachineResult (*Machine)(char*);
