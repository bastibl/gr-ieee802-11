#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Wifi Loopback
# Generated: Sat Oct 31 12:11:06 2015
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


class wifi_loopback(gr.top_block):

    def __init__(self, chan_est=1, encoding=0, freq_offset=0, pdu_length=500, snr=10, interval=500):
        gr.top_block.__init__(self, "Wifi Loopback")

        ##################################################
        # Parameters
        ##################################################
        self.chan_est = chan_est
        self.encoding = encoding
        self.freq_offset = freq_offset
        self.pdu_length = pdu_length
        self.snr = snr
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
        self.ieee802_11_ofdm_parse_mac_0 = ieee802_11.ofdm_parse_mac(False, True)
        self.ieee802_11_ofdm_mac_0 = ieee802_11.ofdm_mac(([0x23, 0x23, 0x23, 0x23, 0x23, 0x23]), ([0x42, 0x42, 0x42, 0x42, 0x42, 0x42]), ([0xff, 0xff, 0xff, 0xff, 0xff, 0xff]))
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
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.float_t, "packet_len")
        self.blocks_null_sink_1 = blocks.null_sink(gr.sizeof_float*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc(((10**(snr/10.0))**.5, ))
        self.blocks_message_strobe_0 = blocks.message_strobe(pmt.intern("".join("x" for i in range(pdu_length))), interval)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_message_strobe_0, 'strobe'), (self.ieee802_11_ofdm_mac_0, 'app in'))    
        self.msg_connect((self.ieee802_11_ofdm_mac_0, 'phy out'), (self.wifi_phy_hier_0, 'mac_in'))    
        self.msg_connect((self.ieee802_11_ofdm_parse_mac_0, 'fer'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))    
        self.msg_connect((self.wifi_phy_hier_0, 'mac_out'), (self.ieee802_11_ofdm_parse_mac_0, 'in'))    
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.channels_channel_model_0, 0))    
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.blocks_null_sink_1, 0))    
        self.connect((self.channels_channel_model_0, 0), (self.wifi_phy_hier_0, 0))    
        self.connect((self.foo_packet_pad2_0, 0), (self.blocks_multiply_const_vxx_0, 0))    
        self.connect((self.wifi_phy_hier_0, 1), (self.blocks_null_sink_0, 0))    
        self.connect((self.wifi_phy_hier_0, 0), (self.foo_packet_pad2_0, 0))    

    def get_chan_est(self):
        return self.chan_est

    def set_chan_est(self, chan_est):
        self.chan_est = chan_est
        self.wifi_phy_hier_0.set_chan_est(self.chan_est)

    def get_encoding(self):
        return self.encoding

    def set_encoding(self, encoding):
        self.encoding = encoding
        self.wifi_phy_hier_0.set_encoding(self.encoding)

    def get_freq_offset(self):
        return self.freq_offset

    def set_freq_offset(self, freq_offset):
        self.freq_offset = freq_offset
        self.channels_channel_model_0.set_frequency_offset(self.freq_offset)

    def get_pdu_length(self):
        return self.pdu_length

    def set_pdu_length(self, pdu_length):
        self.pdu_length = pdu_length
        self.blocks_message_strobe_0.set_msg(pmt.intern("".join("x" for i in range(self.pdu_length))))

    def get_snr(self):
        return self.snr

    def set_snr(self, snr):
        self.snr = snr
        self.blocks_multiply_const_vxx_0.set_k(((10**(self.snr/10.0))**.5, ))

    def get_interval(self):
        return self.interval

    def set_interval(self, interval):
        self.interval = interval
        self.blocks_message_strobe_0.set_period(self.interval)

    def get_out_buf_size(self):
        return self.out_buf_size

    def set_out_buf_size(self, out_buf_size):
        self.out_buf_size = out_buf_size


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--chan-est", dest="chan_est", type="intx", default=1,
        help="Set chan_est [default=%default]")
    parser.add_option(
        "", "--encoding", dest="encoding", type="intx", default=0,
        help="Set encoding [default=%default]")
    parser.add_option(
        "", "--freq-offset", dest="freq_offset", type="eng_float", default=eng_notation.num_to_str(0),
        help="Set freq_offset [default=%default]")
    parser.add_option(
        "", "--pdu-length", dest="pdu_length", type="intx", default=500,
        help="Set pdu_length [default=%default]")
    parser.add_option(
        "", "--snr", dest="snr", type="eng_float", default=eng_notation.num_to_str(10),
        help="Set snr [default=%default]")
    parser.add_option(
        "", "--interval", dest="interval", type="intx", default=500,
        help="Set interval [default=%default]")
    return parser


def main(top_block_cls=wifi_loopback, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(chan_est=options.chan_est, encoding=options.encoding, freq_offset=options.freq_offset, pdu_length=options.pdu_length, snr=options.snr, interval=options.interval)
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
