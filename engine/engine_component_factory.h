#pragma once

#include <vector>
#include <GLFW/glfw3.h>

#include "engine.h"
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
    VkSwapchainCreateInfoKHR createSwapchainCreateInfo(VkSurfaceKHR surface, const SwapchainSupportDetails& swapchainInfo, bool useSameQueueFamily);
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

    VkDevice createDevice(VkPhysicalDevice physicalDevice, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList);

    // Create Shaders
    VkShaderModuleCreateInfo createShaderModuleCreateInfo(const BinaryFile& binary);
    VkShaderModule createShaderModule(VkDevice device, const BinaryFile& binary);

    // Create Render Pass
    VkRenderPassCreateInfo createRenderPassCreateInfo(
        const VkSubpassDependency& subpassDependency,
        const VkAttachmentDescription& attachmentDescription,
        const VkSubpassDescription& subpassDescription
    );
    VkRenderPass createRenderPass(VkDevice device, VkFormat swapchainImageFormat);

    // Create Pipeline
    VkPipelineLayout createPipelineLayout(VkDevice device);
    VkViewport createViewport(const VkExtent2D& swapchainExtent);
    std::vector<VkPipelineShaderStageCreateInfo> createShaderStages(const ShaderMap& shaderModules);

    VkGraphicsPipelineCreateInfo createGraphicsPipelineCreateInfo(
        const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
        const VkPipelineViewportStateCreateInfo& viewportState,
        const VkPipelineVertexInputStateCreateInfo& vertexInputState,
        const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState,
        const VkPipelineRasterizationStateCreateInfo& rasterizationState,
        const VkPipelineMultisampleStateCreateInfo& multisampleState,
        const VkPipelineColorBlendStateCreateInfo& colorBlendState,
        VkPipelineLayout pipelineLayout,
        VkRenderPass renderPass
    );

    VkPipeline createGraphicsPipeline(
        VkDevice device,
        VkRenderPass renderPass,
        VkPipelineLayout pipelineLayout,
        const ShaderMap& shaderModules,
        const VkExtent2D& swapchainExtent
    );

    // Create Framebuffer
    VkFramebufferCreateInfo createFramebufferCreateInfo(
        VkRenderPass renderPass,
        const std::vector<VkImageView>& imageViews,
        const VkExtent2D& swapchainExtent
    );

    VkFramebuffer createFramebuffer(
        VkDevice device,
        VkRenderPass renderPass,
        const std::vector<VkImageView> &imageViews,
        const VkExtent2D& swapchainExtent
    );

    // Create Command Pool
    VkCommandPoolCreateInfo createCommandPoolCreateInfo(uint32_t queueFamilyIndex);
    VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamilyIndex);

    // Create Command Buffers
    VkCommandBufferAllocateInfo createCommandBufferAllocateInfo(VkCommandPool commandPool);
    VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool commandPool);
}
