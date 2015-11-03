#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Wifi Loopback Nogui
# Generated: Tue Nov  3 15:44:35 2015
##################################################

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from gnuradio import blocks
from gnuradio import channels
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
from wifi_phy_hier import wifi_phy_hier  # grc-generated hier_block
import foo
import ieee802_11
import math
import pmt


class wifi_loopback_noGUI(gr.top_block):

    def __init__(self, nmessages=50, chan_est=1, snr=10, freq_offset=0, encoding=0, interval=500):
        gr.top_block.__init__(self, "Wifi Loopback Nogui")

        ##################################################
        # Parameters
        ##################################################
        self.nmessages = nmessages
        self.chan_est = chan_est
        self.snr = snr
        self.freq_offset = freq_offset
        self.encoding = encoding
        self.interval = interval

        ##################################################
        # Variables
        ##################################################
        self.out_buf_size = out_buf_size = 96000

        ##################################################
        # Blocks
        ##################################################
        self.wifi_phy_hier_0 = wifi_phy_hier(
            encoding=encoding,
            chan_est=chan_est,
        )
        self.ieee802_11_ofdm_mac_0 = ieee802_11.ofdm_mac(([0x23, 0x23, 0x23, 0x23, 0x23, 0x23]), ([0x42, 0x42, 0x42, 0x42, 0x42, 0x42]), ([0xff, 0xff, 0xff, 0xff, 0xff, 0xff]))
        self.foo_wireshark_connector_0 = foo.wireshark_connector(127, False)
        self.foo_periodic_msg_source_0 = foo.periodic_msg_source(pmt.intern("Hello World!"), interval, nmessages, True, False)
        self.foo_packet_pad2_0 = foo.packet_pad2(False, False, 0.001, 500, 0)
        (self.foo_packet_pad2_0).set_min_output_buffer(96000)
        self.channels_channel_model_0 = channels.channel_model(
        	noise_voltage=.5**.5,
        	frequency_offset=freq_offset,
        	epsilon=1.0,
        	taps=(1.0, ),
        	noise_seed=0,
        	block_tags=False
        )
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc(((10**(snr/10.0))**.5, ))
        self.blocks_file_sink_0_0 = blocks.file_sink(gr.sizeof_char*1, "/tmp/ofdm_n" + str(nmessages) + "_s" + str(snr) + "_e" + str(encoding) + "_i" + str(interval) + ".pcap", False)
        self.blocks_file_sink_0_0.set_unbuffered(True)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.foo_periodic_msg_source_0, 'out'), (self.foo_wireshark_connector_0, 'in'))    
        self.msg_connect((self.foo_periodic_msg_source_0, 'out'), (self.ieee802_11_ofdm_mac_0, 'app in'))    
        self.msg_connect((self.ieee802_11_ofdm_mac_0, 'phy out'), (self.wifi_phy_hier_0, 'mac_in'))    
        self.msg_connect((self.wifi_phy_hier_0, 'mac_out'), (self.foo_wireshark_connector_0, 'in'))    
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.channels_channel_model_0, 0))    
        self.connect((self.channels_channel_model_0, 0), (self.wifi_phy_hier_0, 0))    
        self.connect((self.foo_packet_pad2_0, 0), (self.blocks_multiply_const_vxx_0, 0))    
        self.connect((self.foo_wireshark_connector_0, 0), (self.blocks_file_sink_0_0, 0))    
        self.connect((self.wifi_phy_hier_0, 1), (self.blocks_null_sink_0, 0))    
        self.connect((self.wifi_phy_hier_0, 0), (self.foo_packet_pad2_0, 0))    

    def get_nmessages(self):
        return self.nmessages

    def set_nmessages(self, nmessages):
        self.nmessages = nmessages
        self.blocks_file_sink_0_0.open("/tmp/ofdm_n" + str(self.nmessages) + "_s" + str(self.snr) + "_e" + str(self.encoding) + "_i" + str(self.interval) + ".pcap")

    def get_chan_est(self):
        return self.chan_est

    def set_chan_est(self, chan_est):
        self.chan_est = chan_est
        self.wifi_phy_hier_0.set_chan_est(self.chan_est)

    def get_snr(self):
        return self.snr

    def set_snr(self, snr):
        self.snr = snr
        self.blocks_multiply_const_vxx_0.set_k(((10**(self.snr/10.0))**.5, ))
        self.blocks_file_sink_0_0.open("/tmp/ofdm_n" + str(self.nmessages) + "_s" + str(self.snr) + "_e" + str(self.encoding) + "_i" + str(self.interval) + ".pcap")

    def get_freq_offset(self):
        return self.freq_offset

    def set_freq_offset(self, freq_offset):
        self.freq_offset = freq_offset
        self.channels_channel_model_0.set_frequency_offset(self.freq_offset)

    def get_encoding(self):
        return self.encoding

    def set_encoding(self, encoding):
        self.encoding = encoding
        self.wifi_phy_hier_0.set_encoding(self.encoding)
        self.blocks_file_sink_0_0.open("/tmp/ofdm_n" + str(self.nmessages) + "_s" + str(self.snr) + "_e" + str(self.encoding) + "_i" + str(self.interval) + ".pcap")

    def get_interval(self):
        return self.interval

    def set_interval(self, interval):
        self.interval = interval
        self.blocks_file_sink_0_0.open("/tmp/ofdm_n" + str(self.nmessages) + "_s" + str(self.snr) + "_e" + str(self.encoding) + "_i" + str(self.interval) + ".pcap")

    def get_out_buf_size(self):
        return self.out_buf_size

    def set_out_buf_size(self, out_buf_size):
        self.out_buf_size = out_buf_size


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "-n", "--nmessages", dest="nmessages", type="intx", default=50,
        help="Set nmessages [default=%default]")
    parser.add_option(
        "-c", "--chan-est", dest="chan_est", type="intx", default=1,
        help="Set chan_est [default=%default]")
    parser.add_option(
        "-s", "--snr", dest="snr", type="eng_float", default=eng_notation.num_to_str(10),
        help="Set snr [default=%default]")
    parser.add_option(
        "-o", "--freq-offset", dest="freq_offset", type="eng_float", default=eng_notation.num_to_str(0),
        help="Set freq_offset [default=%default]")
    parser.add_option(
        "-e", "--encoding", dest="encoding", type="intx", default=0,
        help="Set encoding [default=%default]")
    parser.add_option(
        "-i", "--interval", dest="interval", type="intx", default=500,
        help="Set interval [default=%default]")
    return parser


def main(top_block_cls=wifi_loopback_noGUI, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(nmessages=options.nmessages, chan_est=options.chan_est, snr=options.snr, freq_offset=options.freq_offset, encoding=options.encoding, interval=options.interval)
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
