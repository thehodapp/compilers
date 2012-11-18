#include<stdio.h>

#ifndef TYPES_H
#define TYPES_H

#ifndef X
#define TYPES_H_DEFINED_X
#define X(a, b) a b,
#endif

#define SYMBS \
X(NULL_, =0) \
X(T_RELOP, ) \
X(T_WS, ) \
X(T_NUM, ) \
X(T_EOF, ) \
X(T_ADDOP, =104) \
X(T_MULOP, =105) \
X(T_ASSIGNOP, ) \
X(T_PAREN, ) \
X(T_COMMA, ) \
X(T_SEMICOLON, ) \
X(T_COLON, ) \
X(T_PERIOD, ) \
X(T_DOUBLEPERIOD, ) \
X(T_ID, ) \
X(T_LBRACK, ) \
X(T_RBRACK, ) \
X(T_LPAREN, ) \
X(T_RPAREN, ) \
X(T_LEXERR, ) \
X(T_PROGRAM, =300) \
X(T_VAR, =301) \
X(T_ARRAY, =302) \
X(T_OF, =303) \
X(T_INTEGER, =304) \
X(T_REAL, =305) \
X(T_PROCEDURE, =307) \
X(T_BEGIN, =308) \
X(T_END, =309) \
X(T_IF, =310) \
X(T_THEN, =311) \
X(T_ELSE, =312) \
X(T_WHILE, =313) \
X(T_DO, =314) \
X(T_NOT, =315) \
X(T_CALL, =316) \
X(T_PLUS, ) \
X(T_MINUS, ) \
X(RELOP_EQ, ) \
X(RELOP_GE, ) \
X(RELOP_GT, ) \
X(RELOP_LE, ) \
X(RELOP_NE, ) \
X(RELOP_LT, ) \
X(NUM_INT, ) \
X(NUM_REAL, ) \
X(NUM_LONGREAL, ) \
X(ADDOP_PLUS, ) \
X(ADDOP_MINUS, ) \
X(ADDOP_OR, =1042) \
X(MULOP_TIMES, ) \
X(MULOP_DIVIDE, ) \
X(MULOP_DIV, =1052) \
X(MULOP_MOD, =1053) \
X(MULOP_AND, =1054) \
X(ERR_ID_LEN, ) \
X(ERR_INT_LEN, ) \
X(ERR_INT_LEADING_ZERO, ) \
X(ERR_DECIMAL_LEN, ) \
X(ERR_EXPONENT_LEN, ) \
X(ERR_EXPONENT_LEADING_ZERO, ) \
X(ERR_UNRECOG_TOKEN, ) \

#ifdef TYPES_H_DEFINED_X

enum Symbol {
	SYMBS
};

#undef X
#undef TYPES_H_DEFINED_X
#endif

typedef struct rword {
	char* word;
	enum Symbol type;
	enum Symbol attribute;
} ReservedWord;

typedef struct rwordll {
	ReservedWord* rword;
	struct rwordll* next;
} ReservedWordList;

ReservedWordList* parseResWordFile(FILE* fp);
char* convertConstantToString(enum Symbol);

#endif
