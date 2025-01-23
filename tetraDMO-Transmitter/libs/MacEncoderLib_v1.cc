/*
 *  tetra-kit
 *  Copyright (C) 2020  LarryTh <dev@logami.fr>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include "MacEncoderLib_v1.h"

void timeIncrease()
{
    m_tetraTime.tn ++;
    if (m_tetraTime.tn > 4)
    {
        m_tetraTime.tn -=4;
        m_tetraTime.fn ++;
    }
    if (m_tetraTime.fn > 18)
    {
        m_tetraTime.fn -=18;
        m_tetraTime.mn ++;
    }

}

void updateTnFnCntDn_DmSetupOccupiedTxCeasedReserved()
{
    uint16_t pos = 12;
    DM_SETUP[pos]   = DM_OCCUPIED[pos]   = DM_TX_CEASED_DSB[pos]   = DM_RESERVED[pos]   = (m_tetraTime.tn-1) / 2; 
    DM_SETUP[pos+1] = DM_OCCUPIED[pos+1] = DM_TX_CEASED_DSB[pos+1] = DM_RESERVED[pos+1] = (m_tetraTime.tn-1) % 2;
    pos += 2; 
    DM_SETUP[pos]   = DM_OCCUPIED[pos]   = DM_TX_CEASED_DSB[pos]   = DM_RESERVED[pos]   = (m_tetraTime.fn >> 4) & 0x0001; 
    DM_SETUP[pos+1] = DM_OCCUPIED[pos+1] = DM_TX_CEASED_DSB[pos+1] = DM_RESERVED[pos+1] = (m_tetraTime.fn >> 3) & 0x0001;
    DM_SETUP[pos+2] = DM_OCCUPIED[pos+2] = DM_TX_CEASED_DSB[pos+2] = DM_RESERVED[pos+2] = (m_tetraTime.fn >> 2) & 0x0001; 
    DM_SETUP[pos+3] = DM_OCCUPIED[pos+3] = DM_TX_CEASED_DSB[pos+3] = DM_RESERVED[pos+3] = (m_tetraTime.fn >> 1) & 0x0001;
    DM_SETUP[pos+4] = DM_OCCUPIED[pos+4] = DM_TX_CEASED_DSB[pos+4] = DM_RESERVED[pos+4] = m_tetraTime.fn & 0x0001; 

    pos = 71;
    if (DM_SETUP[pos] == true) // check fragFlag
    {
        pos+= 5;
    }
    else
    {
        pos++;
    }
    DM_SETUP[pos]   = (m_frameCntDn >> 1) & 0x0001;
    DM_SETUP[pos+1] =  m_frameCntDn & 0x0001; 
}

void updatePtt(bool showTxt, bool ptt, const uint32_t talkgroupId, const uint32_t radioId)
{
    m_showTxt = showTxt;
    if (m_pttPressed != ptt)
    {
        m_pttPressed = ptt;
        if (m_pttPressed)
        {
            m_dmSetupFlag = true;
            printf("PTT pressed\n");

            m_showTxt = showTxt;
            m_sourceAddress = radioId;
            m_destinAddress = talkgroupId;
            
            updateScramblingCode(m_sourceAddress, (m_mcc << 14) + m_mnc);
            // update soure & destinAddress in DM_SETUP, DM_OCCUPIED, DM_RESERVED, DM_INFO, DM_TX_CEASED
            
            uint16_t pos = 71;
            uint8_t  bitval;

            if (DM_SETUP[pos] == true) // check fragFlag
            {
                pos+= 9;
            }
            else
            {
                pos+= 5;
            }
            for (uint16_t k=0; k<24; k++)
            {
                bitval = (m_destinAddress >> (23-k)) & 0x00000001;
                DM_SETUP[pos+k]    = bitval; 
                DM_OCCUPIED[pos+k] = bitval;
                DM_TX_CEASED_DSB[pos+k] = bitval;
                DM_TX_CEASED_DNBSF[12+k] = bitval;
            }
            pos += 24 + 2;
            for (uint16_t k=0; k<24; k++)
            {
                bitval = (m_sourceAddress >> (23-k)) & 0x00000001;
                DM_SETUP[pos+k]    = bitval; 
                DM_OCCUPIED[pos+k] = bitval;
                DM_TX_CEASED_DSB[pos+k] = bitval;
                DM_TX_CEASED_DNBSF[12+26+k] = bitval;
            }
            

        }
        else
        {
            m_dmTxReleased = true;
            printf("PTT released\n");

        }
    }
}

void generate_DmSetupOccupiedTxCeasedDsbReserved(uint16_t typeOfDsbBurst) 
// 0 = DM-SETUP, 1 = DM-OCCUPIED, 2 = DM-TX CEASED DSB, 3 = DM-RESERVED
{
    std::vector<uint8_t> DM_BURST;
    uint16_t crc = 0; 
    
    if (typeOfDsbBurst == 0)
    {
        DM_BURST = DM_SETUP;
    }
    else if (typeOfDsbBurst == 1)
    {
        DM_BURST = DM_OCCUPIED;
    }
    else if (typeOfDsbBurst == 2)
    {
        DM_BURST = DM_TX_CEASED_DSB;
    }
    else if (typeOfDsbBurst == 3)
    {
        DM_BURST = DM_RESERVED;
    }

    // insert GUARD34 & PREAMBLE_P3
    bkn1.insert(bkn1.end(),GUARD34.begin(), GUARD34.end());
    bkn1.insert(bkn1.end(),PREAMBLE_P3.begin(), PREAMBLE_P3.end());
    
    // insert PA bits
    bkn1.push_back(0);
    bkn1.push_back(0);
    // insert 80 bit freq correction
    bkn1.insert(bkn1.end(), FREQ_CORRECTION.begin(), FREQ_CORRECTION.end());

    // insert 60 -> 120 bit of 1st half DM_OCCUPIED
    bkn2.insert(bkn2.begin(), DM_BURST.begin(),DM_BURST.begin()+ 60);
    crc = computeCrc16Ccitt(bkn2, 60);
    crc = ~crc;
    for (uint16_t k=0; k<16; k++)
    {
        bkn2.insert(bkn2.end(), (crc>>(15-k)) & 0x0001 );
    }
    bkn2.push_back(0);
    bkn2.push_back(0);
    bkn2.push_back(0);
    bkn2.push_back(0);                
    bkn2 = motherEncode1614(bkn2, 80);                      // Viterbi encode - see 8.3.1.2  (K1 + 16, K1) block code with K1 = 60
    bkn2 = puncture23(bkn2, 120);                           // puncture with 2/3 rate 120 bits -> 4 * 80 bits before Viterbi decoding
    bkn2 = interleave(bkn2, 120, 11);                       // interleave 120, 11
    bkn2 = scramble(bkn2, 120, 0x0003);                     // scramble with predefined code 0x0003
    bkn1.insert(bkn1.end(), bkn2.begin(), bkn2.end());

    // insert NTS3
    bkn1.insert(bkn1.end(),SYNC_TRAINING_SEQ.begin(), SYNC_TRAINING_SEQ.end());

    // insert 124 - 216 bit of 2nd half DM_OCCUPIED
    bkn2.clear();
    bkn2.insert(bkn2.begin(), DM_BURST.begin()+60,DM_BURST.end());
    crc = computeCrc16Ccitt(bkn2, 124);
    crc = ~crc;
    for (uint16_t k=0; k<16; k++)
    {
        bkn2.insert(bkn2.end(), (crc>>(15-k)) & 0x0001 );
    }
    bkn2.push_back(0);
    bkn2.push_back(0);
    bkn2.push_back(0);
    bkn2.push_back(0);                
    bkn2 = motherEncode1614(bkn2, 144);                      // Viterbi encode - see 8.3.1.2  (K1 + 16, K1) block code with K1 = 60
    bkn2 = puncture23(bkn2, 216);                           // puncture with 2/3 rate 120 bits -> 4 * 80 bits before Viterbi decoding
    bkn2 = interleave(bkn2, 216, 101);                       // interleave 120, 11
    bkn2 = scramble(bkn2, 216, 0x0003);                     // scramble with predefined code 0x0003
    bkn1.insert(bkn1.end(), bkn2.begin(), bkn2.end());

    // insert 02 TAIL + 6 GUARD
    bkn1.insert(bkn1.end(),TAILGUARD8.begin(), TAILGUARD8.end());
}

void generate_Dm_TxCeased()
// 0 = TX_CEASED FOR DNB_SF
{
    uint16_t crc = 0; 
    DM_TX_CEASED_DNBSF[6] = ( (m_dmTxCeasedCntDn-1)>>1) & 0x0001;
    DM_TX_CEASED_DNBSF[7] = (m_dmTxCeasedCntDn-1) & 0x0001;
    
    bkn1.insert(bkn1.end(),GUARD34.begin(), GUARD34.end());
    bkn1.insert(bkn1.end(),PREAMBLE_P2.begin(), PREAMBLE_P2.end());
    
    // insert PA bits
    bkn1.push_back(0);
    bkn1.push_back(0);

    bkn2 = DM_TX_CEASED_DNBSF;
    crc = computeCrc16Ccitt(bkn2, 124);
    crc = ~crc;
    for (uint16_t k=0; k<16; k++)
    {
        bkn2.insert(bkn2.end(), (crc>>(15-k)) & 0x0001 );
    }
    bkn2.push_back(0);
    bkn2.push_back(0);
    bkn2.push_back(0);
    bkn2.push_back(0);                
    bkn2 = motherEncode1614(bkn2, 144);                      // Viterbi encode - see 8.3.1.2  (K1 + 16, K1) block code with K1 = 60
    bkn2 = puncture23(bkn2, 216);                           // puncture with 2/3 rate 120 bits -> 4 * 80 bits before Viterbi decoding
    bkn2 = interleave(bkn2, 216, 101);                       // interleave 120, 11
    bkn2 = scramble(bkn2, 216, getScramblingCode());                     // scramble with predefined code 0x0003
    bkn1.insert(bkn1.end(), bkn2.begin(), bkn2.end());

    bkn1.insert(bkn1.end(),NORMAL_TRAINING_SEQ_2.begin(), NORMAL_TRAINING_SEQ_2.end());
    // try to send bkn2 in second half frame ???
    bkn1.insert(bkn1.end(), bkn2.begin(), bkn2.end());
    // insert 02 TAIL + 6 GUARD
    bkn1.insert(bkn1.end(),TAILGUARD8.begin(), TAILGUARD8.end());
}

uint32_t serviceMacEncode(bool showTxt, bool ptt, const uint32_t talkgroupId, const uint32_t radioId, uint8_t* in_data, const uint32_t in_len, uint8_t* out_data, uint8_t* out_envelop)
/* in_data: 432 bit speech stream (uint8) from tetraSpeechEncoder (first assume no FS frame)
   out_data:510 * 4 tetraDMO burst bitstream to feed pi4dqpsk Modulator */
{
    /*iterate thru every items of in_data and feed to uPlane vector an internal processor
    */
    uint32_t out_ptr = 0;
    uint16_t crc = 0; 

    updatePtt(showTxt, ptt, talkgroupId, radioId);

    for (uint32_t i =0; i < in_len; i++)
    {
        tx.uPlane.push_back(in_data[i]);
        tx.bitCnt += 5;
    
        if (tx.bitCnt >= 510)
        {
            bkn1.clear();
            bkn2.clear();
            if (m_dmSetupFlag)    //transmit DM-SETUP frames.
            {
                if ((m_tetraTime.tn == 1) && (m_dmSetupStart == false))
                {
                    m_dmSetupStart = true;
                    m_dmSetupCntDn = 12;
                }

                if (m_dmSetupStart)
                {
                    m_dmSetupCntDn-- ;
                    if (m_dmSetupCntDn == 0)
                    {
                        m_dmSetupFlag = false;
                        m_dmSetupStart = false;
                    }
                    m_frameCntDn = (m_dmSetupCntDn/4);
                    // update FN, TN, FrameCntDn, etc... into DM_SETUP_OCCUPIED 
                    updateTnFnCntDn_DmSetupOccupiedTxCeasedReserved();

                    // generate DM_SETUP BURST and put to bkn1
                    generate_DmSetupOccupiedTxCeasedDsbReserved(0);
                    for (int16_t j=0; j< FRAME_LEN/2; j++)
                    {
                        out_data[out_ptr+j] = bkn1[2*j] * 2 + bkn1[2*j+1];
                        out_envelop[out_ptr+j] = 3;     // dibit "11"
                    }
                }
                else
                {
                    for (int16_t j=0; j< FRAME_LEN/2; j++)
                    {
                        out_data[out_ptr+j] = 0;
                        out_envelop[out_ptr+j] = 0;
                    }
                }
                
                if (tx.uPlane.size() >= 432*2)  // consume inputs, in all cases
                {
                    tx.uPlane.erase(tx.uPlane.begin(),tx.uPlane.begin()+432);
                }
                
            }
            else  // not dmSetupFlag --> either pttPressed or Released 
            {
                if (m_pttPressed)
                {
                    if ((m_tetraTime.tn == 1) && (m_tetraTime.fn != 18)) // sending DNB burst in TN1 of FN 1-17
                    {
                        if (m_showTxt)
                        {
                            printf("MN:FN:TN=%d:%d:%d   DNB\n", m_tetraTime.mn, m_tetraTime.fn, m_tetraTime.tn);
                        }
                        if (tx.uPlane.size() >= 432)
                        //if tx.uPlane vector has > 432 bit, transmitDNB: first 432 bit 
                        {
                            // process 432 bits 
                            bkn2 = scramble(tx.uPlane, UPLANE_LEN, getScramblingCode());
                            // remove 432 bit in tx.uPlane vector
                            tx.uPlane.erase(tx.uPlane.begin(),tx.uPlane.begin()+432);

                            // insert GUARD34 & PREAMBLE_P1
                            bkn1.insert(bkn1.end(),GUARD34.begin(), GUARD34.end());
                            bkn1.insert(bkn1.end(),PREAMBLE_P1.begin(), PREAMBLE_P1.end());
                            // insert PA bits
                            bkn1.insert(bkn1.end(), 0);
                            bkn1.insert(bkn1.end(), 0);
                            // insert 216 bit 1st half u_plane
                            bkn1.insert(bkn1.end(), bkn2.begin(), bkn2.begin()+216);
                            // insert NTS1
                            bkn1.insert(bkn1.end(),NORMAL_TRAINING_SEQ_1.begin(), NORMAL_TRAINING_SEQ_1.end());
                            // insert 216 bit 2nd half u_plane
                            bkn1.insert(bkn1.end(), bkn2.begin()+216, bkn2.end());
                            // insert 02 TAIL + 6 GUARD
                            bkn1.insert(bkn1.end(),TAILGUARD8.begin(), TAILGUARD8.end());
                            // copy DNB burst to output
                            for (int16_t j=0; j< (FRAME_LEN/2); j++)
                            {
                                out_data[out_ptr+j] = bkn1[j*2]*2 + bkn1[j*2+1];
                                out_envelop[out_ptr+j] = BURST_ENVELOP[j];
                            }
                        }
                        else    //not enough 432 input samples, fill out_data with zeros.
                        {
                            for (int16_t j=0; j< FRAME_LEN/2; j++)
                            {
                                out_data[out_ptr+j] = 0;
                                out_envelop[out_ptr+j] = 0;
                            }
                        }
                    } 
                    // sending DSB burst in TN3 of FN 6,12 and TN1, TN3 of FN18
                    else if( ( (m_tetraTime.tn == 3) && ((m_tetraTime.fn == 6) || (m_tetraTime.fn == 12)) ) || 
                            ( ((m_tetraTime.tn == 1) || (m_tetraTime.tn == 3)) && (m_tetraTime.fn == 18) ) )
                    {
                        if (m_showTxt)
                        {
                            printf("MN:FN:TN=%d:%d:%d   DSB\n", m_tetraTime.mn, m_tetraTime.fn, m_tetraTime.tn);
                        }
                        // update FN, TN, FrameCntDn, etc... into DM_OCCUPIED 
                        updateTnFnCntDn_DmSetupOccupiedTxCeasedReserved();

                        // generate DM_OCCUPIED BURST and put to bkn1
                        generate_DmSetupOccupiedTxCeasedDsbReserved(1);
                        // copy DSB burst to output
                        for (int16_t j=0; j< FRAME_LEN/2; j++)
                        {
                            out_data[out_ptr+j] = bkn1[j*2]*2 + bkn1[j*2+1];
                            out_envelop[out_ptr+j] = BURST_ENVELOP[j];
                        }
                    }
                    else  // other bursts - fill with zero
                    {
                        for (int16_t j=0; j< FRAME_LEN/2; j++)
                        {
                            out_data[out_ptr+j] = 0;
                            out_envelop[out_ptr+j] = 0;
                        }
                    }
                }
                else    // PTT released, just discard data and increase tetraTime counter
                {
                    if (m_dmTxReleased)
                    {
                        // send DM-TX-CEASED 02 times (msg type 0xF) on TS1. If FN 6, 12 => also send on TS3 using DSB
                        // send DM-RESERVED 16 times with Cntdn counter, not counting the current
                        // frame senting DM-TX-CEASED. DM-Reserved only sent in FN 6, 12, 18
                        if (m_dmTxCeasedStart == false)
                        {
                            m_dmTxCeasedStart = true;
                            m_dmReservedStart = false;
                            m_dmTxCeasedCntDn = 2;
                        }

                        if ( (m_dmTxCeasedStart) && (!m_dmReservedStart))
                        {
                            if (m_tetraTime.tn == 1)  
                            {
                                m_dmTxCeasedCntDn-- ;                            
                                if (m_dmTxCeasedCntDn == 0)
                                {
                                    m_dmReservedStart = true;     // complete DM-TX CEASED, start DM-RESERVED
                                    m_dmReservedCntDn = 16;
                                }
                            }
                            if ((m_tetraTime.tn == 1) || (m_tetraTime.fn != 18))
                            {
                                generate_Dm_TxCeased(); // 0 = TX_CEASED DNB_SF
                                for (int16_t j=0; j< FRAME_LEN/2; j++)
                                {
                                    out_data[out_ptr+j] = bkn1[j*2]*2 + bkn1[j*2+1];
                                    out_envelop[out_ptr+j] = BURST_ENVELOP[j];
                                }
                            }
                            else if ( ((m_tetraTime.tn ==  3)   && ((m_tetraTime.fn == 6) || (m_tetraTime.fn == 12)
                                    || (m_tetraTime.fn == 18))) || ((m_tetraTime.tn == 1) && (m_tetraTime.fn == 18)) )
                            // if TN:3 in FN: 6,12,18 OR TN:1 in FN 18
                            {
                                updateTnFnCntDn_DmSetupOccupiedTxCeasedReserved();   
                                // update DmReserved CountDn 
                                for (uint16_t pos = 0; pos < 6; pos++)
                                {
                                    DM_TX_CEASED_DSB[184-29+pos] = (m_dmReservedCntDn>>(5-pos)) & 0x0001;
                                }
                                generate_DmSetupOccupiedTxCeasedDsbReserved(2);
                                for (int16_t j=0; j< FRAME_LEN/2; j++)
                                {
                                    out_data[out_ptr+j] = bkn1[j*2]*2 + bkn1[j*2+1];
                                    out_envelop[out_ptr+j] = BURST_ENVELOP[j];
                                }
                            }
                            else
                            {
                                for (int16_t j=0; j< FRAME_LEN/2; j++)
                                {
                                    out_data[out_ptr+j] = 0;
                                    out_envelop[out_ptr+j] = 0;
                                }
                            }
                        }
                        else if ( (m_dmTxCeasedStart) && (m_dmReservedStart))
                        {                        
                            if ( ((m_tetraTime.tn == 1) || (m_tetraTime.tn == 3)) 
                                 && ((m_tetraTime.fn == 6) || (m_tetraTime.fn == 12) || (m_tetraTime.fn == 18)) )
                            {
                                if (m_tetraTime.tn == 1)  
                                {
                                    m_dmReservedCntDn-- ;
                                    if (m_dmReservedCntDn == 0)
                                    {
                                        m_dmTxReleased = false; // complete both DM-TX CEASED & DM-RESERVED
                                        m_dmTxCeasedStart = false;
                                        m_dmReservedStart = false;
                                    }
                                }

                                updateTnFnCntDn_DmSetupOccupiedTxCeasedReserved();   
                                // update DmReserved CountDn 
                                for (uint16_t pos = 0; pos < 6; pos++)
                                {
                                    DM_RESERVED[184-29+pos] = (m_dmReservedCntDn>>(5-pos)) & 0x0001;
                                }
                                generate_DmSetupOccupiedTxCeasedDsbReserved(3);

                                for (int16_t j=0; j< FRAME_LEN/2; j++)
                                {
                                    out_data[out_ptr+j] = bkn1[j*2]*2 + bkn1[j*2+1];
                                    out_envelop[out_ptr+j] = BURST_ENVELOP[j];
                                }

                            }
                            else
                            {
                                for (int16_t j=0; j< FRAME_LEN/2; j++)
                                {
                                    out_data[out_ptr+j] = 0;
                                    out_envelop[out_ptr+j] = 0;
                                }
                            }
                        }
                        else  // for other TimeSlots when no ptt, just output zeros
                        { 
                            for (int16_t j=0; j< FRAME_LEN/2; j++)
                            {
                                out_data[out_ptr+j] = 0;
                                out_envelop[out_ptr+j] = 0;
                            }
                        }
                    }
                    else
                    {
                        for (int16_t j=0; j< FRAME_LEN/2; j++)
                        {
                            out_data[out_ptr+j] = 0;
                            out_envelop[out_ptr+j] = 0;
                        }
                    }
                    if (tx.uPlane.size() >= 432*2)
                        tx.uPlane.erase(tx.uPlane.begin(),tx.uPlane.begin()+432);
                }
            }            
            timeIncrease();
            out_ptr += FRAME_LEN/2;
            tx.bitCnt -= 510;
        }
    }
    return out_ptr;
}

