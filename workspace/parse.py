#!/usr/bin/python

import glob, os, re, sys
import numpy as np
import matplotlib.pyplot as plt

log_info = []
table = {}

enc_table = ["BPSK 1/2", "BPSK 3/4", "QPSK 1/2",
			 "QPSK 3/4", "16QAM 1/2", "16QAM 3/4", 
			 "64QAM 2/3", "64QAM 3/4"]

path = sys.argv[1]
ntrials = int(sys.argv[2])

snr_list = []

for filename in glob.glob(os.path.join(path, "*.log")):
	snr_arg = re.search(r"s([-+]?\d+.\d+)", filename)  	# SNR
	nfiles_arg = re.search(r"n(\d+)", filename)			# Nfiles
	enc_arg = re.search(r"e(\d+)", filename)			# Encoding
 	trial_arg = re.search(r"t(\d+)", filename)			# trial number

	try:
		assert snr_arg and nfiles_arg and enc_arg and trial_arg
	except AssertionError:
		print "Log filename parse failed; exiting!"
		sys.exit()		

	total = nfiles_arg.group(1)		
	pdrate = sum(1 for line in open(filename))/float(total)
	encode = int(enc_arg.group(1))
	snr = float(snr_arg.group(1))

	if snr not in snr_list:
		snr_list.append(snr)

	if encode not in table:
		table[encode] = {}
	if snr not in table[encode]:
		table[encode][snr] = []
	table[encode][snr].append(pdrate)  # could sum here but want to check
										# we got enough datapoints

for enc in table.keys(): 
	for snr in table[enc].keys():
		print table[enc][snr]
		try:
			assert len(table[enc][snr]) == ntrials
		except AssertionError: 
			print "You don't have enough datapoints; exiting!" 
			sys.exit()

snr_list.sort()

for enc in sorted(table.keys()):  # sort so legend labels are in order
	y = [sum(table[enc][snr])/float(ntrials) for snr in snr_list]

	print [table[enc][snr] for snr in snr_list]
	yerr = [np.std(table[enc][snr]) for snr in snr_list]

	print enc, snr_list, y
	print yerr
	plt.errorbar(snr_list, y, yerr=yerr, marker='o', label="%s" % enc_table[enc])

plt.ylabel("PDR")
plt.xlabel("SNR")
plt.title("Delivery rate vs. SNR for different encoding schemes")
plt.legend()
plt.savefig(os.path.join(path,"plot.png"))
plt.show()

