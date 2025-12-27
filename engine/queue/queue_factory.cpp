#include "queue_factory.h"

#include <vector>
#include <GLFW/glfw3.h>


std::vector<VkQueueFamilyProperties> QueueFactory::getQueueFamilyProperties(VkPhysicalDevice physicalDevice) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    return queueFamilies;
}

VkBool32 supportsGraphics(const VkQueueFamilyProperties& queueFamilyProperties) {
    return queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
}

VkBool32 supportsPresentation(VkPhysicalDevice physicalDevice, int index, VkSurfaceKHR surface) {
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &presentSupport);
    return presentSupport;
}

QueueFamilyIndices QueueFactory::getQueueFamilyIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices queueFamilyIndices {};
    std::vector<VkQueueFamilyProperties> queueFamilyPropertiesList = getQueueFamilyProperties(physicalDevice);

    for (int index = 0; auto& queueFamilyProperties : queueFamilyPropertiesList) {
        if (!queueFamilyIndices.hasGraphicsFamily() && supportsGraphics(queueFamilyProperties)) {
            queueFamilyIndices.graphicsFamily = index;
        }
        if (!queueFamilyIndices.hasPresentFamily() && supportsPresentation(physicalDevice, index, surface)) {
            queueFamilyIndices.presentFamily = index;
        }
        if (queueFamilyIndices.isComplete()) {
            break;
        }
        index++;
    }
    return queueFamilyIndices;
}

VkDeviceQueueCreateInfo QueueFactory::createDeviceQueueCreateInfo(const float* queuePriority, uint32_t queueFamilyIndex) {
    VkDeviceQueueCreateInfo queueCreateInfo {};
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriority;

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    return queueCreateInfo;
}

std::vector<VkDeviceQueueCreateInfo> QueueFactory::createQueueCreateInfos(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    const float* queuePriority
) {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfoList {};
    QueueFamilyIndices queueFamilyIndices = getQueueFamilyIndices(physicalDevice, surface);

    for (uint32_t queueFamilyIndex : queueFamilyIndices.getUniqueQueueIndexSet()) {
        VkDeviceQueueCreateInfo queueCreateInfo = createDeviceQueueCreateInfo(queuePriority, queueFamilyIndex);
        queueCreateInfoList.push_back(queueCreateInfo);
    }
    return queueCreateInfoList;
}