uint32_t getValue(std::vector<uint8_t> pdu, const uint16_t pos, const uint16_t len)
{
    uint32_t res=0;
    for (uint16_t i=0; i < len; i++)
    {
        res = res*2 + pdu[pos+i];
    }
    return res;
}
std::vector<uint8_t> processDmacSync(std::vector<uint8_t> pdu, const uint16_t pdu_len)
{

    std::vector<uint8_t> sdu;

    // Table 21 & 22: DMAC-SYNC PDU contents
    uint16_t pos = 0;
    uint16_t systemCode = getValue(pdu, pos, 4);                             // system code
    pos += 4;
    uint16_t syncPduType = getValue(pdu, pos, 2);
    pos += 2;
    uint16_t commType = getValue(pdu, pos, 2);
    pos += 2;
    pos += 2;
    uint16_t abChanUsage = getValue(pdu, pos, 2);                            // A/B channel usage
    pos += 2;
    m_tetraTime.tn = getValue(pdu, pos, 2) + 1;
    pos += 2;                                                               
    m_tetraTime.fn = getValue(pdu, pos, 5);
    //** no MNC found in DMO Mode. Default start from zero
    pos += 5;
    pos += 2;
    pos += 39;  // no AIE encryption data
    pos += 10;
    bool fillBitInd = getValue(pdu, pos, 1);
    pos += 1;
    bool fragFlag = getValue(pdu, pos, 1);
    pos += 1;

    uint16_t noSchfSlot = 0;
    if (fragFlag)
    {
        noSchfSlot = getValue(pdu, pos, 4);
        pos += 4;
    }
    m_frameCntDn = getValue(pdu, pos, 2);
    pos += 2;
    uint16_t destinAddrType = getValue(pdu, pos, 2);
    pos += 2;

    if (destinAddrType != 2)
    {
        m_destinAddress = getValue(pdu, pos, 24);
        pos += 24;
    }
    uint32_t sourceAddrType = getValue(pdu, pos, 2);
    pos += 2;

    if (sourceAddrType != 2)
    {
        m_sourceAddress = getValue(pdu, pos, 24);
        pos += 24;
    }
    uint32_t mnIdentity = 0;
    if ((commType == 0) || (commType == 1))
    {
        mnIdentity = getValue(pdu, pos, 24);
    }
    pos += 24;
    uint16_t messageType = getValue(pdu, pos, 5);
    pos += 5;
    // ASSUME msgDependElem length = 0
        
    sdu.insert(sdu.begin(),pdu.begin()+pos, pdu.end());

    if ((mnIdentity != 0) && (m_sourceAddress != 0))      // only updateScramblingCode if valid data
    {
        updateScramblingCode(m_sourceAddress, mnIdentity);
    }

return sdu;
}

