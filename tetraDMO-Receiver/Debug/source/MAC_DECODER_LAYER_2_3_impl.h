/* -*- c++ -*- */
/*
 * Copyright 2024 kien.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_DMO_TETRA_MAC_DECODER_LAYER_2_3_IMPL_H
#define INCLUDED_DMO_TETRA_MAC_DECODER_LAYER_2_3_IMPL_H

#include <gnuradio/DMO_TETRA/MAC_DECODER_LAYER_2_3.h>
#include "gnuradio/DMO_TETRA/Pdu.h"
#include "gnuradio/DMO_TETRA/tetracell.h"
#include "gnuradio/DMO_TETRA/uplane.h"
#include "gnuradio/DMO_TETRA/viterbicodec.h"


namespace gr {
  namespace DMO_TETRA {

    class MAC_DECODER_LAYER_2_3_impl : public MAC_DECODER_LAYER_2_3
    {
     private:
      std::shared_ptr<TetraCell> m_tetraCell;
      std::shared_ptr<UPlane> m_Uplane;
      std::shared_ptr<ViterbiCodec> m_viterbiCodec1614;
      TetraTime m_tetraTime;

     public:
      MAC_DECODER_LAYER_2_3_impl();
      ~MAC_DECODER_LAYER_2_3_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );

      std::vector<uint8_t> START_PATTERN = {0,1,0,1,0,1,0,1};

      void Mac_channel_decode(std::vector<uint8_t> bkn1, std::vector<uint8_t> bkn2, BurstType type, uint8_t *out, int &out_index);
      void Mac_service(Pdu pdu, MacLogicalChannel LogicalChannel, uint8_t *out, int &out_index);
      void Mac_service(Pdu pdu, MacLogicalChannel LogicalChannel, uint8_t *out, int &out_index, bool stolenflag);
      Pdu  pduProcessDmacSync(const Pdu pdu);
      Pdu  pduProcessDmacData(const Pdu pdu);

      std::vector<uint8_t> descramble(std::vector<uint8_t> data, const int len, const uint32_t scramblingCode);
      std::vector<uint8_t> deinterleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a);
      std::vector<uint8_t> depuncture23(std::vector<uint8_t> data, const uint32_t len);
      std::vector<uint8_t> viterbiDecode1614(std::vector<uint8_t> data);
      int checkCrc16Ccitt(std::vector<uint8_t> data, const int len);
      std::vector<uint8_t> nullPDU;
    };
  } // namespace DMO_TETRA
} // namespace gr

#endif 
/* INCLUDED_DMO_TETRA_MAC_DECODER_LAYER_2_3_IMPL_H */
