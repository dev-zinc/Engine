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
    QueueFamilyIndices queueFamilyIndices = QueueFactory::getQueueFamilyIndices(physicalDevice, surface);

    constexpr float queuePriority = 1.0f;
    std::vector queueCreateInfos = QueueFactory::createQueueCreateInfos(physicalDevice, surface, &queuePriority);
    VkDevice device = EngineComponentFactory::createDevice(physicalDevice, queueCreateInfos);

    SwapchainSupportDetails swapchainSupportDetails = SwapchainSupports::getSwapchainSupportDetails(physicalDevice, surface);
    VkSwapchainKHR swapchain = EngineComponentFactory::createSwapchain(device, surface, swapchainSupportDetails, queueCreateInfos.size() == 1);
    VkSurfaceFormatKHR surfaceFormat = swapchainSupportDetails.getProperSurfaceFormat();
    VkFormat swapchainImageFormat = surfaceFormat.format;

    std::vector<VkImageView> imageViews = EngineLoader::getImageViews(device, swapchain, swapchainImageFormat);
    ShaderMap shaderModules = EngineLoader::getShaderModules(device);

    VkRenderPass renderPass = EngineComponentFactory::createRenderPass(device, swapchainImageFormat);
    VkPipelineLayout pipelineLayout = EngineComponentFactory::createPipelineLayout(device);
    VkExtent2D swapchainExtent = swapchainSupportDetails.getProperExtent();

    VkPipeline graphicsPipeline = EngineComponentFactory::createGraphicsPipeline(device, renderPass, pipelineLayout, shaderModules, swapchainExtent);

    VkFramebuffer framebuffer = EngineComponentFactory::createFramebuffer(device, renderPass, imageViews, swapchainExtent);
    VkCommandPool commandPool = EngineComponentFactory::createCommandPool(device, queueFamilyIndices.graphicsFamily.value());

    return { window, instance, device, surface, swapchain, imageViews, shaderModules, renderPass, pipelineLayout, graphicsPipeline };
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

        const bool isAvailable = std::ranges::all_of(
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

std::vector<VkImageView> EngineLoader::getImageViews(VkDevice device, VkSwapchainKHR swapchain, VkFormat swapchainImageFormat) {
    std::vector<VkImageView> imageViews {};

    for (VkImage image : EngineComponentFactory::getSwapchainImages(device, swapchain)) {
        VkImageView imageView = EngineComponentFactory::createImageView(device, swapchainImageFormat, image);
        imageViews.push_back(imageView);
    }
    return imageViews;
}

ShaderMap EngineLoader::getShaderModules(VkDevice device) {
    ShaderMap shaderModules {};

    for (BinaryFile& binaryFile : BinaryFileUtils::getAllFiles()) {
        ShaderType shaderType = Shaders::getShaderType(binaryFile.fileName);
        VkShaderModule shaderModule = EngineComponentFactory::createShaderModule(device, binaryFile);
        shaderModules[shaderType] = shaderModule;
    }
    return shaderModules;
}

Engine::~Engine() {
    // Destroy Pipeline
    vkDestroyPipeline(m_device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(m_device, m_renderPass, nullptr);

    // Destroy Shader
    for (const auto& shaderModule: m_shaderModules | std::views::values) {
        vkDestroyShaderModule(m_device, shaderModule, nullptr);
    }

    // Destroy Swapchain
    for (auto& imageView : m_imageViews) {
        vkDestroyImageView(m_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    // Destroy Device, Surface, Instance
    vkDestroyDevice(m_device, nullptr);
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);

    // Destroy Window
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Engine::waitEventsUntilExit() const {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
    }
}