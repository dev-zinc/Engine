#pragma once

#include <vector>
#include <GLFW/glfw3.h>

#include "classic_string.h"

namespace Validations {

#if defined(NDEBUG)
    constexpr bool isValidationLayersEnabled = false;
#else
    constexpr bool isValidationLayersEnabled = true;
#endif

    const std::vector LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };

    bool isAvailable(const std::vector<VkLayerProperties>& availableLayers, ClassicString layerName);
}
