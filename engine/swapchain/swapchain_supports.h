#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    std::vector<VkPresentModeKHR> presentModes;

    VkSurfaceFormatKHR getProperSurfaceFormat() const;

    VkPresentModeKHR getProperPresentMode() const;

    uint32_t getProperMinImageCount() const;

    VkExtent2D getProperExtent() const;
};

namespace SwapchainSupports {

    bool supportsSwapchain(VkPhysicalDevice physicalDevice);

    SwapchainSupportDetails getSwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
}
