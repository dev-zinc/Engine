#include "binary_file_utils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "../shader/shaders.h"

std::vector<BinaryFile> BinaryFileUtils::getAllFiles() {
    std::vector<BinaryFile> files {};
    searchFiles(Shaders::SHADER_DIR, files);
    return files;
}

void BinaryFileUtils::searchFiles(const char* path, std::vector<BinaryFile>& files) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const auto filePath = entry.path().c_str();

        if (entry.is_regular_file()) {
            files.push_back(readBinaryFile(filePath));
        } else if (entry.is_directory()) {
            searchFiles(filePath, files);
        }
    }
}

BinaryFile BinaryFileUtils::readBinaryFile(const char* filePath) {
    std::ifstream fileStream { filePath, std::ios::binary | std::ios::ate };
    const std::filesystem::path path { filePath };

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

    return { path.filename().string() , contentsBuffer };
}