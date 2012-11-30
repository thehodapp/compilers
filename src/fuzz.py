from rules import *
from collections import defaultdict
import sys
import random
import textwrap
import rstr

if len(sys.argv) > 1:
	suggLen = int(sys.argv[1])
else:
	suggLen = 0

rwords = [
	'program',
	'var',
	'array',
	'of',
	'integer',
	'real',
	'procedure',
	'begin',
	'end',
	'if',
	'then',
	'else',
	'while',
	'do',
	'not',
	'call',
	'div',
	'mod',
	'and',
	'or',
]

def genID():
	att = rstr.xeger(r'[a-zA-Z][a-zA-Z0-9]{1,9}')
	if att in rwords:
		return genID()
	else:
		return att

patterns = {
	"T_PROGRAM": lambda: rstr.xeger(r'program'),
	"T_ID": genID,
	"T_VAR": lambda: rstr.xeger(r'var'),
	"T_ARRAY": lambda: rstr.xeger(r'array'),
	"T_NUM": lambda: rstr.xeger(r'(0)|([1-9]\d{0,9})|(\d{1,5}\.\d{1,5})|(\d{1,5}\.\d{1,5}E[1-9]\d?)'),
	"T_OF": lambda: rstr.xeger(r'of'),
	"T_INTEGER": lambda: rstr.xeger(r'integer'),
	"T_REAL": lambda: rstr.xeger(r'real'),
	"T_PROCEDURE": lambda: rstr.xeger(r'procedure'),
	"T_BEGIN": lambda: rstr.xeger(r'begin'),
	"T_END": lambda: rstr.xeger(r'end'),
	"T_ASSIGNOP": lambda: rstr.xeger(r':='),
	"T_IF": lambda: rstr.xeger(r'if'),
	"T_THEN": lambda: rstr.xeger(r'then'),
	"T_ELSE": lambda: rstr.xeger(r'else'),
	"T_WHILE": lambda: rstr.xeger(r'while'),
	"T_DO": lambda: rstr.xeger(r'do'),
	"T_CALL": lambda: rstr.xeger(r'call'),
	"T_RELOP": lambda: rstr.xeger(r'(=)|([<>]=?)|(<>)'),
	"T_ADDOP": lambda: rstr.xeger(r'\+|-|(or)'),
	"T_MULOP": lambda: rstr.xeger(r'\*|/|(div)|(mod)|(and)'),
	"T_NOT": lambda: rstr.xeger(r'not'),
	"T_LPAREN": lambda: rstr.xeger(r'\('),
	"T_RPAREN": lambda: rstr.xeger(r'\)'),
	"T_SEMICOLON": lambda: rstr.xeger(r';'),
	"T_PERIOD": lambda: rstr.xeger(r'\.'),
	"T_COMMA": lambda: rstr.xeger(r','),
	"T_COLON": lambda: rstr.xeger(r':'),
	"T_LBRACK": lambda: rstr.xeger(r'\['),
	"T_RBRACK": lambda: rstr.xeger(r'\]'),
	"T_PLUS": lambda: rstr.xeger(r'\+'),
	"T_MINUS": lambda: rstr.xeger(r'-'),
	"T_DOUBLEPERIOD": lambda: rstr.xeger(r'\.{2}'),
	"": lambda: rstr.xeger(r''),
}

pgmm = ['NT_PROGRAM']

bannedRules = [
	("NT_SIMPLE_EXPRESSION", "NT_SIGN NT_TERM".split()),
]

def translate(pgm):
	if pgm in T:
		return patterns[pgm]()
	else:
		return [translate(l) for l in pgm]

pgm = ['NT_PROGRAM']
while not all(sym in T for sym in pgm):
	grow = (len(pgm) < suggLen) or (not suggLen)
	newPgm = []
	for sym in pgm:
		if sym in T:
			newPgm.append(sym)
		elif sym in V:
			rhss = [r for (l,r) in P if l == sym and (l,r) not in bannedRules]
			if not grow and [''] in rhss:
				rhs = ['']
			elif not grow:
				rhs = sorted(rhss, key=len)[0]
			else:
				rhs = random.choice(rhss)
			newPgm.extend(rhs)
	pgm = newPgm
print textwrap.fill(' '.join(filter(lambda a: a, translate(pgm))))
