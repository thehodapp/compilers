
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"
#include "machines.h"
#include "parser.h"

int depth = 0;

#define X(a) case a: return #a;
char* ntToString(NonTerminal nt) {
	switch(nt) {
		NONTERMS
	}
	return "";
}
#undef X

void parse(void);
void consume(NonTerminal);
int match(int, NonTerminal);
void synch(NonTerminal);
void synerr(int*, int, Terminal);
void lexerr(Terminal);
Terminal nextTerminal(void);

Terminal currTerm;
FILE *fSrc, *fTree, *fList;
char sLine[90] = {0};
char *psLine;
int cLine;
int cColumn;

void parse() {
	NonTerminal top_nonterminal = NT_PROGRAM;
	currTerm = nextTerminal();
	consume(top_nonterminal);
}

void consume(NonTerminal nt) {
	for(int i = 0; i < depth; i++) fprintf(fTree, " ");
	fprintf(fTree, "%s\n", ntToString(nt));
	depth++;
	switch(nt) {
		case NT_ARGUMENTS:
			switch(currTerm.type) {
				case T_LPAREN:
					if(!match(T_LPAREN, nt)) goto nt_arguments_synch;
					consume(NT_PARAMETER_LIST);
					if(!match(T_RPAREN, nt)) goto nt_arguments_synch;
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
					if(!match(T_BEGIN, nt)) goto nt_compound_statement_synch;
					consume(NT_COMPOUND_STATEMENT1);
					break;
				default:
					synerr((int[]){T_BEGIN}, 1, currTerm);
				nt_compound_statement_synch:
					synch(nt);
					break;
			}
			break;
		case NT_COMPOUND_STATEMENT1:
			switch(currTerm.type) {
				case T_BEGIN:
				case T_CALL:
				case T_ID:
				case T_IF:
				case T_WHILE:
					consume(NT_OPTIONAL_STATEMENTS);
					if(!match(T_END, nt)) goto nt_compound_statement1_synch;
					break;
				case T_END:
					if(!match(T_END, nt)) goto nt_compound_statement1_synch;
					break;
				default:
					synerr((int[]){T_ID, T_IF, T_WHILE, T_BEGIN, T_CALL, T_END}, 6, currTerm);
				nt_compound_statement1_synch:
					synch(nt);
					break;
			}
			break;
		case NT_DECLARATIONS:
			switch(currTerm.type) {
				case T_VAR:
					if(!match(T_VAR, nt)) goto nt_declarations_synch;
					if(!match(T_ID, nt)) goto nt_declarations_synch;
					if(!match(T_COLON, nt)) goto nt_declarations_synch;
					consume(NT_TYPE);
					if(!match(T_SEMICOLON, nt)) goto nt_declarations_synch;
					consume(NT_DECLARATIONS_);
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
					if(!match(T_VAR, nt)) goto nt_declarations__synch;
					if(!match(T_ID, nt)) goto nt_declarations__synch;
					if(!match(T_COLON, nt)) goto nt_declarations__synch;
					consume(NT_TYPE);
					if(!match(T_SEMICOLON, nt)) goto nt_declarations__synch;
					consume(NT_DECLARATIONS_);
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
					consume(NT_SIMPLE_EXPRESSION);
					consume(NT_EXPRESSION1);
					break;
				default:
					synerr((int[]){T_ID, T_LPAREN, T_NUM, T_PLUS, T_MINUS, T_NOT}, 6, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_EXPRESSION1:
			switch(currTerm.type) {
				case T_COMMA:
				case T_DO:
				case T_ELSE:
				case T_END:
				case T_RBRACK:
				case T_RPAREN:
				case T_SEMICOLON:
				case T_THEN:
					break;
				case T_RELOP:
					if(!match(T_RELOP, nt)) goto nt_expression1_synch;
					consume(NT_SIMPLE_EXPRESSION);
					break;
				default:
					synerr((int[]){T_THEN, T_ELSE, T_COMMA, T_SEMICOLON, T_RBRACK, T_DO, T_RPAREN, T_RELOP, T_END}, 9, currTerm);
				nt_expression1_synch:
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
					consume(NT_EXPRESSION);
					consume(NT_EXPRESSION_LIST_);
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
					if(!match(T_COMMA, nt)) goto nt_expression_list__synch;
					consume(NT_EXPRESSION);
					consume(NT_EXPRESSION_LIST_);
					break;
				case T_RPAREN:
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
					if(!match(T_ID, nt)) goto nt_factor_synch;
					consume(NT_FACTOR1);
					break;
				case T_LPAREN:
					if(!match(T_LPAREN, nt)) goto nt_factor_synch;
					consume(NT_EXPRESSION);
					if(!match(T_RPAREN, nt)) goto nt_factor_synch;
					break;
				case T_NOT:
					if(!match(T_NOT, nt)) goto nt_factor_synch;
					consume(NT_FACTOR);
					break;
				case T_NUM:
					if(!match(T_NUM, nt)) goto nt_factor_synch;
					break;
				default:
					synerr((int[]){T_ID, T_NUM, T_LPAREN, T_NOT}, 4, currTerm);
				nt_factor_synch:
					synch(nt);
					break;
			}
			break;
		case NT_FACTOR1:
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
					break;
				case T_LBRACK:
					if(!match(T_LBRACK, nt)) goto nt_factor1_synch;
					consume(NT_EXPRESSION);
					if(!match(T_RBRACK, nt)) goto nt_factor1_synch;
					break;
				default:
					synerr((int[]){T_THEN, T_ELSE, T_COMMA, T_SEMICOLON, T_LBRACK, T_ADDOP, T_DO, T_RPAREN, T_RELOP, T_MULOP, T_RBRACK, T_END}, 12, currTerm);
				nt_factor1_synch:
					synch(nt);
					break;
			}
			break;
		case NT_IDENTIFIER_LIST:
			switch(currTerm.type) {
				case T_ID:
					if(!match(T_ID, nt)) goto nt_identifier_list_synch;
					consume(NT_IDENTIFIER_LIST_);
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
					if(!match(T_COMMA, nt)) goto nt_identifier_list__synch;
					if(!match(T_ID, nt)) goto nt_identifier_list__synch;
					consume(NT_IDENTIFIER_LIST_);
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
					consume(NT_STATEMENT_LIST);
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
					if(!match(T_ID, nt)) goto nt_parameter_list_synch;
					if(!match(T_COLON, nt)) goto nt_parameter_list_synch;
					consume(NT_TYPE);
					consume(NT_PARAMETER_LIST_);
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
					if(!match(T_SEMICOLON, nt)) goto nt_parameter_list__synch;
					if(!match(T_ID, nt)) goto nt_parameter_list__synch;
					if(!match(T_COLON, nt)) goto nt_parameter_list__synch;
					consume(NT_TYPE);
					consume(NT_PARAMETER_LIST_);
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
					if(!match(T_CALL, nt)) goto nt_procedure_statement_synch;
					if(!match(T_ID, nt)) goto nt_procedure_statement_synch;
					consume(NT_PROCEDURE_STATEMENT1);
					break;
				default:
					synerr((int[]){T_CALL}, 1, currTerm);
				nt_procedure_statement_synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROCEDURE_STATEMENT1:
			switch(currTerm.type) {
				case T_ELSE:
				case T_END:
				case T_SEMICOLON:
					break;
				case T_LPAREN:
					if(!match(T_LPAREN, nt)) goto nt_procedure_statement1_synch;
					consume(NT_EXPRESSION_LIST);
					if(!match(T_RPAREN, nt)) goto nt_procedure_statement1_synch;
					break;
				default:
					synerr((int[]){T_ELSE, T_LPAREN, T_SEMICOLON, T_END}, 4, currTerm);
				nt_procedure_statement1_synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROGRAM:
			switch(currTerm.type) {
				case T_PROGRAM:
					if(!match(T_PROGRAM, nt)) goto nt_program_synch;
					if(!match(T_ID, nt)) goto nt_program_synch;
					if(!match(T_LPAREN, nt)) goto nt_program_synch;
					consume(NT_IDENTIFIER_LIST);
					if(!match(T_RPAREN, nt)) goto nt_program_synch;
					if(!match(T_SEMICOLON, nt)) goto nt_program_synch;
					consume(NT_PROGRAM11);
					break;
				default:
					synerr((int[]){T_PROGRAM}, 1, currTerm);
				nt_program_synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROGRAM1:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT);
					if(!match(T_PERIOD, nt)) goto nt_program1_synch;
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS);
					consume(NT_COMPOUND_STATEMENT);
					if(!match(T_PERIOD, nt)) goto nt_program1_synch;
					break;
				default:
					synerr((int[]){T_BEGIN, T_PROCEDURE}, 2, currTerm);
				nt_program1_synch:
					synch(nt);
					break;
			}
			break;
		case NT_PROGRAM11:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT);
					if(!match(T_PERIOD, nt)) goto nt_program11_synch;
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS);
					consume(NT_COMPOUND_STATEMENT);
					if(!match(T_PERIOD, nt)) goto nt_program11_synch;
					break;
				case T_VAR:
					consume(NT_DECLARATIONS);
					consume(NT_PROGRAM1);
					break;
				default:
					synerr((int[]){T_BEGIN, T_VAR, T_PROCEDURE}, 3, currTerm);
				nt_program11_synch:
					synch(nt);
					break;
			}
			break;
		case NT_SIGN:
			switch(currTerm.type) {
				case T_MINUS:
					if(!match(T_MINUS, nt)) goto nt_sign_synch;
					break;
				case T_PLUS:
					if(!match(T_PLUS, nt)) goto nt_sign_synch;
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
					consume(NT_TERM);
					consume(NT_SIMPLE_EXPRESSION_);
					break;
				case T_MINUS:
				case T_PLUS:
					consume(NT_SIGN);
					consume(NT_TERM);
					consume(NT_SIMPLE_EXPRESSION_);
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
					if(!match(T_ADDOP, nt)) goto nt_simple_expression__synch;
					consume(NT_TERM);
					consume(NT_SIMPLE_EXPRESSION_);
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
					if(!match(T_INTEGER, nt)) goto nt_standard_type_synch;
					break;
				case T_REAL:
					if(!match(T_REAL, nt)) goto nt_standard_type_synch;
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
					consume(NT_COMPOUND_STATEMENT);
					break;
				case T_CALL:
					consume(NT_PROCEDURE_STATEMENT);
					break;
				case T_ID:
					consume(NT_VARIABLE);
					if(!match(T_ASSIGNOP, nt)) goto nt_statement_synch;
					consume(NT_EXPRESSION);
					break;
				case T_IF:
					if(!match(T_IF, nt)) goto nt_statement_synch;
					consume(NT_EXPRESSION);
					if(!match(T_THEN, nt)) goto nt_statement_synch;
					consume(NT_STATEMENT);
					consume(NT_STATEMENT1);
					break;
				case T_WHILE:
					if(!match(T_WHILE, nt)) goto nt_statement_synch;
					consume(NT_EXPRESSION);
					if(!match(T_DO, nt)) goto nt_statement_synch;
					consume(NT_STATEMENT);
					break;
				default:
					synerr((int[]){T_ID, T_BEGIN, T_IF, T_WHILE, T_CALL}, 5, currTerm);
				nt_statement_synch:
					synch(nt);
					break;
			}
			break;
		case NT_STATEMENT1:
			switch(currTerm.type) {
				case T_ELSE:
					if(!match(T_ELSE, nt)) goto nt_statement1_synch;
					consume(NT_STATEMENT);
					break;
				case T_END:
				case T_SEMICOLON:
					break;
				default:
					synerr((int[]){T_ELSE, T_SEMICOLON, T_END}, 3, currTerm);
				nt_statement1_synch:
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
					consume(NT_STATEMENT);
					consume(NT_STATEMENT_LIST_);
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
					if(!match(T_SEMICOLON, nt)) goto nt_statement_list__synch;
					consume(NT_STATEMENT);
					consume(NT_STATEMENT_LIST_);
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
					consume(NT_SUBPROGRAM_HEAD);
					consume(NT_SUBPROGRAM_DECLARATION11);
					break;
				default:
					synerr((int[]){T_PROCEDURE}, 1, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATION1:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT);
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS);
					consume(NT_COMPOUND_STATEMENT);
					break;
				default:
					synerr((int[]){T_BEGIN, T_PROCEDURE}, 2, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATION11:
			switch(currTerm.type) {
				case T_BEGIN:
					consume(NT_COMPOUND_STATEMENT);
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS);
					consume(NT_COMPOUND_STATEMENT);
					break;
				case T_VAR:
					consume(NT_DECLARATIONS);
					consume(NT_SUBPROGRAM_DECLARATION1);
					break;
				default:
					synerr((int[]){T_BEGIN, T_VAR, T_PROCEDURE}, 3, currTerm);
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_DECLARATIONS:
			switch(currTerm.type) {
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATION);
					if(!match(T_SEMICOLON, nt)) goto nt_subprogram_declarations_synch;
					consume(NT_SUBPROGRAM_DECLARATIONS_);
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
					consume(NT_SUBPROGRAM_DECLARATION);
					if(!match(T_SEMICOLON, nt)) goto nt_subprogram_declarations__synch;
					consume(NT_SUBPROGRAM_DECLARATIONS_);
					break;
				default:
					synerr((int[]){T_BEGIN, T_SEMICOLON, T_PROCEDURE}, 3, currTerm);
				nt_subprogram_declarations__synch:
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_HEAD:
			switch(currTerm.type) {
				case T_PROCEDURE:
					if(!match(T_PROCEDURE, nt)) goto nt_subprogram_head_synch;
					if(!match(T_ID, nt)) goto nt_subprogram_head_synch;
					consume(NT_SUBPROGRAM_HEAD1);
					break;
				default:
					synerr((int[]){T_PROCEDURE}, 1, currTerm);
				nt_subprogram_head_synch:
					synch(nt);
					break;
			}
			break;
		case NT_SUBPROGRAM_HEAD1:
			switch(currTerm.type) {
				case T_LPAREN:
					consume(NT_ARGUMENTS);
					if(!match(T_SEMICOLON, nt)) goto nt_subprogram_head1_synch;
					break;
				case T_SEMICOLON:
					if(!match(T_SEMICOLON, nt)) goto nt_subprogram_head1_synch;
					break;
				default:
					synerr((int[]){T_LPAREN, T_SEMICOLON}, 2, currTerm);
				nt_subprogram_head1_synch:
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
					consume(NT_FACTOR);
					consume(NT_TERM_);
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
					break;
				case T_MULOP:
					if(!match(T_MULOP, nt)) goto nt_term__synch;
					consume(NT_FACTOR);
					consume(NT_TERM_);
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
					if(!match(T_ARRAY, nt)) goto nt_type_synch;
					if(!match(T_LBRACK, nt)) goto nt_type_synch;
					if(!match(T_NUM, nt)) goto nt_type_synch;
					if(!match(T_DOUBLEPERIOD, nt)) goto nt_type_synch;
					if(!match(T_NUM, nt)) goto nt_type_synch;
					if(!match(T_RBRACK, nt)) goto nt_type_synch;
					if(!match(T_OF, nt)) goto nt_type_synch;
					consume(NT_STANDARD_TYPE);
					break;
				case T_INTEGER:
				case T_REAL:
					consume(NT_STANDARD_TYPE);
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
					if(!match(T_ID, nt)) goto nt_variable_synch;
					consume(NT_VARIABLE1);
					break;
				default:
					synerr((int[]){T_ID}, 1, currTerm);
				nt_variable_synch:
					synch(nt);
					break;
			}
			break;
		case NT_VARIABLE1:
			switch(currTerm.type) {
				case T_ASSIGNOP:
					break;
				case T_LBRACK:
					if(!match(T_LBRACK, nt)) goto nt_variable1_synch;
					consume(NT_EXPRESSION);
					if(!match(T_RBRACK, nt)) goto nt_variable1_synch;
					break;
				default:
					synerr((int[]){T_LBRACK, T_ASSIGNOP}, 2, currTerm);
				nt_variable1_synch:
					synch(nt);
					break;
			}
			break;

	}
	depth--;
}

