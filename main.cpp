#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "platform.h"

int main() {
    // 1. GLFW 초기화
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // OpenGL 컨텍스트 생성 방지 (Vulkan 사용 시 필수)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // 2. 윈도우 생성
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan!", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 3. Vulkan 확장 프로그램 확인
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << "Vulkan extensions supported: " << extensionCount << std::endl;

    // 4. Vulkan Instance 생성 (간단 테스트)
    VkInstance instance;
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Mac(MoltenVK) 호환성을 위해 필요한 확장 설정
    if (Platform::isMac) {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        // macOS Vulkan 1.2+ 사용 시 Portability subset 필수
        requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        // (참고: 인스턴스 생성 시 플래그에도 VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR 추가 필요)

        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // Mac 필수 플래그
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        std::cerr << "failed to create instance!" << std::endl;
        return -1;
    }
    std::cout << "Successfully created Vulkan Instance!" << std::endl;

    // 루프
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // 종료
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}