#include<stdlib.h>
#include<string.h>
#include "machines.h"
#include "types.h"

ReservedWordList *rwl;

ReservedWord* CheckReservedWords(char*);
SymbolTableEntry* checkSymbolTable(char*);

MachineResult IDRES(char* str) {
	char* b = str;
	char* f = b;
	int state = 0;
	MachineResult res;
	res.error = 0;
	while(1) {
		switch(state) {
		case 0:
			if(('a' <= *f && *f <= 'z') || ('A' <= *f && *f <= 'Z'))
				state = 1;
			else {
				res.newString = str;
				res.type = -1;
				res.attribute = -1;
				res.validToken = 0;
				return res;
			}
			break;
		case 1:
			if(('a' <= *f && *f <= 'z' ) || ( 'A' <= *f && *f <= 'Z' ) || ( '0' <= *f && *f <= '9'))
				state = 1;
			else
				state = 2;
			break;
		case 2:
			//valid identifier
			//Decrement forward pointer, since we consumed a character we shouldn't have
			f--;
			res.newString = f;
			char *lex = malloc((f-str)*sizeof(char)+1);
			memcpy(lex, str, f-str);
			lex[f-str] = 0;
			ReservedWord *possResWord = CheckReservedWords(lex);
			if(possResWord) {
				res.type = possResWord->type;
				res.attribute = possResWord->attribute;
				res.validToken = 1;
			} else {
				SymbolTableEntry* entry = checkSymbolTable(lex);
				res.type = T_ID;
				res.pointer = entry;
				res.validToken = 1;
				if((f-str) > 10) {
					res.error = ERR_ID_LEN;
				}
			}
			free(lex);
			return res;
		}
		//increment forward pointer
		f++;
	}
}

ReservedWord* CheckReservedWords(char* word) {
	//get local pointer
	ReservedWordList* prwl = rwl;

	//Search for word in prwl
	while(prwl != NULL && prwl->rword != NULL) {
		if(!strcmp(word, prwl->rword->word)) {
			//found it!
			return prwl->rword;
		} else {
			//go to next link
			prwl = prwl->next;
		}
	}
	return NULL;
}

SymbolTableEntry* checkSymbolTable(char* word) {
	SymbolTable* pTab = tab;
	while(pTab->entry) {
		if(!strcmp(word, pTab->entry->word)) {
			//found it
			return pTab->entry;
		} else {
			//go to next link
			if(!pTab->next) {
				//make new link
				pTab->next = malloc(sizeof(SymbolTable));
			}
			pTab = pTab->next;
		}
	}
	pTab->entry = malloc(sizeof(SymbolTableEntry));
	pTab->entry->word = malloc(strlen(word)+1);
	strcpy(pTab->entry->word, word);
	return pTab->entry;
}

MachineResult RELOP(char* str) {
	char* f = str;
	int state = 0;
	MachineResult res;
	res.error = 0;
	res.validToken = 1;
	res.type = T_RELOP;
	res.newString = f;
	while(1) {
		switch(state) {
		case 0:
			state = -1;
			if(*f == '>') state = 1;
			if(*f == '<') state = 2;
			if(*f == '=') state = 3;
			break;
		case 1:
			state = 5;
			if(*f == '=') state = 4;
			break;
		case 2:
			state = 8;
			if(*f == '=') state = 6;
			if(*f == '>') state = 7;
			break;
		case 3:
			res.attribute = RELOP_EQ;
			res.newString = f;
			return res;
		case 4:
			res.attribute = RELOP_GE;
			res.newString = f;
			return res;
		case 5:
			res.attribute = RELOP_GT;
			res.newString = f;
			return res;
		case 6:
			res.attribute = RELOP_LE;
			res.newString = f;
			return res;
		case 7:
			res.attribute = RELOP_NE;
			res.newString = f;
			return res;
		case 8:
			res.attribute = RELOP_LT;
			res.newString = f;
			return res;
		case -1:
			res.validToken = 0;
			res.newString = str;
			res.type = res.attribute = -1;
			return res;
		}
		f++;
	}
}

