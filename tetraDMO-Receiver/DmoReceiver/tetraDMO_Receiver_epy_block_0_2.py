"""
Embedded Python Blocks:

"""

import numpy as np
from gnuradio import gr
from ctypes import *

IN_STEP     = 240
OUT_STEP    = 240

L_FRAME     = 240
SERIAL_SIZE = 138
PRM_SIZE    = 24

synth = np.array([0]*L_FRAME, dtype ='int16')      #/* Synthesis              */
parm  = np.array([0]*PRM_SIZE, dtype ='int16')     #/* Synthesis parameters   */
serial= np.array([0]*SERIAL_SIZE, dtype ='int16')  #/* Serial stream          */

class tetraSpDecoder(gr.sync_block):
    lib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Receiver/codec/tetraSpDecoderLib.so') # class level loading lib

    # C-type corresponding to numpy array 
    ND_POINTER = np.ctypeslib.ndpointer(dtype=np.int16, 
                                          ndim=1,
                                          flags="C")
    lib.Bits2prm_Tetra.argtypes = [ND_POINTER, ND_POINTER]
    lib.Decod_Tetra.argtypes    = [ND_POINTER, ND_POINTER]
    lib.Post_Process.argtypes   = [ND_POINTER, c_int16]

    def Init_Decod_Tetra():
        tetraSpDecoder.lib.Init_Decod_Tetra()
        
    def Bits2prm_Tetra(serial, parm):
        tetraSpDecoder.lib.Bits2prm_Tetra(serial, parm)

    def Decod_Tetra(parm, synth):
        tetraSpDecoder.lib.Decod_Tetra(parm, synth)

    def Post_Process(synth, L_frame):    
        tetraSpDecoder.lib.Post_Process(synth, L_frame)

    
    def __init__(self):
        gr.sync_block.__init__(
            self,
            name='Tetra ACELP\nSpeech Decoder',   # will show up in GRC
            in_sig=[np.int16],
            out_sig=[np.int16]
        )
        self.frame = 0

    def work(self, input_items, output_items):
        cod_ptr = 0
        raw_ptr = 0

        tetraSpDecoder.Init_Decod_Tetra()
        while (cod_ptr + IN_STEP <= len(input_items[0])) and (raw_ptr + OUT_STEP <= len(output_items[0])):

            for j in range (SERIAL_SIZE):
                serial[j] = input_items[0][cod_ptr+j]
                
            cod_ptr += IN_STEP
            self.frame +=1
            
            print("frame= %d" %self.frame)
            tetraSpDecoder.Bits2prm_Tetra(serial, parm)
            tetraSpDecoder.Decod_Tetra(parm, synth)
            tetraSpDecoder.Post_Process(synth, L_FRAME)

            for j in range (L_FRAME):
                output_items[0][raw_ptr+j] = synth[j]
            
            raw_ptr += OUT_STEP
        #self.consume(0, cod_ptr)
        return raw_ptr
