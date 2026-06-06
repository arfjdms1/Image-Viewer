#include <cstring>
#include <iostream>
#include <fstream>
#include "helpers.hpp"
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

    while (true) {
        uint32_t rawNextLength = 0;
        image.read(reinterpret_cast<char*>(&rawNextLength), 4);

        // Safety
        if (!image) {
            cout << "Reached end of FS" << endl;
            break;
        }

        // Get Next Chunk Metadata
        const uint32_t nextLength = bigToLittle(rawNextLength);

        char nextChunkType[5] = {0};
        image.read(nextChunkType, 4);

        // Check Chunk for Metadata
        if (strcmp(nextChunkType, "tEXt") == 0) {
            cout << "Found Metadata Chunk" << endl << "size: " << nextLength << " Bytes" << endl;

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
        } else if (strcmp(nextChunkType, "IEND") == 0) {
            cout << "Reached end of FS" << endl;
            break;
        } else {
            image.seekg(nextLength + 4, ios::cur);
            cout << "Found & Skipped Chunk: " << nextChunkType << " (" << nextLength << " bytes)" << endl;
        }
    }

    return 0;
}