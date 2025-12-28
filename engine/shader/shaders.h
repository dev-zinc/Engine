#pragma once

#include <iostream>
#include <string>

enum ShaderType {
    VERTEX_SHADER,
    FRAGMENT_SHADER,
    GEOMETRY_SHADER,
};

namespace Shaders {
    constexpr auto SHADER_DIR { "./shaders" };

    inline ShaderType getShaderType(const std::string fileName) {
        if (fileName.find(".vert") != std::string::npos) {
            return VERTEX_SHADER;
        }
        if (fileName.find(".frag") != std::string::npos) {
            return FRAGMENT_SHADER;
        }
        if (fileName.find(".geom") != std::string::npos) {
            return GEOMETRY_SHADER;
        }
        throw std::runtime_error("Shader type not found: " + fileName);
    }
}