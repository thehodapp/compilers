#include<stdlib.h>
#include "item.h"
#include "symtab.h"

Item* createItem() {
	Item *a = malloc(sizeof(Item));
		a->in.count = 0;
		a->in.proc = NULL;
			a->in.type.st_type = NONE;
			a->in.type.isArray = false;
			a->in.type.isParam = false;
			a->in.type.low = 0;
			a->in.type.high = 0;
		a->type.st_type = NONE;
		a->type.isArray = false;
		a->type.isParam = false;
		a->type.low = 0;
		a->type.high = 0;
	a->lexeme = NULL;
	a->error = false;
	a->errHere = false;
	return a;
}