void updateScramblingCode(const uint32_t sourceAddress, const uint32_t mnIdentity)
{
    m_mcc = mnIdentity >> 14;
    m_mnc = mnIdentity & 0x3ff;
    m_scramblingCode = sourceAddress | ((mnIdentity & 0x003f) << 24);           // 30 MSB bits
    m_scramblingCode = (m_scramblingCode << 2) | 0x0003;                        // scrambling initialized to 1 on bits 31-32 - 8.2.5.2 (54)

    m_cellInformationsAcquired = true;
}

/**
 * @brief Return scrambling code
 *
 */

uint32_t getScramblingCode()
{
    return m_scramblingCode;
}

void updateSynchronizer(bool frameFound)
{
    if (frameFound)
    {
        if (m_bPreSynchronized == true)
        {
            m_bIsSynchronized = true;
        }
        else
        {
            m_bPreSynchronized = true;
        }
        m_syncBitCounter  = 0;             // reset syncBitCounter if frameFound 
    }
    else
    {
        if ( (m_bPreSynchronized) || (m_bIsSynchronized) )
        {
            if (m_syncBitCounter > FRAME_LEN * 8)   // if syncBitCounter > 8 frames, 
            {
                m_bIsSynchronized = false;          // ie. no frameFound in 8 frames
                m_bPreSynchronized = false;         // then reset Synch status
                m_syncBitCounter  = 0;
            }
        }
    }
}

