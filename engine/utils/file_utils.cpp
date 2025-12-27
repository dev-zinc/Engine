#include "file_utils.h"

#include <fstream>
#include <iostream>
#include <vector>

std::vector<char> FileUtils::readBinaryFile(ClassicString filePath) {
    std::ifstream fileStream(filePath, std::ios::binary | std::ios::ate);

    if (!fileStream.is_open()) {
        std::cerr << "could not open file." << std::endl;
        return {};
    }
    std::streamsize fileSize = fileStream.tellg();
    std::vector<char> contentsBuffer(fileSize);

    fileStream.seekg(0, std::ios::beg);

    if (!fileStream.read(contentsBuffer.data(), fileSize)) {
        std::cerr << "failed to read file." << std::endl;
        return {};
    }
    return contentsBuffer;
}
