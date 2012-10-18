
#ifndef DATA_H
#include <stdbool.h>
#define NT_E 6
#define NT_P 7
#define NT_T 8
#define T_LPAREN 0
#define T_RPAREN 1
#define T_PLUS 2
#define T_F 3
#define T_V 4
#define T_EOF 5

typedef int Symbol;
typedef int Terminal;
typedef int NonTerminal;

Symbol*** parseTableGen(void);
extern int* parseTableLengths[9];
extern Symbol* synchSets[3];
extern int synchSetLengths[3];
extern int cNonTerminals;
extern int cTerminals;

NonTerminal firstNonTerminal(void);
Terminal nextTerminal(void);
bool isTerminal(Symbol s);
bool isNonTerminal(Symbol s);

#define DATA_H
#endif
