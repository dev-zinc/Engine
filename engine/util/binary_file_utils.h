#pragma once

#include <vector>
#include <string>

struct BinaryFile {
    const std::string fileName;
    std::vector<char> contents;

    const char* data() const {
        return contents.data();
    }

    size_t size() const {
        return contents.size();
    }
};

namespace BinaryFileUtils {

    BinaryFile readBinaryFile(const char* filePath);

    void searchFiles(const char* path, std::vector<BinaryFile>& files);

    std::vector<BinaryFile> getAllFiles();
}
