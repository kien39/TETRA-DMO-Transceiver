/*
 *  tetra-kit
 *  Copyright (C) 2020  LarryTh <dev@logami.fr>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "mac.h"

using namespace Tetra;


/**
 * @brief Fibonacci LFSR descrambling - 8.2.5
 *
 */

std::vector<uint8_t> Mac::descramble(std::vector<uint8_t> data, const int len, const uint32_t scramblingCode)
{
    const uint8_t poly[14] = {32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1}; // Feedback polynomial - see 8.2.5.2 (8.39)
    std::vector<uint8_t> res;

    uint32_t lfsr = scramblingCode;                                             // linear feedback shift register initialization (=0 + 3 for BSCH, calculated from Color code ch 19 otherwise)

    for (int i = 0; i < len; i++)
    {
        uint32_t bit = lfsr >> (32 - poly[0]);                                  // apply poly (Xj + ...)
        for (int j = 1; j < 14; j++)
        {
            bit = bit ^ (lfsr >> (32 - poly[j]));
        }
        bit = bit & 1;                                                          // finish apply feedback polynomial (+ 1)
        lfsr = (lfsr >> 1) | (bit << 31);

        res.push_back(data[i] ^ (bit & 0xff));
    }

    return res;
}

std::vector<uint8_t> Mac::scramble(std::vector<uint8_t> data, int len, uint32_t scramblingCode)
{
    const uint8_t poly[14] = {32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1}; // Feedback polynomial - see 8.2.5.2 (8.39)
    std::vector<uint8_t> res;

    uint32_t lfsr = scramblingCode;
    uint32_t bit;
	for (int i = 0; i < len; i++)
    {
        bit = lfsr >> (32 - poly[0]);                                           // apply poly (Xj + ...)
        for (int j = 1; j < 14; j++)                                            // remaining poly X(j-1)...
        {
            bit = bit ^ (lfsr >> (32 - poly[j]));
        }
        bit = bit & 1;                                                          // finish apply feedback polynomial (+ 1)
        lfsr = (lfsr >> 1) | (bit << 31);

		res.push_back(data[i] ^ (bit & 0xff));
    }
	return res;
}

/**
 * @brief (K,a) block deinterleaver - 8.2.4
 *
 */

std::vector<uint8_t> Mac::deinterleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a)
{
    std::vector<uint8_t> res(K, 0);                                             // output vector is size K

    for (unsigned int idx = 1; idx <= K; idx++)
    {
        uint32_t k = 1 + (a * idx) % K;
        res[idx - 1] = data[k - 1];                                             // to interleave: DataOut[i-1] = DataIn[k-1]
    }

    return res;
}

std::vector<uint8_t> Mac::interleave(std::vector<uint8_t> data, const uint32_t K, const uint32_t a)
{
    std::vector<uint8_t> res(K, 0);                                             // output vector is size K

    for (unsigned int idx = 1; idx <= K; idx++)
    {
        uint32_t k = 1 + (a * idx) % K;
        res[k - 1] = data[idx - 1];                                             // to interleave: DataOut[i-1] = DataIn[k-1]
    }

    return res;
}


/**
 * @brief Depuncture with 2/3 rate - 8.2.3.1.3
 *
 */

std::vector<uint8_t> Mac::depuncture23(std::vector<uint8_t> data, const uint32_t len)
/* depuncture with 2/3 rate */
{
    const uint8_t P[] = {0, 1, 2, 5};                                           // 8.2.3.1.3 - P[1..t] 1st element (0) is not used.
    std::vector<uint8_t> res(4 * len * 2 / 3, 2);                               // 8.2.3.1.2 with flag 2 for erase bit in Viterbi routine

    uint8_t t = 3;                                                              // 8.2.3.1.3
    uint8_t period = 8;                                                         // 8.2.3.1.2

    for (uint32_t j = 1; j <= len; j++)
    {
        uint32_t i = j;                                                         // punct->i_func(j);
        uint32_t k = period * ((i - 1) / t) + P[i - t * ((i - 1) / t)];         // punct->period * ((i-1)/t) + P[i - t*((i-1)/t)];
        res[k - 1] = data[j - 1];
    }

    return res;
}

