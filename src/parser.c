
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "types.h"
#include "machines.h"
#include "item.h"

#define RET_SYNERR  2
#define RET_LEXERR  4
#define RET_SEMERR  8
#define RET_TYPERR 16

#define semerrf(format, ...) \
char *buff = malloc((strlen(format)+100)*sizeof(char)); \
sprintf(buff, format, __VA_ARGS__); \
semerr(buff); \
free(buff);

extern SymbolTable* root;
int depth = 0;
typedef MachineResult Terminal;
#define NONTERMS \
x(NT_ARGUMENTS) \
x(NT_COMPOUND_STATEMENT) \
x(NT_COMPOUND_STATEMENT_) \
x(NT_DECLARATIONS) \
x(NT_DECLARATIONS_) \
x(NT_EXPRESSION) \
x(NT_EXPRESSION_) \
x(NT_EXPRESSION_LIST) \
x(NT_EXPRESSION_LIST_) \
x(NT_FACTOR) \
x(NT_FACTOR_) \
x(NT_IDENTIFIER_LIST) \
x(NT_IDENTIFIER_LIST_) \
x(NT_OPTIONAL_STATEMENTS) \
x(NT_PARAMETER_LIST) \
x(NT_PARAMETER_LIST_) \
x(NT_PROCEDURE_STATEMENT) \
x(NT_PROCEDURE_STATEMENT_) \
x(NT_PROGRAM) \
x(NT_PROGRAM_) \
x(NT_PROGRAM__) \
x(NT_SIGN) \
x(NT_SIMPLE_EXPRESSION) \
x(NT_SIMPLE_EXPRESSION_) \
x(NT_STANDARD_TYPE) \
x(NT_STATEMENT) \
x(NT_STATEMENT_) \
x(NT_STATEMENT_LIST) \
x(NT_STATEMENT_LIST_) \
x(NT_SUBPROGRAM_DECLARATION) \
x(NT_SUBPROGRAM_DECLARATIONS) \
x(NT_SUBPROGRAM_DECLARATIONS_) \
x(NT_SUBPROGRAM_DECLARATION_) \
x(NT_SUBPROGRAM_DECLARATION__) \
x(NT_SUBPROGRAM_HEAD) \
x(NT_SUBPROGRAM_HEAD_) \
x(NT_TERM) \
x(NT_TERM_) \
x(NT_TYPE) \
x(NT_VARIABLE) \
x(NT_VARIABLE_)

#define x(a) a,
typedef enum {
	NONTERMS
} NonTerminal;
#undef x
#define x(a) case a: return #a;
char* ntToString(NonTerminal nt) {
	switch(nt) {
		NONTERMS
	}
	return "";
}
#undef x
void parse(void);
void consume(NonTerminal, Item*);
int match(int, Item*);
void synch(NonTerminal);
void synerr(int*, int, Terminal);
void lexerr(Terminal);
void semerr(char*);
Terminal nextTerminal(void);

Terminal currTerm;
FILE *fSrc, *fTree, *fList, *fTable;
char sLine[90] = {0};
char *psLine;
int cLine;
int cColumn;

int retCode = 0;

void parse() {
	NonTerminal top_nonterminal = NT_PROGRAM;
	currTerm = nextTerminal();
	Item *a0 = malloc(sizeof(Item));
	consume(top_nonterminal, a0);

	if(a0->error) retCode |= RET_TYPERR;
	printTable(fTable, root, 0, 0);
}

