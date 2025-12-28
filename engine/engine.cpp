#include "engine.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <ranges>

#include "engine_component_factory.h"
#include "util/validations.h"
#include "queue/queue_factory.h"
#include "util/binary_file_utils.h"

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

    std::vector<VkImageView> imageViews {};
    for (
        VkFormat format = swapchainSupportDetails.getProperSurfaceFormat().format;
        VkImage image : EngineComponentFactory::getSwapchainImages(device, swapchain)
    ) {
        VkImageView imageView = EngineComponentFactory::createImageView(device, format, image);
        imageViews.push_back(imageView);
    }

    ShaderMap shaderModules {};
    for (BinaryFile& binaryFile : BinaryFileUtils::getAllFiles()) {
        ShaderType shaderType = Shaders::getShaderType(binaryFile.fileName);
        VkShaderModule shaderModule = EngineComponentFactory::createShaderModule(device, binaryFile);
        shaderModules[shaderType] = shaderModule;
    }

    return { window, instance, device, surface, swapchain, imageViews, shaderModules };
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
    for (const auto& shaderModule: m_shaderModules | std::views::values) {
        vkDestroyShaderModule(m_device, shaderModule, nullptr);
    }
    for (auto& imageView : m_imageViews) {
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