uint32_t patternAtPositionScore(std::uint8_t *data, std::vector<uint8_t> pattern, std::size_t position)
{
    uint32_t errors = 0;

    for (std::size_t idx = 0; idx < pattern.size(); idx++)
    {
        errors += (uint32_t)(pattern[idx] ^ data[position + idx]);
    }

    return errors;
}

bool isFrameFound()
{
    bool frameFound = false;
/*
    uint32_t score_preamble_p1 = patternAtPositionScore(m_frame, PREAMBLE_P1, 0+34);  // do not count 34 guard bits, burst starts @ zero
    uint32_t score_preamble_p2 = patternAtPositionScore(m_frame, PREAMBLE_P2, 0+34);  // do not count 34 guard bits
    uint32_t score_preamble_p3 = patternAtPositionScore(m_frame, PREAMBLE_P3, 0+34);  // do not count 34 guard bits

    uint32_t scoreSync    = patternAtPositionScore(m_frame, SYNC_TRAINING_SEQ,     214+34);
    uint32_t scoreNormal1 = patternAtPositionScore(m_frame, NORMAL_TRAINING_SEQ_1, 230+34);
    uint32_t scoreNormal2 = patternAtPositionScore(m_frame, NORMAL_TRAINING_SEQ_2, 230+34);
*/
    uint32_t score_preamble_p1 = patternAtPositionScore(m_burst, PREAMBLE_P1, 0+34);  // do not count 34 guard bits, burst starts @ zero
    uint32_t score_preamble_p2 = patternAtPositionScore(m_burst, PREAMBLE_P2, 0+34);  // do not count 34 guard bits
    uint32_t score_preamble_p3 = patternAtPositionScore(m_burst, PREAMBLE_P3, 0+34);  // do not count 34 guard bits

    uint32_t scoreSync    = patternAtPositionScore(m_burst, SYNC_TRAINING_SEQ,     214+34);
    uint32_t scoreNormal1 = patternAtPositionScore(m_burst, NORMAL_TRAINING_SEQ_1, 230+34);
    uint32_t scoreNormal2 = patternAtPositionScore(m_burst, NORMAL_TRAINING_SEQ_2, 230+34);

    scoreNormal1 = (scoreNormal1*38)/22;                                           // multifly with 1.5 to be comparable with STS
    scoreNormal2 = (scoreNormal2*38)/22;

    scoreNormal1 += score_preamble_p1;                                             // combine bit errors of preamble & training sequences
    scoreNormal2 += score_preamble_p2;
    scoreSync += score_preamble_p3;

    // soft decision to detect burst: bit errors in preamble & training sequence less than 6
    uint32_t scoreMin = scoreSync;                                                  // if same score, Sync Burst is chosen
    m_burstType = DSB;                                                              // same enum name is used for TMO & DMO (DSB, DNB DNB_SF)

    if (scoreNormal1 < scoreMin)
    {
        scoreMin  = scoreNormal1;
        m_burstType = DNB;
    }
    if (scoreNormal2 < scoreMin)
    {
        scoreMin  = scoreNormal2;
        m_burstType = DNB_SF;
    }
    
    if (scoreMin <= FRAME_DETECT_THRESHOLD)                                                          // frame (burst) is matched and can be processed
    {                                                                           // max 1 error for preamble + 5 errors for sts 
        frameFound = true;
        m_validBurstFound = true;
    }
    else
    {
        if (m_bIsSynchronized)
        {
            m_burstType = IDLE;
        }
    }
    return frameFound;
}

