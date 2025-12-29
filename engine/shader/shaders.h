#pragma once

#include <iostream>
#include <string>
#include "vulkan/vulkan_core.h"

enum ShaderType {
    VERTEX_SHADER = VK_SHADER_STAGE_VERTEX_BIT,
    FRAGMENT_SHADER = VK_SHADER_STAGE_FRAGMENT_BIT,
    GEOMETRY_SHADER = VK_SHADER_STAGE_GEOMETRY_BIT,
};

namespace Shaders {
    constexpr auto SHADER_DIR { "./shaders" };

    inline ShaderType getShaderType(const std::string& fileName) {
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