void consume(NonTerminal nt, Item *a0) {
	depth++;
	Item *a1 = malloc(sizeof(Item));
	Item *a2 = malloc(sizeof(Item));
	Item *a3 = malloc(sizeof(Item));
	Item *a4 = malloc(sizeof(Item));
	Item *a5 = malloc(sizeof(Item));
	Item *a6 = malloc(sizeof(Item));
	Item *a7 = malloc(sizeof(Item));
	Item *a8 = malloc(sizeof(Item));

	switch(nt) {
		case NT_ARGUMENTS:
			switch(currTerm.type) {
				case T_LPAREN:
					match(T_LPAREN, a1); if(a1->error) goto nt_arguments_synch;
					consume(NT_PARAMETER_LIST, a2);
					match(T_RPAREN, a3); if(a3->error) goto nt_arguments_synch;
					break;
				default:
					synerr((int[]){T_LPAREN}, 1, currTerm);
				nt_arguments_synch:
					synch(nt);
					break;
			}
			break;
		case NT_COMPOUND_STATEMENT:
			switch(currTerm.type) {
				case T_BEGIN:
					match(T_BEGIN, a1); if(a1->error) goto nt_compound_statement_synch;
					consume(NT_COMPOUND_STATEMENT_, a2);
					break;
				default:
					synerr((int[]){T_BEGIN}, 1, currTerm);
				nt_compound_statement_synch:
					synch(nt);
					break;
			}
			break;
		case NT_COMPOUND_STATEMENT_:
			switch(currTerm.type) {
				case T_BEGIN:
				case T_CALL:
				case T_ID:
				case T_IF:
				case T_WHILE:
					consume(NT_OPTIONAL_STATEMENTS, a1);
					match(T_END, a2); if(a2->error) goto nt_compound_statement__synch;
					break;
				case T_END:
					match(T_END, a1); if(a1->error) goto nt_compound_statement__synch;
					break;
				default:
					synerr((int[]){T_ID, T_IF, T_WHILE, T_BEGIN, T_CALL, T_END}, 6, currTerm);
				nt_compound_statement__synch:
					synch(nt);
					break;
			}
			break;
		case NT_DECLARATIONS:
			switch(currTerm.type) {
				case T_VAR:
					match(T_VAR, a1); if(a1->error) goto nt_declarations_synch;
					match(T_ID, a2); if(a2->error) goto nt_declarations_synch;
					match(T_COLON, a3); if(a3->error) goto nt_declarations_synch;
					consume(NT_TYPE, a4);
					addVariable(a2->lexeme, a4->type);
					match(T_SEMICOLON, a5); if(a5->error) goto nt_declarations_synch;
					consume(NT_DECLARATIONS_, a6);
					break;
				default:
					synerr((int[]){T_VAR}, 1, currTerm);
				nt_declarations_synch:
					synch(nt);
					break;
			}
			break;
		case NT_DECLARATIONS_:
			switch(currTerm.type) {
				case T_BEGIN:
				case T_PROCEDURE:
				case T_SEMICOLON:
					break;
				case T_VAR:
					match(T_VAR, a1); if(a1->error) goto nt_declarations__synch;
					match(T_ID, a2); if(a2->error) goto nt_declarations__synch;
					match(T_COLON, a3); if(a3->error) goto nt_declarations__synch;
					consume(NT_TYPE, a4);
					addVariable(a2->lexeme, a4->type);
					match(T_SEMICOLON, a5); if(a5->error) goto nt_declarations__synch;
					consume(NT_DECLARATIONS_, a6);
					break;
				default:
					synerr((int[]){T_BEGIN, T_VAR, T_PROCEDURE, T_SEMICOLON}, 4, currTerm);
				nt_declarations__synch:
					synch(nt);
					break;
			}
			break;
		case NT_EXPRESSION:
			switch(currTerm.type) {
				case T_ID:
				case T_LPAREN:
				case T_MINUS:
				case T_NOT:
				case T_NUM:
				case T_PLUS:
					consume(NT_SIMPLE_EXPRESSION, a1);
					a2->in.type = a1->type;
					consume(NT_EXPRESSION_, a2);
					a0->type = a2->type;
					break;
				default:
					synerr((int[]){T_ID, T_LPAREN, T_NUM, T_PLUS, T_MINUS, T_NOT}, 6, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_EXPRESSION_:
			switch(currTerm.type) {
				case T_COMMA:
				case T_DO:
				case T_ELSE:
				case T_END:
				case T_RBRACK:
				case T_RPAREN:
				case T_SEMICOLON:
				case T_THEN:
					a0->type = a0->in.type;
					break;
				case T_RELOP:
					match(T_RELOP, a1); if(a1->error) goto nt_expression__synch;
					consume(NT_SIMPLE_EXPRESSION, a2);

					if(typeEqual(a2->type, a0->in.type))
						a0->type.st_type = INT;
					else {
						a0->errHere = true;
						semerr("type mismatch");
					}
					break;
				default:
					synerr((int[]){T_THEN, T_ELSE, T_COMMA, T_SEMICOLON, T_RBRACK, T_DO, T_RPAREN, T_RELOP, T_END}, 9, currTerm);
				nt_expression__synch:
					synch(nt);
					break;
			}
			break;
		case NT_EXPRESSION_LIST:
			switch(currTerm.type) {
				case T_ID:
				case T_LPAREN:
				case T_MINUS:
				case T_NOT:
				case T_NUM:
				case T_PLUS:
					consume(NT_EXPRESSION, a1);
					a2->in.count = 1;
					a2->in.proc = a0->in.proc;
					consume(NT_EXPRESSION_LIST_, a2);
					
					if(!nthParamOfProc(a0->in.proc, 0)) {
						a0->errHere = true;
						semerr("argument count mismatch at first parameter");
					} else if(!typeEqual(nthParamOfProc(a0->in.proc, 0)->type, a1->type)) {
						a0->errHere = true;
						semerrf("type mismatch, parameter %d; formal type %s; received type %s", a0->in.count, typeToString(nthParamOfProc(a0->in.proc, a0->in.count)->type), typeToString(a1->type));
					}
					break;
				default:
					synerr((int[]){T_ID, T_LPAREN, T_NUM, T_PLUS, T_MINUS, T_NOT}, 6, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_EXPRESSION_LIST_:
			switch(currTerm.type) {
				case T_COMMA:
					match(T_COMMA, a1); if(a1->error) goto nt_expression_list__synch;
					consume(NT_EXPRESSION, a2);
					a3->in.count = a0->in.count + 1;
					a3->in.proc = a0->in.proc;
					consume(NT_EXPRESSION_LIST_, a3);

					if(!nthParamOfProc(a0->in.proc, a0->in.count)) {
						a0->errHere = true;
						semerrf("Argument count mismatch while calling procedure %s", a0->in.proc);
					} else if(!typeEqual(nthParamOfProc(a0->in.proc, a0->in.count)->type, a2->type)) {
						a0->errHere = true;
						semerrf("Type mismatch while calling procedure %s: argument %d, formal param type %s, actual param type %s\n", a0->in.proc, a0->in.count, typeToString(nthParamOfProc(a0->in.proc, a0->in.count)->type), typeToString(a2->type));
					}
					break;
				case T_RPAREN:
					if(nthParamOfProc(a0->in.proc, a0->in.count)) {
						a0->errHere = true;
						semerrf("Argument count mismatch while calling procedure %s: argument %d, formal param exists; did not pass actual param\n", a0->in.proc, a0->in.count);
					}
					break;
				default:
					synerr((int[]){T_COMMA, T_RPAREN}, 2, currTerm);
				nt_expression_list__synch:
					synch(nt);
					break;
			}
			break;
		case NT_FACTOR:
			switch(currTerm.type) {
				case T_ID:
					match(T_ID, a1); if(a1->error) goto nt_factor_synch;
					if(checkSymbolTable(a1->lexeme, false)) {
						a2->in.type = checkSymbolTable(a1->lexeme, false)->type;
						consume(NT_FACTOR_, a2);
						a0->type = a2->type;
					} else {
						a0->errHere = true;
						semerrf("Unknown symbol: %s", a1->lexeme);
					}
					break;
				case T_LPAREN:
					match(T_LPAREN, a1); if(a1->error) goto nt_factor_synch;
					consume(NT_EXPRESSION, a2);
					match(T_RPAREN, a3); if(a3->error) goto nt_factor_synch;
					a0->type = a2->type;
					break;
				case T_NOT:
					match(T_NOT, a1); if(a1->error) goto nt_factor_synch;
					consume(NT_FACTOR, a2);
					if(a2->type.st_type == INT)
						a0->type = a2->type;
					else
						a0->errHere = true;
					break;
				case T_NUM:
					match(T_NUM, a1); if(a1->error) goto nt_factor_synch;
					a0->type = a1->type;
					break;
				default:
					synerr((int[]){T_ID, T_NUM, T_LPAREN, T_NOT}, 4, currTerm);
				nt_factor_synch:
					synch(nt);
					break;
			}
			break;
		case NT_FACTOR_:
			switch(currTerm.type) {
				case T_ADDOP:
				case T_COMMA:
				case T_DO:
				case T_ELSE:
				case T_END:
				case T_MULOP:
				case T_RBRACK:
				case T_RELOP:
				case T_RPAREN:
				case T_SEMICOLON:
				case T_THEN:
					a0->type = a0->in.type;
					break;
				case T_LBRACK:
					match(T_LBRACK, a1); if(a1->error) goto nt_factor__synch;
					consume(NT_EXPRESSION, a2);
					match(T_RBRACK, a3); if(a3->error) goto nt_factor__synch;
					if(a2->type.st_type == INT && a0->in.type.isArray) {
						a0->type = a0->in.type;
						a0->type.isArray = false;
					} else
						a0->errHere = true;
					break;
				default:
					synerr((int[]){T_THEN, T_ELSE, T_COMMA, T_SEMICOLON, T_LBRACK, T_ADDOP, T_DO, T_RPAREN, T_RELOP, T_MULOP, T_RBRACK, T_END}, 12, currTerm);
				nt_factor__synch:
					synch(nt);
					break;
			}
			break;
		case NT_IDENTIFIER_LIST:
			switch(currTerm.type) {
				case T_ID:
					match(T_ID, a1); if(a1->error) goto nt_identifier_list_synch;
					addVariable(a1->lexeme, (Type) {.st_type = PGPARM});
					consume(NT_IDENTIFIER_LIST_, a2);
					break;
				default:
					synerr((int[]){T_ID}, 1, currTerm);
				nt_identifier_list_synch:
					synch(nt);
					break;
			}
			break;
		case NT_IDENTIFIER_LIST_:
			switch(currTerm.type) {
				case T_COMMA:
					match(T_COMMA, a1); if(a1->error) goto nt_identifier_list__synch;
					match(T_ID, a2); if(a2->error) goto nt_identifier_list__synch;
					addVariable(a2->lexeme, (Type) {.st_type = PGPARM});
					consume(NT_IDENTIFIER_LIST_, a3);
					break;
				case T_RPAREN:
					break;
				default:
					synerr((int[]){T_COMMA, T_RPAREN}, 2, currTerm);
				nt_identifier_list__synch:
					synch(nt);
					break;
			}
			break;
		case NT_OPTIONAL_STATEMENTS:
			switch(currTerm.type) {
				case T_BEGIN:
				case T_CALL:
				case T_ID:
				case T_IF:
				case T_WHILE:
					consume(NT_STATEMENT_LIST, a1);
					break;
				default:
					synerr((int[]){T_ID, T_BEGIN, T_IF, T_WHILE, T_CALL}, 5, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_PARAMETER_LIST:
			switch(currTerm.type) {
				case T_ID:
					match(T_ID, a1); if(a1->error) goto nt_parameter_list_synch;
					match(T_COLON, a2); if(a2->error) goto nt_parameter_list_synch;
					consume(NT_TYPE, a3);

					addVariable(a1->lexeme, makeParameterType(a3->type));

					consume(NT_PARAMETER_LIST_, a4);
					break;
				default:
					synerr((int[]){T_ID}, 1, currTerm);
				nt_parameter_list_synch:
					synch(nt);
					break;
			}
			break;
		case NT_PARAMETER_LIST_:
			switch(currTerm.type) {
				case T_RPAREN:
					break;
				case T_SEMICOLON:
					match(T_SEMICOLON, a1); if(a1->error) goto nt_parameter_list__synch;
					match(T_ID, a2); if(a2->error) goto nt_parameter_list__synch;
					match(T_COLON, a3); if(a3->error) goto nt_parameter_list__synch;
					consume(NT_TYPE, a4);

					addVariable(a2->lexeme, makeParameterType(a4->type));

					consume(NT_PARAMETER_LIST_, a5);
					break;
				default:
					synerr((int[]){T_RPAREN, T_SEMICOLON}, 2, currTerm);
				nt_parameter_list__synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROCEDURE_STATEMENT:
			switch(currTerm.type) {
				case T_CALL:
					match(T_CALL, a1); if(a1->error) goto nt_procedure_statement_synch;
					match(T_ID, a2); if(a2->error) goto nt_procedure_statement_synch;
					a3->in.count = 0;
					a3->in.proc = a2->lexeme;
					consume(NT_PROCEDURE_STATEMENT_, a3);

					if(!checkSymbolTable(a2->lexeme, false) || checkSymbolTable(a2->lexeme, false)->type.st_type != PROCNAME) {
						a0->errHere = true;
						semerrf("Unknown symbol: %s", a2->lexeme);
					}
					break;
				default:
					synerr((int[]){T_CALL}, 1, currTerm);
				nt_procedure_statement_synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROCEDURE_STATEMENT_:
			switch(currTerm.type) {
				case T_ELSE:
				case T_END:
				case T_SEMICOLON:
					if(nthParamOfProc(a0->in.proc, a0->in.count)) {
						semerr("Too many parameters");
					}
					break;
				case T_LPAREN:
					match(T_LPAREN, a1); if(a1->error) goto nt_procedure_statement__synch;
					a2->in.count = a0->in.count;
					a2->in.proc = a0->in.proc;
					consume(NT_EXPRESSION_LIST, a2);
					match(T_RPAREN, a3); if(a3->error) goto nt_procedure_statement__synch;
					break;
				default:
					synerr((int[]){T_ELSE, T_LPAREN, T_SEMICOLON, T_END}, 4, currTerm);
				nt_procedure_statement__synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROGRAM:
			switch(currTerm.type) {
				case T_PROGRAM:
					match(T_PROGRAM, a1); if(a1->error) goto nt_program_synch;
					match(T_ID, a2); if(a2->error) goto nt_program_synch;
					enterProcedure(a2->lexeme, (Type) {.st_type = PGNAME});
					match(T_LPAREN, a3); if(a3->error) goto nt_program_synch;
					consume(NT_IDENTIFIER_LIST, a4);
					match(T_RPAREN, a5); if(a5->error) goto nt_program_synch;
					match(T_SEMICOLON, a6); if(a6->error) goto nt_program_synch;
					consume(NT_PROGRAM__, a7);
					exitProcedure();
					break;
				default:
					synerr((int[]){T_PROGRAM}, 1, currTerm);
				nt_program_synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROGRAM_:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT, a1);
					match(T_PERIOD, a2); if(a2->error) goto nt_program__synch;
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, a1);
					consume(NT_COMPOUND_STATEMENT, a2);
					match(T_PERIOD, a3); if(a3->error) goto nt_program__synch;
					break;
				default:
					synerr((int[]){T_BEGIN, T_PROCEDURE}, 2, currTerm);
				nt_program__synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROGRAM__:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT, a1);
					match(T_PERIOD, a2); if(a2->error) goto nt_program___synch;
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, a1);
					consume(NT_COMPOUND_STATEMENT, a2);
					match(T_PERIOD, a3); if(a3->error) goto nt_program___synch;
					break;
				case T_VAR:
					consume(NT_DECLARATIONS, a1);
					consume(NT_PROGRAM_, a2);
					break;
				default:
					synerr((int[]){T_BEGIN, T_VAR, T_PROCEDURE}, 3, currTerm);
				nt_program___synch:
					synch(nt);
					break;
			}
			break;
		case NT_SIGN:
			switch(currTerm.type) {
				case T_MINUS:
					match(T_MINUS, a1); if(a1->error) goto nt_sign_synch;
					break;
				case T_PLUS:
					match(T_PLUS, a1); if(a1->error) goto nt_sign_synch;
					break;
				default:
					synerr((int[]){T_PLUS, T_MINUS}, 2, currTerm);
				nt_sign_synch:
					synch(nt);
					break;
			}
			break;
		case NT_SIMPLE_EXPRESSION:
			switch(currTerm.type) {
				case T_ID:
				case T_LPAREN:
				case T_NOT:
				case T_NUM:
					consume(NT_TERM, a1);
					consume(NT_SIMPLE_EXPRESSION_, a2);
					if(a2->type.st_type == NONE) {
						a0->type = a1->type;
					} else if(a1->type.st_type == INT && a2->type.st_type == INT) {
						a0->type.st_type = INT;
					} else {
						a0->type.st_type = REAL;
					}
					break;
				case T_MINUS:
				case T_PLUS:
					consume(NT_SIGN, a1);
					consume(NT_TERM, a2);
					consume(NT_SIMPLE_EXPRESSION_, a3);
					if(a2->type.st_type == INT && a3->type.st_type == INT)
						a0->type.st_type = INT;
					else
						a0->type.st_type = REAL;
					break;
				default:
					synerr((int[]){T_ID, T_LPAREN, T_NUM, T_PLUS, T_MINUS, T_NOT}, 6, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_SIMPLE_EXPRESSION_:
			switch(currTerm.type) {
				case T_ADDOP:
					match(T_ADDOP, a1); if(a1->error) goto nt_simple_expression__synch;
					consume(NT_TERM, a2);
					consume(NT_SIMPLE_EXPRESSION_, a3);
					if(a2->type.st_type == INT && (a3->type.st_type == INT || a3->type.st_type == NONE))
						a0->type.st_type = INT;
					else
						a0->type.st_type = REAL;
					break;
				case T_COMMA:
				case T_DO:
				case T_ELSE:
				case T_END:
				case T_RBRACK:
				case T_RELOP:
				case T_RPAREN:
				case T_SEMICOLON:
				case T_THEN:
					a0->type.st_type = NONE;
					break;
				default:
					synerr((int[]){T_THEN, T_ELSE, T_COMMA, T_SEMICOLON, T_ADDOP, T_DO, T_RPAREN, T_RELOP, T_RBRACK, T_END}, 10, currTerm);
				nt_simple_expression__synch:
					synch(nt);
					break;
			}
			break;
		case NT_STANDARD_TYPE:
			switch(currTerm.type) {
				case T_INTEGER:
					match(T_INTEGER, a1); if(a1->error) goto nt_standard_type_synch;
					a0->type.st_type = INT;
					break;
				case T_REAL:
					match(T_REAL, a1); if(a1->error) goto nt_standard_type_synch;
					a0->type.st_type = REAL;
					break;
				default:
					synerr((int[]){T_INTEGER, T_REAL}, 2, currTerm);
				nt_standard_type_synch:
					synch(nt);
					break;
			}
			break;
		case NT_STATEMENT:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT, a1);
					break;
				case T_CALL:
					consume(NT_PROCEDURE_STATEMENT, a1);
					break;
				case T_ID:
					consume(NT_VARIABLE, a1);
					match(T_ASSIGNOP, a2); if(a2->error) goto nt_statement_synch;
					consume(NT_EXPRESSION, a3);
					if(!typeEqual(a1->type, a3->type)) {
						a0->errHere = true;
						semerrf("Type mismatch on an assignment: tried assigning %s to %s", typeToString(a3->type), typeToString(a1->type));
					}
					break;
				case T_IF:
					match(T_IF, a1); if(a1->error) goto nt_statement_synch;
					consume(NT_EXPRESSION, a2);
					if(a2->type.st_type != INT) {
						a0->errHere = true;
						semerr("Type mismatch on an if statement");
					}
					match(T_THEN, a3); if(a3->error) goto nt_statement_synch;
					consume(NT_STATEMENT, a4);
					consume(NT_STATEMENT_, a5);
					break;
				case T_WHILE:
					match(T_WHILE, a1); if(a1->error) goto nt_statement_synch;
					consume(NT_EXPRESSION, a2);
					if(a2->type.st_type != INT)
						a0->errHere = true;
					match(T_DO, a3); if(a3->error) goto nt_statement_synch;
					consume(NT_STATEMENT, a4);
					break;
				default:
					synerr((int[]){T_ID, T_BEGIN, T_IF, T_WHILE, T_CALL}, 5, currTerm);
				nt_statement_synch:
					synch(nt);
					break;
			}
			break;
		case NT_STATEMENT_:
			switch(currTerm.type) {
				case T_ELSE:
					match(T_ELSE, a1); if(a1->error) goto nt_statement__synch;
					consume(NT_STATEMENT, a2);
					break;
				case T_END:
				case T_SEMICOLON:
					break;
				default:
					synerr((int[]){T_ELSE, T_SEMICOLON, T_END}, 3, currTerm);
				nt_statement__synch:
					synch(nt);
					break;
			}
			break;
		case NT_STATEMENT_LIST:
			switch(currTerm.type) {
				case T_BEGIN:
				case T_CALL:
				case T_ID:
				case T_IF:
				case T_WHILE:
					consume(NT_STATEMENT, a1);
					consume(NT_STATEMENT_LIST_, a2);
					break;
				default:
					synerr((int[]){T_ID, T_BEGIN, T_IF, T_WHILE, T_CALL}, 5, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_STATEMENT_LIST_:
			switch(currTerm.type) {
				case T_END:
					break;
				case T_SEMICOLON:
					match(T_SEMICOLON, a1); if(a1->error) goto nt_statement_list__synch;
					consume(NT_STATEMENT, a2);
					consume(NT_STATEMENT_LIST_, a3);
					break;
				default:
					synerr((int[]){T_SEMICOLON, T_END}, 2, currTerm);
				nt_statement_list__synch:
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATION:
			switch(currTerm.type) {
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_HEAD, a1);
					consume(NT_SUBPROGRAM_DECLARATION__, a2);
					exitProcedure();
					break;
				default:
					synerr((int[]){T_PROCEDURE}, 1, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATIONS:
			switch(currTerm.type) {
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATION, a1);
					match(T_SEMICOLON, a2); if(a2->error) goto nt_subprogram_declarations_synch;
					consume(NT_SUBPROGRAM_DECLARATIONS_, a3);
					break;
				default:
					synerr((int[]){T_PROCEDURE}, 1, currTerm);
				nt_subprogram_declarations_synch:
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATIONS_:
			switch(currTerm.type) {
				case T_BEGIN:
				case T_SEMICOLON:
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATION, a1);
					match(T_SEMICOLON, a2); if(a2->error) goto nt_subprogram_declarations__synch;
					consume(NT_SUBPROGRAM_DECLARATIONS_, a3);
					break;
				default:
					synerr((int[]){T_BEGIN, T_SEMICOLON, T_PROCEDURE}, 3, currTerm);
				nt_subprogram_declarations__synch:
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATION_:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT, a1);
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, a1);
					consume(NT_COMPOUND_STATEMENT, a2);
					break;
				default:
					synerr((int[]){T_BEGIN, T_PROCEDURE}, 2, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATION__:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT, a1);
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, a1);
					consume(NT_COMPOUND_STATEMENT, a2);
					break;
				case T_VAR:
					consume(NT_DECLARATIONS, a1);
					consume(NT_SUBPROGRAM_DECLARATION_, a2);
					break;
				default:
					synerr((int[]){T_BEGIN, T_VAR, T_PROCEDURE}, 3, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_HEAD:
			switch(currTerm.type) {
				case T_PROCEDURE:
					match(T_PROCEDURE, a1); if(a1->error) goto nt_subprogram_head_synch;
					match(T_ID, a2); if(a2->error) goto nt_subprogram_head_synch;
					enterProcedure(a2->lexeme, (Type) {.st_type = PROCNAME});
					consume(NT_SUBPROGRAM_HEAD_, a3);
					break;
				default:
					synerr((int[]){T_PROCEDURE}, 1, currTerm);
				nt_subprogram_head_synch:
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_HEAD_:
			switch(currTerm.type) {
				case T_LPAREN:
					consume(NT_ARGUMENTS, a1);
					match(T_SEMICOLON, a2); if(a2->error) goto nt_subprogram_head__synch;
					break;
				case T_SEMICOLON:
					match(T_SEMICOLON, a1); if(a1->error) goto nt_subprogram_head__synch;
					break;
				default:
					synerr((int[]){T_LPAREN, T_SEMICOLON}, 2, currTerm);
				nt_subprogram_head__synch:
					synch(nt);
					break;
			}
			break;
		case NT_TERM:
			switch(currTerm.type) {
				case T_ID:
				case T_LPAREN:
				case T_NOT:
				case T_NUM:
					consume(NT_FACTOR, a1);
					consume(NT_TERM_, a2);
					if(a2->type.st_type == NONE) {
						a0->type = a1->type;
					} else if(a1->type.st_type == INT && a2->type.st_type == INT) {
						a0->type.st_type = INT;
					} else {
						a0->type.st_type = REAL;
					}
					break;
				default:
					synerr((int[]){T_ID, T_NUM, T_LPAREN, T_NOT}, 4, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_TERM_:
			switch(currTerm.type) {
				case T_ADDOP:
				case T_COMMA:
				case T_DO:
				case T_ELSE:
				case T_END:
				case T_RBRACK:
				case T_RELOP:
				case T_RPAREN:
				case T_SEMICOLON:
				case T_THEN:
					a0->type.st_type = NONE;
					break;
				case T_MULOP:
					match(T_MULOP, a1); if(a1->error) goto nt_term__synch;
					consume(NT_FACTOR, a2);
					consume(NT_TERM_, a3);
					if(a2->type.st_type == INT && (a3->type.st_type == INT || a3->type.st_type == NONE))
						a0->type.st_type = INT;
					else
						a0->type.st_type = REAL;
					break;
				default:
					synerr((int[]){T_THEN, T_ELSE, T_COMMA, T_SEMICOLON, T_ADDOP, T_DO, T_RPAREN, T_MULOP, T_RELOP, T_RBRACK, T_END}, 11, currTerm);
				nt_term__synch:
					synch(nt);
					break;
			}
			break;
		case NT_TYPE:
			switch(currTerm.type) {
				case T_ARRAY:
					match(T_ARRAY, a1); if(a1->error) goto nt_type_synch;
					match(T_LBRACK, a2); if(a2->error) goto nt_type_synch;
					match(T_NUM, a3); if(a3->error) goto nt_type_synch;
					match(T_DOUBLEPERIOD, a4); if(a4->error) goto nt_type_synch;
					match(T_NUM, a5); if(a5->error) goto nt_type_synch;
					match(T_RBRACK, a6); if(a6->error) goto nt_type_synch;
					match(T_OF, a7); if(a7->error) goto nt_type_synch;
					consume(NT_STANDARD_TYPE, a8);
					if(a3->type.st_type == INT && a5->type.st_type == INT) {
						a0->type = a8->type;
						a0->type.isArray = true;
						a0->type.low = atoi(a3->lexeme);
						a0->type.high = atoi(a5->lexeme);
						if(a0->type.high < a0->type.low) {
							a0->errHere = true;
							semerr("Array bounds should be nondecreasing.");
						}
					} else {
						a0->errHere = true;
						semerr("Array bounds must be integer.");
					}
					break;
				case T_INTEGER:
				case T_REAL:
					consume(NT_STANDARD_TYPE, a1);
					a0->type = a1->type;
					break;
				default:
					synerr((int[]){T_ARRAY, T_INTEGER, T_REAL}, 3, currTerm);
				nt_type_synch:
					synch(nt);
					break;
			}
			break;
		case NT_VARIABLE:
			switch(currTerm.type) {
				case T_ID:
					match(T_ID, a1); if(a1->error) goto nt_variable_synch;
					if(checkSymbolTable(a1->lexeme, false)) {
						a2->in.type = checkSymbolTable(a1->lexeme, false)->type;
						consume(NT_VARIABLE_, a2);
						a0->type = a2->type;
					} else {
						a0->errHere = true;
						semerrf("Unknown symbol: %s", a1->lexeme);
					}
					break;
				default:
					synerr((int[]){T_ID}, 1, currTerm);
				nt_variable_synch:
					synch(nt);
					break;
			}
			break;
		case NT_VARIABLE_:
			switch(currTerm.type) {
				case T_ASSIGNOP:
					a0->type = a0->in.type;
					break;
				case T_LBRACK:
					match(T_LBRACK, a1); if(a1->error) goto nt_variable__synch;
					consume(NT_EXPRESSION, a2);
					match(T_RBRACK, a3); if(a3->error) goto nt_variable__synch;
					a0->type = a0->in.type;
					a0->type.isArray = false;
					break;
				default:
					synerr((int[]){T_LBRACK, T_ASSIGNOP}, 2, currTerm);
				nt_variable__synch:
					synch(nt);
					break;
			}
			break;

	}
	for(int i = 0; i < depth; i++) fprintf(fTree, " ");
	fprintf(fTree, "%s", ntToString(nt));
	fprintf(fTree, " %s", typeToString(a0->type));
	if(a0->error) fprintf(fTree, " ERR");
	if(a0->errHere) fprintf(fTree, " ERR*");
	fprintf(fTree, "\n");
	depth--;
	if(a1->errHere || a1->error || a2->errHere || a2->error || a3->errHere || a3->error || a4->errHere || a4->error || a5->errHere || a5->error || a6->errHere || a6->error || a7->errHere || a7->error || a8->errHere || a8->error) {
		a0->error = true;
	}

}

void synch(NonTerminal nt) {
	int *synchSet;
	int len;
	switch(nt) {
		case NT_EXPRESSION_:
		case NT_EXPRESSION:
			synchSet = (int[]){T_THEN, T_EOF, T_COMMA, T_SEMICOLON, T_RBRACK, T_DO, T_ELSE, T_RPAREN, T_END}; len = 9;
			break;
		case NT_PROCEDURE_STATEMENT:
		case NT_PROCEDURE_STATEMENT_:
		case NT_STATEMENT:
		case NT_STATEMENT_:
			synchSet = (int[]){T_EOF, T_END, T_ELSE, T_SEMICOLON}; len = 4;
			break;
		case NT_FACTOR_:
		case NT_FACTOR:
			synchSet = (int[]){T_THEN, T_EOF, T_COMMA, T_SEMICOLON, T_ADDOP, T_DO, T_RPAREN, T_ELSE, T_RELOP, T_MULOP, T_RBRACK, T_END}; len = 12;
			break;
		case NT_PARAMETER_LIST:
		case NT_IDENTIFIER_LIST:
		case NT_EXPRESSION_LIST_:
		case NT_PARAMETER_LIST_:
		case NT_IDENTIFIER_LIST_:
		case NT_EXPRESSION_LIST:
			synchSet = (int[]){T_EOF, T_RPAREN}; len = 2;
			break;
		case NT_PROGRAM:
		case NT_PROGRAM__:
		case NT_PROGRAM_:
			synchSet = (int[]){T_EOF}; len = 1;
			break;
		case NT_SIMPLE_EXPRESSION_:
		case NT_SIMPLE_EXPRESSION:
			synchSet = (int[]){T_THEN, T_EOF, T_COMMA, T_SEMICOLON, T_RBRACK, T_DO, T_ELSE, T_RPAREN, T_RELOP, T_END}; len = 10;
			break;
		case NT_SIGN:
			synchSet = (int[]){T_ID, T_NUM, T_EOF, T_LPAREN, T_NOT}; len = 5;
			break;
		case NT_SUBPROGRAM_HEAD:
		case NT_SUBPROGRAM_HEAD_:
			synchSet = (int[]){T_BEGIN, T_EOF, T_VAR, T_SEMICOLON, T_PROCEDURE}; len = 5;
			break;
		case NT_TERM:
		case NT_TERM_:
			synchSet = (int[]){T_THEN, T_EOF, T_COMMA, T_SEMICOLON, T_ADDOP, T_RBRACK, T_DO, T_ELSE, T_RPAREN, T_RELOP, T_END}; len = 11;
			break;
		case NT_SUBPROGRAM_DECLARATION:
		case NT_SUBPROGRAM_DECLARATION__:
		case NT_ARGUMENTS:
		case NT_SUBPROGRAM_DECLARATION_:
			synchSet = (int[]){T_EOF, T_SEMICOLON}; len = 2;
			break;
		case NT_OPTIONAL_STATEMENTS:
		case NT_STATEMENT_LIST_:
		case NT_STATEMENT_LIST:
			synchSet = (int[]){T_EOF, T_END}; len = 2;
			break;
		case NT_COMPOUND_STATEMENT_:
		case NT_COMPOUND_STATEMENT:
			synchSet = (int[]){T_PERIOD, T_EOF, T_END, T_ELSE, T_SEMICOLON}; len = 5;
			break;
		case NT_DECLARATIONS:
		case NT_DECLARATIONS_:
			synchSet = (int[]){T_BEGIN, T_EOF, T_SEMICOLON, T_PROCEDURE}; len = 4;
			break;
		case NT_TYPE:
		case NT_STANDARD_TYPE:
			synchSet = (int[]){T_EOF, T_SEMICOLON, T_RPAREN}; len = 3;
			break;
		case NT_SUBPROGRAM_DECLARATIONS_:
		case NT_SUBPROGRAM_DECLARATIONS:
			synchSet = (int[]){T_BEGIN, T_EOF, T_SEMICOLON}; len = 3;
			break;
		case NT_VARIABLE:
		case NT_VARIABLE_:
			synchSet = (int[]){T_EOF, T_ASSIGNOP}; len = 2;
			break;

	}
	while(true) {
		for(int i = 0; i < len; i++) {
			if(currTerm.type == synchSet[i]) {
				char* lexPrint = currTerm.lexeme;
				if(currTerm.type == T_EOF) {
					fprintf(stderr, "Reached end-of-file; terminating\n");
					exit(1);
				} else {
					fprintf(stderr, "Synchronized on term \"%s\" (%s)\n", lexPrint, convertConstantToString(currTerm.type));
					return;
				}
			}
		}
		currTerm = nextTerminal();
	}
}

int match(int termtype, Item *a0) {
	if(currTerm.type == termtype) {
		for(int i = 0; i < depth+1; i++) fprintf(fTree, " ");
		fprintf(fTree, "\"%s\"\n", currTerm.lexeme);
		for(int i = 0; i < depth; i++) fprintf(fTree, " ");
		fprintf(fTree, "%s\n", convertConstantToString(currTerm.type));

		a0->lexeme = currTerm.lexeme;
		if(currTerm.type == T_NUM) {
			switch(currTerm.attribute) {
				case NUM_INT:
					a0->type.st_type = INT;
					break;
				case NUM_REAL:
				case NUM_LONGREAL:
					a0->type.st_type = REAL;
					break;
			}
		}
		currTerm = nextTerminal();
		return true;
	}
	synerr((int[]){termtype}, 1, currTerm);
	a0->error = true;
	return false;
}

void synerr(int *expected, int expLen, Terminal encountered) {
	char *str = " one of";
	if(expLen == 1) str = "";
	fprintf(fList, "Syntax error, column %d: Unexpected %s; expected%s: ", cColumn, convertConstantToString(encountered.type), str);
	for(int i = 0; i < expLen - 1; i++) {
		fprintf(fList, "%s, ", convertConstantToString(expected[i]));
	}
	fprintf(fList, "%s\n", convertConstantToString(expected[expLen-1]));
	fprintf(stderr, "Syntax error, line %d, column %d: Unexpected %s; expected%s: ", cLine, cColumn, convertConstantToString(encountered.type), str);
	for(int i = 0; i < expLen - 1; i++) {
		fprintf(stderr, "%s, ", convertConstantToString(expected[i]));
	}
	fprintf(stderr, "%s\n", convertConstantToString(expected[expLen-1]));

	retCode |= RET_SYNERR;
}

int main(int argc, char** argv) {
	if(argc < 2) exit(2);
	char *sfSrc = argv[1];
	fSrc = fopen(sfSrc, "r");
	char sfTree[80];
	strcpy(sfTree, sfSrc);
	strcpy(sfTree + strlen(sfSrc) - 4, ".tree");
	fTree = fopen(sfTree, "w");
	char sfList[80];
	strcpy(sfList, sfSrc);
	strcpy(sfList + strlen(sfSrc) - 4, ".lst");
	fList = fopen(sfList, "w");
	char sfTable[80];
	strcpy(sfTable, sfSrc);
	strcpy(sfTable + strlen(sfSrc) - 4, ".tbl");
	fTable = fopen(sfTable, "w");

	cLine = 1;
	cColumn = 0;
	machinesInit("data/reserved-words.txt");
	parse();
	fclose(fSrc);
	fclose(fTree);
	fclose(fList);
	fclose(fTable);

	if(retCode) fprintf(stderr, "\n");
	if(retCode & RET_LEXERR) {
		fprintf(stderr, "Lexical errors encountered.\n");
		//remove(sfList);
	}
	if(retCode & RET_SYNERR) {
		fprintf(stderr, "Syntax errors encountered.\n");
		//remove(sfTree);
	}
	if(retCode & RET_SEMERR) {
		fprintf(stderr, "Semantic errors encountered.\n");
		//remove(sfTable);
	}
	if(retCode & RET_TYPERR) {
		fprintf(stderr, "Type errors encountered.\n");
	}
	return retCode;
}

Terminal nextTerminal() {
	if(!psLine) psLine = sLine;
	if(!*psLine) {
		fgets(sLine, sizeof(sLine), fSrc);
		fprintf(fList, "%d. %s", cLine, sLine);
		if(feof(fSrc)) {
			sLine[0] = EOF;
			sLine[1] = 0;
		}
		psLine = sLine;
		cLine++;
		cColumn = 1;
	}
	MachineResult res = identifyToken(psLine);
	cColumn += res.newString - psLine;
	psLine = res.newString;
	if(res.type == T_WS) {
		return nextTerminal();
	}
	if(res.type == T_LEXERR) {
		lexerr(res);
		return nextTerminal();
	}
	return res;
}

void lexerr(Terminal res) {
	fprintf(fList, "%s, column %d\n", convertConstantToString(res.error), cColumn);
	fprintf(stderr, "%s, line %d, column %d\n", convertConstantToString(res.error), cLine, cColumn);

	retCode |= RET_LEXERR;
}

void semerr(char *msg) {
	fprintf(fList, "Semantic error: %s\n", msg);
	fprintf(stderr, "Semantic error, line %d: %s\n", cLine-1, msg);

	retCode |= RET_SEMERR;
}
