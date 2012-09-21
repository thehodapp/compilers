from rules import *
import copy

def remove_epsilon_productions((V, T, S, P)):
	#identify set of nullable variables
	Ve = set()
	while True:
		changed = False
		for (lhs, rhs) in P:
			if rhs == [""] or all(w in Ve for w in rhs):
				if lhs not in Ve:
					changed = True
					Ve.add(lhs)
		if not changed:
			break
	grammar_produces_e = (S in Ve)
	
	# use set of nullable variables
	new_P = [(lhs, rhs) for (lhs, rhs) in copy.copy(P) if rhs != [""]]
	for (lhs, rhs) in P:
		for i in range(len(rhs)):
			if rhs[i] in Ve:
				new_P.append((lhs, rhs[:i] + rhs[i+1:]))
	
	return (V, T, S, new_P)

def eliminate_left_recursion((V, T, S, P)):
	(V, T, S, P) = remove_epsilon_productions((V, T, S, P))

	new_P = copy.copy(P)
	toRemove = []
	new_V = copy.copy(V)
	for v in V:
		rhss = [r for (l, r) in P if l == v]
		if any(r[0] == v for r in rhss):
			# remove original rules
			print "%s has left recursion!" % v 
			for r in rhss:
				print "\t-> %s" % ' '.join(r)
				new_P.remove((v, r))
			# create new rules
			alpha = [r[1:] for r in rhss if r[0] == v]
			beta = [r for r in rhss if r[0] and r[0] != v]
			vprime = v
			while vprime in V or vprime in new_V:
				vprime = vprime + '\''
			new_V.append(vprime)
			print "\tAdding new variable %s" % vprime

			for b in beta:
				b.append(vprime)
				new_P.append((v, b))
				print "\t\tAdding new rule: %s -> %s" % (v, ' '.join(b))

			for a in alpha:
				a.append(vprime)
				new_P.append((vprime, a))
				print "\t\tAdding new rule: %s -> %s" % (vprime, ' '.join(a))
			print "\t\tAdding new rule: %s -> e" % vprime
			new_P.append((vprime, ['']))

	return (new_V, T, S, new_P)

def contains_epsilon_productions((V, T, S, P)):
	for (l, r) in P:
		if r == [""] and l != S:
			return True
	return False

def contains_immediate_left_recursion((V, T, S, P)):
	for (l, r) in P:
		if r[0] == l:
			return True
	return False

G = (V, T, S, P)
(Vh, Th, Sh, Ph) = H = eliminate_left_recursion(G)
if contains_immediate_left_recursion(H):
	print "WTFWTF"
