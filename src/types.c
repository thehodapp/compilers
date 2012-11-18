#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"

ReservedWordList* parseResWordFile(FILE* fp) {
	ReservedWordList *list = malloc(sizeof(ReservedWordList));
	ReservedWordList *hare = list;

	char word[11];
	int type;
	int attribute;
	while(fscanf(fp, "%s %d %d", word, &type, &attribute) != EOF) {
		ReservedWord *rw = malloc(sizeof(ReservedWord));
		rw->word = malloc((strlen(word) + 1)*sizeof(char));
		strcpy(rw->word, word);
		rw->type = type;
		rw->attribute = attribute;

		hare->rword = rw;
		hare->next = malloc(sizeof(ReservedWordList));
		hare->next->rword = NULL;
		hare = hare->next;
	}
	return list;
}

char* convertConstantToString(enum Symbol constant) {
	switch(constant) {
#define X(a,b) case a: return #a;
		SYMBS;
#undef X
	default: return "UNKNOWN";
	}
}
