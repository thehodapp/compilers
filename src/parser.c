
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "types.h"
#include "machines.h"
#include "parser.h"
#include "item.h"

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
Item consume(NonTerminal, Item);
Item match(int);
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

Item nullItem;

void parse() {
	NonTerminal top_nonterminal = NT_PROGRAM;
	currTerm = nextTerminal();
	consume(top_nonterminal, nullItem);
}

Item consume(NonTerminal nt, Item context) {
	for(int i = 0; i < depth; i++) fprintf(fTree, " ");
	fprintf(fTree, "%s\n", ntToString(nt));
	depth++;

	Item a1, a2, a3, a4, a5, a6, a7, ret;
	switch(nt) {
		case NT_ARGUMENTS:
			switch(currTerm.type) {
				case T_LPAREN:
					if(match(T_LPAREN).error) goto nt_arguments_synch;
					consume(NT_PARAMETER_LIST, nullItem);
					if(match(T_RPAREN).error) goto nt_arguments_synch;
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
					if(match(T_BEGIN).error) goto nt_compound_statement_synch;
					consume(NT_COMPOUND_STATEMENT1, nullItem);
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
					consume(NT_OPTIONAL_STATEMENTS, nullItem);
					if(match(T_END).error) goto nt_compound_statement1_synch;
					break;
				case T_END:
					if(match(T_END).error) goto nt_compound_statement1_synch;
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
					if(match(T_VAR).error) goto nt_declarations_synch;
					if(match(T_ID).error) goto nt_declarations_synch;
					if(match(T_COLON).error) goto nt_declarations_synch;
					consume(NT_TYPE, nullItem);
					if(match(T_SEMICOLON).error) goto nt_declarations_synch;
					consume(NT_DECLARATIONS_, nullItem);
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
					if(match(T_VAR).error) goto nt_declarations__synch;
					if(match(T_ID).error) goto nt_declarations__synch;
					if(match(T_COLON).error) goto nt_declarations__synch;
					consume(NT_TYPE, nullItem);
					if(match(T_SEMICOLON).error) goto nt_declarations__synch;
					consume(NT_DECLARATIONS_, nullItem);
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
					consume(NT_SIMPLE_EXPRESSION, nullItem);
					consume(NT_EXPRESSION1, nullItem);
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
					if(match(T_RELOP).error) goto nt_expression1_synch;
					consume(NT_SIMPLE_EXPRESSION, nullItem);
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
					consume(NT_EXPRESSION, nullItem);
					consume(NT_EXPRESSION_LIST_, nullItem);
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
					if(match(T_COMMA).error) goto nt_expression_list__synch;
					consume(NT_EXPRESSION, nullItem);
					consume(NT_EXPRESSION_LIST_, nullItem);
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
					a1 = match(T_ID);
					if(a1.error) goto nt_factor_synch;

					a2.in = a1.type;
					consume(NT_FACTOR1, a2);

					ret.type = a2.type;
					return ret;
				case T_LPAREN:
					if(match(T_LPAREN).error) goto nt_factor_synch;
					ret = consume(NT_EXPRESSION, nullItem);
					if(match(T_RPAREN).error) goto nt_factor_synch;
					return ret;
				case T_NOT:
					if(match(T_NOT).error) goto nt_factor_synch;
					
					a2.in = TYPE_BOOL;
					a2 = consume(NT_FACTOR, a2);

					ret.type = a2.type;
					return ret;
				case T_NUM:
					a1 = match(T_NUM);
					if(a1.error) goto nt_factor_synch;

					return a1;
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
					return context;
				case T_LBRACK:
					if(match(T_LBRACK).error) goto nt_factor1_synch;
					a2.in = TYPE_INTEGER;
					a2 = consume(NT_EXPRESSION, a2);
					if(match(T_RBRACK).error) goto nt_factor1_synch;
					return a2;
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
					if(match(T_ID).error) goto nt_identifier_list_synch;
					consume(NT_IDENTIFIER_LIST_, nullItem);
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
					if(match(T_COMMA).error) goto nt_identifier_list__synch;
					if(match(T_ID).error) goto nt_identifier_list__synch;
					consume(NT_IDENTIFIER_LIST_, nullItem);
					break;
				case T_RPAREN:
					return context;
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
					consume(NT_STATEMENT_LIST, nullItem);
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
					if(match(T_ID).error) goto nt_parameter_list_synch;
					if(match(T_COLON).error) goto nt_parameter_list_synch;
					consume(NT_TYPE, nullItem);
					consume(NT_PARAMETER_LIST_, nullItem);
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
					if(match(T_SEMICOLON).error) goto nt_parameter_list__synch;
					if(match(T_ID).error) goto nt_parameter_list__synch;
					if(match(T_COLON).error) goto nt_parameter_list__synch;
					consume(NT_TYPE, nullItem);
					consume(NT_PARAMETER_LIST_, nullItem);
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
					if(match(T_CALL).error) goto nt_procedure_statement_synch;
					if(match(T_ID).error) goto nt_procedure_statement_synch;
					consume(NT_PROCEDURE_STATEMENT1, nullItem);
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
					if(match(T_LPAREN).error) goto nt_procedure_statement1_synch;
					consume(NT_EXPRESSION_LIST, nullItem);
					if(match(T_RPAREN).error) goto nt_procedure_statement1_synch;
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
					if(match(T_PROGRAM).error) goto nt_program_synch;
					if(match(T_ID).error) goto nt_program_synch;
					if(match(T_LPAREN).error) goto nt_program_synch;
					consume(NT_IDENTIFIER_LIST, nullItem);
					if(match(T_RPAREN).error) goto nt_program_synch;
					if(match(T_SEMICOLON).error) goto nt_program_synch;
					consume(NT_PROGRAM11, nullItem);
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
					consume(NT_COMPOUND_STATEMENT, nullItem);
					if(match(T_PERIOD).error) goto nt_program1_synch;
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, nullItem);
					consume(NT_COMPOUND_STATEMENT, nullItem);
					if(match(T_PERIOD).error) goto nt_program1_synch;
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
					consume(NT_COMPOUND_STATEMENT, nullItem);
					if(match(T_PERIOD).error) goto nt_program11_synch;
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, nullItem);
					consume(NT_COMPOUND_STATEMENT, nullItem);
					if(match(T_PERIOD).error) goto nt_program11_synch;
					break;
				case T_VAR:
					consume(NT_DECLARATIONS, nullItem);
					consume(NT_PROGRAM1, nullItem);
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
					if(match(T_MINUS).error) goto nt_sign_synch;
					break;
				case T_PLUS:
					if(match(T_PLUS).error) goto nt_sign_synch;
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
					consume(NT_TERM, nullItem);
					consume(NT_SIMPLE_EXPRESSION_, nullItem);
					break;
				case T_MINUS:
				case T_PLUS:
					consume(NT_SIGN, nullItem);
					consume(NT_TERM, nullItem);
					consume(NT_SIMPLE_EXPRESSION_, nullItem);
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
					if(match(T_ADDOP).error) goto nt_simple_expression__synch;
					consume(NT_TERM, nullItem);
					consume(NT_SIMPLE_EXPRESSION_, nullItem);
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
					if(match(T_INTEGER).error) goto nt_standard_type_synch;
					break;
				case T_REAL:
					if(match(T_REAL).error) goto nt_standard_type_synch;
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
					consume(NT_COMPOUND_STATEMENT, nullItem);
					break;
				case T_CALL:
					consume(NT_PROCEDURE_STATEMENT, nullItem);
					break;
				case T_ID:
					consume(NT_VARIABLE, nullItem);
					if(match(T_ASSIGNOP).error) goto nt_statement_synch;
					consume(NT_EXPRESSION, nullItem);
					break;
				case T_IF:
					if(match(T_IF).error) goto nt_statement_synch;
					consume(NT_EXPRESSION, nullItem);
					if(match(T_THEN).error) goto nt_statement_synch;
					consume(NT_STATEMENT, nullItem);
					consume(NT_STATEMENT1, nullItem);
					break;
				case T_WHILE:
					if(match(T_WHILE).error) goto nt_statement_synch;
					consume(NT_EXPRESSION, nullItem);
					if(match(T_DO).error) goto nt_statement_synch;
					consume(NT_STATEMENT, nullItem);
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
					if(match(T_ELSE).error) goto nt_statement1_synch;
					consume(NT_STATEMENT, nullItem);
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
					consume(NT_STATEMENT, nullItem);
					consume(NT_STATEMENT_LIST_, nullItem);
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
					if(match(T_SEMICOLON).error) goto nt_statement_list__synch;
					consume(NT_STATEMENT, nullItem);
					consume(NT_STATEMENT_LIST_, nullItem);
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
					consume(NT_SUBPROGRAM_HEAD, nullItem);
					consume(NT_SUBPROGRAM_DECLARATION11, nullItem);
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
					consume(NT_COMPOUND_STATEMENT, nullItem);
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, nullItem);
					consume(NT_COMPOUND_STATEMENT, nullItem);
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
					consume(NT_COMPOUND_STATEMENT, nullItem);
					break;
				case T_PROCEDURE:
					consume(NT_SUBPROGRAM_DECLARATIONS, nullItem);
					consume(NT_COMPOUND_STATEMENT, nullItem);
					break;
				case T_VAR:
					consume(NT_DECLARATIONS, nullItem);
					consume(NT_SUBPROGRAM_DECLARATION1, nullItem);
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
					consume(NT_SUBPROGRAM_DECLARATION, nullItem);
					if(match(T_SEMICOLON).error) goto nt_subprogram_declarations_synch;
					consume(NT_SUBPROGRAM_DECLARATIONS_, nullItem);
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
					consume(NT_SUBPROGRAM_DECLARATION, nullItem);
					if(match(T_SEMICOLON).error) goto nt_subprogram_declarations__synch;
					consume(NT_SUBPROGRAM_DECLARATIONS_, nullItem);
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
					if(match(T_PROCEDURE).error) goto nt_subprogram_head_synch;
					if(match(T_ID).error) goto nt_subprogram_head_synch;
					consume(NT_SUBPROGRAM_HEAD1, nullItem);
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
					consume(NT_ARGUMENTS, nullItem);
					if(match(T_SEMICOLON).error) goto nt_subprogram_head1_synch;
					break;
				case T_SEMICOLON:
					if(match(T_SEMICOLON).error) goto nt_subprogram_head1_synch;
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
					consume(NT_FACTOR, nullItem);
					consume(NT_TERM_, nullItem);
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
					if(match(T_MULOP).error) goto nt_term__synch;
					consume(NT_FACTOR, nullItem);
					consume(NT_TERM_, nullItem);
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
					if(match(T_ARRAY).error) goto nt_type_synch;
					if(match(T_LBRACK).error) goto nt_type_synch;
					if(match(T_NUM).error) goto nt_type_synch;
					if(match(T_DOUBLEPERIOD).error) goto nt_type_synch;
					if(match(T_NUM).error) goto nt_type_synch;
					if(match(T_RBRACK).error) goto nt_type_synch;
					if(match(T_OF).error) goto nt_type_synch;
					consume(NT_STANDARD_TYPE, nullItem);
					break;
				case T_INTEGER:
				case T_REAL:
					consume(NT_STANDARD_TYPE, nullItem);
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
					if(match(T_ID).error) goto nt_variable_synch;
					consume(NT_VARIABLE1, nullItem);
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
					if(match(T_LBRACK).error) goto nt_variable1_synch;
					consume(NT_EXPRESSION, nullItem);
					if(match(T_RBRACK).error) goto nt_variable1_synch;
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

Item match(int termtype) {
	Item ret;
	if(currTerm.type == termtype) {
		for(int i = 0; i < depth; i++) fprintf(fTree, " ");
		fprintf(fTree, "%s\n", convertConstantToString(currTerm.type));
		for(int i = 0; i < depth+1; i++) fprintf(fTree, " ");
		fprintf(fTree, "\"%s\"\n", currTerm.lexeme);
		currTerm = nextTerminal();

		return ret;
	}
	synerr((int[]){termtype}, 1, currTerm);
	return ret;
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

