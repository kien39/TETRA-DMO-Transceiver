"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr
from ctypes import *

DSB = 1
DNB = 2
DNB_SF = 3
INACTIVE = 0
BURST_LEN = 510
IN_STEP = 512
OUT_STEP = 480

class tetraTime():
    def __init__(self):
        self.tn = 1
        self.fn = 1
        self.mn = 1
    def timeIncrease(self):
        self.tn += 1
        if self.tn > 4:
            self.tn -= 4
            self.fn += 1
        if self.fn > 18:
            self.fn -= 18
            self.mn += 1
        
class tetraCell():
    def __init__(self):
        self.m_mcc               = 0
        self.m_mnc               = 0
        self.m_colorCode         = 0
        self.m_scramblingCode    = 0
        self.m_locationArea      = 0

        self.m_downlinkFrequency = 0
        self.m_uplinkFrequency   = 0

        self.m_cellInformationsAcquired = False
        
    def updateScramblingCode(self, sourceAddress, mnIdentity): 
        self.m_mcc = mnIdentity >> 14
        self.m_mnc = mnIdentity & 0x3ff

        self.m_scramblingCode = sourceAddress | ((mnIdentity & 0x003f) << 24)
        self.m_scramblingCode = (self.m_scramblingCode << 2) | 0x0003

        self.m_cellInformationsAcquired = True

    def getScramblingCode(self):
        return self.m_scramblingCode

