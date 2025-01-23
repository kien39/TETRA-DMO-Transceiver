"""
Embedded Python Blocks:

"""

import numpy as np
from gnuradio import gr


class speechDeccoder(gr.sync_block):
    
    def __init__(self, example_param=1.0):
        gr.sync_block.__init__(
            self,
            name='Tetra ACELP\nSpeech Decoder',   # will show up in GRC
            in_sig=[np.uint8],
            out_sig=[np.int16]
        )
        self.example_param = example_param

    def work(self, input_items, output_items):
        output_items[0][:] = input_items[0]
        return len(output_items[0])
