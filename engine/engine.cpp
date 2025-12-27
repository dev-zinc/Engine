#include "engine.h"

#include <GLFW/glfw3.h>
#include <iostream>

#include "engine_component_factory.h"
#include "../utils/validations.h"
#include "queue/queue_factory.h"

Engine Engine::createEngine() {
    EngineLoader::checkGlfwInit();

    GLFWwindow* window = EngineComponentFactory::createWindow();

    EngineLoader::checkValidationLayerSupport();
    EngineLoader::checkVkExtensions();

    VkInstance instance = EngineComponentFactory::createVkInstance();
    VkSurfaceKHR surface = EngineComponentFactory::createSurface(instance, window);

    std::vector physicalDevices = EngineComponentFactory::getPhysicalDevices(instance);
    VkPhysicalDevice physicalDevice = EngineComponentFactory::getProperPhysicalDevice(physicalDevices, surface);

    constexpr float queuePriority = 1.0f;
    std::vector queueCreateInfos = QueueFactory::createQueueCreateInfos(physicalDevice, surface, &queuePriority);
    VkDevice device = EngineComponentFactory::createDevice(physicalDevice, queueCreateInfos);

    SwapchainSupportDetails swapchainSupportDetails = SwapchainSupports::getSwapchainSupportDetails(physicalDevice, surface);
    VkSwapchainKHR swapchain = EngineComponentFactory::createSwapchain(device, surface, swapchainSupportDetails, queueCreateInfos.size() == 1);

    std::vector<VkImage> images = EngineComponentFactory::getSwapchainImages(device, swapchain);
    std::vector<VkImageView> imageViews {};

    for (
        VkFormat format = swapchainSupportDetails.getProperSurfaceFormat().format;
        VkImage image : images
    ) {
        VkImageView imageView = EngineComponentFactory::createImageView(device, format, image);
        imageViews.push_back(imageView);
    }

    return { window, instance, device, surface, swapchain, imageViews };
}


void EngineLoader::checkGlfwInit() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
}

void EngineLoader::checkValidationLayerSupport() {
    if constexpr (Validations::isValidationLayersEnabled) {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        bool isAvailable = std::ranges::all_of(
            Validations::LAYERS,
            [&](const char* layerName) {
                return Validations::isAvailable(availableLayers, layerName);
            }
        );

        if (!isAvailable) {
            glfwTerminate();
            throw std::runtime_error("Failed to find validation layer support");
        }
    }
}

void EngineLoader::checkVkExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << "Vulkan extensions supported: " << extensionCount << std::endl;
}


Engine::~Engine() {
    for (auto imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Engine::waitEventsUntilExit() const {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
    }
}