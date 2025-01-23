"""
- perform the ctypes conversion to call tetraSpDecoderLib dynamic library in c++
- inspect the decoding functions whether being performed correctly or not by
feeding a sample (.cod file) input and check the result (.raw file). 
"""

from ctypes import *
import numpy as np

#std::vector<uint8_t> ptrVec(std::uint8_t* ptr, int size) 

lib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Receiver/codec/tetraSpDecoderLib.so') # class level loading lib

# C-type corresponding to numpy array 
ND_POINTER = np.ctypeslib.ndpointer(dtype=np.int16, 
                                      ndim=1,
                                      flags="C")
lib.Bits2prm_Tetra.argtypes = [ND_POINTER, ND_POINTER]
lib.Decod_Tetra.argtypes    = [ND_POINTER, ND_POINTER]
lib.Post_Process.argtypes   = [ND_POINTER, c_int16]

def Init_Decod_Tetra():
    lib.Init_Decod_Tetra()
    
def Bits2prm_Tetra(serial, parm):
    lib.Bits2prm_Tetra(serial, parm)

def Decod_Tetra(parm, synth):
    lib.Decod_Tetra(parm, synth)

def Post_Process(synth, L_frame):    
    lib.Post_Process(synth, L_frame)


fileNcod = '/home/ctn008/tetraDMO-Receiver/codec/tetraDMO_01.cod'
fileNraw = '/home/ctn008/tetraDMO-Receiver/codec/tetraDMO_01GEN.raw'

sig_cod = np.fromfile(fileNcod, dtype = 'int16')
sig_raw = np.array([0]*( (len(sig_cod)//138) + 1 ) * 240, dtype = 'int16')

L_FRAME     = 240
SERIAL_SIZE = 138
PRM_SIZE    = 24

synth = np.array([0]*L_FRAME, dtype ='int16')      #/* Synthesis              */
parm  = np.array([0]*PRM_SIZE, dtype ='int16')     #/* Synthesis parameters   */
serial= np.array([0]*SERIAL_SIZE, dtype ='int16')  #/* Serial stream          */

cod_ptr = 0
raw_ptr = 0

Init_Decod_Tetra()
frame = 0
while cod_ptr + SERIAL_SIZE <= len(sig_cod):

    for j in range (SERIAL_SIZE):
        serial[j] = sig_cod[cod_ptr+j]
        
    cod_ptr += SERIAL_SIZE
    frame +=1
    
    print("frame= %d" %frame)
    Bits2prm_Tetra(serial, parm)
    Decod_Tetra(parm, synth)
    Post_Process(synth, L_FRAME)

    for j in range (L_FRAME):
        sig_raw[raw_ptr+j] = synth[j]
    
    raw_ptr += L_FRAME
    #DEBUG print(synth)
    #DEBUG input("Press any key to continue.")
#sig_raw.tofile(fileNraw)


