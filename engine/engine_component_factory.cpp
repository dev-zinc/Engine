#include "engine_component_factory.h"

#include "engine.h"
#include "pipeline/graphics_pipeline_supports.h"
#include "util/platform.h"
#include "util/validations.h"
#include "queue/queue_factory.h"
#include "shader/render_pass_supports.h"
#include "shader/shaders.h"
#include "swapchain/swapchain_supports.h"
#include "util/binary_file_utils.h"

GLFWwindow *EngineComponentFactory::createWindow() {
    // OpenGL 컨텍스트 생성 방지 (Vulkan 사용 시 필수)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Vulkan!", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    return window;
}

VkApplicationInfo EngineComponentFactory::createApplicationInfo() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    return appInfo;
}

std::vector<const char*> EngineComponentFactory::getRequiredGlfwExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return { glfwExtensions, glfwExtensions + glfwExtensionCount };
}

VkInstanceCreateInfo EngineComponentFactory::createInstanceCreateInfo(VkApplicationInfo appInfo, std::vector<const char*> &extensions) {
    VkInstanceCreateInfo createInfo{};

    // Mac(MoltenVK) 호환성을 위해 필요한 확장 설정
    if constexpr (Platform::isMac) {
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

    createInfo.pApplicationInfo = &appInfo;

    if constexpr (Validations::isValidationLayersEnabled) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(Validations::LAYERS.size());
        createInfo.ppEnabledLayerNames = Validations::LAYERS.data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }
    return createInfo;
}

VkInstance EngineComponentFactory::createVkInstance() {
    VkApplicationInfo appInfo = createApplicationInfo();

    std::vector extensions = getRequiredGlfwExtensions();
    VkInstanceCreateInfo instanceCreateInfo = createInstanceCreateInfo(appInfo, extensions);

    VkInstance instance;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    return instance;
}

VkSwapchainCreateInfoKHR EngineComponentFactory::createSwapchainCreateInfo(
    VkSurfaceKHR surface,
    const SwapchainSupportDetails& swapchainInfo,
    const bool useSameQueueFamily
) {
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    VkSurfaceCapabilitiesKHR capabilities = swapchainInfo.surfaceCapabilities;
    VkSurfaceFormatKHR surfaceFormat = swapchainInfo.getProperSurfaceFormat();
    VkPresentModeKHR presentMode = swapchainInfo.getProperPresentMode();

    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = swapchainInfo.getProperMinImageCount();

    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;

    swapchainCreateInfo.imageExtent = swapchainInfo.getProperExtent();
    swapchainCreateInfo.imageArrayLayers = 1;

    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = useSameQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

    swapchainCreateInfo.preTransform = capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    return swapchainCreateInfo;
}

VkSwapchainKHR EngineComponentFactory::createSwapchain(
    VkDevice device,
    VkSurfaceKHR surface,
    SwapchainSupportDetails &swapchainInfo,
    bool useSameQueueFamily
) {
    VkSwapchainCreateInfoKHR swapchainCreateInfo = createSwapchainCreateInfo(surface, swapchainInfo, useSameQueueFamily);
    VkSwapchainKHR swapchain;

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
        glfwTerminate();
        throw std::runtime_error("failed to create swapchain!");
    }
    return swapchain;
}

std::vector<VkImage> EngineComponentFactory::getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain) {
    uint32_t swapchainImageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);

    std::vector<VkImage> swapchainImages(swapchainImageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());

    return swapchainImages;
}

VkImageViewCreateInfo EngineComponentFactory::createImageViewCreateInfo(VkFormat format, VkImage image) {
    VkImageViewCreateInfo imageViewCreateInfo{};

    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;

    imageViewCreateInfo.components = {
        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
        VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
    };

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    return imageViewCreateInfo;
}

VkImageView EngineComponentFactory::createImageView(VkDevice device, VkFormat format, VkImage image) {
    VkImageViewCreateInfo imageViewCreateInfo = createImageViewCreateInfo(format, image);
    VkImageView imageView;

    if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
        glfwTerminate();
        throw std::runtime_error("failed to create image view!");
    }
    return imageView;
}

