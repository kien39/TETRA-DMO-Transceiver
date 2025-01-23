"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr

class Pi4DQPSK(gr.interp_block):
    # Create our raised-cosine filter
    num_taps = 11 * 2 + 1
    beta = 0.35
    Ts = 2
    t = np.arange(num_taps) - (num_taps-1) // 2
    h = np.sinc(t/Ts) * np.cos(np.pi*beta*t/Ts) / (1 - (2*beta*t/Ts)**2)
    preConvI = np.array([0] * 22, dtype = 'float32')
    preConvQ = np.array([0] * 22, dtype = 'float32')
    
    def __init__(self, envelope_in = False):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.interp_block.__init__(
            self,
            name='Pi4DQPSK 18kDibit-to-36kIQk',   	# will show up in GRC
            in_sig=[np.uint8, np.uint8,],
            out_sig=[np.complex64], interp = 2
        )
        self.interp = 2        
        self.cur_phase_index = 	0				# current phase index value S(0) = 1
        self.envelope_in = envelope_in
        
    def work(self, input_items, output_items):
        # Phase Index Change Table: corresponding to current phase (row 0-7) and current dibit value (column 0-3)
        # B2k_1,B2k:   00 01 10 11
            
        phase_shift = (1, 3, 7, 5)

        # Phase value corresponding to Phase Index
        IQ_table = (( np.cos(np.pi/4 * 0) , np.sin(np.pi/4 * 0) ),
                    ( np.cos(np.pi/4 * 1) , np.sin(np.pi/4 * 1) ),
                    ( np.cos(np.pi/4 * 2) , np.sin(np.pi/4 * 2) ),
                    ( np.cos(np.pi/4 * 3) , np.sin(np.pi/4 * 3) ),
                    ( np.cos(np.pi/4 * 4) , np.sin(np.pi/4 * 4) ),
                    ( np.cos(np.pi/4 * 5) , np.sin(np.pi/4 * 5) ),
                    ( np.cos(np.pi/4 * 6) , np.sin(np.pi/4 * 6) ),
                    ( np.cos(np.pi/4 * 7) , np.sin(np.pi/4 * 7) ))

		# process first sample - set initial phase
        """ self.cur_phase_index += phase_shift[ 0 ]	# don't take the 1st input - due to error ???
        if self.cur_phase_index >= 8:
            self.cur_phase_index -= 8
        output_items[0][0] = IQ_table[ self.cur_phase_index ][0]
        output_items[1][0] = IQ_table[ self.cur_phase_index ][1]
		"""
        # perform rrc filter
        tempI = np.array([0] * (len(input_items[0]) * 2), dtype='float32') # 22 = rrc_taps - 1
        tempQ = np.array([0] * (len(input_items[0]) * 2), dtype='float32') # 22 = rrc_taps - 1

        for i in range (0,len(input_items[0])):
            if self.envelope_in:
                input_dibit = input_items[0][i] & input_items[1][i]
            else:
                input_dibit = input_items[0][i]
                
            self.cur_phase_index += phase_shift[ input_dibit ]
            if self.cur_phase_index >= 8:
                self.cur_phase_index -= 8

            if input_items[1][i] :
                tempI[i*2] = IQ_table[ self.cur_phase_index ][0]
                tempQ[i*2] = IQ_table[ self.cur_phase_index ][1]

        tempIconvolved = np.convolve(tempI, Pi4DQPSK.h) 
        tempQconvolved = np.convolve(tempQ, Pi4DQPSK.h)
        
        for i in range(len(Pi4DQPSK.h)-1):
            tempIconvolved[i] += Pi4DQPSK.preConvI[i]
            tempQconvolved[i] += Pi4DQPSK.preConvQ[i]

        for i in range(len(Pi4DQPSK.h)-1):
            Pi4DQPSK.preConvI[-i-1] = tempIconvolved[-i-1]
            Pi4DQPSK.preConvQ[-i-1] = tempQconvolved[-i-1]
        for i in range(len(input_items[0])*2):
            output_items[0][i] = tempIconvolved [i] + 1j*tempQconvolved[i]
                                   
        return len(output_items[0])
