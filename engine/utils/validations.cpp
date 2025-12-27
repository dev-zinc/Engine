#include "validations.h"

#include "classic_string.h"

bool Validations::isAvailable(const std::vector<VkLayerProperties>& availableLayers, ClassicString layerName) {
    return std::ranges::any_of(
        availableLayers,
        [&](ClassicString name) {
            return strcmp(name, layerName) == 0;
        },
        &VkLayerProperties::layerName
    );
}
