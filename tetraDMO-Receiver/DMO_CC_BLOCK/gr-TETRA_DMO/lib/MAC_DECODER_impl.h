/* -*- c++ -*- */
/*
 * Copyright 2024 kien.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TETRA_DMO_MAC_DECODER_IMPL_H
#define INCLUDED_TETRA_DMO_MAC_DECODER_IMPL_H

#include <gnuradio/TETRA_DMO/MAC_DECODER.h>
#include "gnuradio/TETRA_DMO/Pdu.h"
#include "gnuradio/TETRA_DMO/tetracell.h"
#include "gnuradio/TETRA_DMO/uplane.h"
#include "gnuradio/TETRA_DMO/viterbicodec.h"


namespace gr {
  namespace TETRA_DMO {

    class MAC_DECODER_impl : public MAC_DECODER
    {
     private:
      const std::vector<uint8_t> NORMAL_TRAINING_SEQ_1       = {1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0}; // n1..n22
      const std::vector<uint8_t> NORMAL_TRAINING_SEQ_2       = {0,1,1,1,1,0,1,0,0,1,0,0,0,0,1,1,0,1,1,1,1,0}; // p1..p22
      const std::vector<uint8_t> NORMAL_TRAINING_SEQ_3_BEGIN = {0,0,0,1,1,0,1,0,1,1,0,1};                     // q11..q22
      const std::vector<uint8_t> NORMAL_TRAINING_SEQ_3_END   = {1,0,1,1,0,1,1,1,0,0};                         // q1..q10

      // 9.4.4.3.4 Synchronisation training sequence
      const std::vector<uint8_t> SYNC_TRAINING_SEQ = {1,1,0,0,0,0,0,1,1,0,0,1,1,1,0,0,1,1,1,0,1,0,0,1,1,1,0,0,0,0,0,1,1,0,0,1,1,1}; // y1..y38
      const std::vector<uint8_t> PREAMBLE_P1  =   {0,0,1,1,0,0,1,0,0,0,1,1};   // j1..j12
      const std::vector<uint8_t> PREAMBLE_P2  =   {1,0,0,1,1,0,1,0,1,0,0,1};   // k1..k12
      const std::vector<uint8_t> PREAMBLE_P3  =   {0,0,0,1,0,1,0,0,0,1,1,1};   // I1..I12

      // Nothing to declare in this block.
      std::shared_ptr<TetraCell> m_tetraCell;
      std::shared_ptr<UPlane> m_Uplane;
      std::shared_ptr<ViterbiCodec> m_viterbiCodec1614;
      TetraTime m_tetraTime;
      std::uint16_t m_burst_ptr;

      bool m_bPreSynchronized;
      bool m_bIsSynchronized;
      uint64_t m_syncBitCounter; 
      bool m_frameFound;
      bool m_validBurstFound;

      std::uint8_t m_burst[510];
      BurstType m_burstType;
      int FRAME_DETECT_THRESHOLD;
      
     public:
      MAC_DECODER_impl();
      ~MAC_DECODER_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void Mac_channel_decode(std::vector<uint8_t> bkn1, std::vector<uint8_t> bkn2, BurstType type, uint8_t *out, int &out_index);
      void Mac_service(Pdu pdu, MacLogicalChannel LogicalChannel, uint8_t *out, int &out_index);
      Pdu  pduProcessDmacSync(const Pdu pdu);
      Pdu  pduProcessDmacData(const Pdu pdu);

      std::vector<uint8_t> descramble(std::vector<uint8_t> data, const int len, const uint32_t scramblingCode);
      std::vector<uint8_t> deinterleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a);
      std::vector<uint8_t> depuncture23(std::vector<uint8_t> data, const uint32_t len);
      std::vector<uint8_t> viterbiDecode1614(std::vector<uint8_t> data);
      int checkCrc16Ccitt(std::vector<uint8_t> data, const int len);
      std::vector<uint8_t> nullPDU;

      void updateSynchronizer(bool frameFound);
      bool isFrameFound();
      uint32_t patternAtPositionScore(std::uint8_t *data, std::vector<uint8_t> pattern, std::size_t position);
    };
  } // namespace TETRA_DMO
} // namespace gr

#endif /* INCLUDED_TETRA_DMO_MAC_DECODER_IMPL_H */