uint16_t serviceMacDecode(uint8_t* in_data, const int in_len, uint8_t* out_data)
/* in_data: bitstream (uint8) from pi4dqpsk demodulator
   out_data:432 bit u-plane bitstream to feed tetra speech decoder */
{ 
    int res = 0;
    for (int i=0; i< in_len; i++)
	{
		//m_frame.push_back(in_data[i]);
        m_burst[m_burst_ptr] = in_data[i];
        m_burst_ptr ++;
        
        if ( (m_bIsSynchronized == true) || (m_bPreSynchronized == true) )
        {
            m_syncBitCounter++;
        }

        if (m_burst_ptr == FRAME_LEN)
        {
            m_frameFound = isFrameFound();
            updateSynchronizer(m_frameFound);           // update sync status        

            if (m_bIsSynchronized)
            {                         
                //m_frame.clear();
                m_burst_ptr = 0;                
                bkn1.clear();
                bkn2.clear();
                
                if (m_burstType == DSB) 
                {
                    
                    //bkn1 = vectorExtract(data, 94,  120);
                    bkn1.insert(bkn1.begin(),&m_burst[34+94], &m_burst[34+94+120]);
                    bkn1 = descramble(bkn1, 120, 0x0003);                                   // descramble with predefined code 0x0003
                    bkn1 = deinterleave(bkn1, 120, 11);                                     // deinterleave 120, 11
                    bkn1 = depuncture23(bkn1, 120);                                         // depuncture with 2/3 rate 120 bits -> 4 * 80 bits before Viterbi decoding
                    bkn1 = viterbiDecode1614(bkn1);                                         // Viterbi decode - see 8.3.1.2  (K1 + 16, K1) block code with K1 = 60

                    // BKN2 block - SCH/H - in DMO DSB burst, BKN2 contains only SCH/H Half block signaling; BKN2 block starts at bit pos 252
                    // bkn2 = vectorExtract(data, 252, 216);
                    bkn2.insert(bkn2.begin(),&m_burst[34+252], &m_burst[34+252+216]);
                    bkn2 = descramble(bkn2, 216, 0x0003);                                   // descramble with default scrambling code 0x0003
                    bkn2 = deinterleave(bkn2, 216, 101);                                    // deinterleave
                    bkn2 = depuncture23(bkn2, 216);                                         // depuncture with 2/3 rate 144 bits -> 4 * 144 bits before Viterbi decoding
                    bkn2 = viterbiDecode1614(bkn2);                                         // Viterbi decode
                    
                    if ((checkCrc16Ccitt(bkn1, 76)) && (checkCrc16Ccitt(bkn2, 140)))        // check CRC for both bkn1 & bkn2
                    {
                        // merge bkn1 60 bits + bkn2 124 bits to bkn1
                        bkn1.insert(bkn1.begin()+60,bkn2.begin(), bkn2.begin()+124);                       
                        processDmacSync(bkn1, 184);
                    }
                }

                else if (m_burstType == DNB)
                {
                    // BKN1 + BKN2
                    bkn1.insert(bkn1.begin(),&m_burst[34+14], &m_burst[34+14+216]);
                    bkn2.insert(bkn2.begin(),&m_burst[34+252], &m_burst[34+252+216]);
                    // reconstruct block to BKN1
                    bkn1.insert(bkn1.end(), bkn2.begin(), bkn2.end());
                    bkn1 = descramble(bkn1, UPLANE_LEN, getScramblingCode());        // descramble

                    for (int j=0; j < UPLANE_LEN; j++)
                    {
                        out_data[res+j] = bkn1[j];
                    }
                    res += UPLANE_LEN;
                }
                else if (m_burstType == DNB_SF)      //ONLY carry STCH stealing channel
                {
                    bool bkn1ValidFlag = false;
                    bool bkn2ValidFlag = false;

                    // BKN1 block - DMAC STCH 
                    // bkn1 = vectorExtract(data, 14,  216);
                    bkn1.insert(bkn1.begin(),&m_burst[34+14], &m_burst[34+14+216]);
                    bkn1 = descramble(bkn1, 216, getScramblingCode());         // descramble
                    bkn1 = deinterleave(bkn1, 216, 101);                       // deinterleave 120, 11
                    bkn1 = depuncture23(bkn1, 216);                            // depuncture with 2/3 rate 120 bits -> 4 * 80 bits before Viterbi decoding
                    bkn1 = viterbiDecode1614(bkn1);                            // Viterbi decode - see 8.3.1.2  (K1 + 16, K1) block code with K1 = 60
                    if (checkCrc16Ccitt(bkn1, 140))                            // check CRC
                    {
                        bkn1.erase(bkn1.end()-16,bkn1.end());
                        bkn1ValidFlag = true;
                    }

                    // BKN2 block - DMAC STCH or TCH/S
                    // bkn2 = vectorExtract(data, 286, 216);
                    bkn2.insert(bkn2.begin(),&m_burst[34+252], &m_burst[34+252+216]);
                    bkn2 = descramble(bkn2, 216, getScramblingCode());         // descramble
                    bkn2 = deinterleave(bkn2, 216, 101);                                    // deinterleave
                    bkn2 = depuncture23(bkn2, 216);                                         // depuncture with 2/3 rate 144 bits -> 4 * 144 bits before Viterbi decoding
                    bkn2 = viterbiDecode1614(bkn2);                                         // Viterbi decode
                    if (checkCrc16Ccitt(bkn2, 140))                                         // check CRC if FALSE ==> TCH/S
                    {
                        bkn2.erase(bkn2.end()-16,bkn2.end());
                        bkn2ValidFlag = true;
                    }

                    // Frame 18 always SCH/S SCH/H, therefore STCH only occurs in FN 1:17
                    if (bkn1ValidFlag)
                    {
                        // serviceUpperMac(Pdu(bkn1), DSTCH);                               // first block is stolen for C or U signalling
                    }

                    if (m_sfStolenFlag)                                                 // if second-half slot is also stolen
                    {
                        if (bkn2ValidFlag)
                        {
                            //serviceUpperMac(Pdu(bkn2), DSTCH);                           // second block also stolen, reset flag
                        }
                    }
                    else   // Second-half slot not stolen, so traffic mode DTCH/S
                    {
                        bkn2.insert(bkn2.begin(),&m_burst[34+252], &m_burst[34+252+216]);
                        bkn2 = descramble(bkn2, 216, getScramblingCode());         // descramble
                        for (int j=0; j < (UPLANE_LEN/2); j++)
                        {
                            out_data[res+j]              = 0;
                            out_data[res+UPLANE_LEN/2+j] = bkn2[j];
                        }
                        res += UPLANE_LEN;
                    }
                }
            }
            else // m_bIsSynchronized NOT SYNC
            {
                // remove first symbol from buffer to make space for next one
                //m_frame.erase(m_frame.begin());
                for (int k=0; k< FRAME_LEN-1; k++)
                {
                    m_burst[k] = m_burst[k+1];
                }
                m_burst_ptr --;
            }
        }    
	}
    return res;

}


