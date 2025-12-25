#pragma once

#include <GLFW/glfw3.h>

namespace Engine {

    void checkGlfwInit();

    GLFWwindow *createWindow();

    void checkVkExtensions();

    VkInstance createVkInstance();

    void waitEventsUntilExit(GLFWwindow* window);

    void destroy(GLFWwindow *window, VkInstance instance);
}