VkSurfaceKHR EngineComponentFactory::createSurface(VkInstance instance, GLFWwindow *window) {
    VkSurfaceKHR surface;

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

std::vector<VkPhysicalDevice> EngineComponentFactory::getPhysicalDevices(VkInstance instance) {
    uint32_t physicalDeviceCount{};
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    return physicalDevices;
}

VkPhysicalDevice EngineComponentFactory::getProperPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices, VkSurfaceKHR surface) {
    for (auto &physicalDevice : physicalDevices) {
        if (!SwapchainSupports::supportsSwapchain(physicalDevice)) {
            continue;
        }

        if (
            const auto &[capabilities, surfaceFormats, presentModes] = SwapchainSupports::getSwapchainSupportDetails(physicalDevice, surface);
            presentModes.empty() || surfaceFormats.empty()
        ) {
            continue;
        }

        if (
            QueueFamilyIndices queueFamilyIndices = QueueFactory::getQueueFamilyIndices(physicalDevice, surface);
            queueFamilyIndices.isComplete()
        ) {
            return physicalDevice;
        }
    }
    glfwTerminate();
    throw std::runtime_error("Failed to find physical device!");
}

VkPhysicalDeviceFeatures EngineComponentFactory::createPhysicalDeviceFeatures() {
  VkPhysicalDeviceFeatures physicalDeviceFeatures{};
  // physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
  return physicalDeviceFeatures;
}

std::vector<const char*> EngineComponentFactory::getDeviceExtensions() {
    std::vector deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    if constexpr (Platform::isMac) {
        deviceExtensions.push_back("VK_KHR_portability_subset");
    }
    return deviceExtensions;
}

VkDeviceCreateInfo EngineComponentFactory::createDeviceCreateInfo(
    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList,
    VkPhysicalDeviceFeatures physicalDeviceFeatures,
    const std::vector<const char*>& deviceExtensions
) {
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount = queueCreateInfoList.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if constexpr (Validations::isValidationLayersEnabled) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Validations::LAYERS.size());
        deviceCreateInfo.ppEnabledLayerNames = Validations::LAYERS.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }
    return deviceCreateInfo;
}

VkDevice EngineComponentFactory::createDevice(VkPhysicalDevice physicalDevice, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList) {
    VkPhysicalDeviceFeatures physicalDeviceFeatures = createPhysicalDeviceFeatures();

    std::vector deviceExtensions = getDeviceExtensions();
    VkDeviceCreateInfo deviceCreateInfo = createDeviceCreateInfo(queueCreateInfoList, physicalDeviceFeatures, deviceExtensions);

    VkDevice device;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        glfwTerminate();
        throw std::runtime_error("Failed to create logical device!");
    }
    return device;
}

VkShaderModuleCreateInfo EngineComponentFactory::createShaderModuleCreateInfo(const BinaryFile& binary) {
    VkShaderModuleCreateInfo createInfo {};

    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = binary.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(binary.data());

    return createInfo;
}

VkShaderModule EngineComponentFactory::createShaderModule(VkDevice device, const BinaryFile& binary) {
    VkShaderModuleCreateInfo createInfo = createShaderModuleCreateInfo(binary);
    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    return shaderModule;
}

VkRenderPassCreateInfo EngineComponentFactory::createRenderPassCreateInfo(
    const VkSubpassDependency& subpassDependency,
    const VkAttachmentDescription& attachmentDescription,
    const VkSubpassDescription& subpassDescription
) {
    VkRenderPassCreateInfo renderPassCreateInfo {};

    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pDependencies = &subpassDependency;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pAttachments = &attachmentDescription;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.subpassCount = 1;
    return renderPassCreateInfo;
}

VkRenderPass EngineComponentFactory::createRenderPass(VkDevice device, VkFormat format) {
    VkRenderPass renderPass;

    VkSubpassDependency subpassDependency = RenderPassSupports::createSubpassDependency();
    VkAttachmentDescription attachmentDescription = RenderPassSupports::createAttachmentDescription(format);

    VkAttachmentReference attachmentReference = RenderPassSupports::createAttachmentReference();
    VkSubpassDescription subpassDescription = RenderPassSupports::createSubpassDescription(&attachmentReference);

    VkRenderPassCreateInfo renderPassCreateInfo = createRenderPassCreateInfo(subpassDependency, attachmentDescription, subpassDescription);

    if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
    return renderPass;
}

VkPipelineLayout EngineComponentFactory::createPipelineLayout(VkDevice device) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = GraphicsPipelineSupports::createPipelineLayoutCreateInfo();
    VkPipelineLayout pipelineLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    return pipelineLayout;
}