int descramble_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const int len, const uint32_t scramblingCode)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	std::vector<uint8_t> res;
	res = descramble(data, len, scramblingCode);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

/**
 * @brief Fibonacci LFSR descrambling - 8.2.5
 *
 */

std::vector<uint8_t> descramble(std::vector<uint8_t> data, const int len, const uint32_t scramblingCode)
{
    const uint8_t poly[14] = {32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1}; // Feedback polynomial - see 8.2.5.2 (8.39)
    std::vector<uint8_t> res;

    uint32_t lfsr = scramblingCode;                                             // linear feedback shift register initialization (=0 + 3 for BSCH, calculated from Color code ch 19 otherwise)

    for (int i = 0; i < len; i++)
    {
        uint32_t bit = lfsr >> (32 - poly[0]);                                  // apply poly (Xj + ...)
        for (int j = 1; j < 14; j++)
        {
            bit = bit ^ (lfsr >> (32 - poly[j]));
        }
        bit = bit & 1;                                                          // finish apply feedback polynomial (+ 1)
        lfsr = (lfsr >> 1) | (bit << 31);

        res.push_back(data[i] ^ (bit & 0xff));
    }

    return res;
}

int scramble_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const int len, const uint32_t scramblingCode)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	/*
	for (int i=0; i< in_len; i++)
	{
		data.push_back(in_data[i]);
	}*/
	std::vector<uint8_t> res;
	res = scramble(data, len, scramblingCode);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

std::vector<uint8_t> scramble(std::vector<uint8_t> data, int len, uint32_t scramblingCode)
{
    const uint8_t poly[14] = {32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1}; // Feedback polynomial - see 8.2.5.2 (8.39)
    std::vector<uint8_t> res;

    uint32_t lfsr = scramblingCode;
    uint32_t bit;
	for (int i = 0; i < len; i++)
    {
        bit = lfsr >> (32 - poly[0]);                                           // apply poly (Xj + ...)
        for (int j = 1; j < 14; j++)                                            // remaining poly X(j-1)...
        {
            bit = bit ^ (lfsr >> (32 - poly[j]));
        }
        bit = bit & 1;                                                          // finish apply feedback polynomial (+ 1)
        lfsr = (lfsr >> 1) | (bit << 31);

		res.push_back(data[i] ^ (bit & 0xff));
    }
	return res;
}

