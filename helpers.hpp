#pragma once
#include <fstream>
#include <iosfwd>
#include <vector>
#include <bit>
using namespace std;

namespace helpers {
    inline bool verifyPNG(ifstream& image) {
        vector<unsigned char> data(8);
        image.read(reinterpret_cast<char*>(data.data()), 8);

        // check image hat
        if (data[0] == 0x89 && data[1] == 0x50 && data[2] == 0x4E && data[3] == 0x47 && data[4] == 0x0D && data[5] == 0x0A && data[6] == 0x1A && data[7] == 0x0A) {
            return true;
        }
        return false;
    }

    inline uint32_t bigToLittle(uint32_t bigEndianValue) {
        return byteswap(bigEndianValue);
    }
}
