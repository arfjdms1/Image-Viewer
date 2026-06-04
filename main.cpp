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

    //Image Verification
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

    char chunkType[5] = {0};
    image.read(chunkType, 4);
    cout << "Chunk Type: " << chunkType << endl;

    return 0;
}