ifdef DIR
result_dir = $(DIR)
else 
DIR := $(shell date '+%m_%d__%H_%M_%S')
result_dir := result_$(DIR)
endif

trials := 10
packets := 50
min_snr := -5
max_snr := 15
snr_steps := 1
interval := 100

# logfile name: loop_(trials)_(npackets)_(snr)_(encoding)_interval 
LOGFILES=$(shell /bin/bash -c "echo $(result_dir)/loop_{1..$(trials)}_$(packets)_{$(min_snr)..$(max_snr)..$(snr_steps)}.0_{0..7}_$(interval).log")
PCAPFILES=$(shell /bin/bash -c "echo $(result_dir)/ofdm_{1..$(trials)}_$(packets)_{$(min_snr)..$(max_snr)..$(snr_steps)}.0_{0..7}_$(interval).pcap")

all: $(result_dir)/plot.png

# Will parse logfiles and create a plot
$(result_dir)/plot.png: $(LOGFILES)
	./parse.py $(result_dir) $(trials)

%.pcap:
	@echo $@
	$(eval PARAMS=$(shell python -c "print \"$@\".split(\"/\")[1]"))
	@echo PARAMS=$(PARAMS)
	$(eval TRIAL=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[1]"))
	$(eval NPKTS=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[2]"))
	$(eval SNR=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[3]"))
	$(eval ENC=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[4]"))
	$(eval INTVL=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[5][:-5]"))
	@echo TRIAL=$(TRIAL)
	@echo NPKTS=$(NPKTS)
	@echo SNR=$(SNR)
	@echo ENC=$(ENC)
	@echo INTVL=$(INTVL)
	$(shell mkdir -p $(result_dir))
	./wifi_loopback_noGUI.py -t $(TRIAL) -n $(NPKTS) -c 1 -s $(SNR) -o 0 -e $(ENC) -i $(INTVL) -r $(result_dir)
	sleep 1

%.log: $(PCAPFILES)
	@echo $@
	$(eval PARAMS=$(shell python -c "print \"$@\".split(\"/\")[1]"))
	@echo PARAMS=$(PARAMS)
	$(eval TRIAL=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[1]"))
	$(eval NPKTS=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[2]"))
	$(eval SNR=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[3]"))
	$(eval ENC=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[4]"))
	$(eval INTVL=$(shell python -c "print \"$(PARAMS)\".split(\"_\")[5][:-4]"))
	@echo TRIAL=$(TRIAL)
	@echo NPKTS=$(NPKTS)
	@echo SNR=$(SNR)
	@echo ENC=$(ENC)
	@echo INTVL=$(INTVL)
	tshark -r $(result_dir)/ofdm_$(TRIAL)_$(NPKTS)_$(SNR)_$(ENC)_$(INTVL).pcap > $@
