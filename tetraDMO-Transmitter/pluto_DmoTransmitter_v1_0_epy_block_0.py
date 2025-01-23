"""
Embedded Python Block: tetraDmoEncoder

perform encoding Dmo tetra signal
- input: 432 byte frames voice signal one per 4 time slot per 17 frame
- output 510 byte tetra burst signal, 4 per frame per 18 frame
"""

import numpy as np
from gnuradio import gr
from ctypes import *
import pmt

class DmoEncoder(gr.basic_block):

    MacEncoderLib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Transmitter/libs/MacEncoderLib_v1.so')
    ND_POINTER = np.ctypeslib.ndpointer(dtype=np.uint8, 
                                              ndim=1,
                                              flags="C")
    def Init_serviceMacEncode(show_txt, talkgroup_id, radio_id):
        DmoEncoder.MacEncoderLib.Init_serviceMacEncode.argtypes = [c_bool, c_size_t, c_size_t]
        DmoEncoder.MacEncoderLib.Init_serviceMacEncode(show_txt, talkgroup_id, radio_id)
      
    def serviceMacEncode(showTxt, ptt, talkgroup_id, radio_id, in_data, in_len, out_data, out_envelop):
        DmoEncoder.MacEncoderLib.serviceMacEncode.argtypes = [c_bool, c_bool, c_size_t, c_size_t, DmoEncoder.ND_POINTER, c_size_t, DmoEncoder.ND_POINTER, DmoEncoder.ND_POINTER]
        DmoEncoder.MacEncoderLib.serviceMacEncode.restype = c_size_t
        res = DmoEncoder.MacEncoderLib.serviceMacEncode(showTxt, ptt, talkgroup_id, radio_id, in_data, in_len, out_data, out_envelop)
        return res
    
    def __init__(self, show_txt=True, talkgroup_id = 1001, radio_id =6081751, ptt = False):
        gr.basic_block.__init__(
            self,
            name='TetraDMO Encoder',
            in_sig=[np.uint8],
            out_sig=[np.uint8, np.uint8]
        )
        self.showTxt        = show_txt
        self.prev_showTxt    = False
        self.update_showTxt = False
        self.talkgroupId    = talkgroup_id
        self.radioId        = radio_id
        self.ptt            = ptt
        self.prev_ptt       = False
        self.update_ptt     = False

        self.message_port_register_in(pmt.intern('ptt'))
        self.set_msg_handler(pmt.intern('ptt'), self.handle_msg)

        self.message_port_register_out(pmt.intern('tx_keyed'))
        self.tx_keyed_status = pmt.cons( pmt.intern('tx_keyed'), pmt.from_bool(False) )

    def handle_msg(self, msg):
        # rotate curChannel + 1
        #self.ptt = True       
        print(msg)
        
    def general_work(self, input_items, output_items):
        if self.showTxt != self.prev_showTxt:
            self.prev_showTxt = self.showTxt
            self.update_showTxt = True
        if self.update_showTxt:
            self.update_showTxt = False
            print("showTxt = ", self.ptt)
        
        in_len  = len(input_items[0])
        out_len = len(output_items[0])
        in_ptr = 0
        out_ptr = 0

        in_ptr  = in_len if (out_len*2-255) >= (in_len*5) else (out_len*2-255)//5
        out_ptr = DmoEncoder.serviceMacEncode(self.showTxt, self.ptt, self.talkgroupId, self.radioId, input_items[0], in_ptr, output_items[0], output_items[1])

        self.consume(0, in_ptr)
        return out_ptr
