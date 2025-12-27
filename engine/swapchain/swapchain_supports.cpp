#include "swapchain_supports.h"

#include "../../utils/classic_string.h"


bool SwapchainSupports::supportsSwapchain(VkPhysicalDevice physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    return std::ranges::any_of(
        availableExtensions,
        [](ClassicString extensionName) {
            return strcmp(extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) != 0;
        }, &VkExtensionProperties::extensionName
    );
}


SwapchainSupportDetails SwapchainSupports::getSwapchainSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {

    VkSurfaceCapabilitiesKHR surfaceCapabilities {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());

    uint32_t presentModeCount {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    return { surfaceCapabilities, surfaceFormats, presentModes };
}

VkSurfaceFormatKHR SwapchainSupportDetails::getProperSurfaceFormat() const {
    for (VkSurfaceFormatKHR surfaceFormat : surfaceFormats) {
        if (
            const auto& [format, color] = surfaceFormat;
            format == VK_FORMAT_B8G8R8A8_SRGB && color == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
        ) {
            return surfaceFormat;
        }
    }
    return surfaceFormats[0];
}

VkPresentModeKHR SwapchainSupportDetails::getProperPresentMode() const {
    for (auto presentMode : presentModes) {
        // 삼중 버퍼링, 티어링 없음, 저지연
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }
    // VSync
    return VK_PRESENT_MODE_FIFO_KHR;
}

uint32_t SwapchainSupportDetails::getProperMinImageCount() const {
    const uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
    const uint32_t maxImageCount = surfaceCapabilities.maxImageCount;

    return maxImageCount > 0 && minImageCount > maxImageCount
            ? maxImageCount
            : minImageCount;
}

VkExtent2D SwapchainSupportDetails::getProperExtent() const {
    return surfaceCapabilities.currentExtent;
}
