#pragma once

#include <set>
#include <vector>
#include <vulkan/vulkan_core.h>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool hasGraphicsFamily() const {
        return graphicsFamily.has_value();
    }

    bool hasPresentFamily() const {
        return presentFamily.has_value();
    }

    bool isComplete() const {
        return hasGraphicsFamily() && hasPresentFamily();
    }

    std::set<uint32_t> getUniqueQueueIndexSet() {
        return {
            graphicsFamily.value(),
            presentFamily.value()
        };
    }
};

namespace QueueFactory {

    std::vector<VkQueueFamilyProperties> getQueueFamilyProperties(VkPhysicalDevice& physicalDevice);

    // Device의 Queue Family 위치를 가져옴 (graphics index, presentation index)
    QueueFamilyIndices getQueueFamilyIndices(VkPhysicalDevice& physicalDevice, VkSurfaceKHR& surface);

    VkDeviceQueueCreateInfo createDeviceQueueCreateInfo(const float* queuePriority, uint32_t queueFamilyIndex);

    std::vector<VkDeviceQueueCreateInfo> createQueueCreateInfoList(
        VkPhysicalDevice& physicalDevice,
        VkSurfaceKHR& surface,
        const float* queuePriority
    );
}