void synch(NonTerminal nt) {
	int *synchSet;
	int len;
	switch(nt) {
		case NT_PROCEDURE_STATEMENT:
		case NT_STATEMENT:
		case NT_STATEMENT1:
		case NT_PROCEDURE_STATEMENT1:
			synchSet = (int[]){T_EOF, T_END, T_ELSE, T_SEMICOLON}; len = 4;
			break;
		case NT_SUBPROGRAM_DECLARATION:
		case NT_SUBPROGRAM_DECLARATION11:
		case NT_ARGUMENTS:
		case NT_SUBPROGRAM_DECLARATION1:
			synchSet = (int[]){T_EOF, T_SEMICOLON}; len = 2;
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
		case NT_PROGRAM1:
		case NT_PROGRAM11:
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
		case NT_SUBPROGRAM_HEAD1:
			synchSet = (int[]){T_BEGIN, T_EOF, T_VAR, T_SEMICOLON, T_PROCEDURE}; len = 5;
			break;
		case NT_TERM:
		case NT_TERM_:
			synchSet = (int[]){T_THEN, T_EOF, T_COMMA, T_SEMICOLON, T_ADDOP, T_RBRACK, T_DO, T_ELSE, T_RPAREN, T_RELOP, T_END}; len = 11;
			break;
		case NT_TYPE:
		case NT_STANDARD_TYPE:
			synchSet = (int[]){T_EOF, T_SEMICOLON, T_RPAREN}; len = 3;
			break;
		case NT_OPTIONAL_STATEMENTS:
		case NT_STATEMENT_LIST_:
		case NT_STATEMENT_LIST:
			synchSet = (int[]){T_EOF, T_END}; len = 2;
			break;
		case NT_DECLARATIONS:
		case NT_DECLARATIONS_:
			synchSet = (int[]){T_BEGIN, T_EOF, T_SEMICOLON, T_PROCEDURE}; len = 4;
			break;
		case NT_SUBPROGRAM_DECLARATIONS:
		case NT_SUBPROGRAM_DECLARATIONS_:
			synchSet = (int[]){T_BEGIN, T_EOF, T_SEMICOLON}; len = 3;
			break;
		case NT_COMPOUND_STATEMENT:
		case NT_COMPOUND_STATEMENT1:
			synchSet = (int[]){T_PERIOD, T_EOF, T_END, T_ELSE, T_SEMICOLON}; len = 5;
			break;
		case NT_VARIABLE1:
		case NT_VARIABLE:
			synchSet = (int[]){T_EOF, T_ASSIGNOP}; len = 2;
			break;
		case NT_EXPRESSION:
		case NT_EXPRESSION1:
			synchSet = (int[]){T_THEN, T_EOF, T_COMMA, T_SEMICOLON, T_RBRACK, T_DO, T_ELSE, T_RPAREN, T_END}; len = 9;
			break;
		case NT_FACTOR1:
		case NT_FACTOR:
			synchSet = (int[]){T_THEN, T_EOF, T_COMMA, T_SEMICOLON, T_ADDOP, T_DO, T_RPAREN, T_ELSE, T_RELOP, T_MULOP, T_RBRACK, T_END}; len = 12;
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

int match(int termtype, NonTerminal nt) {
	if(currTerm.type == termtype) {
		for(int i = 0; i < depth; i++) fprintf(fTree, " ");
		fprintf(fTree, "%s\n", convertConstantToString(currTerm.type));
		for(int i = 0; i < depth+1; i++) fprintf(fTree, " ");
		fprintf(fTree, "\"%s\"\n", currTerm.lexeme);
		currTerm = nextTerminal();
		return true;
	}
	synerr((int[]){termtype}, 1, currTerm);
	return false;
}

void synerr(int *expected, int expLen, Terminal encountered) {
	char *str = " one of";
	if(expLen == 1) str = "";
	fprintf(fList, "SYNERR, column %d: Unexpected %s; expected%s: ", cColumn, convertConstantToString(encountered.type), str);
	for(int i = 0; i < expLen - 1; i++) {
		fprintf(fList, "%s, ", convertConstantToString(expected[i]));
	}
	fprintf(fList, "%s\n", convertConstantToString(expected[expLen-1]));
	fprintf(stderr, "SYNERR, line %d, column %d: Unexpected %s; expected%s: ", cLine, cColumn, convertConstantToString(encountered.type), str);
	for(int i = 0; i < expLen - 1; i++) {
		fprintf(stderr, "%s, ", convertConstantToString(expected[i]));
	}
	fprintf(stderr, "%s\n", convertConstantToString(expected[expLen-1]));
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

	cLine = 1;
	cColumn = 0;
	machinesInit("data/reserved-words.txt");
	parse();
	fclose(fSrc);
	fclose(fTree);
	return 0;
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
	fprintf(stderr, "%s, line %d, column %d\n", convertConstantToString(res.error), cColumn, cLine);
}

