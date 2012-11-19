
from optparse import OptionParser
import sys

parser = OptionParser()
parser.add_option("-0", "--initial",   dest="fnameInitial"  , action="store")
parser.add_option("-1", "--recursion", dest="fnameRecursion", action="store")
parser.add_option("-2", "--factoring", dest="fnameFactoring", action="store")
(options, args) = parser.parse_args(sys.argv)

LEFT_RECURSION_SHIV = LEFT_FACTORING_SHIV = "*"

def printSym(tnt):
	if not tnt: return '$\\epsilon$'
	if tnt[0] == 'N':
		return printVar(tnt)
	elif tnt[0] == 'T':
		return printTerm(tnt)
	else:
		sys.stderr.write("Danger danger Will Robinson\n")
		sys.stderr.write(tnt + "\n")
		sys.exit(0)
def printTerm(t):
	return '\\textbf{%s}' % {
		"T_PROGRAM": "program",
		"T_ID": "id",
		"T_VAR": "var",
		"T_ARRAY": "array",
		"T_NUM": "num",
		"T_OF": "of",
		"T_INTEGER": "integer",
		"T_REAL": "real",
		"T_PROCEDURE": "procedure",
		"T_BEGIN": "begin",
		"T_END": "end",
		"T_ASSIGNOP": ":=",
		"T_IF": "if",
		"T_THEN": "then",
		"T_ELSE": "else",
		"T_WHILE": "while",
		"T_DO": "do",
		"T_CALL": "call",
		"T_RELOP": "relop",
		"T_ADDOP": "addop",
		"T_MULOP": "mulop",
		"T_NOT": "not",
		"T_LPAREN": "(",
		"T_RPAREN": ")",
		"T_SEMICOLON": ";",
		"T_PERIOD": ".",
		"T_COMMA": ",",
		"T_COLON": ":",
		"T_LBRACK": "[",
		"T_RBRACK": "]",
		"T_PLUS": "+",
		"T_MINUS": "-",
		"T_DOUBLEPERIOD": ".."
	}[t]

def printVar(v):
	for i in reversed(range(len(v))):
		if v[i] in [LEFT_RECURSION_SHIV, LEFT_FACTORING_SHIV]:
			v = v[:i] + '\'' + v[i+1:]
		else: break

	return '\\textit{%s}' % v[3:].replace('_', '\\_').lower()

lastVar = None
def printGrammar(P):
	def printProd((l,r)):
		global lastVar
		if l != lastVar:
			lastVar = l
			return ("%s \\\\" % printVar(l)) + printProd((l,r))
		else:
			return "\\phantom{1in} $\\rightarrow$ %s" % (' '.join(printSym(t) for t in r))
	return ' \\\\ \n'.join(printProd(p) for p in P)

from rules import *

if options.fnameInitial: fInitial = open(options.fnameInitial, "w")
else: fInitial = None;
if options.fnameRecursion: fRecursion = open(options.fnameRecursion, "w")
else: fRecursion = None;
if options.fnameFactoring: fFactoring = open(options.fnameFactoring, "w")
else: fFactoring = None;

if fInitial: fInitial.write(printGrammar(sorted(P)) + "\n")

from massage import *
_, _, _, P1 = eliminate_left_recursion((V,T,S,P))

if fRecursion: fRecursion.write(printGrammar(sorted(P1)) + "\n")

_, _, _, P2 = perform_left_factoring(eliminate_left_recursion((V, T, S, P)))

if fFactoring: fFactoring.write(printGrammar(sorted(P2)) + "\n")
