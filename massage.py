from rules import *
import copy
import itertools

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
				new_rule = (lhs, rhs[:i] + rhs[i+1:])
				if new_rule not in new_P:
					new_P.append(new_rule)
	
	return (V, T, S, new_P)

def eliminate_left_recursion((V, T, S, P)):
	(V, T, S, P) = remove_epsilon_productions((V, T, S, P))

	new_P = copy.copy(P)
	new_V = copy.copy(V)
	for v in V:
		rhss = [r for (l, r) in P if l == v]
		if any(r[0] == v for r in rhss):
			# remove original rules
			for r in rhss:
				new_P.remove((v, r))
			# create new rules
			alpha = [r[1:] for r in rhss if r[0] == v]
			beta = [r for r in rhss if r[0] and r[0] != v]
			vprime = v
			while vprime in V + new_V:
				vprime += '\''
			new_V.append(vprime)

			for b in beta:
				new_P.append((v, b + [vprime]))

			for a in alpha:
				new_P.append((vprime, a + [vprime]))
			new_P.append((vprime, ['']))

	return (new_V, T, S, new_P)

def perform_left_factoring((V, T, S, P)):
	
	def common_starting_subseq(s, t):
		subseq = []
		for (e1, e2) in zip(s, t):
			if e1 == e2:
				subseq.append(e1)
			else:
				return subseq
		return subseq
	
	def longest_common_start(seqs):
		if len(seqs) < 2: return []
		else: return max([common_starting_subseq(s1, s2) for (s1, s2) in itertools.combinations(seqs, 2)], key=len)
	
	def startsWith(seq, subseq):
		for (e1, e2) in zip(seq, subseq):
			if e1 != e2:
				return False
		return True
	
	def obtainConflict(rules, var):
		start = longest_common_start([r for (l,r) in rules if l == var])
		if start: return start, [r for (l,r) in rules if l == var and startsWith(r, start)]
		else: return False

	new_P = copy.copy(P)
	new_V = copy.copy(V)

	for var in new_V:
		conflict = obtainConflict(new_P, var)
		while conflict:
			varprime = var
			while varprime in new_V:
				varprime += '*'
			new_V.append(varprime)
			alpha, rhss = conflict
			
			new_P.append((var, alpha + [varprime]))
			for rhs in rhss:
				new_P.remove((var, rhs))
				beta = rhs[len(alpha):]
				new_P.append((varprime, beta))

			conflict = obtainConflict(new_P, var)

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
(Vj, Tj, Sj, Pj) = J = perform_left_factoring(H)
