#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <vector>
#include <string>

static inline uint16_t crc16_ccitt_init(void) {
	return 0xFFFF;
}

static inline uint16_t crc16_ccitt_update(uint8_t byte, uint16_t crc) {
	int i;
	int xor_flag;

	/* For each bit in the data byte, starting from the leftmost bit */
	for (i = 7; i >= 0; i--) {
		/* If leftmost bit of the CRC is 1, we will XOR with
		 * the polynomial later */
		xor_flag = crc & 0x8000;

		/* Shift the CRC, and append the next bit of the
		 * message to the rightmost side of the CRC */
		crc <<= 1;
		crc |= (byte & (1 << i)) ? 1 : 0;

		/* Perform the XOR with the polynomial */
		if (xor_flag)
			crc ^= 0x1021;
	}

	return crc;
}

static inline uint16_t crc16_ccitt_finalize(uint16_t crc) {
	int i;

	/* Augment 16 zero-bits */
	for (i = 0; i < 2; i++) {
		crc = crc16_ccitt_update(0, crc);
	}

	return crc;
}

uint16_t crc_calculate_algorithm(const std::vector<uint8_t> data, const size_t len) {

	uint16_t crc = crc16_ccitt_init();

	/* Update the CRC using the data */
	for (size_t i = 0; i < len; i++) {
		crc = crc16_ccitt_update(data[i], crc);
	}

	crc = crc16_ccitt_finalize(crc);

	return crc;
}

static inline std::string tobinary(uint16_t word)
{
	std::string str;
	
	for( uint8_t i = 0; i < 16; i++)
	{
		str += (word & (1 << (15-i))) ? "1" : "0";
	}
	return str;
}

uint16_t checkCrc16Ccitt(std::vector<uint8_t> data, const int len)
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

    return crc; // == 0x1D0F;                                                       // CRC16-CCITT reminder value
}

uint16_t computeCrc16Ccitt(std::vector<uint8_t> data, const int len)
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

/*
int main(int argc, char * argv[])
{
    //
	std::vector<uint8_t> b60 {0,0,0,0,1,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,1};
	std::vector<uint8_t> b124 {1,0,0,0,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0};
	std::vector<uint8_t> b268 {0,0,0,0,1,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,1,1,0,0,1,0,0,0,0,1,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,1,
							   1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    printf("testing CRC16 implementation\n");

	uint16_t crc60  = computeCrc16Ccitt(b60, 60);
	printf("crc60  = 0x%4X, 0b%s\n", crc60, tobinary(crc60).c_str());
	std::cout << "checkCrc16Ccitt:   " << tobinary(checkCrc16Ccitt(b60, 60)) << "\n";
	std::cout << "Add 16 bit CRC into the data and recalculate CRC for entire data + crc\n";
	crc60 = ~crc60;
	for (uint8_t i=0; i<16; i++)
	{
		b60.push_back((crc60 >> (15-i)) & 0x0001);
	}
	std::cout << "checkCrc16Ccitt (correct = 0x1D0F):   " << std::hex << checkCrc16Ccitt(b60, 76) << "\n";

	uint16_t crc124 = computeCrc16Ccitt(b124, 124);
	printf("crc124 = 0x%4X, 0b%s\n", crc124, tobinary(crc124).c_str());
	std::cout << "checkCrc16Ccitt:   " << tobinary(checkCrc16Ccitt(b124, 124)) << "\n";
	std::cout << "Add 16 bit CRC into the data and recalculate CRC for entire data + crc\n";
	crc124 = ~crc124;
	for (uint8_t i=0; i<16; i++)
	{
		b124.push_back((crc124 >> (15-i)) & 0x0001);
	}
	std::cout << "checkCrc16Ccitt (correct = 0x1D0F):   " << std::hex << checkCrc16Ccitt(b124, 140) << "\n";

	uint16_t crc268 = computeCrc16Ccitt(b268, 268);
	printf("crc268 = 0x%4X, 0b%s\n", crc268, tobinary(crc268).c_str());
	std::cout << "checkCrc16Ccitt:   " << tobinary(checkCrc16Ccitt(b268, 268)) << "\n";
	std::cout << "Add 16 bit CRC into the data and recalculate CRC for entire data + crc\n";
	crc268 = ~crc268;
	for (uint8_t i=0; i<16; i++)
	{
		b268.push_back((crc268 >> (15-i)) & 0x0001);
	}
	std::cout << "checkCrc16Ccitt (correct = 0x1D0F):   " << std::hex << checkCrc16Ccitt(b268, 284) << "  binary conversion: " << std::boolalpha << crc268 << "\n";
}
*/


std::vector<uint8_t> reedMuller3014Decode(std::vector<uint8_t> data)
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


uint32_t tetra_rm3014_compute(const uint16_t in)
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


int main(int argc, char * argv[])
{
    //
	std::vector<uint8_t> di30 (30, 0);
	std::vector<uint8_t> do14;
	std::string inv;
	uint16_t ei14;

	printf("testing Reed-Muller 3014 Implementation\n");
	std::cin >> inv;

	for (int ind=0; ind < 1000; ind++)
	{
		ei14 = rand() & 0x3fff; // take 14 bits
		
		uint32_t eo30 = tetra_rm3014_compute(ei14);
		std::cout << "\nEncoder Input : " << std::hex << ei14 << " 0b" << tobinary(ei14) << "\n";
		//std::cout << "\nEncoder Output: " << std::hex << eo30 << " 0b" << tobinary((eo30>>16))  << tobinary((eo30&0xffff)) <<"\n"; 

		for (uint8_t i = 0; i < 30; i++)
		{
			uint32_t bit = (eo30>>(29-i)) & 0x1;
			di30[i] = (bit) ? 1 : 0;
			printf ("%u", di30[i] );
		}
		printf ("\n" );

		do14 = reedMuller3014Decode(di30);
		uint16_t decoded14 = 0;
		printf ("\ndo14:" );
		for (uint8_t i = 0; i < 14; i++)
		{
			printf ("%u", di30[i] );
			decoded14 = decoded14*2 +di30[i];
		}
		printf ("\n" );
		decoded14 &= 0x3fff;
		if (decoded14 == ei14)
		{
			std::cout <<"\nind = " << ind << "  Encode & decode correctly \n";
		}
		else
		{
			std::cout <<"\nind = " << ind << "  Encode & decode INCORRECTLY \n";
			std::cin >> inv;
		}

	}
	printf("\nComplete.");
}
