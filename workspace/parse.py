#!/usr/bin/python

import glob, os, re, sys
import matplotlib.pyplot as plt

log_info = []
table = {}

enc_table = ["BPSK 1/2", "BPSK 3/4", "QPSK 1/2",
			 "QPSK 3/4", "16QAM 1/2", "16QAM 3/4", 
			 "64QAM 2/3", "64QAM 3/4"]

path = sys.argv[1]

for filename in glob.glob(os.path.join(path, "*.log")):
	snr_arg = re.search(r"s([-+]?\d+.\d+)", filename)  	# SNR
	nfiles_arg = re.search(r"n(\d+)", filename)			# Nfiles
	enc_arg = re.search(r"e(\d+)", filename)			# Encoding
 
	if snr_arg and nfiles_arg and enc_arg:
		total = nfiles_arg.group(1)		
		pdrate = sum(1 for line in open(filename))/float(total)
		encode = int(enc_arg.group(1))

		log_info.append((float(snr_arg.group(1)), pdrate, encode))

	else:
		print "Missing a log file--exiting!"
		sys.exit()

print log_info

for snr,pdr,enc in log_info:
	if enc not in table:
		table[enc] = [] 
	table[enc].append((snr,pdr)) 

print table

for enc in sorted(table.keys()):
	tmp = table[enc]	

	table[enc] = sorted(tmp, key=lambda tmp: tmp[0])
#	print tmp
	x = [snr for snr,pdr in table[enc]]
	y = [pdr for snr,pdr in table[enc]]

	print enc, x, y
	plt.plot(x, y, marker='o', label="%s" % enc_table[enc])

plt.ylabel("PDR")
plt.xlabel("SNR")
plt.title("Delivery rate vs. SNR for different encoding schemes")
plt.legend()
plt.show()
plt.savefig(os.path.join(path,"plot.png"))

