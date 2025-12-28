#include "engine_component_factory.h"

#include <set>

#include "util/platform.h"
#include "util/validations.h"
#include "queue/queue_factory.h"
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
    SwapchainSupportDetails& swapchainInfo,
    bool useSameQueueFamily
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