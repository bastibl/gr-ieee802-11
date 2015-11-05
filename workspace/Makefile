all: 
	./wifi_loopback_noGUI.py -n $(n) -c $(c) -s $(s) -o $(o) -e $(e) -i $(i) -r $(result)
	tshark -r $(result)/ofdm_n$(n)_s$(s)_e$(e)_i$(i).pcap > $(result)/n$(n)_s$(s)_e$(e)_i$(i).log 


