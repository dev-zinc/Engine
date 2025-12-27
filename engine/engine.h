#pragma once

#include <utility>
#include <vector>
#include <GLFW/glfw3.h>

namespace EngineLoader {

    void checkGlfwInit();

    void checkValidationLayerSupport();

    void checkVkExtensions();
}

class Engine {

public:
    static Engine createEngine();

    [[nodiscard]]
    GLFWwindow* getWindow() const {
        return m_window;
    }

    [[nodiscard]]
    VkInstance getInstance() const {
        return m_instance;
    }

    [[nodiscard]]
    VkDevice getDevice() const {
        return m_device;
    }

    [[nodiscard]]
    VkSurfaceKHR getSurface() const {
        return m_surface;
    }

    Engine(
        GLFWwindow *window,
        VkInstance instance,
        VkDevice device,
        VkSurfaceKHR surface,
        VkSwapchainKHR swapchain,
        std::vector<VkImageView> imageViews
    ) {
        m_window = window;
        m_instance = instance;
        m_device = device;
        m_surface = surface;
        m_swapchain = swapchain;
        m_imageViews = std::move(imageViews);
    };

    ~Engine();

    void waitEventsUntilExit() const;

private:
    GLFWwindow* m_window;
    VkInstance m_instance;
    VkDevice m_device;
    VkSurfaceKHR m_surface;
    VkSwapchainKHR m_swapchain;
    std::vector<VkImageView> m_imageViews;
};
