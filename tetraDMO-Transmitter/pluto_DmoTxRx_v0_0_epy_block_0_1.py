"""
Blockage
- True: just throw samples away, do nothing
- False: copy input items to output
"""

import numpy as np
from gnuradio import gr


class blockage_real(gr.sync_block):
    
    def __init__(self, mode=True):
        gr.basic_block.__init__(
            self,
            name='Receive Gate',   # will show up in GRC
            in_sig=[np.float32],
            out_sig=[np.float32]
        )
        self.mode = mode

    def general_work(self, input_items, output_items):
        if self.mode:
            self.consume(0, len(input_items[0]))
            return 0

        consume_len = len(input_items[0]) if (len(output_items[0])>len(input_items[0])) else len(output_items[0]) 
        output_items[0][:consume_len] = input_items[0][:consume_len]
        self.consume(0, consume_len)
        return consume_len
