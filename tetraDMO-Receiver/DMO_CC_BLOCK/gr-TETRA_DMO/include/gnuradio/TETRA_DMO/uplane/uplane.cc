#include "uplane.h"

using namespace Tetra;

/**
 * @brief Constructor
 *
 */

UPlane::UPlane(Log * log, Report * report) : Layer(log, report)
{
    // nothing
    dmoFilename = new std::string();
    *dmoFilename = "noname";
}

/**
 * @brief Destructor
 *
 */

UPlane::~UPlane()
{

}

/**
 * @brief User-Plane traffic handling
 *
 */

void UPlane::service(Pdu pdu, const MacLogicalChannel macLogicalChannel, TetraTime tetraTime, MacAddress macAddress, MacState macState, uint8_t encryptionMode)
{
    Layer::service(macLogicalChannel, tetraTime, macAddress);

    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - mac_channel = %s pdu = %s encr = %u\n", "service_u_plane", macLogicalChannelName(macLogicalChannel).c_str(), pdu.toString().c_str(), encryptionMode);

    if (macLogicalChannel == TCH_S)                                             // speech frame
    {
        m_report->startUPlane("UPLANE", "TCH_S", tetraTime, macAddress);

        const std::size_t MIN_SIZE = 432;
        const int FRAME_SIZE = 690;

        if (pdu.size() >= MIN_SIZE)
        {

            uint16_t speechFrame[FRAME_SIZE] = {0};

            for (int idx = 0; idx < 6; idx++)
            {
                speechFrame[115 * idx] = 0x6b21 + (uint16_t)idx;                // FIXME to check (uint16_t)
            }

            for (int idx = 0; idx < 114; idx++)
            {
                speechFrame[1 + idx]  = pdu.at(idx) ? -127 : 127;
            }

            for (int idx = 0; idx < 114; idx++)
            {
                speechFrame[116 + idx] = pdu.at(114 + idx) ? -127 : 127;
            }

            for (int idx = 0; idx < 114; idx++)
            {
                speechFrame[231 + idx] = pdu.at(228 + idx) ? -127 : 127;
            }

            for (int idx = 0; idx < 90; idx++)
            {
                speechFrame[346 + idx] = pdu.at(342 + idx) ? -127 : 127;
            }

            // speech frame will be converted in base64 string

            m_report->add("downlink usage marker", macState.downlinkUsageMarker);                 // current usage marker
            m_report->add("encryption mode",  encryptionMode);                                    // current encryption mode
            m_report->addCompressed("frame", (const unsigned char *)speechFrame, 2 * FRAME_SIZE); // actual binary frame 1380 bytes
        }
        else
        {
            m_report->add("invalid pdu size", (uint64_t)pdu.size());
            m_report->add("pdu minimum size", (uint64_t)MIN_SIZE);
        }

        m_report->send();
    }
}

/* service DMO-DMAC traffic frames
1. When calling UPlane::service - is the SF frame status is known ? indicating is is a full frame or second half frame
   pdu data length shall tell whether it is a full frame (432) of sf frame (216)
*/
void UPlane::service(Pdu pdu, const MacLogicalChannel macLogicalChannel, TetraTime tetraTime)
{
   bool sfFrame = (pdu.size() == 216);                                         // receiving pdu with size 216 ---> sfFrame
    if (macLogicalChannel == DTCH_S)
    {
        m_log->print(LogLevel::NONE, "DEBUG ::%-44s - mac_channel = %s pdu = %s\n", "service_u_plane", macLogicalChannelName(macLogicalChannel).c_str(), pdu.toString().c_str());

        const std::size_t MIN_SIZE = 432;
        const int FRAME_SIZE = 690;
        uint16_t speechFrame[FRAME_SIZE] = {0};

        if (!sfFrame)
        {
            for (int idx = 0; idx < 6; idx++)
            {
                speechFrame[115 * idx] = 0x6b21 + (uint16_t)idx;                // FIXME to check (uint16_t)
            }

            for (int idx = 0; idx < 114; idx++)
            {
                speechFrame[1 + idx]  = pdu.at(idx) ? -127 : 127;
            }

            for (int idx = 0; idx < 114; idx++)
            {
                speechFrame[116 + idx] = pdu.at(114 + idx) ? -127 : 127;
            }

            for (int idx = 0; idx < 114; idx++)
            {
                speechFrame[231 + idx] = pdu.at(228 + idx) ? -127 : 127;
            }

            for (int idx = 0; idx < 90; idx++)
            {
                speechFrame[346 + idx] = pdu.at(342 + idx) ? -127 : 127;
            }

            // speech frame will be converted in base64 string

            // m_report->add("downlink usage marker", macState.downlinkUsageMarker);                 // current usage marker
            // m_report->add("encryption mode",  encryptionMode);                                    // current encryption mode
            // m_report->addCompressed("frame", (const unsigned char *)speechFrame, 2 * FRAME_SIZE); // actual binary frame 1380 bytes
        }
        else
        {
            for (int idx = 0; idx < 6; idx++)
            {
                speechFrame[115 * idx] = 0x6b21 + (uint16_t)idx;                // FIXME to check (uint16_t)
            }

            for (int idx = 0; idx < 114; idx++)
            {
                // leave first half 216 bit empty;
            }

            for (int idx = 0; idx < 102; idx++)
            {
                // leave first half 216 bit empty;
            }

            for (int idx = 102; idx < 114; idx++)
            {
                speechFrame[116 + idx] = pdu.at(idx-102) ? -127 : 127;
            }

            for (int idx = 0; idx < 114; idx++)
            {
                speechFrame[231 + idx] = pdu.at(12 + idx) ? -127 : 127;
            }

            for (int idx = 0; idx < 90; idx++)
            {
                speechFrame[346 + idx] = pdu.at(126 + idx) ? -127 : 127;
            }
            // m_log->print(LogLevel::NONE, "DEBUG ::%-44s - mac_channel = %s\n", "service_u_plane", macLogicalChannelName(macLogicalChannel).c_str());
            // for (int i = 0; i < 690; i++)
            // {
            //     printf("%u ,", speechFrame[i]);
            // }
        }

        if (*dmoFilename == "noname")
        {
            time_t now;
            struct tm * timeinfo;
            timeinfo = localtime(&now);

            char filename[512] = "";
            char tmp[16]       = "";

            strftime(tmp, 16, "%Y%m%d_%H%M%S", timeinfo);                                             // get time
            snprintf(filename, sizeof(filename), "audio/tetraDMO_%s.out", tmp); // create file filename

            *dmoFilename = filename;
        }

        FILE * file = fopen((*dmoFilename).c_str(), "ab");
        //std::string dmoFilename ="audio/TetraDMO_01.out";
        //FILE * file = fopen(dmoFilename.c_str(), "ab");
        fwrite(speechFrame, 2, 690, file);                                      // 1 word * len elements
        fflush(file);
        fclose(file);
    }
    else
    {
        // no other case
    }
 
}