std::vector<uint8_t> Mac::motherEncode1614(std::vector<uint8_t> data, const uint32_t len)
{
    //
    uint8_t ces_delay[] = {0, 0, 0, 0};                                           // initiate ces - reset to all 'zero'
    std::vector<uint8_t> res(4 * len, 0);

    //int conv_enc_input(struct conv_enc_state *ces, uint8_t *in, int len, uint8_t *out)
	for (uint32_t i = 0; i < len; i++) 
    {
        uint8_t g1, g2, g3, g4;
        uint8_t *delayed = ces_delay;

        /* G1 = 1 + D + D4 */
        g1 = (data[i] + delayed[0] + delayed[3]) % 2;
        /* G2 = 1 + D2 + D3 + D4 */
        g2 = (data[i] + delayed[1] + delayed[2] + delayed[3]) % 2;
        /* G3 = 1 + D + D2 + D4 */
        g3 = (data[i] + delayed[0] + delayed[1] + delayed[3]) % 2;
        /* G4 = 1 + D + D3 + D4 */
        g4 = (data[i] + delayed[0] + delayed[2] + delayed[3]) % 2;

        /* shift the state and input our new bit */
        delayed[3] = delayed[2];
        delayed[2] = delayed[1];
        delayed[1] = delayed[0];
        delayed[0] = data[i];

        res[4*i]   = g1; 
        res[4*i+1] = g2;
        res[4*i+2] = g3;
        res[4*i+3] = g4;
	}
    return res;
}

/* Puncture the mother code (x4) and write 'len' symbols to out */
std::vector<uint8_t> Mac::puncture23(std::vector<uint8_t> data, const uint32_t len)
{
    const uint8_t P[] = {0, 1, 2, 5};                                           // 8.2.3.1.3 - P[1..t]
    std::vector<uint8_t> res(len, 0);                                           // set default value to ZERO
	uint8_t t = 3;
    uint8_t period = 8;

	/* Section 8.2.3.1.2 */
	for (uint32_t j = 1; j <= len; j++) {
		uint32_t i = j;
		uint32_t k = period * ((i-1)/t) + P[i - t*((i-1)/t)];
		res[j-1] = data[k-1];
	}
	return res;
}


/**
 * @brief Viterbi decoding of RCPC code 16-state mother code of rate 1/4 - 8.2.3.1.1
 *
 */

std::vector<uint8_t> Mac::viterbiDecode1614(std::vector<uint8_t> data)
{
    std::string sIn = "";
    for (std::size_t idx = 0; idx < data.size(); idx++)
    {
        sIn += (char)(data[idx] + '0');
    }

    std::string sOut = m_viterbiCodec1614->Decode(sIn);

    std::vector<uint8_t> res;

    for (size_t idx = 0; idx < sOut.size(); idx++)
    {
        res.push_back((uint8_t)(sOut[idx] - '0'));
    }

    return res;
}

/**
 * @brief Reed-Muller decoder and FEC correction 30 bits in, 14 bits out
 *
 * FEC thanks to Lollo Gollo @logollo see "issue #21"
 *
 */

