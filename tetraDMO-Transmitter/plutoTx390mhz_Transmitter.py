#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: plutoTx390mhz_Transmitter
# GNU Radio version: 3.10.10.0

from PyQt5 import Qt
from gnuradio import qtgui
from PyQt5 import QtCore
from gnuradio import blocks
import pmt
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
from gnuradio import iio
import plutoTx390mhz_Transmitter_epy_block_1 as epy_block_1  # embedded python block
import sip



class plutoTx390mhz_Transmitter(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "plutoTx390mhz_Transmitter", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("plutoTx390mhz_Transmitter")
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

        self.settings = Qt.QSettings("GNU Radio", "plutoTx390mhz_Transmitter")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)

        ##################################################
        # Variables
        ##################################################
        self.decim = decim = 8
        self.symbol_rate = symbol_rate = 18000
        self.samp_rate = samp_rate = 80000*decim
        self.samp_per_symbol = samp_per_symbol = 2
        self.num_tap = num_tap = 11
        self.frequency_hz = frequency_hz = 390000000
        self.freq_adjust = freq_adjust = 3400
        self.envelope_in = envelope_in = 0

        ##################################################
        # Blocks
        ##################################################

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
        self._freq_adjust_range = qtgui.Range(-7000, 7000, 10, 3400, 200)
        self._freq_adjust_win = qtgui.RangeWidget(self._freq_adjust_range, self.set_freq_adjust, "'freq_adjust'", "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._freq_adjust_win)
        self._envelope_in_choices = {'Pressed': bool(1), 'Released': bool(0)}

        _envelope_in_toggle_switch = qtgui.GrToggleSwitch(self.set_envelope_in, '', self._envelope_in_choices, False, "green", "gray", 4, 50, 1, 1, self, 'value')
        self.envelope_in = _envelope_in_toggle_switch

        self.top_layout.addWidget(_envelope_in_toggle_switch)
        self.qtgui_time_sink_x_1 = qtgui.time_sink_c(
            4080, #size
            samp_rate, #samp_rate
            "", #name
            1, #number of inputs
            None # parent
        )
        self.qtgui_time_sink_x_1.set_update_time(0.10)
        self.qtgui_time_sink_x_1.set_y_axis(-1.5, 1.5)

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


        for i in range(2):
            if len(labels[i]) == 0:
                if (i % 2 == 0):
                    self.qtgui_time_sink_x_1.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_1.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_1.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_1_win = sip.wrapinstance(self.qtgui_time_sink_x_1.qwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_time_sink_x_1_win)
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
        self.iio_pluto_sink_0 = iio.fmcomms2_sink_fc32('' if '' else iio.get_pluto_uri(), [True, True], int(32768), False)
        self.iio_pluto_sink_0.set_len_tag_key('')
        self.iio_pluto_sink_0.set_bandwidth(2000000)
        self.iio_pluto_sink_0.set_frequency((frequency_hz+freq_adjust))
        self.iio_pluto_sink_0.set_samplerate(samp_rate)
        self.iio_pluto_sink_0.set_attenuation(0, 10)
        self.iio_pluto_sink_0.set_filter_params('Auto', '', 0, 0)
        self.epy_block_1 = epy_block_1.Pi4DQPSK(envelope_in=envelope_in)
        self.blocks_vector_source_x_0 = blocks.vector_source_b([3]*180000, False, 1, [])
        self.blocks_throttle2_0 = blocks.throttle( gr.sizeof_gr_complex*1, samp_rate, True, 0 if "auto" == "auto" else max( int(float(0.1) * samp_rate) if "auto" == "time" else int(0.1), 1) )
        self.blocks_pack_k_bits_bb_0 = blocks.pack_k_bits_bb(2)
        self.blocks_file_source_0_0_0_1 = blocks.file_source(gr.sizeof_char*1, '/home/ctn008/tetraDMO-Transmitter/samples/tetraDMO_s36kBit_tx.uint8', False, 0, 0)
        self.blocks_file_source_0_0_0_1.set_begin_tag(pmt.PMT_NIL)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0_0_0_1, 0), (self.blocks_pack_k_bits_bb_0, 0))
        self.connect((self.blocks_pack_k_bits_bb_0, 0), (self.epy_block_1, 0))
        self.connect((self.blocks_throttle2_0, 0), (self.qtgui_time_sink_x_1, 0))
        self.connect((self.blocks_vector_source_x_0, 0), (self.epy_block_1, 1))
        self.connect((self.epy_block_1, 0), (self.mmse_resampler_xx_0, 0))
        self.connect((self.low_pass_filter_0_0, 0), (self.blocks_throttle2_0, 0))
        self.connect((self.low_pass_filter_0_0, 0), (self.iio_pluto_sink_0, 0))
        self.connect((self.mmse_resampler_xx_0, 0), (self.low_pass_filter_0_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "plutoTx390mhz_Transmitter")
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

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle2_0.set_sample_rate(self.samp_rate)
        self.iio_pluto_sink_0.set_samplerate(self.samp_rate)
        self.low_pass_filter_0_0.set_taps(firdes.low_pass((self.decim/2), self.samp_rate, 25000, 5000, window.WIN_HAMMING, 6.76))
        self.qtgui_time_sink_x_1.set_samp_rate(self.samp_rate)

    def get_samp_per_symbol(self):
        return self.samp_per_symbol

    def set_samp_per_symbol(self, samp_per_symbol):
        self.samp_per_symbol = samp_per_symbol

    def get_num_tap(self):
        return self.num_tap

    def set_num_tap(self, num_tap):
        self.num_tap = num_tap

    def get_frequency_hz(self):
        return self.frequency_hz

    def set_frequency_hz(self, frequency_hz):
        self.frequency_hz = frequency_hz
        Qt.QMetaObject.invokeMethod(self._frequency_hz_line_edit, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.frequency_hz)))
        self.iio_pluto_sink_0.set_frequency((self.frequency_hz+self.freq_adjust))

    def get_freq_adjust(self):
        return self.freq_adjust

    def set_freq_adjust(self, freq_adjust):
        self.freq_adjust = freq_adjust
        self.iio_pluto_sink_0.set_frequency((self.frequency_hz+self.freq_adjust))

    def get_envelope_in(self):
        return self.envelope_in

    def set_envelope_in(self, envelope_in):
        self.envelope_in = envelope_in
        self.epy_block_1.envelope_in = self.envelope_in




def main(top_block_cls=plutoTx390mhz_Transmitter, options=None):

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
