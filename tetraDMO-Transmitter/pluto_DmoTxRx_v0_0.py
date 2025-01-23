#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: pluto_DmoTxRx_v0_0
# GNU Radio version: 3.10.10.0

from PyQt5 import Qt
from gnuradio import qtgui
from PyQt5 import QtCore
from gnuradio import audio
from gnuradio import blocks
from gnuradio import digital
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
import cmath
import pluto_DmoTxRx_v0_0_epy_block_0 as epy_block_0  # embedded python block
import pluto_DmoTxRx_v0_0_epy_block_0_0 as epy_block_0_0  # embedded python block
import pluto_DmoTxRx_v0_0_epy_block_0_0_0 as epy_block_0_0_0  # embedded python block
import pluto_DmoTxRx_v0_0_epy_block_0_0_1 as epy_block_0_0_1  # embedded python block
import pluto_DmoTxRx_v0_0_epy_block_0_1 as epy_block_0_1  # embedded python block
import pluto_DmoTxRx_v0_0_epy_block_0_2_0 as epy_block_0_2_0  # embedded python block
import pluto_DmoTxRx_v0_0_epy_block_1 as epy_block_1  # embedded python block
import pluto_DmoTxRx_v0_0_epy_block_2 as epy_block_2  # embedded python block
import sip