/**
 * @brief (K,a) block deinterleaver - 8.2.4
 *
 */
int deinterleave_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t K, const uint32_t a)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	std::vector<uint8_t> res;
	res = deinterleave(data, K, a);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

std::vector<uint8_t> deinterleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a)
{
    std::vector<uint8_t> res(K, 0);                                             // output vector is size K

    for (unsigned int idx = 1; idx <= K; idx++)
    {
        uint32_t k = 1 + (a * idx) % K;
        res[idx - 1] = data[k - 1];                                             // to interleave: DataOut[i-1] = DataIn[k-1]
    }

    return res;
}

int interleave_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t K, const uint32_t a)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	std::vector<uint8_t> res;
	res = interleave(data, K, a);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

std::vector<uint8_t> interleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a)
{
    std::vector<uint8_t> res(K, 0);                                             // output vector is size K

    for (unsigned int idx = 1; idx <= K; idx++)
    {
        uint32_t k = 1 + (a * idx) % K;
        res[k - 1] = data[idx - 1];                                             // to interleave: DataOut[i-1] = DataIn[k-1]
    }

    return res;
}


/**
 * @brief Depuncture with 2/3 rate - 8.2.3.1.3
 *
 */
int depuncture23_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t len)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	std::vector<uint8_t> res;
	res = depuncture23(data, len);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

std::vector<uint8_t> depuncture23(std::vector<uint8_t> data, const uint32_t len)
/* depuncture with 2/3 rate */
{
    const uint8_t P[] = {0, 1, 2, 5};                                           // 8.2.3.1.3 - P[1..t] 1st element (0) is not used.
    std::vector<uint8_t> res(4 * len * 2 / 3, 2);                               // 8.2.3.1.2 with flag 2 for erase bit in Viterbi routine

    uint8_t t = 3;                                                              // 8.2.3.1.3
    uint8_t period = 8;                                                         // 8.2.3.1.2

    for (uint32_t j = 1; j <= len; j++)
    {
        uint32_t i = j;                                                         // punct->i_func(j);
        uint32_t k = period * ((i - 1) / t) + P[i - t * ((i - 1) / t)];         // punct->period * ((i-1)/t) + P[i - t*((i-1)/t)];
        res[k - 1] = data[j - 1];
    }

    return res;
}

int motherEncode1614_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t len)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	std::vector<uint8_t> res;
	res = motherEncode1614(data, len);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

std::vector<uint8_t> motherEncode1614(std::vector<uint8_t> data, const uint32_t len)
{
    //
    uint8_t ces_delay[] = {0, 0, 0, 0, 0};                        // initiate ces - reset to all 'zero'
    std::vector<uint8_t> res(4 * len, 0);

	for (uint32_t k = 1; k <= len; k++) 
    {
        uint8_t g1, g2, g3, g4;
        uint8_t *delayed = ces_delay;

        delayed[0] = data[k-1];
        /* G1 = 1 + D + D4 */
        g1 = (delayed[0] + delayed[1] + delayed[4]) % 2;
        /* G2 = 1 + D2 + D3 + D4 */
        g2 = (delayed[0] + delayed[2] + delayed[3] + delayed[4]) % 2;
        /* G3 = 1 + D + D2 + D4 */
        g3 = (delayed[0] + delayed[1] + delayed[2] + delayed[4]) % 2;
        /* G4 = 1 + D + D3 + D4 */
        g4 = (delayed[0] + delayed[1] + delayed[3] + delayed[4]) % 2;

        /* shift the state and input our new bit */
        delayed[4] = delayed[3];
        delayed[3] = delayed[2];
        delayed[2] = delayed[1];
        delayed[1] = delayed[0];

        res[4*(k-1)+0] = g1; 
        res[4*(k-1)+1] = g2;
        res[4*(k-1)+2] = g3;
        res[4*(k-1)+3] = g4;
	}
    return res;
}

int puncture23_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t len)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	std::vector<uint8_t> res;
	res = puncture23(data, len);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

/* Puncture the mother code (x4) and write 'len' symbols to out */
std::vector<uint8_t> puncture23(std::vector<uint8_t> data, const uint32_t len)
{
    const uint8_t P[] = {0, 1, 2, 5};                                           // 8.2.3.1.3 - P[1..t]
    std::vector<uint8_t> res(len, 0);                                           // set default value to ZERO
	uint8_t t = 3;
    uint8_t period = 8;

	/* Section 8.2.3.1.2 */
	for (uint32_t j = 1; j <= len; j++) {
		uint32_t i = j;
		uint32_t k = period * ((i-1)/t) + P[i - t*((i-1)/t)];
		res[j-1] = data[k-1];
	}
	return res;
}

/**
 * @brief Viterbi decoding of RCPC code 16-state mother code of rate 1/4 - 8.2.3.1.1
 *
 */
int viterbiDecode1614_raw(uint8_t* in_data, int in_len, uint8_t* out_data)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	std::vector<uint8_t> res;
	res = viterbiDecode1614(data);
	int out_len = res.size();
	for (int i=0; i< out_len; i++)
	{
		out_data[i] = res[i];
	}
	return out_len;
}

std::vector<uint8_t> viterbiDecode1614(std::vector<uint8_t> data)
{
    std::string sIn = "";
    for (std::size_t idx = 0; idx < data.size(); idx++)
    {
        sIn += (char)(data[idx] + '0');
    }

    std::vector<int> polynomials;
    int constraint = 6;
    polynomials.push_back(0b10011);
    polynomials.push_back(0b11101);
    polynomials.push_back(0b10111);
    polynomials.push_back(0b11011);

    m_viterbiCodec1614 = new ViterbiCodec(constraint, polynomials);


    std::string sOut = m_viterbiCodec1614->Decode(sIn);

    std::vector<uint8_t> res;

    for (size_t idx = 0; idx < sOut.size(); idx++)
    {
        res.push_back((uint8_t)(sOut[idx] - '0'));
    }
    
    delete m_viterbiCodec1614;
    return res;
}

/**
 * @brief Reed-Muller decoder and FEC correction 30 bits in, 14 bits out
 *
 * FEC thanks to Lollo Gollo @logollo see "issue #21"
 *
 */

