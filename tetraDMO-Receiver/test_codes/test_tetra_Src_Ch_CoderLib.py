"""
- perform the ctypes conversion to call tetraSpDecoderLib dynamic library in c++
- inspect the decoding functions whether being performed correctly or not by
feeding a sample (.cod file) input and check the result (.raw file). 
"""

from ctypes import *
import numpy as np

#*********************#
# Test tetraSrcCoder  #
#*********************#

SrcCoderlib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Receiver/codec/tetraSrcCoderLib.so')

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
    SrcCoderlib.Copy_Speech_Frame(speech_frame)

def Init_Pre_Process():
    SrcCoderlib.Init_Pre_Process()

def Init_Coder_Tetra():
    SrcCoderlib.Init_Coder_Tetra()
    
def Pre_Process(new_speech, L_frame):
    SrcCoderlib.Pre_Process(new_speech, L_frame)

def Prm2bits_Tetra(ana, serial):
    SrcCoderlib.Prm2bits_Tetra(ana, serial)

def Coder_Tetra(ana, syn):
    SrcCoderlib.Coder_Tetra(ana, syn)

def Post_Process(synth, L_frame):    
    SrcCoderlib.Post_Process(synth, L_frame)


fileNraw = '/home/ctn008/tetraDMO-Receiver/codec/audio_src_01.raw'
fileNcod = '/home/ctn008/tetraDMO-Receiver/codec/audio_src_01GEN.cod'


sig_raw = np.fromfile(fileNraw, dtype = 'int16')
sig_cod = np.array([0]*( (len(sig_raw)//240) + 1 ) * 138, dtype = 'int16')

L_FRAME     = 240
SERIAL_SIZE = 138
ANA_SIZE    =  23

syn    = np.array([0]*L_FRAME, dtype = 'int16')
ana    = np.array([0]*ANA_SIZE, dtype = 'int16')    #/* Analysis parameters.   */
serial = np.array([0]*SERIAL_SIZE, dtype = 'int16') #/* Serial stream.         */

new_speech = np.array([], dtype='int16')           #/* Pointer on new_speech. */

raw_ptr = 0
cod_ptr = 0

Init_Pre_Process()
Init_Coder_Tetra()
frame = 0

while raw_ptr + L_FRAME <= len(sig_raw):

    new_speech = np.array(sig_raw[raw_ptr:raw_ptr+L_FRAME])
    Pre_Process(new_speech, L_FRAME)
    Copy_Speech_Frame(new_speech)    # copy to c global 'new_speech' pointer
    Coder_Tetra(ana, syn)

    Post_Process(syn, L_FRAME)
    Prm2bits_Tetra(ana, serial)
            
    sig_cod[cod_ptr:cod_ptr+SERIAL_SIZE] = serial

    raw_ptr += L_FRAME
    cod_ptr += SERIAL_SIZE
    frame +=1
    print("frame= %d" %frame)

    
    #print(ana)
    #print(serial)
    #input("Press any key to continue.")
sig_cod.tofile(fileNcod)
input("Complete testing SrcCoder. Press anykey to continue testing ChCoder.")

#*********************#
# Test tetraChCoder   #
#*********************#

ChCoderlib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Receiver/codec/tetraChCoderLib.so')

ChCoderlib.Channel_Encoding.argtypes    = [c_int16, c_int16, ND_POINTER, ND_POINTER]
ChCoderlib.Interleaving_Signalling.argtypes = [ND_POINTER, ND_POINTER]
ChCoderlib.Interleaving_Signalling.restype  = c_int16
ChCoderlib.Interleaving_Speech.argtypes     = [ND_POINTER, ND_POINTER]
ChCoderlib.Interleaving_Speech.restype      = c_int16



def Channel_Encoding(first_pass, FS_Flag, Vocod_array, Coded_array):
    ChCoderlib.Channel_Encoding(first_pass,FS_Flag,Vocod_array,Coded_array)

def Interleaving_Signalling( Input_Frame, Output_frame ):
    return ChCoderlib.Interleaving_Signalling( Input_Frame, Output_frame )

def Interleaving_Speech( Input_Frame, Output_frame ):
    return ChCoderlib.Interleaving_Speech( Input_Frame, Output_frame )

Loop_counter = 0
first_pass = True
Vocod_array             = np.array([0]*274, dtype = 'int16')
Coded_array             = np.array([0]*432, dtype = 'int16')
Interleaved_coded_array = np.array([0]*432, dtype = 'int16')
FS_Flag = 0

fileNchan = '/home/ctn008/tetraDMO-Receiver/codec/audio_src_01GEN.chan'
sig_chan = np.array([0]*( (len(sig_raw)//240) + 1 ) * 216, dtype = 'int16')

cod_ptr = 0
chan_ptr = 0

while cod_ptr + 276 <= len(sig_cod):
    Vocod_array[  0:137] = sig_cod[cod_ptr+  1:cod_ptr+138]
    Vocod_array[137:274] = sig_cod[cod_ptr+139:cod_ptr+276]

    Channel_Encoding(first_pass, FS_Flag, Vocod_array, Coded_array)
    first_pass = False

    if (not FS_Flag):
        Interleaving_Speech(Coded_array, Interleaved_coded_array)
    else:
        Interleaving_Signalling( Coded_array + 216, Interleaved_coded_array + 216 )
        Interleaved_coded_array[0:216] = Coded_array[0:216]
	    
    Loop_counter += 1

    sig_chan[chan_ptr:chan_ptr+432] = Interleaved_coded_array
    cod_ptr  += 276
    chan_ptr += 432

print(f"%d Speech Frames processed" %(2*Loop_counter))
print(f"ie %ld Channel Frames" %Loop_counter)
    
#
sig_chan.tofile(fileNchan)
