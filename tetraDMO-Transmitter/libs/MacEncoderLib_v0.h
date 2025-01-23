#ifndef CODINGLIB
#define CODINGLIB
#include "viterbi.h"

extern "C" {

enum BurstType {
        DSB    = 1,
        DNB    = 2,
        DNB_SF = 3,
        IDLE   = 0
    };
struct TetraTime {
    uint16_t tn=1;                                                            ///< time slot
    uint16_t fn=1;                                                            ///< frame number
    uint16_t mn=1;                                                            ///< multi-frame number
};

/* for serviceMacEncode - Tx */
struct macEncode
{
    std::vector<uint8_t> uPlane;
    u_int16_t bitCnt = 0;
};
macEncode tx;
void timeIncrease();
void updateTnFn_DmOccupied();

void Init_serviceMacEncode(bool showTxt, const uint32_t talkgroupId, const uint32_t radioId);
uint32_t serviceMacEncode(uint8_t* in_data, const uint32_t in_len, uint8_t* out_data);


/* temporary - use the DM field below for 
Scrambling Code=9173335F  MCC/MNC = 452/ 100  Dest address= 0/1001  Source address= 1/6081751
Scambling Code is calculated from Source Address and MCC/MNC
Other fields to be updated: FN:TN, message type, FrameCntDown...
*/
std::vector<uint8_t> DM_SETUP     = {1,1,0,1,0,0,0,0,0,0,0,0,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0};
std::vector<uint8_t> DM_OCCUPIED  = {1,1,0,1,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,1,0,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0};
std::vector<uint8_t> DM_INFO      = {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,1,0,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,0,0,0,1,0,0,0,0,0,1,0,0,1,1,0,1,0,0,1,0,0,0,0,0,1,1,1,0,1,1,0,0,1,1,1,1,0,1,1,0,1,0,0,1,1,0,0,0,0,0};
std::vector<uint8_t> DM_RESERVED  = {1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0};
std::vector<uint8_t> DM_TX_CEASED = {0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,1,0,0,0,1,1,1,1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0};
const std::vector<uint8_t> FREQ_CORRECTION = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
/* for service_mac function*/
const std::vector<uint8_t> NORMAL_TRAINING_SEQ_1       = {1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,1,1,0,1,0,0}; // n1..n22
const std::vector<uint8_t> NORMAL_TRAINING_SEQ_2       = {0,1,1,1,1,0,1,0,0,1,0,0,0,0,1,1,0,1,1,1,1,0}; // p1..p22
const std::vector<uint8_t> NORMAL_TRAINING_SEQ_3_BEGIN = {0,0,0,1,1,0,1,0,1,1,0,1};                     // q11..q22
const std::vector<uint8_t> NORMAL_TRAINING_SEQ_3_END   = {1,0,1,1,0,1,1,1,0,0};                         // q1..q10

// 9.4.4.3.4 Synchronisation training sequence
const std::vector<uint8_t> SYNC_TRAINING_SEQ = {1,1,0,0,0,0,0,1,1,0,0,1,1,1,0,0,1,1,1,0,1,0,0,1,1,1,0,0,0,0,0,1,1,0,0,1,1,1}; // y1..y38

// ETSI EN 300 396-2 V1.4.1 DMO 9.4.3.3.4 Normal training sequence & 9.4.3.3.4 Synchronisation training sequence
const std::vector<uint8_t> PREAMBLE_P1  =   {0,0,1,1,0,0,1,0,0,0,1,1};   // j1..j12
const std::vector<uint8_t> PREAMBLE_P2  =   {1,0,0,1,1,0,1,0,1,0,0,1};   // k1..k12
const std::vector<uint8_t> PREAMBLE_P3  =   {0,0,0,1,0,1,0,0,0,1,1,1};   // I1..I12
const std::vector<uint8_t> GUARD34      =   {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0};   // bit1-34
const std::vector<uint8_t> TAILGUARD8   =   {0,0,0,0,0,0,0,0};   // bit 503-510

bool m_showTxt = true;
uint32_t m_sourceAddress = 6081751;
uint16_t m_destinAddress = 1001;
uint16_t m_frameCntDn;
uint32_t m_mcc = 452;                                                         ///< Cell MCC
uint32_t m_mnc = 100;                                                         ///< Cell MNC
//uint16_t m_colorCode;                                                   ///< Cell Color code
uint32_t m_scramblingCode = 0x9173335F;                                              ///< Cell Scrambling code
//uint32_t m_locationArea;

//int32_t m_downlinkFrequency;                                            ///< Downlink frequency [Hz]
//int32_t m_uplinkFrequency;                                              ///< Uplink frequency [Hz]
bool    m_cellInformationsAcquired = false;

bool m_bPreSynchronized = false;
bool m_bIsSynchronized = false;
uint64_t m_syncBitCounter = 0;                                              ///< Synchronization bits counter
bool m_frameFound;
bool m_validBurstFound;
bool m_sfStolenFlag = false;

BurstType m_burstType;                                           // same enum name is used for TMO & DMO (DSB, DNB DNB_SF)
TetraTime m_tetraTime ={1, 1, 1};
const std::size_t FRAME_DETECT_THRESHOLD = 6;
const std::size_t FRAME_LEN = 510;                                      ///< Burst length in bits
const std::size_t UPLANE_LEN = 432;
std::vector<uint8_t> m_frame;                                           ///< Burst data
std::uint8_t  static m_burst[510] = {0}; 
std::uint16_t  m_burst_ptr = 0;
std::vector<uint8_t> bkn1(510,0);
std::vector<uint8_t> bkn2(510,0);

uint32_t getValue(std::vector<uint8_t> pdu, const uint16_t pos, const uint16_t len);
std::vector<uint8_t> processDmacSync(std::vector<uint8_t> pdu, const uint16_t pdu_len);
void updateScramblingCode(const uint32_t sourceAddress, const uint32_t mnIdentity);
uint32_t getScramblingCode();
void updateSynchronizer(bool frameFound);
uint32_t patternAtPositionScore(std::uint8_t *data, std::vector<uint8_t> pattern, std::size_t position);
bool isFrameFound();
uint16_t serviceMacDecode(uint8_t* in_data, const int in_len, uint8_t* out_data);


ViterbiCodec * m_viterbiCodec1614;                                      ///< Viterbi codec
int descramble_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const int len, const uint32_t scramblingCode);
std::vector<uint8_t> descramble(std::vector<uint8_t> data, const int len, const uint32_t scramblingCode);
int scramble_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const int len, const uint32_t scramblingCode);
std::vector<uint8_t> scramble(std::vector<uint8_t> data, int len, uint32_t scramblingCode);

