#!/usr/bin/python

import glob, os, re
import matplotlib.pyplot as plt

x= []
y= []

path = "result/"

for filename in glob.glob(os.path.join(path, "*.log")):
	pat = re.search(r"s(\d+.\d+)", filename)
	pat2 = re.search(r"n(\d+)", filename)
 
	if pat and pat2:
		x.append(float(pat.group(1)))
		
		receive = sum(1 for line in open(filename))
		total = pat2.group(1)

		y.append(receive/float(total))	

plt.ylabel("PDR")
plt.xlabel("SNR")
plt.plot(x, y)
plt.show()