std::vector<uint8_t> Mac::reedMuller3014Decode(std::vector<uint8_t> data)
{
    uint8_t q[5];
    std::vector<uint8_t> res(14);

    q[0] = data[0];
    q[1] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 11]) % 2;
    q[2] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 8] + data[13 + 9]) % 2;
    q[3] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 9] + data[13 + 10]) % 2;
    q[4] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 8] + data[13 + 10] + data[13 + 11]) % 2;
    res[0] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[1];
    q[1] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 9] + data[13 + 11]) % 2;
    q[2] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 10]) % 2;
    q[3] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 8]) % 2;
    q[4] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 11]) % 2;
    res[1] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[2];
    q[1] = (data[13 + 2] + data[13 + 5] + data[13 + 8] + data[13 + 10] + data[13 + 11]) % 2;
    q[2] = (data[13 + 1] + data[13 + 3] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 10]) % 2;
    q[3] = (data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 11]) % 2;
    res[2] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[3];
    q[1] = (data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    q[2] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    q[3] = (data[13 + 2] + data[13 + 4] + data[13 + 6] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    q[4] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 14]) % 2;
    res[3] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[4];
    q[1] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    q[2] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    q[4] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 15]) % 2;
    res[4] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[5];
    q[1] = (data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    q[2] = (data[13 + 2] + data[13 + 4] + data[13 + 6] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    q[4] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 12] + data[13 + 14] + data[13 + 15]) % 2;
    res[5] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[6];
    q[1] = (data[13 + 3] + data[13 + 5] + data[13 + 6] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    q[2] = (data[13 + 1] + data[13 + 4] + data[13 + 5] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    q[4] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 13] + data[13 + 14] + data[13 + 15]) % 2;
    res[6] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[7];
    q[1] = (data[13 + 2] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 1] + data[13 + 3] + data[13 + 5] + data[13 + 8] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    res[7] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[8];
    q[1] = (data[13 + 2] + data[13 + 3] + data[13 + 9] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    q[2] = (data[13 + 1] + data[13 + 7] + data[13 + 8] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    q[3] = (data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 4] + data[13 + 6] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 13] + data[13 + 16]) % 2;
    res[8] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[9];
    q[1] = (data[13 + 1] + data[13 + 3] + data[13 + 8] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    q[2] = (data[13 + 4] + data[13 + 6] + data[13 + 10] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    q[3] = (data[13 + 2] + data[13 + 7] + data[13 + 9] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 14] + data[13 + 16]) % 2;
    res[9] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[10];
    q[1] = (data[13 + 1] + data[13 + 2] + data[13 + 7] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    q[2] = (data[13 + 3] + data[13 + 8] + data[13 + 9] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 4] + data[13 + 6] + data[13 + 9] + data[13 + 10] + data[13 + 11] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    q[4] = (data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 10] + data[13 + 13] + data[13 + 14] + data[13 + 16]) % 2;
    res[10] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[11];
    q[1] = (data[13 + 2] + data[13 + 6] + data[13 + 9] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 4] + data[13 + 7] + data[13 + 10] + data[13 + 11] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 3] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 11] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 4] + data[13 + 8] + data[13 + 9] + data[13 + 10] + data[13 + 12] + data[13 + 15] + data[13 + 16]) % 2;
    res[11] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[12];
    q[1] = (data[13 + 5] + data[13 + 8] + data[13 + 10] + data[13 + 11] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 11] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 5] + data[13 + 7] + data[13 + 9] + data[13 + 10] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 2] + data[13 + 4] + data[13 + 5] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 9] + data[13 + 13] + data[13 + 15] + data[13 + 16]) % 2;
    res[12] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    q[0] = data[13];
    q[1] = (data[13 + 2] + data[13 + 4] + data[13 + 7] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[2] = (data[13 + 6] + data[13 + 9] + data[13 + 10] + data[13 + 11] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[3] = (data[13 + 1] + data[13 + 3] + data[13 + 4] + data[13 + 8] + data[13 + 9] + data[13 + 11] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    q[4] = (data[13 + 1] + data[13 + 2] + data[13 + 3] + data[13 + 6] + data[13 + 7] + data[13 + 8] + data[13 + 10] + data[13 + 14] + data[13 + 15] + data[13 + 16]) % 2;
    res[13] = (q[0] + q[1] + q[2] + q[3] + q[4]) >= 3 ? 1 : 0;

    // check deviation from input
    // int deviation = 0;
    // for (int cnt = 0; cnt < 14; cnt++)
    // {
    //     deviation += (data[cnt] != res[cnt]) ? 1 : 0;
    // }
    // printf("FEC correction %.2f\n", deviation / 14.);
    // print_vector(data, 14);
    // print_vector(res, 14);

    //return vector_extract(data, 0, 14);

    return res;
}

std::vector<uint8_t> Mac::reedMuller3014Encode(std::vector<uint8_t> data)
/* input: vector data of 14 uint8_t, output vector res of 30 uint8_t */
{
    std::vector<uint8_t> res(30,0);
    uint16_t data14bit = 0;
    for (uint8_t i=0; i <14; i++)
    {
        data14bit = (data14bit << 1) + data[i];
    }

    uint32_t res30bit = rm3014Compute(data14bit);

    for (uint8_t i=0; i <30; i++)
    {
        res[i] = (res30bit & (1 << (29-i))) ? 1 : 0;
    }

    return res;
}

uint32_t Mac::rm3014Compute(const uint16_t in)
/* code taken from osmo-tetra in 32bit & 16bit formats */
{
	const uint8_t rm_30_14_gen[14][16] = {
		{ 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 },
		{ 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0 },
		{ 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0 },
		{ 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1 },
		{ 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1 },
		{ 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1 },
		{ 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1 },
		{ 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1 },
		{ 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1 },
		{ 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1 }
	};

	uint32_t rm_30_14_rows[14];

	uint32_t val;
	uint32_t ret;

	for (int i = 0; i < 14; i++) {
		/* upper 14 bits identity matrix */
		val = (1 << (16+13 - i));
		/* lower 16 bits from rm_30_14_gen */
		ret = 0;
		/* shift_bits_together */
		for (int j = 15; j >= 0; j--)
			ret |= rm_30_14_gen[i][j] << (15-j);

		val |= ret;
		rm_30_14_rows[i] = val;
		//printf("rm_30_14_rows[%u] = 0x%08x\n", i, val);
	}

	val = 0;

	for (int i = 0; i < 14; i++) {
		uint32_t bit = (in >> (14-1-i)) & 1;
		if (bit)
			val ^= rm_30_14_rows[i];
		/* we can skip the 'else' as XOR with 0 has no effect */
	}
	return val;
}


/**
 * @brief Calculated CRC16 ITU-T X.25 - CCITT
 *
 */

int Mac::checkCrc16Ccitt(std::vector<uint8_t> data, const int len)
{
    uint16_t crc = 0xFFFF;                                                      // CRC16-CCITT initial value

    for (int i = 0; i < len; i++)
    {
        uint16_t bit = (uint16_t)data[i];

        crc ^= bit << 15;
        if(crc & 0x8000)
        {
            crc <<= 1;
            crc ^= 0x1021;                                                      // CRC16-CCITT polynomial
        }
        else
        {
            crc <<= 1;
        }
    }

    return crc == 0x1D0F;                                                       // CRC16-CCITT reminder value
}

uint16_t Mac::computeCrc16Ccitt(std::vector<uint8_t> data, const int len)
/*checkCrc16Ccitt and computeCrcCcitt perform exactly same calculation. Only difference in return value: boolean or crc code. 
To perform crc coding, calculate Crc16, take complement value (~binary) and insert to the end of data stream from MSB to LSBidentical identical */

{
   uint16_t crc = 0xFFFF;                                                      // CRC16-CCITT initial value

    for (int16_t i = 0; i < len; ++i) {
        uint16_t bit = data[i] & 0x1;
        crc ^= bit << 15;
        if ((crc & 0x8000)) {
            crc <<= 1;
            crc ^= 0x1021;                                                      // #define GEN_POLY 0x1021
        } else {
            crc <<= 1;
        }
    }
    return crc;
}