VkViewport EngineComponentFactory::createViewport(const VkExtent2D& swapchainExtent) {
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapchainExtent.width);
    viewport.height = static_cast<float>(swapchainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

std::vector<VkPipelineShaderStageCreateInfo> EngineComponentFactory::createShaderStages(const ShaderMap& shaderModules) {
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages {};

    shaderStages.reserve(shaderModules.size());

    for (auto& [shaderType, shaderModule] : shaderModules) {
        shaderStages.push_back(GraphicsPipelineSupports::createPipelineShaderStageCreateInfo(
            static_cast<VkShaderStageFlagBits>(shaderType),
            shaderModule
        ));
    }
    return shaderStages;
}

VkGraphicsPipelineCreateInfo EngineComponentFactory::createGraphicsPipelineCreateInfo(
    const std::vector<VkPipelineShaderStageCreateInfo>& shaderStages,
    const VkPipelineViewportStateCreateInfo& viewportState,
    const VkPipelineVertexInputStateCreateInfo& vertexInputState,
    const VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState,
    const VkPipelineRasterizationStateCreateInfo& rasterizationState,
    const VkPipelineMultisampleStateCreateInfo& multisampleState,
    const VkPipelineColorBlendStateCreateInfo& colorBlendState,
    VkPipelineLayout pipelineLayout,
    VkRenderPass renderPass
) {
    VkGraphicsPipelineCreateInfo pipelineInfo {};

    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    return pipelineInfo;
}

VkPipeline EngineComponentFactory::createGraphicsPipeline(
    VkDevice device,
    VkRenderPass renderPass,
    VkPipelineLayout pipelineLayout,
    const ShaderMap& shaderModules,
    const VkExtent2D& swapchainExtent
) {
    auto vertexInputState = GraphicsPipelineSupports::createPipelineVertexInputStateCreateInfo();
    auto inputAssemblyState = GraphicsPipelineSupports::createPipelineInputAssemblyStateCreateInfo();
    auto rasterizationState = GraphicsPipelineSupports::createPipelineRasterizationStateCreateInfo();
    auto multisampleState = GraphicsPipelineSupports::createPipelineMultisampleStateCreateInfo();
    auto colorBlendAttachment = GraphicsPipelineSupports::createPipelineColorBlendAttachmentState();
    auto colorBlendState = GraphicsPipelineSupports::createPipelineColorBlendStateCreateInfo(&colorBlendAttachment);

    VkViewport viewport = createViewport(swapchainExtent);
    VkRect2D scissor { {0, 0}, swapchainExtent };

    auto viewportState = GraphicsPipelineSupports::createPipelineViewportStateCreateInfo(&viewport, &scissor);
    auto shaderStages = createShaderStages(shaderModules);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = createGraphicsPipelineCreateInfo(
        shaderStages,
        viewportState,
        vertexInputState,
        inputAssemblyState,
        rasterizationState,
        multisampleState,
        colorBlendState,
        pipelineLayout,
        renderPass
    );

    constexpr uint32_t createInfoCount = 1;
    VkPipeline graphicsPipeline;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, createInfoCount, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    return graphicsPipeline;
}

VkFramebufferCreateInfo EngineComponentFactory::createFramebufferCreateInfo(
    VkRenderPass renderPass,
    const std::vector<VkImageView> &imageViews,
    const VkExtent2D& swapchainExtent
) {
    VkFramebufferCreateInfo framebufferCreateInfo {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = imageViews.size();
    framebufferCreateInfo.pAttachments = imageViews.data();
    framebufferCreateInfo.width = swapchainExtent.width;
    framebufferCreateInfo.height = swapchainExtent.height;
    framebufferCreateInfo.layers = 1;
    return framebufferCreateInfo;
}

VkFramebuffer EngineComponentFactory::createFramebuffer(
    VkDevice device,
    VkRenderPass renderPass,
    const std::vector<VkImageView> &imageViews,
    const VkExtent2D& swapchainExtent
) {
    VkFramebufferCreateInfo framebufferCreateInfo = createFramebufferCreateInfo(renderPass, imageViews, swapchainExtent);
    VkFramebuffer framebuffer;

    if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create framebuffer!");
    }
    return framebuffer;
}

VkCommandPoolCreateInfo EngineComponentFactory::createCommandPoolCreateInfo(const uint32_t queueFamilyIndex) {
    VkCommandPoolCreateInfo commandPoolCreateInfo {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
    return commandPoolCreateInfo;
}

VkCommandPool EngineComponentFactory::createCommandPool(VkDevice device, const uint32_t queueFamilyIndex) {
    VkCommandPoolCreateInfo commandPoolCreateInfo = createCommandPoolCreateInfo(queueFamilyIndex);
    VkCommandPool commandPool;

    if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
    return commandPool;
}

VkCommandBufferAllocateInfo EngineComponentFactory::createCommandBufferAllocateInfo(VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;
    return commandBufferAllocateInfo;
}

VkCommandBuffer EngineComponentFactory::createCommandBuffer(VkDevice device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = createCommandBufferAllocateInfo(commandPool);

    VkCommandBuffer commandBuffer;

    vkCmdBeginRenderPass(commandBuffer, );
    vkCmdBindPipeline();
    vkCmdDraw();
    vkCmdEndRenderPass();
}
