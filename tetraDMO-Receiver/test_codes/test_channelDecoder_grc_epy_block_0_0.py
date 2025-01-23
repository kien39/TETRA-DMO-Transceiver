"""
Embedded Python Blocks:
convert 690 words into 432 words"""

import numpy as np
from gnuradio import gr
from ctypes import *

IN_STEP = 690
OUT_STEP = 480

class outConverter(gr.basic_block):
    
    def __init__(self):
        gr.basic_block.__init__(
            self,
            name='out file\nCONVERTER',   # will show up in GRC
            in_sig=[np.int16],
            out_sig=[np.int16]
        )
        self.Loop_counter = 0
        self.first_pass = True
        self.Frame_stealing = False
        self.bfi1 = 0
        self.bfi2 = 0
        self.Reordered_array         = np.array([0]*286, dtype = 'int16')
        self.Interleaved_coded_array = np.array([0]*432, dtype = 'int16')
        self.Coded_array             = np.array([0]*432, dtype = 'int16')
     
    def general_work(self, input_items, output_items):
        in_index = 0
        out_index = 0

        while (len(input_items[0]) >= in_index + IN_STEP) and (len(output_items[0]) >= out_index + OUT_STEP) :

            if input_items[0][in_index] != 0x6b21:
                print("no correct format")
                break

            #/* Copy first valid block */
            block_ptr = 1
            array_ptr = 0
            for i in range(114) :
                self.Interleaved_coded_array[array_ptr + i] = input_items[0][in_index + block_ptr + i]

            #/* Copy second valid block */
            block_ptr = 161 - 45
            array_ptr += 114
            for i in range(114) :
                self.Interleaved_coded_array[array_ptr + i] = input_items[0][in_index + block_ptr + i]

            #/* Copy third valid block */

            block_ptr = 321 - 45 - 45
            array_ptr += 114
            for i in range(114) :
                self.Interleaved_coded_array[array_ptr + i] = input_items[0][in_index + block_ptr + i]

            #/* Copy fourth valid block */
            block_ptr = 481 - 45 - 45 - 45
            array_ptr += 114
            for i in range(90) :
                self.Interleaved_coded_array[array_ptr + i] = input_items[0][in_index + block_ptr + i]
            
            output_items[0][out_index:out_index+432] = self.Interleaved_coded_array[0:432]

            in_index += IN_STEP
            out_index += OUT_STEP
            
        self.consume(0, in_index)
        return out_index
