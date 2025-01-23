#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: tetraDMO_Decoder_v1
# Author: ctn008
# GNU Radio version: 3.10.10.0

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio import TETRA_DMO
from gnuradio import audio
from gnuradio import blocks
import pmt
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import sip
import tetraDMO_Decoder_v1_epy_block_0_0_0 as epy_block_0_0_0  # embedded python block
import tetraDMO_Decoder_v1_epy_block_0_2_0_0 as epy_block_0_2_0_0  # embedded python block



class tetraDMO_Decoder_v1(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "tetraDMO_Decoder_v1", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("tetraDMO_Decoder_v1")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except BaseException as exc:
            print(f"Qt GUI: Could not set Icon: {str(exc)}", file=sys.stderr)
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "tetraDMO_Decoder_v1")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)

        ##################################################
        # Variables
        ##################################################
        self.view_step = view_step = 512*1
        self.samp_rate = samp_rate = 36000
        self.next_view = next_view = 0
        self.audio_rate = audio_rate = 8000

        ##################################################
        # Blocks
        ##################################################

        self.qtgui_time_sink_x_0_0_0 = qtgui.time_sink_f(
            2000, #size
            audio_rate, #samp_rate
            "speech output", #name
            1, #number of inputs
            None # parent
        )
        self.qtgui_time_sink_x_0_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_0_0_0.set_y_axis(-1.2, 1.2)

        self.qtgui_time_sink_x_0_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_0_0_0.enable_tags(True)
        self.qtgui_time_sink_x_0_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_0_0_0.enable_autoscale(False)
        self.qtgui_time_sink_x_0_0_0.enable_grid(False)
        self.qtgui_time_sink_x_0_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_0_0_0.enable_control_panel(False)
        self.qtgui_time_sink_x_0_0_0.enable_stem_plot(False)


        labels = ['audio', 'Service lowerMac', 'Signal 3', 'Signal 4', 'Signal 5',
            'Signal 6', 'Signal 7', 'Signal 8', 'Signal 9', 'Signal 10']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ['blue', 'red', 'green', 'black', 'cyan',
            'magenta', 'yellow', 'dark red', 'dark green', 'dark blue']
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]
        styles = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        markers = [-1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1]


        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_0_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_0_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_0_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_0_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_0_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_0_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_0_0_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_0_0_0_win)
        self._next_view_choices = {'Pressed': bool(1), 'Released': bool(0)}

        _next_view_toggle_button = qtgui.ToggleButton(self.set_next_view, 'next_view', self._next_view_choices, False, 'value')
        _next_view_toggle_button.setColors("default", "default", "default", "default")
        self.next_view = _next_view_toggle_button

        self.top_layout.addWidget(_next_view_toggle_button)
        self.epy_block_0_2_0_0 = epy_block_0_2_0_0.tetraSpDecoder()
        self.epy_block_0_0_0 = epy_block_0_0_0.tetraChDecoder()
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_ff(4)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_char*1, '/home/von/new/tetraDMO-Receiver/DmoDecoder/tetraDMO_s36kBit_demod_samples.uint8', False, 0, 0)
        self.blocks_file_source_0.set_begin_tag(pmt.PMT_NIL)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_short*1, '/home/von/test/output_sound/out_sound.out', False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self.audio_sink_0 = audio.sink(audio_rate, '', True)
        self.TETRA_DMO_MAC_DECODER_1 = TETRA_DMO.MAC_DECODER()


        ##################################################
        # Connections
        ##################################################
        self.connect((self.TETRA_DMO_MAC_DECODER_1, 0), (self.epy_block_0_0_0, 0))
        self.connect((self.blocks_file_source_0, 0), (self.TETRA_DMO_MAC_DECODER_1, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.audio_sink_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.qtgui_time_sink_x_0_0_0, 0))
        self.connect((self.epy_block_0_0_0, 0), (self.blocks_file_sink_0, 0))
        self.connect((self.epy_block_0_0_0, 0), (self.epy_block_0_2_0_0, 0))
        self.connect((self.epy_block_0_2_0_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "tetraDMO_Decoder_v1")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_view_step(self):
        return self.view_step

    def set_view_step(self, view_step):
        self.view_step = view_step

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate

    def get_next_view(self):
        return self.next_view

    def set_next_view(self, next_view):
        self.next_view = next_view

    def get_audio_rate(self):
        return self.audio_rate

    def set_audio_rate(self, audio_rate):
        self.audio_rate = audio_rate
        self.qtgui_time_sink_x_0_0_0.set_samp_rate(self.audio_rate)




def main(top_block_cls=tetraDMO_Decoder_v1, options=None):

    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
