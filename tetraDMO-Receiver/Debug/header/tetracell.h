
#ifndef TETRACELL_H
#define TETRACELL_H

#include <ostream>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

namespace gr{
    namespace DMO_TETRA{
        class TetraCell {
        public:
            TetraCell();
            ~TetraCell();

            uint32_t getScramblingCode();
            void updateScramblingCode(const uint32_t mcc, const uint32_t mnc, const uint16_t colorCode);
            void updateScramblingCode(const uint32_t sourceAddress, const uint32_t mnIdentity);
            bool isCellInformationsAcquired();
            void setFrequencies(int32_t downlinkFrequency, int32_t uplinkFrequency);
            void setLocationArea(uint32_t la);

            uint32_t mcc();
            uint32_t mnc();
            uint32_t colorCode();
            uint32_t locationArea();
            int32_t  downlinkFrequency();
            int32_t  uplinkFrequency();
            void updateStolenFlag(bool flag);
            bool getStolenFlag();

        private:
            uint32_t m_mcc;                                                         ///< Cell MCC
            uint32_t m_mnc;                                                         ///< Cell MNC
            uint16_t m_colorCode;                                                   ///< Cell Color code
            uint32_t m_scramblingCode;                                              ///< Cell Scrambling code
            uint32_t m_locationArea;

            int32_t m_downlinkFrequency;                                            ///< Downlink frequency [Hz]
            int32_t m_uplinkFrequency;                                              ///< Uplink frequency [Hz]
            bool    m_cellInformationsAcquired;
            bool    StolenFlag;
        };
    }
}
        
#endif