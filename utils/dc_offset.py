#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Dc Offset
# Generated: Sat Jan  3 17:36:59 2015
##################################################

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import time

class dc_offset(gr.top_block):

    def __init__(self, dc=0):
        gr.top_block.__init__(self, "Dc Offset")

        ##################################################
        # Parameters
        ##################################################
        self.dc = dc

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 1000000

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0.set_samp_rate(samp_rate)
        self.uhd_usrp_sink_0.set_center_freq(2.41e9, 0)
        self.uhd_usrp_sink_0.set_gain(0, 0)
        self.uhd_usrp_sink_0.set_dc_offset(bool(dc), 0)
        self.blocks_null_source_0 = blocks.null_source(gr.sizeof_gr_complex*1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_null_source_0, 0), (self.uhd_usrp_sink_0, 0))



    def get_dc(self):
        return self.dc

    def set_dc(self, dc):
        self.dc = dc

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate)

if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option("", "--dc", dest="dc", type="intx", default=0,
        help="Set dc [default=%default]")
    (options, args) = parser.parse_args()
    tb = dc_offset(dc=options.dc)
    tb.start()
    raw_input('Press Enter to quit: ')
    tb.stop()
    tb.wait()
