#ifndef COMMON_LIB_H
#define COMMON_LIB_H

#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

namespace gr {
    namespace TETRA_DMO {
        enum BurstType: int{
        IDLE = 0,
        DSB = 1,
        DNB = 2,
        DNB_SF = 3,
        };

        struct TetraTime {
            uint16_t tn;                                                            ///< time slot
            uint16_t fn;                                                            ///< frame number
            uint16_t mn;                                                            ///< multi-frame number
        };

        void Time_increase(TetraTime time);

        enum MacLogicalChannel {                                                    // CP only
            NULL_CH = 0,
            DSCH_SH = 10,       // synchronisation channel + half slot signaling channel
            DSCH_F  = 12,       // full slot signaling channel  DMO MAC Logical Channels
            DSTCH   = 13,       // stealing channel             DMO MAC Logical Channels
            DTCH_S  = 14,       // traffic channel - vocie only DMO MAC Logical Channels
            DLCH    = 15        // linearization channel        DMO MAC Logical Channels
        }; 

        std::vector<uint8_t> vectorExtract(const std::vector<uint8_t>& source, uint32_t pos, int32_t length);
        std::vector<uint8_t> vectorAppend(std::vector<uint8_t> vec1, std::vector<uint8_t> vec2);
    }//end of TETRA_DMO
}//end of gr

#endif 