class lowerMac(gr.basic_block):
    # C-type corresponding to numpy array 
    ND_POINTER = np.ctypeslib.ndpointer(dtype=np.uint8, ndim=1,flags="C")
    lib = cdll.LoadLibrary('/home/ctn008/tetraDMO-Receiver/codingLib.so') # class level loading lib
    lib.descramble_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t, c_size_t]
    lib.descramble_raw.restype = c_size_t
    lib.deinterleave_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t, c_size_t]
    lib.deinterleave_raw.restype = c_size_t
    lib.depuncture23_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER, c_size_t]
    lib.depuncture23_raw.restype = c_size_t
    lib.viterbiDecode1614_raw.argtypes = [ND_POINTER, c_size_t, ND_POINTER]
    lib.viterbiDecode1614_raw.restype = c_size_t
    lib.checkCrc16Ccitt_raw.argtypes = [ND_POINTER, c_size_t, c_size_t]
    lib.checkCrc16Ccitt_raw.restype = c_bool
    

    def descramble(data, size, scramblingCode):
        res = np.array([0]*size, dtype='uint8')
        res_len = lowerMac.lib.descramble_raw(data, len(data), res, size, scramblingCode)
        return res

    def deinterleave(data, K, a):
        res = np.array([0]*K, dtype='uint8')
        res_len = lowerMac.lib.deinterleave_raw(data, len(data), res, K, a)
        return res
        
    def depuncture23(data, length):
        res = np.array([0]*(4*length*2//3), dtype='uint8')
        res_len = lowerMac.lib.depuncture23_raw(data, len(data), res, length)
        return res

    def viterbiDecode1614(data):
        res = np.array([0]*(len(data)//4), dtype='uint8')
        res_len = lowerMac.lib.viterbiDecode1614_raw(data, len(data), res)
        return res

    def checkCrc16Ccitt(data, length):
        res_value = lowerMac.lib.checkCrc16Ccitt_raw(data, len(data), length)
        return res_value

    def getValue(pdu, pos, len):
        temp = 0
        for i in range(len):
            temp = temp * 2 + pdu[pos+i]
        return temp
    
    def __init__(self, mode= True):
        gr.basic_block.__init__(
            self,
            name='serviceLowerMAC\nserviceUpperMAC',   # will show up in GRC
            in_sig=[np.uint8],
            out_sig=[np.uint8]
        )
        self.mode = mode
        self.burst_cnt = 0
        self.commType = 0

        self.m_tetraTime = tetraTime()
        self.m_tetraCell = tetraCell()
        self.m_sfStolenFlag = False
        
        self.fragFlag = 0
        self.destinAddrType = 0
        self.destinAddress  = 0
        self.sourceAddrType = 0
        self.sourceAddress  = 0
        self.mnIdentity     = 0
           
    def processDmacSync(self, pdu):
        self.commType       = lowerMac.getValue(pdu, 6, 2)
        self.m_tetraTime.tn = lowerMac.getValue(pdu, 12, 2) + 1
        self.m_tetraTime.fn = lowerMac.getValue(pdu, 14, 5)
        self.fragFlag       = lowerMac.getValue(pdu, 71, 1)

        #DEBUG print("TN:FN", self.m_tetraTime.tn,':', self.m_tetraTime.fn, " fragFlag: ", self.fragFlag)
        
        pos = 76 if self.fragFlag else 72
        self.frameCntDn     = lowerMac.getValue(pdu, pos, 2)
        pos += 2
        self.destinAddrType = lowerMac.getValue(pdu, pos, 2)
        pos += 2
        if self.destinAddrType != 2:
            self.destinAddress = lowerMac.getValue(pdu, pos, 24)
            pos += 24
        self.sourceAddrType = lowerMac.getValue(pdu, pos, 2)
        pos += 2
        if self.sourceAddrType != 2:
            self.sourceAddress = lowerMac.getValue(pdu, pos, 24)
            pos += 24
        if (self.commType == 0) or (self.commType == 1):
            self.mnIdentity = lowerMac.getValue(pdu, pos, 24)
            pos += 24
        if self.mnIdentity and self.sourceAddress :
            self.m_tetraCell.updateScramblingCode(self.sourceAddress, self.mnIdentity)

        #DEBUG print("Scrambling code: ", hex(self.m_tetraCell.getScramblingCode()) )
        #DEBUG print("Source address: ", self.sourceAddress, "Destin address: ", self.destinAddress)
        
    def general_work(self, input_items, output_items):
        in_index = 0
        out_index = 0
        while (len(input_items[0]) >= in_index + IN_STEP) and (len(output_items[0]) >= out_index + OUT_STEP):
            print("MN:FN:TN = ", self.m_tetraTime.mn,":",self.m_tetraTime.fn,":",self.m_tetraTime.tn, " ", end ='')
            if input_items[0][in_index+BURST_LEN] == DSB:
                bkn1 = np.array(input_items[0][in_index+34+94 :in_index+34+94 +120])
                bkn2 = np.array(input_items[0][in_index+34+252:in_index+34+252+216])

                bkn1 = lowerMac.descramble(bkn1, 120, 0x03)
                bkn1 = lowerMac.deinterleave(bkn1, 120, 11)
                bkn1 = lowerMac.depuncture23(bkn1, 120)
                bkn1 = lowerMac.viterbiDecode1614(bkn1)

                bkn2 = lowerMac.descramble(bkn2, 216, 0x03)
                bkn2 = lowerMac.deinterleave(bkn2, 216, 101)
                bkn2 = lowerMac.depuncture23(bkn2, 216)
                bkn2 = lowerMac.viterbiDecode1614(bkn2)

                bkn1crc = lowerMac.checkCrc16Ccitt(bkn1, 76)
                bkn2crc = lowerMac.checkCrc16Ccitt(bkn2, 140)
                
                if (bkn1crc==False) or (bkn2crc==False):
                    print("DSB ", "CRC: ", bkn1crc, bkn2crc)
                else:
                    print("DSB")
                    bkn1 = np.concatenate((bkn1[:60], bkn2[:124]))
                    self.processDmacSync( bkn1 )

                    """ bkn1_txt = ''
                    for i in range(len(bkn1)):
                        bkn1_txt += '1' if bkn1[i] else '0'
                    print(bkn1_txt)
                    """      
            elif input_items[0][in_index+BURST_LEN] == DNB:
                bkn1 = np.array(input_items[0][in_index+34+14 :in_index+34+14 +216])
                bkn2 = np.array(input_items[0][in_index+34+252:in_index+34+252+216])
                bkn1 = np.concatenate((bkn1, bkn2))
                bkn1 = lowerMac.descramble(bkn1, 432, self.m_tetraCell.getScramblingCode())
                
                print("DNB")
                output_items[0][out_index:out_index+len(bkn1)] = bkn1
                out_index += OUT_STEP
                output_items[0][out_index-1] = 0 # set SPEECH FRAME

            elif input_items[0][in_index+BURST_LEN] == DNB_SF:
                bkn1ValidFlag = False
                bkn2ValidFlag = False

                # BKN1 block - DMAC STCH 
                bkn1 = np.array(input_items[0][in_index+34+14 :in_index+34+14 +216])
                bkn1 = lowerMac.descramble(bkn1, 216, self.m_tetraCell.getScramblingCode())
                bkn1 = lowerMac.deinterleave(bkn1, 216, 101)                                 
                bkn1 = lowerMac.depuncture23(bkn1, 216)                                      
                bkn1 = lowerMac.viterbiDecode1614(bkn1)                                      
                if (lowerMac.checkCrc16Ccitt(bkn1, 140)) :
                    bkn1 = np.array(bkn1[:124])
                    bkn1ValidFlag = True

                # BKN2 block - DMAC STCH or TCH/S
                bkn2 = np.array(input_items[0][in_index+34+252:in_index+34+252+216])
                bkn2 = lowerMac.descramble(bkn2, 216, self.m_tetraCell.getScramblingCode())
                bkn2_uplane = np.array(bkn2)
                bkn2 = lowerMac.deinterleave(bkn2, 216, 101)
                bkn2 = lowerMac.depuncture23(bkn2, 216)
                bkn2 = lowerMac.viterbiDecode1614(bkn2)         
                if (lowerMac.checkCrc16Ccitt(bkn2, 140)):         # check CRC if FALSE ==> TCH/S
                    bkn2 = np.array(bkn1[:124])
                    bkn2ValidFlag = True

                print("DNB_SF")
            
                if (bkn1ValidFlag):
                    pass # serviceUpperMac(Pdu(bkn1), DSTCH);                               // first block is stolen for C or U signalling
                else:    # for debug purpose - display if CRC errors found
                    print(f"DNB_SF Burst CRC check errors found BKN1 TN/FN/MN: %u/%u/%u. \n\n" % (self.m_tetraTime.tn, self.m_tetraTime.fn, self.m_tetraTime.mn))
                
                output_items[0][out_index:out_index+OUT_STEP] = np.array([0]*OUT_STEP, dtype='uint8')
                
                if (self.m_sfStolenFlag):        # if second-half slot is also stolen
                    if (bkn2ValidFlag):
                        pass #serviceUpperMac(Pdu(bkn2), DSTCH);                           // second block also stolen, reset flag
                    else:
                        print(f"DNB_SF Burst CRC check errors found BKN2 TN/FN/MN: %u/%u/%u. \n\n" %(self.m_tetraTime.tn, self.m_tetraTime.fn, self.m_tetraTime.mn))                    
                else:
                    output_items[0][out_index+216:out_index+432] = bkn2_uplane

                out_index += OUT_STEP
                output_items[0][out_index-1] = 1 # set STOLEN FRAME                
            else:
                print("Inactive")

            in_index += IN_STEP
            self.burst_cnt += 1
            self.m_tetraTime.timeIncrease()

        self.consume(0, in_index) #consume port 0 input
        return out_index