MachineResult WS(char* str) {
	char* f = str;
	int state = 0;
	MachineResult res;
	res.error = 0;
	while(1) {
		switch(state) {
			case 0:
				if(*f == ' ' || *f == '\n' || *f == '\t' || *f == '\r')
					state = 1;
				else
					state = -1;
				break;
			case 1:
				if(*f == ' ' || *f == '\n' || *f == '\t' || *f == '\r')
					state = 1;
				else
					state = 2;
				break;
			case 2:
				f--;
				res.newString = f;
				res.type = T_WS;
				res.attribute = 0;
				res.validToken = 1;
				return res;
			case -1:
				res.newString = str;
				res.type = res.attribute = -1;
				res.validToken = 0;
				return res;
		}
		f++;
	}
}

MachineResult INT(char* str) {
	char* f = str;
	int state = 0;
	MachineResult res;
	res.error = 0;
	while(1) {
		switch(state) {
			case 0:
				if('0' <= *f && *f <= '9')
					state = 1;
				else
					state = -1;
				break;
			case 1:
				if('0' <= *f && *f <= '9')
					state = 1;
				else {
					state = 2;
				}
				break;
			case 2:
				f--;
				res.newString = f;
				res.type = T_NUM;
				res.attribute = NUM_INT;
				res.validToken = 1;
				if((f-str) > 10) {
					res.error = ERR_INT_LEN;
				}
				if(*str == '0' && (f-str) > 1) {
					res.error = ERR_INT_LEADING_ZERO;
				}
				return res;
			case -1:
				res.newString = str;
				res.type = -1;
				res.attribute = -1;
				res.validToken = 0;
				return res;
		}
		f++;
	}
}

MachineResult REAL(char* str) {
	char* f = str;
	int state = 0;
	MachineResult res;
	res.error = 0;
	char* pDot;
	while(1) {
		switch(state) {
			case 0:
				if('0' <= *f && *f <= '9')
					state = 1;
				else
					state = -1;
				break;
			case 1:
				if('0' <= *f && *f <= '9')
					state = 1;
				else if (*f == '.') {
					pDot = f;
					state = 2;
				} else
					state = -1;
				break;
			case 2:
				if('0' <= *f && *f <= '9')
					state = 3;
				else
					state = -1;
				break;
			case 3:
				if('0' <= *f && *f <= '9')
					state = 3;
				else
					state = 4;
				break;
			case 4:
				f--;
				res.newString = f;
				res.type = T_NUM;
				res.attribute = NUM_REAL;
				res.validToken = 1;
				if((pDot-str) > 5) {
					res.error = ERR_INT_LEN;
				}
				if(((f-1)-pDot) > 5) {
					res.error = ERR_DECIMAL_LEN;
				}
				return res;
			case -1:
				res.newString = str;
				res.type = -1;
				res.attribute = -1;
				res.validToken = 0;
				return res;
		}
		f++;
	}
}

MachineResult LONGREAL(char* str) {
	char *f = str;
	int state = 0;
	MachineResult res;
	res.error = 0;
	char *pDot, *pE, *pSign = NULL;
	while(1) {
		switch(state) {
			case 0:
				if('0' <= *f && *f <= '9')
					state = 1;
				else
					state = -1;
				break;
			case 1:
				if('0' <= *f && *f <= '9')
					state = 1;
				else if (*f == '.') {
					pDot = f;
					state = 2;
				} else
					state = -1;
				break;
			case 2:
				if('0' <= *f && *f <= '9')
					state = 3;
				else
					state = -1;
				break;
			case 3:
				if('0' <= *f && *f <= '9')
					state = 3;
				else if (*f == 'E') {
					pE = f;
					state = 4;
				} else
					state = -1;
				break;
			case 4:
				if(*f == '+' || *f == '-') {
					pSign = f;
					state = 5;
				} else if('1' <= *f && *f <= '9') {
					state = 6;
				} else if('0' == *f) {
					res.error = ERR_EXPONENT_LEADING_ZERO;
					state = 6;
				} else
					state = -1;
				break;
			case 5:
				if('1' <= *f && *f <= '9')
					state = 6;
				else if('0' == *f) {
					state = 6;
					res.error = ERR_EXPONENT_LEADING_ZERO;
				} else
					state = -1;
				break;
			case 6:
				if('1' <= *f && *f <= '9')
					state = 6;
				else
					state = 7;
				break;
			case 7:
				f--;
				res.newString = f;
				res.type = T_NUM;
				res.attribute = NUM_LONGREAL;
				res.validToken = 1;
				if((pDot-str) > 5) {
					res.error = ERR_INT_LEN;
				}
				if(((pE-1)-pDot) > 5) {
					res.error = ERR_DECIMAL_LEN;
				}
				if((!pSign && ((f-1)-pE) > 2 ) || ( pSign && ((f-1)-pE) > 3)) {
					res.error = ERR_EXPONENT_LEN;
				}
				return res;
			case -1:
				res.newString = str;
				res.type = -1;
				res.attribute = -1;
				res.validToken = 0;
				return res;
		}
		f++;
	}
}

