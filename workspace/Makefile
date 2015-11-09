all: 
	./wifi_loopback_noGUI.py -t $(t) -n $(n) -c $(c) -s $(s) -o $(o) -e $(e) -i $(i) -r $(result)
	tshark -r $(result)/ofdm_t$(t)_n$(n)_s$(s)_e$(e)_i$(i).pcap > $(result)/t$(t)_n$(n)_s$(s)_e$(e)_i$(i).log 