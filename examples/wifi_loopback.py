#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Wifi Loopback
# Generated: Sat Apr  9 20:40:28 2016
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from gnuradio import blocks
from gnuradio import channels
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.wxgui import forms
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
from wifi_phy_hier import wifi_phy_hier  # grc-generated hier_block
import foo
import ieee802_11
import math
import wx


class wifi_loopback(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Wifi Loopback")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.snr = snr = 30
        self.pdu_length = pdu_length = 500
        self.out_buf_size = out_buf_size = 96000
        self.freq_offset = freq_offset = 0
        self.encoding = encoding = 5
        self.chan_est = chan_est = 0

        ##################################################
        # Blocks
        ##################################################
        _snr_sizer = wx.BoxSizer(wx.VERTICAL)
        self._snr_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_snr_sizer,
        	value=self.snr,
        	callback=self.set_snr,
        	label='snr',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._snr_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_snr_sizer,
        	value=self.snr,
        	callback=self.set_snr,
        	minimum=-15,
        	maximum=30,
        	num_steps=1000,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_snr_sizer)
        _pdu_length_sizer = wx.BoxSizer(wx.VERTICAL)
        self._pdu_length_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_pdu_length_sizer,
        	value=self.pdu_length,
        	callback=self.set_pdu_length,
        	label='pdu_length',
        	converter=forms.int_converter(),
        	proportion=0,
        )
        self._pdu_length_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_pdu_length_sizer,
        	value=self.pdu_length,
        	callback=self.set_pdu_length,
        	minimum=0,
        	maximum=1500,
        	num_steps=1000,
        	style=wx.SL_HORIZONTAL,
        	cast=int,
        	proportion=1,
        )
        self.Add(_pdu_length_sizer)
        _freq_offset_sizer = wx.BoxSizer(wx.VERTICAL)
        self._freq_offset_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_freq_offset_sizer,
        	value=self.freq_offset,
        	callback=self.set_freq_offset,
        	label='freq_offset',
        	converter=forms.float_converter(),
        	proportion=0,
        )
        self._freq_offset_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_freq_offset_sizer,
        	value=self.freq_offset,
        	callback=self.set_freq_offset,
        	minimum=-.1,
        	maximum=.1,
        	num_steps=1000,
        	style=wx.SL_HORIZONTAL,
        	cast=float,
        	proportion=1,
        )
        self.Add(_freq_offset_sizer)
        self._encoding_chooser = forms.radio_buttons(
        	parent=self.GetWin(),
        	value=self.encoding,
        	callback=self.set_encoding,
        	label="Encoding",
        	choices=[0,1,2,3,4,5,6,7],
        	labels=["BPSK 1/2", "BPSK 3/4", "QPSK 1/2", "QPSK 3/4", "16QAM 1/2", "16QAM 3/4", "64QAM 2/3", "64QAM 3/4"],
        	style=wx.RA_HORIZONTAL,
        )
        self.Add(self._encoding_chooser)
        self._chan_est_chooser = forms.radio_buttons(
        	parent=self.GetWin(),
        	value=self.chan_est,
        	callback=self.set_chan_est,
        	label='chan_est',
        	choices=[0, 1],
        	labels=["LMS", "Linear Comb"],
        	style=wx.RA_HORIZONTAL,
        )
        self.Add(self._chan_est_chooser)
        self.wifi_phy_hier_0 = wifi_phy_hier(
            chan_est=chan_est,
            encoding=encoding,
            sensitivity=0.56,
        )
        self.ieee802_11_ofdm_mac_0_0 = ieee802_11.ofdm_mac(([0x23, 0x23, 0x23, 0x23, 0x23, 0x23]), ([0x42, 0x42, 0x42, 0x42, 0x42, 0x42]), ([0xff, 0xff, 0xff, 0xff, 0xff, 0xff]))
        (self.ieee802_11_ofdm_mac_0_0).set_min_output_buffer(1000000)
        self.ieee802_11_ofdm_mac_0 = ieee802_11.ofdm_mac(([0x23, 0x23, 0x23, 0x23, 0x23, 0x23]), ([0x42, 0x42, 0x42, 0x42, 0x42, 0x42]), ([0xff, 0xff, 0xff, 0xff, 0xff, 0xff]))
        (self.ieee802_11_ofdm_mac_0).set_min_output_buffer(1000000)
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
        self.blocks_tagged_stream_to_pdu_0 = blocks.tagged_stream_to_pdu(blocks.byte_t, "packet_len")
        (self.blocks_tagged_stream_to_pdu_0).set_min_output_buffer(1000000)
        self.blocks_stream_to_tagged_stream_0 = blocks.stream_to_tagged_stream(gr.sizeof_char, 1, pdu_length, "packet_len")
        (self.blocks_stream_to_tagged_stream_0).set_min_output_buffer(1000000)
        self.blocks_pdu_to_tagged_stream_0_0_0 = blocks.pdu_to_tagged_stream(blocks.byte_t, "packet_len")
        (self.blocks_pdu_to_tagged_stream_0_0_0).set_min_output_buffer(1000000)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc(((10**(snr/10.0))**.5, ))
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_char*1, "/home/gonzalo/police.tar.gz", False)
        self.blocks_file_sink_1 = blocks.file_sink(gr.sizeof_char*1, "/home/gonzalo/Proyecto/out", False)
        self.blocks_file_sink_1.set_unbuffered(True)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_tagged_stream_to_pdu_0, 'pdus'), (self.ieee802_11_ofdm_mac_0, 'app in'))    
        self.msg_connect((self.ieee802_11_ofdm_mac_0, 'phy out'), (self.wifi_phy_hier_0, 'mac_in'))    
        self.msg_connect((self.ieee802_11_ofdm_mac_0_0, 'app out'), (self.blocks_pdu_to_tagged_stream_0_0_0, 'pdus'))    
        self.msg_connect((self.wifi_phy_hier_0, 'mac_out'), (self.ieee802_11_ofdm_mac_0_0, 'phy in'))    
        self.connect((self.blocks_file_source_0, 0), (self.blocks_stream_to_tagged_stream_0, 0))    
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.channels_channel_model_0, 0))    
        self.connect((self.blocks_pdu_to_tagged_stream_0_0_0, 0), (self.blocks_file_sink_1, 0))    
        self.connect((self.blocks_stream_to_tagged_stream_0, 0), (self.blocks_tagged_stream_to_pdu_0, 0))    
        self.connect((self.channels_channel_model_0, 0), (self.wifi_phy_hier_0, 0))    
        self.connect((self.foo_packet_pad2_0, 0), (self.blocks_multiply_const_vxx_0, 0))    
        self.connect((self.wifi_phy_hier_0, 1), (self.blocks_null_sink_0, 0))    
        self.connect((self.wifi_phy_hier_0, 0), (self.foo_packet_pad2_0, 0))    

    def get_snr(self):
        return self.snr

    def set_snr(self, snr):
        self.snr = snr
        self._snr_slider.set_value(self.snr)
        self._snr_text_box.set_value(self.snr)
        self.blocks_multiply_const_vxx_0.set_k(((10**(self.snr/10.0))**.5, ))

    def get_pdu_length(self):
        return self.pdu_length

    def set_pdu_length(self, pdu_length):
        self.pdu_length = pdu_length
        self.blocks_stream_to_tagged_stream_0.set_packet_len(self.pdu_length)
        self.blocks_stream_to_tagged_stream_0.set_packet_len_pmt(self.pdu_length)
        self._pdu_length_slider.set_value(self.pdu_length)
        self._pdu_length_text_box.set_value(self.pdu_length)

    def get_out_buf_size(self):
        return self.out_buf_size

    def set_out_buf_size(self, out_buf_size):
        self.out_buf_size = out_buf_size

    def get_freq_offset(self):
        return self.freq_offset

    def set_freq_offset(self, freq_offset):
        self.freq_offset = freq_offset
        self._freq_offset_slider.set_value(self.freq_offset)
        self._freq_offset_text_box.set_value(self.freq_offset)
        self.channels_channel_model_0.set_frequency_offset(self.freq_offset)

    def get_encoding(self):
        return self.encoding

    def set_encoding(self, encoding):
        self.encoding = encoding
        self.wifi_phy_hier_0.set_encoding(self.encoding)
        self._encoding_chooser.set_value(self.encoding)

    def get_chan_est(self):
        return self.chan_est

    def set_chan_est(self, chan_est):
        self.chan_est = chan_est
        self._chan_est_chooser.set_value(self.chan_est)
        self.wifi_phy_hier_0.set_chan_est(self.chan_est)


def main(top_block_cls=wifi_loopback, options=None):

    tb = top_block_cls()
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
