#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include "utils/helpers.hpp"
using namespace std;
using namespace helpers;

int main() {
    // get image file
    ifstream image("/home/ubuntu/Documents/GitHub/Image-Viewer/image1.png", ios::in | ios::binary);
    if (!image) {
        throw runtime_error("Could not open image file");
    }

    // Image Verification
    cout << "Verifying Image: ";

    if (!verifyPNG(image)) {
        throw runtime_error("Could not verify image");
    }

    cout << "Image Verified" << endl;

    // Length Sanity Check
    uint32_t rawLength = 0;
    image.read(reinterpret_cast<char*>(&rawLength), 4);
    const uint32_t length = bigToLittle(rawLength);

    cout << "Length: " << length << endl;

    // Chunk Type
    char chunkType[5] = {0};
    image.read(chunkType, 4);

    cout << "Chunk Type: " << chunkType << endl;

    // Width
    uint32_t rawWidth = 0;
    image.read(reinterpret_cast<char*>(&rawWidth), 4);
    const uint32_t width = bigToLittle(rawWidth);

    cout << "Width: " << width << endl;

    // Height
    uint32_t rawHeight = 0;
    image.read(reinterpret_cast<char*>(&rawHeight), 4);
    const uint32_t height = bigToLittle(rawHeight);

    cout << "Height: " << height << endl;

    // Metadata

    cout << endl << "Metadata:" << endl;

    image.seekg(9, ios::cur);

    vector<uint8_t> rawCompressedPixelData;

    while (true) {
        uint32_t rawNextLength = 0;
        image.read(reinterpret_cast<char*>(&rawNextLength), 4);

        // Safety
        if (!image) {
            return 1;
        }

        // Get Next Chunk Metadata
        const uint32_t nextLength = bigToLittle(rawNextLength);

        char nextChunkType[5] = {0};
        image.read(nextChunkType, 4);


        // Check Chunk for Metadata
        if (strcmp(nextChunkType, "tEXt") == 0) {
            cout << "  - Found Metadata Chunk: " << "Size: " << nextLength << " Bytes ";

            // Output Metadata
            vector<char> chunk(nextLength);
            image.read(chunk.data(), nextLength);

            cout << "Content: ";
            for (uint32_t i = 0; i < nextLength; i++) {
                if (chunk[i] == '\0') {
                    cout << ": "; // Switch null-terminator with `:` for readability
                } else {
                    cout << chunk[i];
                }
            }
            image.seekg(4, ios::cur);
        } else if (strcmp(nextChunkType, "IDAT") == 0) {
            // Logging
            cout << "  - Gathering IDAT chunk (" << nextLength << " bytes)" << endl;

            // Get Data
            vector<uint8_t> temp(nextLength);
            image.read(reinterpret_cast<char*>(temp.data()), nextLength);

            // Save Data Efficiently
            rawCompressedPixelData.insert(rawCompressedPixelData.end(), temp.begin(), temp.end());

            // Seek past Next Chunk CRC (Checksum)
            image.seekg(4, ios::cur);
        } else if (strcmp(nextChunkType, "IEND") == 0) {
            cout << endl;
            break;
        } else {
            image.seekg(nextLength + 4, ios::cur);
            cout << "  - Found & Skipped Chunk: " << nextChunkType << " (" << nextLength << " bytes)" << endl;
        }
    }

    // Verify Data Collection
    cout << "Got " << rawCompressedPixelData.size() << " bytes of data." << endl;

    // Decompression
    uint8_t cmfByte = rawCompressedPixelData[0];
    uint8_t flgByte = rawCompressedPixelData[1];

    // Verify PNG compression Compliance
    if (uint8_t compressionMethod = cmfByte & 0x0F; compressionMethod != 8) {
        throw runtime_error("Expected DEFLATE compression (should be 8). Got " + to_string(compressionMethod));
    }

    return 0;
}