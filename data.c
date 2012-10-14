
#include "data.h"

extern int cTerminals = 3;
extern int cNonTerminals = 6;
extern Symbol*** parseTable = \
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	{
		{
			NT_P,
			T_LPAREN,
			NT_E,
			T_RPAREN,
		},
		NULL,
		NULL,
		{
			NT_P,
			T_LPAREN,
			NT_E,
			T_RPAREN,
		},
		{
			T_V,
			NT_T,
		},
		NULL,
	},
	{
		{},
		NULL,
		NULL,
		{
			T_F,
		},
		NULL,
		NULL,
	},
	{
		NULL,
		{
			NT_E,
		},
		{
			T_PLUS,
			NT_E,
		},
		NULL,
		NULL,
		{},
	},
};

extern int** parseTableLengths = {
	{4, 0, 0, 4, 2, 0},
	{0, 0, 0, 1, 0, 0},
	{0, 1, 2, 0, 0, 0},
};

extern Symbol** synchSets = {
	{T_EOF, T_RPAREN, T_P},
	{T_LPAREN},
	{T_EOF, T_RPAREN, T_P},
};

extern int** synchSetLengths = { 3, 1, 3};
