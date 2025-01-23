"""
Embedded Python Blocks:
sync block, 480 samples in, 480 samples out
"""

import numpy as np
from gnuradio import gr
from ctypes import *

IN_STEP = 432
OUT_STEP = 276

class tetraChDecoder(gr.basic_block):
    lib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Receiver/codec/tetraChDecoderLib.so') # class level loading lib

    # C-type corresponding to numpy array 
    ND_POINTER = np.ctypeslib.ndpointer(dtype=np.int16, 
                                          ndim=1,
                                          flags="C")
    lib.Channel_Decoding.argtypes = [c_int16, c_int16, ND_POINTER, ND_POINTER]
    lib.Channel_Decoding.restype = c_int16
    lib.Desinterleaving_Signalling.argtypes = [ND_POINTER, ND_POINTER]
    lib.Desinterleaving_Signalling.restype = c_int16
    lib.Desinterleaving_Speech.argtypes = [ND_POINTER, ND_POINTER]
    lib.Desinterleaving_Speech.restype = c_int16


    def Channel_Decoding(first_pass, Frame_Stealing, Input_Frame, Output_Frame):
        res = tetraChDecoder.lib.Channel_Decoding(first_pass, Frame_Stealing, Input_Frame, Output_Frame)
        return res

    def Desinterleaving_Signalling(Input_Frame, Output_Frame):
        res = tetraChDecoder.lib.Desinterleaving_Signalling(Input_Frame, Output_Frame)
        return res

    def Desinterleaving_Speech(Input_Frame, Output_Frame):
        res = tetraChDecoder.lib.Desinterleaving_Speech(Input_Frame, Output_Frame)
        return res
    
    def __init__(self):
        gr.basic_block.__init__(
            self,
            name='Tetra ACELP\nChannel Decoder',   # will show up in GRC
            in_sig =[np.uint8],
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

            # "0" = 127, "1" = -127
            if type(input_items[0][0]) == np.uint8:
                for i in range(432):
                    self.Interleaved_coded_array[i] = -127 if input_items[0][in_index+i] else 127
            else: #int16                   
                self.Interleaved_coded_array[0:432] = input_items[0][in_index:in_index+432]
                                                        
            #self.Frame_stealing = input_items[0][in_index + IN_STEP - 1]

            if self.Frame_stealing :
                tetraChDecoder.Desinterleaving_Signalling(self.Interleaved_coded_array[216:], self.Coded_array[216:])
                self.Coded_array[:216] = self.Interleaved_coded_array[:216]
            else:
                tetraChDecoder.Desinterleaving_Speech(self.Interleaved_coded_array, self.Coded_array)

            self.bfi1 = self.Frame_stealing
            #if self.bfi1:
            #    print(f"Frame Nb %d was stolen" %(self.Loop_counter+1))

            bfi2 = tetraChDecoder.Channel_Decoding(self.first_pass, self.Frame_stealing, self.Coded_array, self.Reordered_array)
            self.first_pass = False

            if (self.Frame_stealing==0) and (self.bfi2==1):
                self.bfi1 = 1

            self.Loop_counter += 1
            #if self.bfi2:
            #    print(f"Frame Nb %d Bfi active" %(self.Loop_counter+1))

            output_items[0][out_index] = self.bfi1
            output_items[0][out_index+1:out_index+138] = self.Reordered_array[0:137]
            output_items[0][out_index+138] = self.bfi2
            output_items[0][out_index+139:out_index+276] = self.Reordered_array[137:274]

            #if (not self.bfi1) and (not self.bfi2):
            #    print(self.Loop_counter, "  full speech frame decoded ok")

            in_index += IN_STEP
            out_index += OUT_STEP
            
        self.consume(0, in_index)
        return out_index
