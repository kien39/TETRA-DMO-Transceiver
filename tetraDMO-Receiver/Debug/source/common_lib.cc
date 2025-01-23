#include <gnuradio/DMO_TETRA/common_lib.h>

namespace gr {
    namespace DMO_TETRA {

        uint32_t patternAtPositionScore(std::vector<uint8_t> data, std::vector<uint8_t> pattern, std::size_t position)
        {
            uint32_t errors = 0;

            for (std::size_t idx = 0; idx < pattern.size(); idx++)
            {
                errors += (uint32_t)(pattern[idx] ^ data[position + idx]);
            }
        
            return errors;
            }

        std::vector<uint8_t> vectorExtract(const std::vector<uint8_t>& source, uint32_t pos, int32_t length)
        {
            if (length <= 0 || pos >= source.size()) {
                return {}; // Return an empty vector if invalid length or position
            }

            length = std::min(length, static_cast<int32_t>(source.size() - pos)); // Clamp length to remaining elements
            return std::vector<uint8_t>(source.begin() + pos, source.begin() + pos + length);
        }


        std::vector<uint8_t> vectorAppend(std::vector<uint8_t> vec1, std::vector<uint8_t> vec2)
        {
            std::vector<uint8_t> ret(vec1);
            ret.insert(ret.end(), vec2.begin(), vec2.end());
            return ret;
        }

      
 
   

    // tetraCell class


  

    }//end of DMO_TETRA
}//end of gr