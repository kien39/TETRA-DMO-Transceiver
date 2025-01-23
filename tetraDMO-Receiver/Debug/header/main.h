#include "header/Pdu.h"
#include "header/tetracell.h"
#include "header/uplane.h"
#include "header/viterbicodec.h"


class Decoder : 
{
    private:
    std::shared_ptr<TetraCell> m_tetraCell;
    std::shared_ptr<UPlane> m_Uplane;
    std::shared_ptr<ViterbiCodec> m_viterbiCodec1614;
    TetraTime m_tetraTime;

    public:
    Decoder();
    ~Decoder();

    int main_work();

    // Where all the action really happens
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