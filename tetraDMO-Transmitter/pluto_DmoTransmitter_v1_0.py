#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: pluto_DmoTransmitter_v1_0
# GNU Radio version: 3.10.10.0

from PyQt5 import Qt
from gnuradio import qtgui
from PyQt5 import QtCore
from gnuradio import audio
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio.filter import firdes
from gnuradio import gr
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
import osmosdr
import time
import pluto_DmoTransmitter_v1_0_epy_block_0 as epy_block_0  # embedded python block
import pluto_DmoTransmitter_v1_0_epy_block_0_0 as epy_block_0_0  # embedded python block
import pluto_DmoTransmitter_v1_0_epy_block_1 as epy_block_1  # embedded python block
import sip



class pluto_DmoTransmitter_v1_0(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "pluto_DmoTransmitter_v1_0", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("pluto_DmoTransmitter_v1_0")
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

        self.settings = Qt.QSettings("GNU Radio", "pluto_DmoTransmitter_v1_0")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)

        ##################################################
        # Variables
        ##################################################
        self.decim = decim = 9
        self.symbol_rate = symbol_rate = 18000
        self.samp_rate = samp_rate = 80000*decim
        self.samp_per_symbol = samp_per_symbol = 2
        self.ptt_on = ptt_on = 0
        self.num_tap = num_tap = 11
        self.frequency_hz = frequency_hz = 390000000
        self.freq_adjust = freq_adjust = 0
        self.audio_rate = audio_rate = 8000

        ##################################################
        # Blocks
        ##################################################

        _ptt_on_push_button = Qt.QPushButton('Push_To_Talk')
        _ptt_on_push_button = Qt.QPushButton('Push_To_Talk')
        self._ptt_on_choices = {'Pressed': 1, 'Released': 0}
        _ptt_on_push_button.pressed.connect(lambda: self.set_ptt_on(self._ptt_on_choices['Pressed']))
        _ptt_on_push_button.released.connect(lambda: self.set_ptt_on(self._ptt_on_choices['Released']))
        self.top_layout.addWidget(_ptt_on_push_button)
        self._frequency_hz_tool_bar = Qt.QToolBar(self)
        self._frequency_hz_tool_bar.addWidget(Qt.QLabel("Frequency [Hz]" + ": "))
        self._frequency_hz_line_edit = Qt.QLineEdit(str(self.frequency_hz))
        self._frequency_hz_tool_bar.addWidget(self._frequency_hz_line_edit)
        self._frequency_hz_line_edit.editingFinished.connect(
            lambda: self.set_frequency_hz(eng_notation.str_to_num(str(self._frequency_hz_line_edit.text()))))
        self.top_grid_layout.addWidget(self._frequency_hz_tool_bar, 1, 0, 1, 1)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._freq_adjust_range = qtgui.Range(-7000, 7000, 10, 0, 200)
        self._freq_adjust_win = qtgui.RangeWidget(self._freq_adjust_range, self.set_freq_adjust, "'freq_adjust'", "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._freq_adjust_win)
        self.qtgui_time_sink_x_1_0_0 = qtgui.time_sink_c(
            (int(4*2040)), #size
            symbol_rate*2, #samp_rate
            "", #name
            1, #number of inputs
            None # parent
        )
        self.qtgui_time_sink_x_1_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_1_0_0.set_y_axis(-1.5, 1.5)

        self.qtgui_time_sink_x_1_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_1_0_0.enable_tags(True)
        self.qtgui_time_sink_x_1_0_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_1_0_0.enable_autoscale(False)
        self.qtgui_time_sink_x_1_0_0.enable_grid(False)
        self.qtgui_time_sink_x_1_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_1_0_0.enable_control_panel(False)
        self.qtgui_time_sink_x_1_0_0.enable_stem_plot(False)


        labels = ['Signal 1', 'Signal 2', 'Signal 3', 'Signal 4', 'Signal 5',
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


        for i in range(2):
            if len(labels[i]) == 0:
                if (i % 2 == 0):
                    self.qtgui_time_sink_x_1_0_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_1_0_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_1_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_1_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_1_0_0.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_1_0_0_win)
        self.qtgui_time_sink_x_1 = qtgui.time_sink_f(
            (2040*2), #size
            audio_rate, #samp_rate
            "FrameView - 2040 bit ", #name
            1, #number of inputs
            None # parent
        )
        self.qtgui_time_sink_x_1.set_update_time(0.10)
        self.qtgui_time_sink_x_1.set_y_axis(-1, 1)

        self.qtgui_time_sink_x_1.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_1.enable_tags(True)
        self.qtgui_time_sink_x_1.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, 0, "")
        self.qtgui_time_sink_x_1.enable_autoscale(False)
        self.qtgui_time_sink_x_1.enable_grid(False)
        self.qtgui_time_sink_x_1.enable_axis_labels(True)
        self.qtgui_time_sink_x_1.enable_control_panel(False)
        self.qtgui_time_sink_x_1.enable_stem_plot(False)


        labels = ['Signal 1', 'Signal 2', 'Signal 3', 'Signal 4', 'Signal 5',
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
                self.qtgui_time_sink_x_1.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_time_sink_x_1.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_1_win = sip.wrapinstance(self.qtgui_time_sink_x_1.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_1_win)
        self.qtgui_ledindicator_0_0_1 = self._qtgui_ledindicator_0_0_1_win = qtgui.GrLEDIndicator('Transmit', "green", "gray", ptt_on, 40, 1, 1, 1, self)
        self.qtgui_ledindicator_0_0_1 = self._qtgui_ledindicator_0_0_1_win
        self.top_grid_layout.addWidget(self._qtgui_ledindicator_0_0_1_win, 0, 4, 1, 1)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(4, 5):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.osmosdr_sink_0 = osmosdr.sink(
            args="numchan=" + str(1) + " " + ""
        )
        self.osmosdr_sink_0.set_time_unknown_pps(osmosdr.time_spec_t())
        self.osmosdr_sink_0.set_sample_rate(samp_rate)
        self.osmosdr_sink_0.set_center_freq((frequency_hz+freq_adjust), 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(10, 0)
        self.osmosdr_sink_0.set_if_gain(20, 0)
        self.osmosdr_sink_0.set_bb_gain(20, 0)
        self.osmosdr_sink_0.set_antenna('', 0)
        self.osmosdr_sink_0.set_bandwidth(0, 0)
        self.mmse_resampler_xx_0 = filter.mmse_resampler_cc(0, (36/80))
        self.low_pass_filter_0_0 = filter.interp_fir_filter_ccf(
            decim,
            firdes.low_pass(
                (decim/2),
                samp_rate,
                25000,
                5000,
                window.WIN_HAMMING,
                6.76))
        self.epy_block_1 = epy_block_1.Pi4DQPSK(envelope_in=False)
        self.epy_block_0_0 = epy_block_0_0.SrcChCoder(mic_gain=4)
        self.epy_block_0 = epy_block_0.DmoEncoder(show_txt=False, talkgroup_id=1001, radio_id=6081751, ptt=ptt_on)
        self.blocks_char_to_float_0 = blocks.char_to_float(1, 1)
        self.audio_source_0 = audio.source(audio_rate, '', True)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.audio_source_0, 0), (self.epy_block_0_0, 0))
        self.connect((self.blocks_char_to_float_0, 0), (self.qtgui_time_sink_x_1, 0))
        self.connect((self.epy_block_0, 0), (self.blocks_char_to_float_0, 0))
        self.connect((self.epy_block_0, 0), (self.epy_block_1, 0))
        self.connect((self.epy_block_0, 1), (self.epy_block_1, 1))
        self.connect((self.epy_block_0_0, 0), (self.epy_block_0, 0))
        self.connect((self.epy_block_1, 0), (self.mmse_resampler_xx_0, 0))
        self.connect((self.epy_block_1, 0), (self.qtgui_time_sink_x_1_0_0, 0))
        self.connect((self.low_pass_filter_0_0, 0), (self.osmosdr_sink_0, 0))
        self.connect((self.mmse_resampler_xx_0, 0), (self.low_pass_filter_0_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "pluto_DmoTransmitter_v1_0")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_decim(self):
        return self.decim

    def set_decim(self, decim):
        self.decim = decim
        self.set_samp_rate(80000*self.decim)
        self.low_pass_filter_0_0.set_taps(firdes.low_pass((self.decim/2), self.samp_rate, 25000, 5000, window.WIN_HAMMING, 6.76))

    def get_symbol_rate(self):
        return self.symbol_rate

    def set_symbol_rate(self, symbol_rate):
        self.symbol_rate = symbol_rate
        self.qtgui_time_sink_x_1_0_0.set_samp_rate(self.symbol_rate*2)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.low_pass_filter_0_0.set_taps(firdes.low_pass((self.decim/2), self.samp_rate, 25000, 5000, window.WIN_HAMMING, 6.76))
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate)

    def get_samp_per_symbol(self):
        return self.samp_per_symbol

    def set_samp_per_symbol(self, samp_per_symbol):
        self.samp_per_symbol = samp_per_symbol

    def get_ptt_on(self):
        return self.ptt_on

    def set_ptt_on(self, ptt_on):
        self.ptt_on = ptt_on
        self.epy_block_0.ptt = self.ptt_on
        self.qtgui_ledindicator_0_0_1.setState(self.ptt_on)

    def get_num_tap(self):
        return self.num_tap

    def set_num_tap(self, num_tap):
        self.num_tap = num_tap

    def get_frequency_hz(self):
        return self.frequency_hz

    def set_frequency_hz(self, frequency_hz):
        self.frequency_hz = frequency_hz
        Qt.QMetaObject.invokeMethod(self._frequency_hz_line_edit, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.frequency_hz)))
        self.osmosdr_sink_0.set_center_freq((self.frequency_hz+self.freq_adjust), 0)

    def get_freq_adjust(self):
        return self.freq_adjust

    def set_freq_adjust(self, freq_adjust):
        self.freq_adjust = freq_adjust
        self.osmosdr_sink_0.set_center_freq((self.frequency_hz+self.freq_adjust), 0)

    def get_audio_rate(self):
        return self.audio_rate

    def set_audio_rate(self, audio_rate):
        self.audio_rate = audio_rate
        self.qtgui_time_sink_x_1.set_samp_rate(self.audio_rate)




def main(top_block_cls=pluto_DmoTransmitter_v1_0, options=None):

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
