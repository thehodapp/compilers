#ifndef MACHINES_H
#define MACHINES_H
#include "types.h"
#include "symtab.h"

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


void machinesInit(char*);
MachineResult identifyToken(char* str);
typedef MachineResult (*Machine)(char*);

#endif
