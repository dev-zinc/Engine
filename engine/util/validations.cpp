#include "validations.h"

bool Validations::isAvailable(const std::vector<VkLayerProperties>& availableLayers, const char* layerName) {
    return std::ranges::any_of(
        availableLayers,
        [&](const char* name) {
            return strcmp(name, layerName) == 0;
        },
        &VkLayerProperties::layerName
    );
}
