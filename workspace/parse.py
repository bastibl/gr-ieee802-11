#!/usr/bin/python

import glob, os, re
import matplotlib.pyplot as plt

x= []
y= []
tmp = []
table = {}

path = "result/"

for filename in glob.glob(os.path.join(path, "*.log")):
	pat = re.search(r"s([-+]?\d+.\d+)", filename)
	pat2 = re.search(r"n(\d+)", filename)
	pat3 = re.search(r"e(\d+)", filename)
 
	if pat and pat2 and pat3:
		
		receive = sum(1 for line in open(filename))
		total = pat2.group(1)
		encode = pat3.group(1)

		tmp.append((float(pat.group(1)), receive/float(total), encode))


for a,b,c in tmp:
	if c not in table:
		table[c] = [] 
	table[c].append((a,b)) 

print table

for c in table:
	tmp = []
	for a,b in table[c]:
		tmp.append((a,b))	

	tmp = sorted(tmp, key=lambda tmp: tmp[0])
#	print tmp
	x = []
	y = []
	for a,b in tmp:
		x.append(a)
		y.append(b)
	print x
	print y
	plt.plot(x,y, label = "encode " + str(c))
#	plt.text(x[-1], y[-1], "encode " + str(c))
#for a,b in tmp:
#	x.append(a)
#	y.append(b)
#print x
#print y
plt.ylabel("PDR")
plt.xlabel("SNR")
#plt.plot(x, y)
#plt.plot(x)
plt.legend()
plt.show()

