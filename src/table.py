from firstfollow import *
from massage import *

Vh, Th = sorted(Vh), sorted(Th)
H = (Vh, Th, Sh, Ph)
fir = firsts()
fol = follows()

#construct table
pt = defaultdict(lambda: defaultdict(list))

def first(w):
	if w == []:
		return set([''])
	if w[0] in Th:
		return set([w[0]])
	if w[0] in Vh:
		if '' in fir[w[0]]:
			return (fir[w[0]] - set([''])) | first(w[1:])
		else:
			return fir[w[0]]

for (l,r) in Ph:
	for symb in first(r):
		if symb != '':
			pt[l][symb].append(r)
		else:
			for symb in fol[l]:
				if l == 'NT_STATEMENT'+LEFT_FACTORING_SHIV and symb == 'T_ELSE':
					# special-case: dangling else ambiguity
					pass
				else:
					pt[l][symb].append(r)

for v in Vh:
	for t in Th:
		if pt[v] and pt[l][t]:
			if len(pt[v][t]) > 1:
				print 'WTF'
				print v, t, pt[v][t]
				sys.exit()
