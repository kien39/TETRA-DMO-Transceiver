#ifndef UPLANE_H
#define UPLANE_H


#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

#include "common_lib.h"
#include "gnuradio/DMO_TETRA/Pdu.h"

namespace gr
{
    namespace DMO_TETRA
    {
        class UPlane 
        {
            public:
                UPlane(){};
                ~UPlane(){};
                void service(Pdu pdu, const MacLogicalChannel macLogicalChannel);
                void GNUservice(Pdu pdu, uint8_t *out, int &out_index);
                void GNUservice(Pdu pdu, uint8_t *out, int &out_index, bool flag);
                std::string* dmoFilename;                                        ///< File names to use for usage DMO mode
            protected:
                MacLogicalChannel m_macLogicalChannel;                                  ///< Current MAC logical channel
                TetraTime m_tetraTime;                                                  ///< Current service tetra time  
        };
    }
}
        
#endif