std::vector<uint8_t> reedMuller3014Decode(std::vector<uint8_t> data)
{
    uint8_t q[5];
    std::vector<uint8_t> res(14);

    q[0] = data[0];
    q[1] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 11]) % 2;
    q[2] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 8] + data[13 + 9]) % 2;
    q[3] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 9] + data[13 + 10]) % 2;
    q[4] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 8] + data[13 + 10] + data[13 + 11]) % 2;
    res[0] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[1];
    q[1] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 9] + data[13 + 11]) % 2;
    q[2] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 10]) % 2;
    q[3] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 8]) % 2;
    q[4] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 11]) % 2;
    res[1] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[2];
    q[1] = (data[13 + 2] + data[13 + 5] + data[13 + 8] + data[13 + 10] + data[13 + 11]) % 2;
    q[2] = (data[13 + 1] + data[13 + 3] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 10]) % 2;
    q[3] = (data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 11]) % 2;
    res[2] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[3];
    q[1] = (data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    q[2] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    q[3] = (data[13 + 2] + data[13 + 4] + data[13 + 6] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    q[4] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    res[3] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[4];
    q[1] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    q[2] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    q[4] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    res[4] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[5];
    q[1] = (data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    q[2] = (data[13 + 2] + data[13 + 4] + data[13 + 6] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    q[4] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    res[5] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[6];
    q[1] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    q[2] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    q[4] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    res[6] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[7];
    q[1] = (data[13 + 2] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 1] + data[13 + 3] + data[13 + 5] + data[13 + 8] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    res[7] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[8];
    q[1] = (data[13 + 2] + data[13 + 3] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    q[2] = (data[13 + 1] + data[13 + 7] + data[13 + 8] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    q[3] = (data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 4] + data[13 + 6] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    res[8] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[9];
    q[1] = (data[13 + 1] + data[13 + 3] + data[13 + 8] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    q[2] = (data[13 + 4] + data[13 + 6] + data[13 + 10] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    q[3] = (data[13 + 2] + data[13 + 7] + data[13 + 9] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    res[9] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[10];
    q[1] = (data[13 + 1] + data[13 + 2] + data[13 + 7] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    q[2] = (data[13 + 3] + data[13 + 8] + data[13 + 9] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 4] + data[13 + 6] + data[13 + 9] + data[13 + 10] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    q[4] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 10] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    res[10] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[11];
    q[1] = (data[13 + 2] + data[13 + 6] + data[13 + 9] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 4] + data[13 + 7] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 3] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 11] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    res[11] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[12];
    q[1] = (data[13 + 5] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 11] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 2] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    res[12] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[13];
    q[1] = (data[13 + 2] + data[13 + 4] + data[13 + 7] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 6] + data[13 + 9] + data[13 + 10] + data[13 + 11] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 8] + data[13 + 9] + data[13 + 11] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 10] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    res[13] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    // check deviation from input
    // int deviation = 0;
    // for (int cnt = 0; cnt < 14; cnt++)
    // {
    //     deviation += (data[cnt] != res[cnt]) ? 1 : 0;
    // }
    // printf("FEC correction %.2f\n", deviation / 14.);
    // print_vector(data, 14);
    // print_vector(res, 14);

    //return vector_extract(data, 0, 14);

    return res;
}

std::vector<uint8_t> reedMuller3014Encode(std::vector<uint8_t> data)
/* input: vector data of 14 uint8_t, output vector res of 30 uint8_t */
{
    std::vector<uint8_t> res(30,0);
    uint16_t data14bit = 0;
    for (uint8_t i=0; i <14; i++)
    {
        data14bit = (data14bit << 1) + data[i];
    }

    uint32_t res30bit = rm3014Compute(data14bit);

    for (uint8_t i=0; i <30; i++)
    {
        res[i] = (res30bit & (1 << (29-i))) ? 1 : 0;
    }

    return res;
}

uint32_t rm3014Compute(const uint16_t in)
/* code taken from osmo-tetra in 32bit & 16bit formats */
{
	const uint8_t rm_30_14_gen[14][16] = {
		{ 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 },
		{ 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0 },
		{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1 },
		{ 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1 },
		{ 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1 },
		{ 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1 },
		{ 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1 }
	};

	uint32_t rm_30_14_rows[14];

	uint32_t val;
	uint32_t ret;

	for (int i = 0; i < 14; i++) {
		/* upper 14 bits identity matrix */
		val = (1 << (16+13 - i));
		/* lower 16 bits from rm_30_14_gen */
		ret = 0;
		/* shift_bits_together */
		for (int j = 15; j >= 0; j--)
			ret |= rm_30_14_gen[i][j] << (15-j);

		val |= ret;
		rm_30_14_rows[i] = val;
		//printf("rm_30_14_rows[%u] = 0x%08x\n", i, val);
	}

	val = 0;

	for (int i = 0; i < 14; i++) {
		uint32_t bit = (in >> (14-1-i)) & 1;
		if (bit)
			val ^= rm_30_14_rows[i];
		/* we can skip the 'else' as XOR with 0 has no effect */
	}
	return val;
}

/**
 * @brief Calculated CRC16 ITU-T X.25 - CCITT
 *
 */
bool checkCrc16Ccitt_raw(uint8_t* in_data, int in_len, const int len)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	int res = checkCrc16Ccitt(data, len);
	return res;
}

bool checkCrc16Ccitt(std::vector<uint8_t> data, const int len)
{
    uint16_t crc = 0xFFFF;                                                      // CRC16-CCITT initial value

    for (int i = 0; i < len; i++)
    {
        uint16_t bit = (uint16_t)data[i];

        crc ^= bit << 15;
        if(crc & 0x8000)
        {
            crc <<= 1;
            crc ^= 0x1021;                                                      // CRC16-CCITT polynomial
        }
        else
        {
            crc <<= 1;
        }
    }

    return crc == 0x1D0F;                                                       // CRC16-CCITT reminder value
}


uint16_t computeCrc16Ccitt_raw(uint8_t* in_data, int in_len, const int len)
{
	std::vector<uint8_t> data(in_data, in_data + in_len);
	uint16_t res;
	res = computeCrc16Ccitt(data, len);
	return res;
}

uint16_t computeCrc16Ccitt(std::vector<uint8_t> data, const int len)
/*checkCrc16Ccitt and computeCrcCcitt perform exactly same calculation. Only difference in return value: boolean or crc code. 
To perform crc coding, calculate Crc16, take complement value (~binary) and insert to the end of data stream from MSB to LSBidentical identical */

{
   uint16_t crc = 0xFFFF;                                                      // CRC16-CCITT initial value

    for (int16_t i = 0; i < len; ++i) {
        uint16_t bit = data[i] & 0x1;
        crc ^= bit << 15;
        if ((crc & 0x8000)) {
            crc <<= 1;
            crc ^= 0x1021;                                                      // #define GEN_POLY 0x1021
        } else {
            crc <<= 1;
        }
    }
    return crc;
}

