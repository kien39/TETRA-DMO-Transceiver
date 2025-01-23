"""
Embedded Python Blocks:
sync block, 480 samples in, 480 samples out
"""

import numpy as np
from gnuradio import gr
from ctypes import *

IN_STEP = 480
OUT_STEP = 432
L_FRAME     = 240
SERIAL_SIZE = 138
ANA_SIZE    =  23

class SrcChCoder(gr.basic_block):
    SrcCoderlib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Transmitter/libs/tetraSrcCoderLib.so')

    # C-type corresponding to numpy array 
    ND_POINTER = np.ctypeslib.ndpointer(dtype=np.int16, 
                                          ndim=1,
                                          flags="C")
    SrcCoderlib.Pre_Process.argtypes = [ND_POINTER, c_int16]
    SrcCoderlib.Prm2bits_Tetra.argtypes = [ND_POINTER, ND_POINTER]
    SrcCoderlib.Coder_Tetra.argtypes    = [ND_POINTER, ND_POINTER]
    SrcCoderlib.Post_Process.argtypes   = [ND_POINTER, c_int16]

    SrcCoderlib.Copy_Speech_Frame.argtypes   = [ND_POINTER]
    def Copy_Speech_Frame(speech_frame):
        SrcChCoder.SrcCoderlib.Copy_Speech_Frame(speech_frame)

    def Init_Pre_Process():
        SrcChCoder.SrcCoderlib.Init_Pre_Process()

    def Init_Coder_Tetra():
        SrcChCoder.SrcCoderlib.Init_Coder_Tetra()
        
    def Pre_Process(new_speech, L_frame):
        SrcChCoder.SrcCoderlib.Pre_Process(new_speech, L_frame)

    def Prm2bits_Tetra(ana, serial):
        SrcChCoder.SrcCoderlib.Prm2bits_Tetra(ana, serial)

    def Coder_Tetra(ana, syn):
        SrcChCoder.SrcCoderlib.Coder_Tetra(ana, syn)

    def Post_Process(synth, L_frame):    
        SrcChCoder.SrcCoderlib.Post_Process(synth, L_frame)

    ChCoderlib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Transmitter/libs/tetraChCoderLib.so')

    ChCoderlib.Channel_Encoding.argtypes    = [c_int16, c_int16, ND_POINTER, ND_POINTER]
    ChCoderlib.Interleaving_Signalling.argtypes = [ND_POINTER, ND_POINTER]
    ChCoderlib.Interleaving_Signalling.restype  = c_int16
    ChCoderlib.Interleaving_Speech.argtypes     = [ND_POINTER, ND_POINTER]
    ChCoderlib.Interleaving_Speech.restype      = c_int16

    def Channel_Encoding(first_pass, FS_Flag, Vocod_array, Coded_array):
        SrcChCoder.ChCoderlib.Channel_Encoding(first_pass,FS_Flag,Vocod_array,Coded_array)

    def Interleaving_Signalling( Input_Frame, Output_frame ):
        return SrcChCoder.ChCoderlib.Interleaving_Signalling( Input_Frame, Output_frame )

    def Interleaving_Speech( Input_Frame, Output_frame ):
        return SrcChCoder.ChCoderlib.Interleaving_Speech( Input_Frame, Output_frame )
    
    def __init__(self, mic_gain = 1):
        gr.basic_block.__init__(
            self,
            name='Tetra Voice Coder',   # will show up in GRC
            in_sig=[np.float32],
            out_sig=[np.uint8]
        )
        self.mic_gain = mic_gain
        
        self.syn    = np.array([0]*L_FRAME,     dtype = 'int16')
        self.ana    = np.array([0]*ANA_SIZE,    dtype = 'int16')    #/* Analysis parameters.   */
        self.serial = np.array([0]*SERIAL_SIZE, dtype = 'int16') #/* Serial stream.         */

        self.Loop_counter = 0
        self.first_pass = True
        self.FS_Flag = False
        self.Vocod_array         = np.array([0]*274, dtype = 'int16')
        self.Coded_array             = np.array([0]*432, dtype = 'int16')
        self.Interleaved_coded_array = np.array([0]*432, dtype = 'int16')

        SrcChCoder.Init_Pre_Process()
        SrcChCoder.Init_Coder_Tetra()
     
    def general_work(self, input_items, output_items):
        in_index = 0
        out_index = 0

        frame = 0

        while (len(input_items[0]) >= in_index + IN_STEP) and (len(output_items[0]) >= out_index + OUT_STEP) :
            # multiply 32768 to convert from floating to int16
            new_speech = np.array(input_items[0][in_index:in_index+480]*32768 * self.mic_gain, dtype = 'int16')
            # process 1st 240 samples
            SrcChCoder.Pre_Process(new_speech, L_FRAME)
            SrcChCoder.Copy_Speech_Frame(new_speech)    # copy to c global 'new_speech' pointer
            SrcChCoder.Coder_Tetra(self.ana, self.syn)
            SrcChCoder.Post_Process(self.syn, L_FRAME)
            SrcChCoder.Prm2bits_Tetra(self.ana, self.serial)
            self.Vocod_array[  0:137] = self.serial[1:SERIAL_SIZE]
            
            # process next 240 samples
            SrcChCoder.Pre_Process(new_speech[240:], L_FRAME)
            SrcChCoder.Copy_Speech_Frame(new_speech[240:])    # copy to c global 'new_speech' pointer
            SrcChCoder.Coder_Tetra(self.ana, self.syn)
            SrcChCoder.Post_Process(self.syn, L_FRAME)
            SrcChCoder.Prm2bits_Tetra(self.ana, self.serial)
            self.Vocod_array[137:274] = self.serial[1:SERIAL_SIZE]

            SrcChCoder.Channel_Encoding(self.first_pass, self.FS_Flag, self.Vocod_array, self.Coded_array)
            self.first_pass = False

            if (not self.FS_Flag):
                SrcChCoder.Interleaving_Speech(self.Coded_array, self.Interleaved_coded_array)
            else:
                SrcChCoder.Interleaving_Signalling( self.Coded_array[216:], self.Interleaved_coded_array[216:] )
                self.Interleaved_coded_array[0:216] = self.Coded_array[0:216]
                print("FS_Flag ON")
                    
            self.Loop_counter += 1

            if type(output_items[0][0]) == np.int16:
                output_items[0][out_index:out_index+432] = self.Interleaved_coded_array
            else: #uint8
                for i in range(432):
                    output_items[0][out_index+i] = 0 if (self.Interleaved_coded_array[i] == 127) else 1

            in_index += IN_STEP
            out_index += OUT_STEP
            
        self.consume(0, in_index)
        return out_index