int deinterleave_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t K, const uint32_t a);
std::vector<uint8_t> deinterleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a);
int interleave_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t K, const uint32_t a);
std::vector<uint8_t> interleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a);

int depuncture23_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t len);
std::vector<uint8_t> depuncture23(std::vector<uint8_t> data, const uint32_t len);
int puncture23_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t len);
std::vector<uint8_t> puncture23(std::vector<uint8_t> data, const uint32_t len);

int viterbiDecode1614_raw(uint8_t* in_data, int in_len, uint8_t* out_data);
std::vector<uint8_t> viterbiDecode1614(std::vector<uint8_t> data);
int motherEncode1614_raw(uint8_t* in_data, int in_len, uint8_t* out_data, const uint32_t len);
std::vector<uint8_t> motherEncode1614(std::vector<uint8_t> data, const uint32_t len);

std::vector<uint8_t> reedMuller3014Decode(std::vector<uint8_t> data);
std::vector<uint8_t> reedMuller3014Encode(std::vector<uint8_t> data);
uint32_t rm3014Compute(const uint16_t in);

bool checkCrc16Ccitt_raw(uint8_t* in_data, int in_len, const int len);
bool checkCrc16Ccitt(std::vector<uint8_t> data, const int len);
uint16_t computeCrc16Ccitt_raw(uint8_t* in_data, int in_len, const int len);
uint16_t computeCrc16Ccitt(std::vector<uint8_t> data, const int len);

#endif /* CODINGLIB */
}