#include <stdio.h>
#include <stdlib.h>
#include "data.h"

int cTerminals = 6;
int cNonTerminals = 3;
Symbol*** parseTableCache = NULL;
Symbol*** parseTableGen() {
	if(parseTableCache) return parseTableCache;
	Symbol*** pt = malloc((cTerminals+cNonTerminals)*sizeof(Symbol**));
	for(int i = cTerminals; i < (cTerminals + cNonTerminals); i++) {
		pt[i] = malloc(cTerminals*sizeof(Symbol*));
		for(int j = 0; j < cTerminals; j++) {
			pt[i][j] = NULL;
		}
	}
	pt[NT_E][T_LPAREN] = malloc(4*sizeof(Symbol));
	pt[NT_E][T_LPAREN][0] = NT_P;
	pt[NT_E][T_LPAREN][1] = T_LPAREN;
	pt[NT_E][T_LPAREN][2] = NT_E;
	pt[NT_E][T_LPAREN][3] = T_RPAREN;
	pt[NT_E][T_F] = malloc(4*sizeof(Symbol));
	pt[NT_E][T_F][0] = NT_P;
    pt[NT_E][T_F][1] = T_LPAREN;
    pt[NT_E][T_F][2] = NT_E;
    pt[NT_E][T_F][3] = T_RPAREN;
	pt[NT_E][T_V] = malloc(2*sizeof(Symbol));
	pt[NT_E][T_V][0] = T_V;
	pt[NT_E][T_V][1] = NT_T;
	pt[NT_P][T_LPAREN] = malloc(0);
	pt[NT_P][T_F] = malloc(sizeof(Symbol));
	pt[NT_P][T_F][0] = T_F;
	pt[NT_T][T_RPAREN] = malloc(0);
	pt[NT_T][T_PLUS] = malloc(2*sizeof(Symbol));
	pt[NT_T][T_PLUS][0] = T_PLUS;
	pt[NT_T][T_PLUS][1] = NT_E;

	parseTableCache = pt;
	return pt;
}

int* parseTableLengths[] = {
	(int[]){-1, -1, -1, -1, -1, -1},
	(int[]){-1, -1, -1, -1, -1, -1},
	(int[]){-1, -1, -1, -1, -1, -1},
	(int[]){-1, -1, -1, -1, -1, -1},
	(int[]){-1, -1, -1, -1, -1, -1},
	(int[]){-1, -1, -1, -1, -1, -1},
	(int[]){4, -1, -1, 4, 2, -1},
	(int[]){0, -1, -1, 1, -1, -1},
	(int[]){-1, 1, 2, -1, -1, 0},
};

Symbol* synchSets[] = {
	(Symbol[]){T_EOF, T_RPAREN, NT_P},
	(Symbol[]){T_LPAREN},
	(Symbol[]){T_EOF, T_RPAREN, NT_P},
};

int synchSetLengths[] = { 3, 1, 3};
