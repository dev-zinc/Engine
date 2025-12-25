#pragma once

#include <vector>
#include <GLFW/glfw3.h>

namespace EngineComponentFactory {

    // Create Window

    GLFWwindow *createWindow();

    // Create Instance

    std::vector<const char*> getRequiredExtensions();

    VkApplicationInfo createApplicationInfo();

    VkInstanceCreateInfo createVkInstanceCreateInfo(const VkApplicationInfo& appInfo, std::vector<const char*>& extensions);

    VkInstance createVkInstance();

    // Create Surface

    VkSurfaceKHR createSurface(VkInstance& instance, GLFWwindow* window);

    // Create Device

    std::vector<VkPhysicalDevice> getPhysicalDevices(VkInstance& instance);

    VkPhysicalDevice getProperPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices, VkSurfaceKHR& surface);

    VkPhysicalDeviceFeatures createPhysicalDeviceFeatures();

    std::vector<const char*> getDeviceExtensions();

    VkDeviceCreateInfo createDeviceCreateInfo(
        const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList,
        const VkPhysicalDeviceFeatures& physicalDeviceFeatures,
        const std::vector<const char*>& deviceExtensions
    );

    VkDevice createDevice(
        VkPhysicalDevice& physicalDevice,
        std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList
    );
}
