"""
- perform the ctypes conversion to call MacEncoderLib dynamic library in c++
- inspect the coding functions whether being performed correctly or not by
feeding a sample burst input and check the result (uppMac_str). 
"""
from ctypes import *
import numpy as np

#std::vector<uint8_t> ptrVec(std::uint8_t* ptr, int size) 

MacEncoderLib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Transmitter/libs/MacEncoderLib.so') # class level loading lib

# C-type corresponding to numpy array 
ND_POINTER = np.ctypeslib.ndpointer(dtype=np.uint8, 
                                      ndim=1,
                                      flags="C")
def serviceMacEncode(in_data, in_len, out_data):
    MacEncoderLib.serviceMacEncode.argtypes = [ND_POINTER, c_size_t, ND_POINTER]
    MacEncoderLib.serviceMacEncode.restype = c_size_t
    res = MacEncoderLib.serviceMacEncode(in_data, in_len, out_data)
    return res

def Init_serviceMacEncode(show_txt, talkgroup_id, radio_id):
    MacEncoderLib.Init_serviceMacEncode.argtypes = [c_bool, c_size_t, c_size_t]
    MacEncoderLib.Init_serviceMacEncode(show_txt, talkgroup_id, radio_id)

def serviceMacDecode(in_data, in_len, out_data):
    MacEncoderLib.serviceMacDecode.argtypes = [ND_POINTER, c_size_t, ND_POINTER]
    MacEncoderLib.serviceMacDecode.restype = c_size_t
    res = MacEncoderLib.serviceMacDecode(in_data, in_len, out_data)
    return res

def descramble(data, size, scramblingCode):
    MacEncoderLib.descramble_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t, c_size_t]
    MacEncoderLib.descramble_raw.restype = c_size_t
    res = np.array([0]*size, dtype='uint8')
    res_len = MacEncoderLib.descramble_raw(data, len(data), res, size, scramblingCode)
    #DEBUG print("descramble: ", res_len)
    return res

def scramble(data, size, scramblingCode):
    MacEncoderLib.scramble_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t, c_size_t]
    MacEncoderLib.scramble_raw.restype = c_size_t
    res = np.array([0]*size, dtype='uint8')
    res_len = MacEncoderLib.scramble_raw(data, len(data), res, size, scramblingCode)
    #DEBUG print("descramble: ", res_len)
    return res

def deinterleave(data, K, a):
    MacEncoderLib.deinterleave_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t, c_size_t]
    MacEncoderLib.deinterleave_raw.restype = c_size_t
    res = np.array([0]*K, dtype='uint8')
    res_len = MacEncoderLib.deinterleave_raw(data, len(data), res, K, a)
    #DEBUG print("deinterleave: ", res_len)
    return res

def interleave(data, K, a):
    MacEncoderLib.interleave_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t, c_size_t]
    MacEncoderLib.interleave_raw.restype = c_size_t
    res = np.array([0]*K, dtype='uint8')
    res_len = MacEncoderLib.interleave_raw(data, len(data), res, K, a)
    #DEBUG print("deinterleave: ", res_len)
    return res
    
def depuncture23(data, length):
    MacEncoderLib.depuncture23_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t]
    MacEncoderLib.depuncture23_raw.restype = c_size_t
    res = np.array([0]*(4*length*2//3), dtype='uint8')
    res_len = MacEncoderLib.depuncture23_raw(data, len(data), res, length)
    #DEBUG print("depuncture23: ", res_len)
    return res

def puncture23(data, length):
    MacEncoderLib.puncture23_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t]
    MacEncoderLib.puncture23_raw.restype = c_size_t
    res = np.array([0]*length, dtype='uint8')
    res_len = MacEncoderLib.puncture23_raw(data, len(data), res, length)
    #DEBUG print("puncture23: ", res_len)
    return res

def motherEncode1614(data, length):
    MacEncoderLib.motherEncode1614_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t]
    MacEncoderLib.motherEncode1614_raw.restype = c_size_t
    res = np.array([0]*(length * 4), dtype='uint8')
    res_len = MacEncoderLib.motherEncode1614_raw(data, len(data), res, length)
    #DEBUG print("motherEncode1614: ", res_len)
    return res

def viterbiDecode1614(data):
    MacEncoderLib.viterbiDecode1614_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER]
    MacEncoderLib.viterbiDecode1614_raw.restype = c_size_t
    res = np.array([0]*(len(data)//4), dtype='uint8')
    res_len = MacEncoderLib.viterbiDecode1614_raw(data, len(data), res)
    #DEBUG print("viterbiDecode1614: ", res_len)
    return res

def checkCrc16Ccitt(data, length):
    MacEncoderLib.checkCrc16Ccitt_raw.argtypes = [ND_POINTER, c_size_t, c_size_t]
    MacEncoderLib.checkCrc16Ccitt_raw.restype = c_bool
    res_value = MacEncoderLib.checkCrc16Ccitt_raw(data, len(data), length)
    #DEBUG print("checkCrc16Ccitt: ", res_value)
    return res_value

def computeCrc16Ccitt(data, length):
    MacEncoderLib.computeCrc16Ccitt_raw.argtypes = [ND_POINTER, c_size_t, c_size_t]
    MacEncoderLib.computeCrc16Ccitt_raw.restype  = c_size_t
    res_value = MacEncoderLib.computeCrc16Ccitt_raw(data, len(data), length)
    #DEBUG print("computeCrc16Ccitt: ", res_value)
    return res_value

fin ='/home/ctn008/tetraDMO-Transmitter/samples/tetraDMO_s36kBit_uplane.uint8'
fout ='/home/ctn008/tetraDMO-Transmitter/samples/tetraDMO_s36kBit_tx.uint8'
in_data = np.fromfile(fin, dtype = 'uint8')
out_data = np.array([0]*(len(in_data)*5 + 510), dtype = 'uint8')

res = serviceMacEncode(in_data, len(in_data), out_data)
"""
n = 476
for i in range(n,n+1):
    for j in range (1):
        res = service_mac(in_data, i, out_data)
        print(f"i= %d, j=%d" %( i, j))


i = 0
bkn1[j]while i < res:
    print("Bursttype: ", out_data[i])
    i+=510

"""
#out_data.tofile(fout)
