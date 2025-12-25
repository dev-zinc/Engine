#include "engine_component_factory.h"

#include <set>

#include "../utils/platform.h"
#include "../utils/validations.h"
#include "queue/queue_factory.h"

GLFWwindow* EngineComponentFactory::createWindow() {
    // OpenGL 컨텍스트 생성 방지 (Vulkan 사용 시 필수)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan!", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    return window;
}

VkApplicationInfo EngineComponentFactory::createApplicationInfo() {
    VkApplicationInfo appInfo {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    return appInfo;
}

std::vector<const char*> EngineComponentFactory::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return { glfwExtensions, glfwExtensions + glfwExtensionCount };
}

VkInstanceCreateInfo EngineComponentFactory::createVkInstanceCreateInfo(const VkApplicationInfo& appInfo, std::vector<const char*>& extensions) {
    VkInstanceCreateInfo createInfo {};

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

    std::vector extensions = getRequiredExtensions();
    VkInstanceCreateInfo instanceCreateInfo = createVkInstanceCreateInfo(appInfo, extensions);

    VkInstance instance;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    return instance;
}

VkSurfaceKHR EngineComponentFactory::createSurface(VkInstance& instance, GLFWwindow* window) {
    VkSurfaceKHR surface;

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

std::vector<VkPhysicalDevice> EngineComponentFactory::getPhysicalDevices(VkInstance& instance) {
    uint32_t physicalDeviceCount {};
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    return physicalDevices;
}

VkPhysicalDevice EngineComponentFactory::getProperPhysicalDevice(std::vector<VkPhysicalDevice>& physicalDevices, VkSurfaceKHR& surface) {
    for (auto& physicalDevice : physicalDevices) {
        QueueFamilyIndices queueFamilyIndices = QueueFactory::getQueueFamilyIndices(physicalDevice, surface);

        if (queueFamilyIndices.isComplete()) {
            return physicalDevice;
        }
    }
    glfwTerminate();
    throw std::runtime_error("Failed to find physical device!");
}

VkPhysicalDeviceFeatures EngineComponentFactory::createPhysicalDeviceFeatures() {
    VkPhysicalDeviceFeatures physicalDeviceFeatures {};
    // physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
    return physicalDeviceFeatures;
}

std::vector<const char*> EngineComponentFactory::getDeviceExtensions() {
    std::vector<const char*> deviceExtensions {};

    if constexpr (Platform::isMac) {
        deviceExtensions.push_back("VK_KHR_portability_subset");
    }
    return deviceExtensions;
}

VkDeviceCreateInfo EngineComponentFactory::createDeviceCreateInfo(
    const std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList,
    const VkPhysicalDeviceFeatures& physicalDeviceFeatures,
    const std::vector<const char*>& deviceExtensions
) {
    VkDeviceCreateInfo deviceCreateInfo {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount = queueCreateInfoList.size();
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfoList.data();
    deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (Validations::isValidationLayersEnabled) {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Validations::LAYERS.size());
        deviceCreateInfo.ppEnabledLayerNames = Validations::LAYERS.data();
    } else {
        deviceCreateInfo.enabledLayerCount = 0;
    }
    return deviceCreateInfo;
}

VkDevice EngineComponentFactory::createDevice(VkPhysicalDevice& physicalDevice, std::vector<VkDeviceQueueCreateInfo>& queueCreateInfoList) {
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