MachineResult ENDOFFILE(char* str) {
	MachineResult res;
	res.error = 0;
	res.type = T_EOF;
	res.attribute = 0;
	res.validToken = 1;
	if(*str == EOF) {//end-of-file character
		res.newString = str+1;
	} else {
		res.newString = str;
		res.validToken = 0;
	}
	return res;
}

MachineResult CATCHALL(char* str) {
	MachineResult res;
	res.error = 0;
	res.validToken = 1;
	res.newString = str+1;
	switch(*str) {
		case '(':
			res.type = T_LPAREN;
			res.attribute = 0;
			return res;
		case ')':
			res.type = T_RPAREN;
			res.attribute = 0;
			return res;
		case '[':
			res.type = T_LBRACK;
			res.attribute = 0;
			return res;
		case ']':
			res.type = T_RBRACK;
			res.attribute = 0;
			return res;
		case ',':
			res.type = T_COMMA;
			res.attribute = 0;
			return res;
		case ';':
			res.type = T_SEMICOLON;
			res.attribute = 0;
			return res;
		case '.':
			if(str[1] == '.') {
				res.type = T_DOUBLEPERIOD;
				res.attribute = 0;
				res.newString = str+2;
			} else {
				res.type = T_PERIOD;
				res.attribute = 0;
			}
			return res;
		case '+':
			res.type = T_ADDOP;
			res.attribute = ADDOP_PLUS;
			return res;
		case '-':
			res.type = T_ADDOP;
			res.attribute = ADDOP_MINUS;
			return res;
		case '*':
			res.type = T_MULOP;
			res.attribute = MULOP_TIMES;
			return res;
		case '/':
			res.type = T_MULOP;
			res.attribute = MULOP_DIVIDE;
			return res;
		case ':':
			if(str[1] == '=') {
				res.type = T_ASSIGNOP;
				res.attribute = 0;
				res.newString = str+2;
				return res;
			} else {
				res.type = T_COLON;
				res.attribute = 0;
				return res;
			}
	}
	res.validToken = 0;
	res.type = res.attribute = -1;
	res.newString = str;
	return res;
}

void machinesInit(char* sfReservedWords) {
	tab = malloc(sizeof(SymbolTable));
	tab->entry = NULL;

	FILE *fReservedWords = fopen(sfReservedWords, "r");
	if(fReservedWords) {
		rwl = parseResWordFile(fReservedWords);
	} else {
		rwl = malloc(sizeof(ReservedWordList));
	}
}

static Machine machines[] = {&WS, &IDRES, &LONGREAL, &REAL, &INT, &RELOP, &CATCHALL, &ENDOFFILE};

MachineResult identifyToken(char* str) {
	int i;
	for(i = 0; i < sizeof(machines)/sizeof(machines[0]); i++) {
		MachineResult res = (*(machines[i]))(str);
		if(res.error) {
			res.type = T_LEXERR;
			res.attribute = res.error;
		}
		if(res.validToken) {
			int size = res.newString - str;
			res.lexeme = malloc(size*sizeof(char)+1);
			memcpy(res.lexeme, str, size);
			res.lexeme[size] = 0;
			return res;
		}
	}
	//unrecognized token
	MachineResult res;
	res.lexeme = malloc(2*sizeof(char));
	memcpy(res.lexeme, str, 1);
	res.lexeme[1] = 0;
	res.newString = str+1;
	res.type = T_LEXERR;
	res.attribute = res.error = ERR_UNRECOG_TOKEN;
	res.validToken = 0;
	return res;
}
