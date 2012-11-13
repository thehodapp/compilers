from massage import *
from collections import defaultdict
import pprint
import copy

G = (V, T, S, P)
H = (Vh, Th, Sh, Ph) = perform_left_factoring(eliminate_left_recursion(G))
#H = (Vh, Th, Sh, Ph) = (V, T, S, P)
#print Vh, Th

if __name__ == '__main__':
	print '\n'.join("%s -> %s" % (l,r) for (l,r) in Ph)
	print

def firsts():
	fir = defaultdict(set)
	for t in Th: fir[t] = set([t])
	while True:
		old = copy.deepcopy(fir)
		for (l,r) in Ph:
			if r == [''] or r == []:
				fir[l].add('')
			else:
				if '' not in fir[r[0]]:
					fir[l].update(fir[r[0]])
				else:
					i = 0
					while i < len(r) and ('' in fir[r[i]] or r[i] in Th):
						fir[l].update(fir[r[i]] - set(['']))
						i += 1
				if all('' in fir[ri] for ri in r):
					fir[l].add('')
		if old == fir:
			break
	return fir

def follows():
	fir = firsts()
	fol = defaultdict(set)
	fol[Sh].add('T_EOF')
	while True:
		old = copy.deepcopy(fol)
		for (l,r) in Ph:
			for i in range(len(r)):
				B = r[i]
				if B in Vh and i+1 < len(r):
					if r[i+1] in Th:
						fol[B].add(r[i+1])
					else:
						j = i+1
						while j < len(r) and '' in fir[r[j]]:
							fol[B].update(fir[r[j]] - set(['']))
							j += 1

					if all((ri, '') in Ph for ri in r[i:]):
						fol[B].update(fol[l])
				elif B in Vh and i == len(r)-1:
					fol[B].update(fol[l])
		if old == fol:
			break
	return fol

if __name__ == '__main__':
	print '\n'.join("first[%s] = %s" % (v, ', '.join(map(repr,s))) for (v,s) in firsts().items() if v in Vh)
	print
	print '\n'.join("follow[%s] = %s" % (v, ', '.join(map(repr,s))) for (v,s) in follows().items() if v in Vh)
