#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <vector>
#include <string>

#include "mac.h"

using namespace Tetra;

// Sample plutoDMO signal tn1   TN: 01 FN: 10000 CD: 00
// DMAC_SYNC='1101000000000110000000000000000000000000000000000000000000000000000000100000000000000000001111101001010100110100101011100001100111000100000000011001000100001110000000000000000001000100'
// generate signal from FR01 to FR16 of repeated DMAC_SYNC every 04 Frames.

const uint8_t DMAC_SYNC[184] = {1,1,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,0,1,0,1,0,0,1,1,0,1,0,0,1,0,1,0,1,1,1,0,0,0,0,1,1,0,
                                        0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0}; 

struct TetraTime {
	uint8_t tn;                                                            ///< time slot
	uint8_t fn;                                                            ///< frame number
	uint8_t mn;                                                            ///< multi-frame number
};

struct dsbBurst {
	uint8_t guard34[34]  = {(34,0)};
	uint8_t preamb12[12] = {(12,0)};
	uint8_t pa02[2]      = {(2,0)};
	uint8_t freq80[80]   = {(80,0)};
	uint8_t bkn1[120]    = {(120,0)};
	uint8_t sts38[38]    = {(38,0)};
	uint8_t bkn2[216]    = {(216,0)};
	uint8_t tb02[2]      = {(2,0)};
	uint8_t guard06[6]   = {(6,0)};
};


const uint8_t TN_POS = 12;	// bit position in DMAC-SYNC pdu starts from zero 
const uint8_t FN_POS = 14;
const uint8_t CD_POS = 72;

int main(int argc, char * argv[])
{
	TetraTime m_tetraTime; 
	m_tetraTime.tn = 0;
	m_tetraTime.fn = 1;
	m_tetraTime.mn = 1;
	uint8_t m_frameCntDn = 3;
	dsbBurst m_dsbBurst;
	std::vector<uint8_t> bitStream;

	uint8_t Dmac_Sync[16][4][184] ={0};

	printf("Fill DMAC_SYNC pdu for 16 frames with repeated data every 04 frames \n");

	for (uint8_t i= 0; i<16; i++)
	{
		for (uint8_t j = 0; j < 4; j++)
		{
			for (uint8_t k=0; k < 184; k++)
			{
				Dmac_Sync[i][j][k] = DMAC_SYNC[k];
			}
			//update TN, FN, CD
			Dmac_Sync[i][j][TN_POS]   = m_tetraTime.tn >> 1;
			Dmac_Sync[i][j][TN_POS+1] = m_tetraTime.tn & 0x1;

			Dmac_Sync[i][j][FN_POS]   = m_tetraTime.fn >> 4;
			Dmac_Sync[i][j][FN_POS+1] = (m_tetraTime.fn >> 3) & 0x1;
			Dmac_Sync[i][j][FN_POS+2] = (m_tetraTime.fn >> 2) & 0x1;
			Dmac_Sync[i][j][FN_POS+3] = (m_tetraTime.fn >> 1) & 0x1;
			Dmac_Sync[i][j][FN_POS+4] = m_tetraTime.fn & 0x1;

			Dmac_Sync[i][j][CD_POS]   = m_frameCntDn >> 1;
			Dmac_Sync[i][j][CD_POS+1] = m_frameCntDn & 0x1;

			// update m_tetraTime and m_frameCntDn
			m_tetraTime.tn ++;
			if (m_tetraTime.tn == 4)
			{
				m_tetraTime.fn ++;
				m_tetraTime.tn = 0;		
			}
			m_frameCntDn --;
			m_frameCntDn = (m_frameCntDn == 0xff)? 3 : m_frameCntDn;
		}
	}
	for (uint8_t i= 0; i<16; i++)
	{
		for (uint8_t j = 0; j < 4; j++)
		{
			printf("%u %u TN: %u%u, FN: %u%u%u%u%u, CD: %u%u\n",i,j,Dmac_Sync[i][j][TN_POS], Dmac_Sync[i][j][TN_POS+1],Dmac_Sync[i][j][FN_POS], Dmac_Sync[i][j][FN_POS+1], 
			Dmac_Sync[i][j][FN_POS+2], Dmac_Sync[i][j][FN_POS+3], Dmac_Sync[i][j][FN_POS+4], Dmac_Sync[i][j][CD_POS], Dmac_Sync[i][j][CD_POS+1] );
		}
	}
	printf("\nComplete filled up DMAC-SYNC pdu.");

	// calculate bkn1 and bkn2 from Dmac_Sync
	std::vector<uint8_t> bkn1;
	std::vector<uint8_t> bkn2;

	// BKN1 block - SCH/S - in DMO DSB burst, BKN1 contains only SCH/S Synchronisation Channel
	bkn1 = viterbiDecode1614(bkn1);                                         // Viterbi decode - see 8.3.1.2  (K1 + 16, K1) block code with K1 = 60
	bkn1 = depuncture23(bkn1, 120);                                         // depuncture with 2/3 rate 120 bits -> 4 * 80 bits before Viterbi decoding
	bkn1 = deinterleave(bkn1, 120, 11);                                     // deinterleave 120, 11
	bkn1 = descramble(bkn1, 120, 0x0003);                                   // descramble with predefined code 0x0003
	bkn1 = vectorExtract(data, 94,  120);

	// BKN2 block - SCH/H - in DMO DSB burst, BKN2 contains only SCH/H Half block signaling; BKN2 block starts at bit pos 252
	bkn2 = vectorExtract(data, 252, 216);
	bkn2 = descramble(bkn2, 216, 0x0003);                                   // descramble with default scrambling code 0x0003
	bkn2 = deinterleave(bkn2, 216, 101);                                    // deinterleave
	bkn2 = depuncture23(bkn2, 216);                                         // depuncture with 2/3 rate 144 bits -> 4 * 144 bits before Viterbi decoding
	bkn2 = viterbiDecode1614(bkn2);                                         // Viterbi decode

}