class pluto_DmoTxRx_v0_0(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "pluto_DmoTxRx_v0_0", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("pluto_DmoTxRx_v0_0")
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

        self.settings = Qt.QSettings("GNU Radio", "pluto_DmoTxRx_v0_0")

        try:
            geometry = self.settings.value("geometry")
            if geometry:
                self.restoreGeometry(geometry)
        except BaseException as exc:
            print(f"Qt GUI: Could not restore geometry: {str(exc)}", file=sys.stderr)

        ##################################################
        # Variables
        ##################################################
        self.sps = sps = 2
        self.nfilts = nfilts = 32
        self.decim = decim = 16
        self.constel = constel = digital.constellation_dqpsk().base()
        self.constel.set_npwr(1.0)
        self.constel.gen_soft_dec_lut(8)
        self.variable_adaptive_algorithm_0 = variable_adaptive_algorithm_0 = digital.adaptive_algorithm_cma( constel, 10e-3, 1).base()
        self.symbol_rate = symbol_rate = 18000
        self.samp_rate = samp_rate = 80000*decim
        self.samp_per_symbol = samp_per_symbol = 2
        self.rrc_taps = rrc_taps = firdes.root_raised_cosine(nfilts, nfilts, 1.0/float(sps), 0.35, 11*sps*nfilts)
        self.ptt_on = ptt_on = 0
        self.num_tap = num_tap = 11
        self.frequency_hz = frequency_hz = 390000000
        self.freq_offset_hz = freq_offset_hz = 45000
        self.freq_adjust = freq_adjust = 4000
        self.channel_rate = channel_rate = 36000
        self.audio_rate = audio_rate = 8000

        ##################################################
        # Blocks
        ##################################################

        self.qtgui_tab_widget_0 = Qt.QTabWidget()
        self.qtgui_tab_widget_0_widget_0 = Qt.QWidget()
        self.qtgui_tab_widget_0_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.qtgui_tab_widget_0_widget_0)
        self.qtgui_tab_widget_0_grid_layout_0 = Qt.QGridLayout()
        self.qtgui_tab_widget_0_layout_0.addLayout(self.qtgui_tab_widget_0_grid_layout_0)
        self.qtgui_tab_widget_0.addTab(self.qtgui_tab_widget_0_widget_0, 'Tx-Rx')
        self.top_grid_layout.addWidget(self.qtgui_tab_widget_0, 0, 2, 1, 1)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(2, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.freq_parameters = Qt.QTabWidget()
        self.freq_parameters_widget_0 = Qt.QWidget()
        self.freq_parameters_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.freq_parameters_widget_0)
        self.freq_parameters_grid_layout_0 = Qt.QGridLayout()
        self.freq_parameters_layout_0.addLayout(self.freq_parameters_grid_layout_0)
        self.freq_parameters.addTab(self.freq_parameters_widget_0, 'Frequency')
        self.top_grid_layout.addWidget(self.freq_parameters, 1, 0, 1, 2)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        _ptt_on_push_button = Qt.QPushButton('Push_To_Talk')
        _ptt_on_push_button = Qt.QPushButton('Push_To_Talk')
        self._ptt_on_choices = {'Pressed': 1, 'Released': 0}
        _ptt_on_push_button.pressed.connect(lambda: self.set_ptt_on(self._ptt_on_choices['Pressed']))
        _ptt_on_push_button.released.connect(lambda: self.set_ptt_on(self._ptt_on_choices['Released']))
        self.qtgui_tab_widget_0_layout_0.addWidget(_ptt_on_push_button)
        self._frequency_hz_tool_bar = Qt.QToolBar(self)
        self._frequency_hz_tool_bar.addWidget(Qt.QLabel("Frequency [Hz]" + ": "))
        self._frequency_hz_line_edit = Qt.QLineEdit(str(self.frequency_hz))
        self._frequency_hz_tool_bar.addWidget(self._frequency_hz_line_edit)
        self._frequency_hz_line_edit.editingFinished.connect(
            lambda: self.set_frequency_hz(eng_notation.str_to_num(str(self._frequency_hz_line_edit.text()))))
        self.freq_parameters_layout_0.addWidget(self._frequency_hz_tool_bar)
        self._freq_offset_hz_tool_bar = Qt.QToolBar(self)
        self._freq_offset_hz_tool_bar.addWidget(Qt.QLabel("Baseband offset_Hz" + ": "))
        self._freq_offset_hz_line_edit = Qt.QLineEdit(str(self.freq_offset_hz))
        self._freq_offset_hz_tool_bar.addWidget(self._freq_offset_hz_line_edit)
        self._freq_offset_hz_line_edit.editingFinished.connect(
            lambda: self.set_freq_offset_hz(eng_notation.str_to_num(str(self._freq_offset_hz_line_edit.text()))))
        self.freq_parameters_layout_0.addWidget(self._freq_offset_hz_tool_bar)
        self._freq_adjust_range = qtgui.Range(-7000, 7000, 10, 4000, 200)
        self._freq_adjust_win = qtgui.RangeWidget(self._freq_adjust_range, self.set_freq_adjust, "'freq_adjust'", "counter_slider", float, QtCore.Qt.Horizontal)
        self.freq_parameters_layout_0.addWidget(self._freq_adjust_win)
        self.qtgui_time_sink_x_1_0_0_0 = qtgui.time_sink_c(
            (int(4*2040)), #size
            symbol_rate*2, #samp_rate
            "Receive IQ", #name
            1, #number of inputs
            None # parent
        )
        self.qtgui_time_sink_x_1_0_0_0.set_update_time(0.10)
        self.qtgui_time_sink_x_1_0_0_0.set_y_axis(-1.5, 1.5)

        self.qtgui_time_sink_x_1_0_0_0.set_y_label('Amplitude', "")

        self.qtgui_time_sink_x_1_0_0_0.enable_tags(False)
        self.qtgui_time_sink_x_1_0_0_0.set_trigger_mode(qtgui.TRIG_MODE_TAG, qtgui.TRIG_SLOPE_POS, 0.5, 0, 0, "")
        self.qtgui_time_sink_x_1_0_0_0.enable_autoscale(False)
        self.qtgui_time_sink_x_1_0_0_0.enable_grid(False)
        self.qtgui_time_sink_x_1_0_0_0.enable_axis_labels(True)
        self.qtgui_time_sink_x_1_0_0_0.enable_control_panel(False)
        self.qtgui_time_sink_x_1_0_0_0.enable_stem_plot(False)

        self.qtgui_time_sink_x_1_0_0_0.disable_legend()

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
                    self.qtgui_time_sink_x_1_0_0_0.set_line_label(i, "Re{{Data {0}}}".format(i/2))
                else:
                    self.qtgui_time_sink_x_1_0_0_0.set_line_label(i, "Im{{Data {0}}}".format(i/2))
            else:
                self.qtgui_time_sink_x_1_0_0_0.set_line_label(i, labels[i])
            self.qtgui_time_sink_x_1_0_0_0.set_line_width(i, widths[i])
            self.qtgui_time_sink_x_1_0_0_0.set_line_color(i, colors[i])
            self.qtgui_time_sink_x_1_0_0_0.set_line_style(i, styles[i])
            self.qtgui_time_sink_x_1_0_0_0.set_line_marker(i, markers[i])
            self.qtgui_time_sink_x_1_0_0_0.set_line_alpha(i, alphas[i])

        self._qtgui_time_sink_x_1_0_0_0_win = sip.wrapinstance(self.qtgui_time_sink_x_1_0_0_0.qwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_time_sink_x_1_0_0_0_win, 1, 3, 1, 2)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(3, 5):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_time_sink_x_1_0_0 = qtgui.time_sink_c(
            (int(4*2040)), #size
            symbol_rate*2, #samp_rate
            "Transmit IQ", #name
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

        self.qtgui_time_sink_x_1_0_0.disable_legend()

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
        self.top_grid_layout.addWidget(self._qtgui_time_sink_x_1_0_0_win, 0, 3, 1, 2)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(3, 5):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_time_sink_x_1 = qtgui.time_sink_f(
            (2040*2), #size
            audio_rate, #samp_rate
            "Mic Transmit Audio ", #name
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

        self.qtgui_time_sink_x_1.disable_legend()

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
        self.top_grid_layout.addWidget(self._qtgui_time_sink_x_1_win, 0, 0, 1, 2)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_ledindicator_0_0_1_0 = self._qtgui_ledindicator_0_0_1_0_win = qtgui.GrLEDIndicator('Rx', "green", "gray", not ptt_on, 40, 1, 1, 1, self)
        self.qtgui_ledindicator_0_0_1_0 = self._qtgui_ledindicator_0_0_1_0_win
        self.qtgui_tab_widget_0_layout_0.addWidget(self._qtgui_ledindicator_0_0_1_0_win)
        self.qtgui_ledindicator_0_0_1 = self._qtgui_ledindicator_0_0_1_win = qtgui.GrLEDIndicator('Tx', "green", "gray", ptt_on, 40, 1, 1, 1, self)
        self.qtgui_ledindicator_0_0_1 = self._qtgui_ledindicator_0_0_1_win
        self.qtgui_tab_widget_0_layout_0.addWidget(self._qtgui_ledindicator_0_0_1_win)
        self.qtgui_const_sink_x_0 = qtgui.const_sink_c(
            2000, #size
            "Receive IQ Constellation", #name
            1, #number of inputs
            None # parent
        )
        self.qtgui_const_sink_x_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0.set_y_axis((-2), 2)
        self.qtgui_const_sink_x_0.set_x_axis((-2), 2)
        self.qtgui_const_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, "")
        self.qtgui_const_sink_x_0.enable_autoscale(False)
        self.qtgui_const_sink_x_0.enable_grid(True)
        self.qtgui_const_sink_x_0.enable_axis_labels(True)

        self.qtgui_const_sink_x_0.disable_legend()

        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        styles = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0.qwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_const_sink_x_0_win, 1, 2, 1, 1)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(2, 3):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.mmse_resampler_xx_0_0 = filter.mmse_resampler_cc(0, (float(samp_rate)/(float(decim)*float(channel_rate))))
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
        self.iio_pluto_source_0 = iio.fmcomms2_source_fc32('' if '' else iio.get_pluto_uri(), [True, True], 32768)
        self.iio_pluto_source_0.set_len_tag_key('packet_len')
        self.iio_pluto_source_0.set_frequency((frequency_hz-freq_offset_hz))
        self.iio_pluto_source_0.set_samplerate(samp_rate)
        self.iio_pluto_source_0.set_gain_mode(0, 'manual')
        self.iio_pluto_source_0.set_gain(0, 15)
        self.iio_pluto_source_0.set_quadrature(True)
        self.iio_pluto_source_0.set_rfdc(True)
        self.iio_pluto_source_0.set_bbdc(True)
        self.iio_pluto_source_0.set_filter_params('Auto', '', 0, 0)
        self.iio_pluto_sink_0 = iio.fmcomms2_sink_fc32('' if '' else iio.get_pluto_uri(), [True, True], int(32768), False)
        self.iio_pluto_sink_0.set_len_tag_key('')
        self.iio_pluto_sink_0.set_bandwidth(2000000)
        self.iio_pluto_sink_0.set_frequency((frequency_hz+freq_adjust))
        self.iio_pluto_sink_0.set_samplerate(samp_rate)
        self.iio_pluto_sink_0.set_attenuation(0, 10)
        self.iio_pluto_sink_0.set_filter_params('Auto', '', 0, 0)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(decim, firdes.low_pass(1,samp_rate,12500,12500*0.2), freq_offset_hz, samp_rate)
        self.epy_block_2 = epy_block_2.blk()
        self.epy_block_1 = epy_block_1.Pi4DQPSK(envelope_in=False)
        self.epy_block_0_2_0 = epy_block_0_2_0.tetraSpDecoder()
        self.epy_block_0_1 = epy_block_0_1.blockage_real(mode=not ptt_on)
        self.epy_block_0_0_1 = epy_block_0_0_1.blockage_cp(mode=ptt_on)
        self.epy_block_0_0_0 = epy_block_0_0_0.tetraChDecoder()
        self.epy_block_0_0 = epy_block_0_0.SrcChCoder(mic_gain=4)
        self.epy_block_0 = epy_block_0.DmoEncoder(show_txt=False, talkgroup_id=1001, radio_id=6081751, ptt=ptt_on)
        self.digital_pfb_clock_sync_xxx_0 = digital.pfb_clock_sync_ccf(sps, (2*cmath.pi/100.0), rrc_taps, nfilts, (nfilts/2), 1.5, sps)
        self.digital_map_bb_0 = digital.map_bb(constel.pre_diff_code())
        self.digital_linear_equalizer_0 = digital.linear_equalizer(15, sps, variable_adaptive_algorithm_0, True, [ ], 'corr_est')
        self.digital_fll_band_edge_cc_0 = digital.fll_band_edge_cc(sps, 0.35, 45, (cmath.pi/100.0))
        self.digital_diff_phasor_cc_0 = digital.diff_phasor_cc()
        self.digital_constellation_decoder_cb_0 = digital.constellation_decoder_cb(constel)
        self.blocks_unpack_k_bits_bb_0 = blocks.unpack_k_bits_bb(constel.bits_per_symbol())
        self.blocks_multiply_const_vxx_0_0 = blocks.multiply_const_ff(4)
        self.audio_source_0 = audio.source(audio_rate, '', True)
        self.audio_sink_0 = audio.sink(audio_rate, '', True)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.audio_source_0, 0), (self.epy_block_0_1, 0))
        self.connect((self.blocks_multiply_const_vxx_0_0, 0), (self.audio_sink_0, 0))
        self.connect((self.blocks_unpack_k_bits_bb_0, 0), (self.epy_block_2, 0))
        self.connect((self.digital_constellation_decoder_cb_0, 0), (self.digital_map_bb_0, 0))
        self.connect((self.digital_diff_phasor_cc_0, 0), (self.digital_constellation_decoder_cb_0, 0))
        self.connect((self.digital_diff_phasor_cc_0, 0), (self.qtgui_const_sink_x_0, 0))
        self.connect((self.digital_fll_band_edge_cc_0, 0), (self.digital_pfb_clock_sync_xxx_0, 0))
        self.connect((self.digital_linear_equalizer_0, 0), (self.digital_diff_phasor_cc_0, 0))
        self.connect((self.digital_map_bb_0, 0), (self.blocks_unpack_k_bits_bb_0, 0))
        self.connect((self.digital_pfb_clock_sync_xxx_0, 0), (self.digital_linear_equalizer_0, 0))
        self.connect((self.epy_block_0, 0), (self.epy_block_1, 0))
        self.connect((self.epy_block_0, 1), (self.epy_block_1, 1))
        self.connect((self.epy_block_0_0, 0), (self.epy_block_0, 0))
        self.connect((self.epy_block_0_0_0, 0), (self.epy_block_0_2_0, 0))
        self.connect((self.epy_block_0_0_1, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.epy_block_0_1, 0), (self.epy_block_0_0, 0))
        self.connect((self.epy_block_0_1, 0), (self.qtgui_time_sink_x_1, 0))
        self.connect((self.epy_block_0_2_0, 0), (self.blocks_multiply_const_vxx_0_0, 0))
        self.connect((self.epy_block_1, 0), (self.mmse_resampler_xx_0, 0))
        self.connect((self.epy_block_1, 0), (self.qtgui_time_sink_x_1_0_0, 0))
        self.connect((self.epy_block_2, 0), (self.epy_block_0_0_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.mmse_resampler_xx_0_0, 0))
        self.connect((self.iio_pluto_source_0, 0), (self.epy_block_0_0_1, 0))
        self.connect((self.low_pass_filter_0_0, 0), (self.iio_pluto_sink_0, 0))
        self.connect((self.mmse_resampler_xx_0, 0), (self.low_pass_filter_0_0, 0))
        self.connect((self.mmse_resampler_xx_0_0, 0), (self.digital_fll_band_edge_cc_0, 0))
        self.connect((self.mmse_resampler_xx_0_0, 0), (self.qtgui_time_sink_x_1_0_0_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "pluto_DmoTxRx_v0_0")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.set_rrc_taps(firdes.root_raised_cosine(self.nfilts, self.nfilts, 1.0/float(self.sps), 0.35, 11*self.sps*self.nfilts))

    def get_nfilts(self):
        return self.nfilts

    def set_nfilts(self, nfilts):
        self.nfilts = nfilts
        self.set_rrc_taps(firdes.root_raised_cosine(self.nfilts, self.nfilts, 1.0/float(self.sps), 0.35, 11*self.sps*self.nfilts))

    def get_decim(self):
        return self.decim

    def set_decim(self, decim):
        self.decim = decim
        self.set_samp_rate(80000*self.decim)
        self.low_pass_filter_0_0.set_taps(firdes.low_pass((self.decim/2), self.samp_rate, 25000, 5000, window.WIN_HAMMING, 6.76))
        self.mmse_resampler_xx_0_0.set_resamp_ratio((float(self.samp_rate)/(float(self.decim)*float(self.channel_rate))))

    def get_constel(self):
        return self.constel

    def set_constel(self, constel):
        self.constel = constel
        self.digital_constellation_decoder_cb_0.set_constellation(self.constel)

    def get_variable_adaptive_algorithm_0(self):
        return self.variable_adaptive_algorithm_0

    def set_variable_adaptive_algorithm_0(self, variable_adaptive_algorithm_0):
        self.variable_adaptive_algorithm_0 = variable_adaptive_algorithm_0

    def get_symbol_rate(self):
        return self.symbol_rate

    def set_symbol_rate(self, symbol_rate):
        self.symbol_rate = symbol_rate
        self.qtgui_time_sink_x_1_0_0.set_samp_rate(self.symbol_rate*2)
        self.qtgui_time_sink_x_1_0_0_0.set_samp_rate(self.symbol_rate*2)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.freq_xlating_fir_filter_xxx_0.set_taps(firdes.low_pass(1,self.samp_rate,12500,12500*0.2))
        self.iio_pluto_sink_0.set_samplerate(self.samp_rate)
        self.iio_pluto_source_0.set_samplerate(self.samp_rate)
        self.low_pass_filter_0_0.set_taps(firdes.low_pass((self.decim/2), self.samp_rate, 25000, 5000, window.WIN_HAMMING, 6.76))
        self.mmse_resampler_xx_0_0.set_resamp_ratio((float(self.samp_rate)/(float(self.decim)*float(self.channel_rate))))

    def get_samp_per_symbol(self):
        return self.samp_per_symbol

    def set_samp_per_symbol(self, samp_per_symbol):
        self.samp_per_symbol = samp_per_symbol

    def get_rrc_taps(self):
        return self.rrc_taps

    def set_rrc_taps(self, rrc_taps):
        self.rrc_taps = rrc_taps
        self.digital_pfb_clock_sync_xxx_0.update_taps(self.rrc_taps)

    def get_ptt_on(self):
        return self.ptt_on

    def set_ptt_on(self, ptt_on):
        self.ptt_on = ptt_on
        self.epy_block_0.ptt = self.ptt_on
        self.epy_block_0_0_1.mode = self.ptt_on
        self.epy_block_0_1.mode = not self.ptt_on
        self.qtgui_ledindicator_0_0_1.setState(self.ptt_on)
        self.qtgui_ledindicator_0_0_1_0.setState(not self.ptt_on)

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
        self.iio_pluto_source_0.set_frequency((self.frequency_hz-self.freq_offset_hz))

    def get_freq_offset_hz(self):
        return self.freq_offset_hz

    def set_freq_offset_hz(self, freq_offset_hz):
        self.freq_offset_hz = freq_offset_hz
        Qt.QMetaObject.invokeMethod(self._freq_offset_hz_line_edit, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.freq_offset_hz)))
        self.freq_xlating_fir_filter_xxx_0.set_center_freq(self.freq_offset_hz)
        self.iio_pluto_source_0.set_frequency((self.frequency_hz-self.freq_offset_hz))

    def get_freq_adjust(self):
        return self.freq_adjust

    def set_freq_adjust(self, freq_adjust):
        self.freq_adjust = freq_adjust
        self.iio_pluto_sink_0.set_frequency((self.frequency_hz+self.freq_adjust))

    def get_channel_rate(self):
        return self.channel_rate

    def set_channel_rate(self, channel_rate):
        self.channel_rate = channel_rate
        self.mmse_resampler_xx_0_0.set_resamp_ratio((float(self.samp_rate)/(float(self.decim)*float(self.channel_rate))))

    def get_audio_rate(self):
        return self.audio_rate

    def set_audio_rate(self, audio_rate):
        self.audio_rate = audio_rate
        self.qtgui_time_sink_x_1.set_samp_rate(self.audio_rate)




def main(top_block_cls=pluto_DmoTxRx_v0_0, options=None):

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
