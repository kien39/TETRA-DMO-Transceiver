"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr


class frameStep(gr.basic_block):  # other base classes are basic_block, decim_block, interp_block
    """Embedded Python Block example - a simple multiply const"""

    def __init__(self, step = 4800, change = False):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.basic_block.__init__(
            self,
            name='Push_per_Frame',   # will show up in GRC
            in_sig=[np.uint8],
            out_sig=[np.uint8]
        )
        # if an attribute with the same name as a parameter is found,
        # a callback is registered (properties work, too).
        self.step= step
        self.change = change
        self.prechange = False

    def general_work(self, input_items, output_items):
        """example: multiply with constant"""
        next_view = False
        if self.change != self.prechange :
            #print("Next View")
            next_view = True
            self.prechange = self.change
        if next_view and (len(output_items[0]) >= self.step) and (len(input_items[0]) >= self.step):
            output_items[0][:self.step] = input_items[0][:self.step]
            #DEBUG output_txt = ''
            #DEBUG for i in range(self.step):
            #DEBUG output_txt += '1' if output_items[0][i] else '0'
            self.consume(0, self.step)
            #DEBUG print (output_txt)
            return self.step
        else:
            return 0
