"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr
from ctypes import *

class blk(gr.basic_block):
    codingLib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Receiver/DmoDecoder/libs/codingLib.so') # class level loading lib

    # C-type corresponding to numpy array 
    ND_POINTER = np.ctypeslib.ndpointer(dtype=np.uint8, 
                                      ndim=1,
                                      flags="C")
    def service_mac(in_data, in_len, out_data):
        blk.codingLib.service_mac.argtypes = [blk.ND_POINTER, c_size_t, blk.ND_POINTER]
        blk.codingLib.service_mac.restype = c_size_t
        res = blk.codingLib.service_mac(in_data, in_len, out_data)
        return res


    def __init__(self):
        gr.basic_block.__init__(
            self,
            name='DMO\nBurst SYNC',   # will show up in GRC
            in_sig=[np.uint8],
            out_sig=[np.uint8]
        )

    def general_work(self, input_items, output_items):
        in_len = len(input_items[0])
        out_len = len(output_items[0])
        consume_len = in_len if (in_len < (out_len - 510)) else (out_len - 510)
        out_items = blk.service_mac(input_items[0], consume_len, output_items[0])
        self.consume(0, consume_len)
        return out_items
