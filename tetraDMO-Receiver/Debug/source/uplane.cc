#include "gnuradio/DMO_TETRA/uplane.h"
 

 namespace gr
 {
    namespace DMO_TETRA
    {
        void UPlane::service(Pdu pdu, const MacLogicalChannel macLogicalChannel)
        {
            bool sfFrame = (pdu.size() == 216);                                         // receiving pdu with size 216 ---> sfFrame
            if (macLogicalChannel == DTCH_S)
            {
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
                }

                if (*dmoFilename == "noname")
                {
                    time_t now;
                    struct tm * timeinfo;
                    timeinfo = localtime(&now);

                    char filename[512] = "";
                    char tmp[16]       = "";

                    strftime(tmp, 16, "%Y%m%d_%H%M%S", timeinfo);         // get time
                    snprintf(filename, sizeof(filename), "audio/tetraDMO_%s.out", tmp); // create file filename

                    *dmoFilename = filename;
                }

                //mark return data here to push into codec
                
                FILE * file = fopen((*dmoFilename).c_str(), "ab");
                //std::string dmoFilename ="audio/TetraDMO_01.out";
                //FILE * file = fopen(dmoFilename.c_str(), "ab");
                fwrite(speechFrame, 2, 690, file);                                      // 1 word * len elements
                fflush(file);
                fclose(file);
            }
            // else
            //     {
            //         // no other case
            //     }
        }

        void UPlane::GNUservice(Pdu pdu, uint8_t *out, int &out_index)
        {
            UPlane::GNUservice(pdu, out, out_index, false);            
        }
        void UPlane::GNUservice(Pdu pdu, uint8_t *out, int &out_index, bool flag)
        {
            int speechframe_size = static_cast<int>(pdu.size());
            if(flag == false)
            {
                std::vector<uint8_t> speechframe = pdu.extractVec(0,speechframe_size);
                std::copy(speechframe.begin(),speechframe.end(),out + out_index);
                out_index += 480;   
                out[out_index - 1] = 0;// set speech frame

            }
            else
            {
                std::vector<uint8_t> speechframe = pdu.extractVec(0,speechframe_size);
                std::copy(speechframe.begin(),speechframe.end(),out + out_index + 216);
                out_index += 480;
                out[out_index - 1] = 1;// set stolen frame
            }
        }
    }
}

   