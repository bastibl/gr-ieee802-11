#!/usr/bin/python

import glob, os, re, sys
import numpy as np
import matplotlib.pyplot as plt
import scipy as sp
import scipy.stats as ss

log_info = []
table = {}

enc_table = ["BPSK 1/2", "BPSK 3/4", "QPSK 1/2",
			 "QPSK 3/4", "16QAM 1/2", "16QAM 3/4", 
			 "64QAM 2/3", "64QAM 3/4"]

path = sys.argv[1]
ntrials = int(sys.argv[2])

snr_list = []

def mean_confidence_interval(data, confidence=0.95):
	a = 1.0 * np.array(data)
	n = len(a)
	m, se = np.mean(a), ss.sem(a)
	h = se * ss.t._ppf((1 + confidence)/2., n-1)
	return m, h


for filename in glob.glob(os.path.join(path, "*.log")):
	params = filename.split("/")[1].split("_")
	print params

	try:
		trial = int(params[1])
		npkts = int(params[2])
		snr = float(params[3])
		encode = int(params[4])
	except AssertionError:
		print "Log filename parse failed; exiting!"
		sys.exit()		

	pdrate = sum(1 for line in open(filename))/float(npkts)

	if snr not in snr_list:
		snr_list.append(snr)

	if encode not in table:
		table[encode] = {}
	if snr not in table[encode]:
		table[encode][snr] = []
	table[encode][snr].append(pdrate)  

snr_list.sort()

for enc in sorted(table.keys()):  # sort so legend labels are in order
	# Check first that we got all datapoints for each trial
	for snr in table[enc].keys():
		try:
			assert len(table[enc][snr]) == ntrials
		except AssertionError:
			print "You don't have enough datapoints; exiting!"
			sys.exit()

	y = [mean_confidence_interval(table[enc][snr]) for snr in snr_list]

	# print enc_table[enc], snr_list, y
	plt.errorbar(snr_list, [m for m,h in y], 
		yerr=[h for m,h in y], 
		marker='o', 
		label="%s" % enc_table[enc])

plt.ylabel("PDR")
plt.xlabel("SNR")
plt.title("Delivery rate vs. SNR for different encoding schemes")
plt.legend()
plt.savefig(os.path.join(path,"plot.png"))
plt.show()

