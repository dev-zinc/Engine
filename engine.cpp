#include "engine.h"

#include "platform.h"
#include <GLFW/glfw3.h>
#include <iostream>

Engine EngineLoader::createEngine() {
    checkGlfwInit();
    GLFWwindow* window = createWindow();
    checkVkExtensions();
    VkInstance instance = createVkInstance();

    return { window, instance };
}

void EngineLoader::checkGlfwInit() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
}

GLFWwindow* EngineLoader::createWindow() {
    // OpenGL 컨텍스트 생성 방지 (Vulkan 사용 시 필수)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan!", nullptr, nullptr);

    if (window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    return window;
}

void EngineLoader::checkVkExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << "Vulkan extensions supported: " << extensionCount << std::endl;
}

void createApplicationInfo(VkApplicationInfo& appInfo) {
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
}

void createVkInstanceCreateInfo(VkInstanceCreateInfo& createInfo, std::vector<const char*>& extensions) {
    // Mac(MoltenVK) 호환성을 위해 필요한 확장 설정
    if (Platform::isMac) {
        std::cout << "Vulkan extensions supported on MacOS!" << std::endl;
        // macOS Vulkan 1.2+ 사용 시 Portability subset 필수
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
}

VkInstance EngineLoader::createVkInstance() {
    VkApplicationInfo appInfo {};
    createApplicationInfo(appInfo);

    // GLFW 필수 확장 가져오기
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    VkInstanceCreateInfo createInfo {};
    createInfo.pApplicationInfo = &appInfo;
    createVkInstanceCreateInfo(createInfo, extensions);

    VkInstance instance;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    return instance;
}


Engine::~Engine() {
    vkDestroyInstance(m_instance, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

GLFWwindow* Engine::getWindow() const {
    return m_window;
}

VkInstance Engine::getInstance() const {
    return m_instance;
}

void Engine::waitEventsUntilExit() const {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
    }
}