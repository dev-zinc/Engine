#pragma once

#include <set>
#include <vector>
#include <GLFW/glfw3.h>

#include "swapchain/swapchain_supports.h"
#include "util/binary_file_utils.h"

namespace EngineComponentFactory {

    // Create Window

    GLFWwindow *createWindow();

    // Create Instance

    // Get
    std::vector<const char*> getRequiredGlfwExtensions();

    VkApplicationInfo createApplicationInfo();

    VkInstanceCreateInfo createInstanceCreateInfo(VkApplicationInfo appInfo, std::vector<const char*>& extensions);

    VkInstance createVkInstance();

    // Create Surface

    VkSwapchainCreateInfoKHR createSwapchainCreateInfo(VkSurfaceKHR surface, SwapchainSupportDetails& swapchainInfo, bool useSameQueueFamily);

    VkSwapchainKHR createSwapchain(VkDevice device, VkSurfaceKHR surface, SwapchainSupportDetails& swapchainInfo, bool useSameQueueFamily);

    // Get
    std::vector<VkImage> getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain);

    VkImageViewCreateInfo createImageViewCreateInfo(VkFormat format, VkImage image);

    VkImageView createImageView(VkDevice device, VkFormat format, VkImage image);

    VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window);

    // Create Device

    // Get
    std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance instance);

    // Resolve
    VkPhysicalDevice getProperPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices, VkSurfaceKHR surface);

    VkPhysicalDeviceFeatures createPhysicalDeviceFeatures();

    // Get
    std::vector<const char*> getDeviceExtensions();

    VkDeviceCreateInfo createDeviceCreateInfo(
        const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList,
        VkPhysicalDeviceFeatures physicalDeviceFeatures,
        const std::vector<const char*>& deviceExtensions
    );

    VkDevice createDevice(
        VkPhysicalDevice physicalDevice,
        std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList
    );

    // Create Shaders

    VkShaderModuleCreateInfo createShaderModuleCreateInfo(const BinaryFile& binary);

    VkShaderModule createShaderModule(VkDevice device, const BinaryFile& binary);
}
