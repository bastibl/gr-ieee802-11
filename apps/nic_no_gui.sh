#!/bin/bash

FLOWGRAPH="ofdm_transceiver_no_gui.py"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


### create tap interface
if [[ `ifconfig -a | grep tap0 | wc -l` -eq 0 ]]
then
	sudo ip tuntap add dev tap0 user ${USER} mode tap
fi

### reconfigure it in any case, just to be sure it's up
sudo ifconfig tap0 down
sudo ifconfig tap0 hw ether 12:34:56:78:90:ab
sudo ifconfig tap0 mtu 260
sudo ifconfig tap0 up
sudo ifconfig tap0 192.168.123.1

sudo route del -net 192.168.123.0/24
sudo route add -net 192.168.123.0/24 mss 200 dev tap0

sudo tc qdisc del dev tap0 root
sudo tc qdisc add dev tap0 root netem delay 100ms

sudo arp -s 192.168.123.2 30:14:4a:e6:46:e4


### start transceiver
cd ${DIR}
cd ../examples/
./${FLOWGRAPH}


