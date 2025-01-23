/* -*- c++ -*- */
/*
 * Copyright 2024 kien.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "MAC_DECODER_impl.h"

namespace gr {
  namespace TETRA_DMO {

    using input_type = uint8_t;
    using output_type = uint8_t;
    MAC_DECODER::sptr
    MAC_DECODER::make()
    {
      return gnuradio::make_block_sptr<MAC_DECODER_impl>(
        );
    }


    /*
     * The private constructor
     */
    MAC_DECODER_impl::MAC_DECODER_impl()
      : gr::block("MAC_DECODER",
              gr::io_signature::make(1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
              gr::io_signature::make(1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
    {
    // create TetraCell object
    m_tetraCell = std::make_shared<TetraCell>();
    m_Uplane = std::make_shared<UPlane>();
    TetraTime m_tetraTime;
    m_burst_ptr = 0;
    std::fill(std::begin(m_burst), std::end(m_burst), 0);
    nullPDU.resize(432,0);

    m_bPreSynchronized = false;
    m_bIsSynchronized = false;
    m_syncBitCounter = 0; 
    m_frameFound = false;
    m_validBurstFound = false;
    FRAME_DETECT_THRESHOLD = 3;

    // create Viterbicodec object
    std::vector<int> polynomials;
    int constraint = 6;
    polynomials.push_back(0b10011);
    polynomials.push_back(0b11101);
    polynomials.push_back(0b10111);
    polynomials.push_back(0b11011);
    m_viterbiCodec1614 = std::make_shared<ViterbiCodec>(constraint, polynomials);
    std::cout << "init done\n";
    // object done;
    }

    /*
     * Our virtual destructor.
     */
    MAC_DECODER_impl::~MAC_DECODER_impl()
    {
    }

    void
    MAC_DECODER_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = (noutput_items / 432) * 510;
      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    }

    int MAC_DECODER_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      auto in = static_cast<const input_type*>(input_items[0]);
      auto out_stream = static_cast<output_type*>(output_items[0]);
      BurstType burst_type;
      int scoreDesync;
      int in_index = 0;
      int out_index = 0;
      int in_length = ninput_items[0];

      // std::cout <<"available items: " << nitems_read(0) - last_consumed << "\n";

      for(int i = 0; i < in_length; i++)
      {        
        m_burst[m_burst_ptr] = in[i];
        m_burst_ptr++;
        if ((m_bIsSynchronized == true) || (m_bPreSynchronized == true))
        {
            m_syncBitCounter++;
        }
        if (m_burst_ptr == 510)
        {
          m_frameFound = isFrameFound();
          updateSynchronizer(m_frameFound); 
         
          if (m_bIsSynchronized)
          {   
            std::vector<uint8_t> burst;
            burst.insert(burst.begin(), m_burst, m_burst + 510);
            m_burst_ptr = 0;

            if(m_burstType == DNB || m_burstType == DNB_SF)
            {
            std::vector<uint8_t> bkn1(216, 0);  
            std::vector<uint8_t> bkn2(216, 0);

            bkn1 = vectorExtract(burst, 48, 216);
            bkn2 = vectorExtract(burst, 286,216);

            Mac_channel_decode(bkn1,bkn2,m_burstType, out_stream, out_index);
            }
            else if(m_burstType == DSB)
            {
            std::vector<uint8_t> bkn1(120, 0);  
            std::vector<uint8_t> bkn2(216, 0);

            bkn1 = vectorExtract(burst,128,120);
            bkn2 = vectorExtract(burst,286,216);

            Mac_channel_decode(bkn1,bkn2,m_burstType, out_stream, out_index);
            }
          }
          else
          {
            //m_frame.erase(m_frame.begin());
            for (int k=0; k<509; k++)
            {
                m_burst[k] = m_burst[k+1];
            }
            m_burst_ptr --;
          }
        }
      }
      
      consume_each(in_length);
      return out_index;
    }

    uint32_t MAC_DECODER_impl::patternAtPositionScore(std::uint8_t *data, std::vector<uint8_t> pattern, std::size_t position)
    {
      uint32_t errors = 0;

      for (std::size_t idx = 0; idx < pattern.size(); idx++)
      {
          errors += (uint32_t)(pattern[idx] ^ data[position + idx]);
      }

      return errors;
    }

    void MAC_DECODER_impl::updateSynchronizer(bool frameFound)
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
          if (m_syncBitCounter > 510 * 8)   // if syncBitCounter > 8 frames, 
          {
            m_bIsSynchronized = false;          // ie. no frameFound in 8 frames
            m_bPreSynchronized = false;         // then reset Synch status
            m_syncBitCounter  = 0;
          }
        }
      }
    }

    bool MAC_DECODER_impl::isFrameFound()
    {
      bool frameFound = false;
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

    void MAC_DECODER_impl::Mac_channel_decode(std::vector<uint8_t> bkn1,std::vector<uint8_t> bkn2, BurstType type, uint8_t *out, int &out_index)
    {
      switch(type)
      {
        case DSB:
        {
          // std::cout<<"DSB detected \n";
          bkn1 = descramble(bkn1,120,0x0003);
          bkn1 = deinterleave(bkn1,120,11);
          bkn1 = depuncture23(bkn1,120);
          bkn1 = viterbiDecode1614(bkn1);

          bkn2 = descramble(bkn2,216,0x0003);
          bkn2 = deinterleave(bkn2,216,101);
          bkn2 = depuncture23(bkn2,216);
          bkn2 = viterbiDecode1614(bkn2);

          if(checkCrc16Ccitt(bkn1,76) && checkCrc16Ccitt(bkn2,140))
          {
            bkn1 = vectorExtract(bkn1,0,60);
            bkn2 = vectorExtract(bkn2,0,124);
            bkn1 = vectorAppend(bkn1,bkn2);
            Mac_service(bkn1,DSCH_SH,out, out_index);
          }
          else
          {
            // std::cout<<"CRC BKN1: " << checkCrc16Ccitt(bkn1,76) << "\n";
            // std::cout<<"CRC BKN2: " << checkCrc16Ccitt(bkn2,140)<< "\n";
          }
          break;
        }

        case DNB:
        {
          // std::cout<<"DNB detected \n";
          bkn1 = vectorAppend(bkn1,bkn2);
          bkn1 = descramble(bkn1, 432, m_tetraCell->getScramblingCode());
          Mac_service(bkn1,DTCH_S,out, out_index);
          break;
        }

        case DNB_SF:
        {
          // std::cout<<"DNB_SF detected \n";
          bool bkn1valid = false;
          bool bkn2valid = false;

          bkn1 = descramble(bkn1,216, m_tetraCell->getScramblingCode());
          bkn1 = deinterleave(bkn1,216,101);
          bkn1 = depuncture23(bkn1,216);
          bkn1 = viterbiDecode1614(bkn1);
          if(checkCrc16Ccitt(bkn1,140))
          {
            // std::cout<<"CRC succeed \n";
            bkn1 = vectorExtract(bkn1,0,124);
            bkn1valid = true;
          }
          else
          {
            // std::cout<<"CRC failed for bkn1 \n";
          }

          bkn2 = descramble(bkn2,216,m_tetraCell->getScramblingCode());
          std::vector<uint8_t> bkn2_U = bkn2;
          bkn2 = deinterleave(bkn2,216,101);
          bkn2 = depuncture23(bkn2,216);
          bkn2 = viterbiDecode1614(bkn2);
          if(checkCrc16Ccitt(bkn2,140))
          {
            // std::cout<<"CRC succeed \n";
            bkn2 = vectorExtract(bkn2,0,124);
            bkn2valid = true;
          }

          if(bkn1valid)
          {
            Mac_service(bkn1,DSTCH,out,out_index); //DSTCH called
          }

          // Mac_service(nullPDU, DTCH_S, out, out_index);

          if(m_tetraCell->getStolenFlag())
          {
            if(bkn2valid)
            {
              Mac_service(bkn2,DSTCH,out,out_index); //DSTCH called
            }
            else
            {
              // std::cout<<"bkn2 CRC error \n";
            }
          }
          else
          {
            // std::cout<<"bkn2 belongs to DTCH/s \n";
            Mac_service(bkn2_U,DTCH_S,out,out_index); //DTCH_S called
          }
          break;
        }

        case IDLE:
          {
          break;
          }

        default:
          break;
      }
    }

    void MAC_DECODER_impl::Mac_service(Pdu pdu, MacLogicalChannel LogicalChannel, uint8_t *out, int &out_index)
    {
      Pdu sdu;
      std::vector<uint8_t> traffic_data(432,0);
      int speech_size = static_cast<int>(pdu.size());
      switch(LogicalChannel)
      {
        case DSCH_SH:
          // std::cout<<"DSCH/SH called \n";
          sdu = pduProcessDmacSync(pdu);
        break;

        case DSTCH:
          // std::cout<<"DSTCH called \n";
          sdu = pduProcessDmacData(pdu);
        break;

        case DTCH_S:
        // std::cout<<"DTCH_S called \n";
          if(speech_size > 432 || speech_size <= 0){break;}
          if(speech_size == 432)
          {
            traffic_data = pdu.extractVec(0,speech_size);
            std::copy(traffic_data.begin(), traffic_data.end(), out + out_index);
          }
          else
          {
            std::vector<uint8_t> speech = pdu.extractVec(0,speech_size);
            traffic_data.insert(traffic_data.begin() + 216,speech.begin(),speech.end());
          }
          out_index += 432;
        break;

        default:

          break;
      }
    }

    Pdu MAC_DECODER_impl::pduProcessDmacSync(Pdu pdu)
    {
      Pdu sdu;
      static const std::size_t MIN_SIZE = 60+124;     // Table 21 & 22: DMAC-SYNC PDU contents
      if (pdu.size() >= MIN_SIZE)
      {
          uint16_t pos = 0;
          uint16_t systemCode = pdu.getValue(pos, 4);                             // system code
          pos += 4;
          uint16_t syncPduType = pdu.getValue(pos, 2);
          pos += 2;
          uint16_t commType = pdu.getValue(pos, 2);
          pos += 2;
          pos += 2;
          uint16_t abChanUsage = pdu.getValue(pos, 2);                            // A/B channel usage
          pos += 2;
          m_tetraTime.tn = pdu.getValue(pos, 2) + 1;
          pos += 2;                                                               
          m_tetraTime.fn = pdu.getValue(pos, 5);
          //** no MNC found in DMO Mode. Default start from zero
          pos += 5;
          pos += 2;
          pos += 39;  // no AIE encryption data
          pos += 10;
          bool fillBitInd = pdu.getValue(pos, 1);
          pos += 1;
          bool fragFlag = pdu.getValue(pos, 1);
          pos += 1;

          uint16_t noSchfSlot = 0;
          if (fragFlag)
          {
              noSchfSlot = pdu.getValue(pos, 4);
              pos += 4;
          }
          uint16_t frameCntDn = pdu.getValue(pos, 2);
          pos += 2;
          uint16_t destinAddrType = pdu.getValue(pos, 2);
          pos += 2;

          uint16_t destinAddress = 0;                         // may need to update destinAddress from somewhere ?
          if (destinAddrType != 2)
          {
              destinAddress = pdu.getValue(pos, 24);
              pos += 24;
          }
          uint32_t sourceAddrType = pdu.getValue(pos, 2);
          pos += 2;

          uint32_t sourceAddress = 0;                         // may need to update sourceAddress from somewhere ?
          if (sourceAddrType != 2)
          {
              sourceAddress = pdu.getValue(pos, 24);
              pos += 24;
          }
          uint32_t mnIdentity = 0;
          if ((commType == 0) || (commType == 1))
          {
              mnIdentity = pdu.getValue(pos, 24);
          }
          pos += 24;
          uint16_t messageType = pdu.getValue(pos, 5);
          pos += 5;
          // ASSUME msgDependElem length = 0
          
          sdu = Pdu(pdu, pos, MIN_SIZE - pos);

          if ((mnIdentity != 0) && (sourceAddress != 0))      // only updateScramblingCode if valid data
          {
              // std::cout<<"Scrambling code is: " << m_tetraCell->getScramblingCode() << "\n";
              m_tetraCell->updateScramblingCode(sourceAddress, mnIdentity);
          }
      } 
      return sdu;
    }

    Pdu MAC_DECODER_impl::pduProcessDmacData(Pdu pdu)
    {
      Pdu sdu;
      m_tetraCell->updateStolenFlag(false);                     // set second half slot stolen = False

      static const std::size_t MIN_SIZE = 124;

      if (pdu.size() >= MIN_SIZE)
      {
          uint16_t pos = 0;
          uint16_t dmacPduType = pdu.getValue(pos, 2);
          pos += 2;
          bool fillBitInd = pdu.getValue(pos, 1);
          pos += 1;
          bool sfStolenFlag = pdu.getValue(pos, 1);
          pos += 1;
          bool fragFlag = pdu.getValue(pos, 1);
          pos += 1;
          bool nullPduFlag = pdu.getValue(pos, 1);
          pos += 1;
          uint16_t frameCntDn = pdu.getValue(pos, 2);
          pos += 2;
          uint16_t aieEncryptState = pdu.getValue(pos, 2);
          pos += 2;
          uint16_t destinAddrType = pdu.getValue(pos, 2);
          pos += 2;
          uint16_t destinAddress = 0;                         // may need to update destinAddress from somewhere ?
          if (destinAddrType != 2)
          {
              destinAddress = pdu.getValue(pos, 24);
              pos += 24;
          }
          uint32_t sourceAddrType = pdu.getValue(pos, 2);
          pos += 2;

          uint32_t sourceAddress = 0;                         // may need to update sourceAddress from somewhere ?
          if (sourceAddrType != 2)
          {
              sourceAddress = pdu.getValue(pos, 24);
              pos += 24;
          }
          uint32_t mnIdentity = pdu.getValue(pos, 24);        // ASSUME mnIdentity ALWAYS present (Table 23: DMAC-DATA PDU contents)
          pos += 24;
          uint16_t messageType = pdu.getValue(pos, 5);
          pos += 5;
          // ASSUME msgDependElem length = 0
          
          m_tetraCell->updateStolenFlag(sfStolenFlag);                      // raise flag for serviceUpperMac to process bkn2
          sdu = Pdu(pdu, pos, MIN_SIZE - pos);

          if ((mnIdentity != 0) && (sourceAddress != 0))      // only updateScramblingCode if valid data
          {
              m_tetraCell->updateScramblingCode(sourceAddress, mnIdentity);
          }
        }
      return sdu;
    }

    //coding method
    std::vector<uint8_t> MAC_DECODER_impl::descramble(std::vector<uint8_t> data, const int len, const uint32_t scramblingCode)
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

    std::vector<uint8_t> MAC_DECODER_impl::deinterleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a)
    {
      std::vector<uint8_t> res(K, 0);                                             // output vector is size K

      for (unsigned int idx = 1; idx <= K; idx++)
      {
          uint32_t k = 1 + (a * idx) % K;
          res[idx - 1] = data[k - 1];                                             // to interleave: DataOut[i-1] = DataIn[k-1]
      }
      return res;
    }

    std::vector<uint8_t> MAC_DECODER_impl::depuncture23(std::vector<uint8_t> data, const uint32_t len)
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
    
    std::vector<uint8_t> MAC_DECODER_impl::viterbiDecode1614(std::vector<uint8_t> data)
    {
      std::string sIn = "";
      for (std::size_t idx = 0; idx < data.size(); idx++)
      {
          sIn += (char)(data[idx] + '0');
      }
      std::string sOut = m_viterbiCodec1614->Decode(sIn);
      std::vector<uint8_t> res;
      for (size_t idx = 0; idx < sOut.size(); idx++)
      {
          res.push_back((uint8_t)(sOut[idx] - '0'));
      }
      return res;
    }

    int MAC_DECODER_impl::checkCrc16Ccitt(std::vector<uint8_t> data, const int len)
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
  } /* namespace DMO_TETRA */
} /* namespace gr */
