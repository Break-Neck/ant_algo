#!/usr/bin/python3
import sys, random

n, p = int(sys.argv[1]), float(sys.argv[2])
out = sys.argv[3] if len(sys.argv) >= 4 else "input.txt"
gr = [[(x + 1) % n] for x in range(n)]
for i in range(n):
	for j in range(i + 2, n):
		if random.random() <= p:
			gr[i].append(j)
with open(out, "wt") as of:
	of.write("{0} {1} 1 {2}\n".format(n, sum([len(x) for x in gr]), n))
	for _from in range(len(gr)):
		if len(gr[_from]) > 0:
			of.write("\n".join(("{0} {1} 1\n{1} {0} 1".format(_from + 1, x + 1) for x in gr[_from])))
			of.write("\n")
