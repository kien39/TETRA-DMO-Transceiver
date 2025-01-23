/* -*- c++ -*- */
/*
 * Copyright 2024 kien.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TETRA_DMO_MAC_DECODER_H
#define INCLUDED_TETRA_DMO_MAC_DECODER_H

#include <gnuradio/TETRA_DMO/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace TETRA_DMO {

    /*!
     * \brief <+description of block+>
     * \ingroup TETRA_DMO
     *
     */
    class TETRA_DMO_API MAC_DECODER : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<MAC_DECODER> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of TETRA_DMO::MAC_DECODER.
       *
       * To avoid accidental use of raw pointers, TETRA_DMO::MAC_DECODER's
       * constructor is in a private implementation
       * class. TETRA_DMO::MAC_DECODER::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace TETRA_DMO
} // namespace gr

#endif /* INCLUDED_TETRA_DMO_MAC_DECODER_H */
