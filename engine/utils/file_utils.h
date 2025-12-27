#pragma once
#include <vector>

#include "classic_string.h"

namespace FileUtils {

    std::vector<char> readBinaryFile(ClassicString filePath);
}
