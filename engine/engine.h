#pragma once

#include <map>
#include <utility>
#include <vector>
#include <GLFW/glfw3.h>

#include "shader/shaders.h"

using ShaderMap = std::map<ShaderType, VkShaderModule>;

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

    [[nodiscard]]
    std::vector<VkImageView> getImageViews() const {
        return m_imageViews;
    }

    [[nodiscard]]
    ShaderMap getShaderModules() const {
        return m_shaderModules;
    }

    Engine(
        GLFWwindow *window,
        VkInstance instance,
        VkDevice device,
        VkSurfaceKHR surface,
        VkSwapchainKHR swapchain,
        std::vector<VkImageView> imageViews,
        ShaderMap shaderModules,
        VkRenderPass renderPass
    ) {
        m_window = window;
        m_instance = instance;
        m_device = device;
        m_surface = surface;
        m_swapchain = swapchain;
        m_imageViews = std::move(imageViews);
        m_shaderModules = std::move(shaderModules);
        m_renderPass = renderPass;
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
    ShaderMap m_shaderModules;
    VkRenderPass m_renderPass;
};
