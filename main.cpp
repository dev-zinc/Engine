#include "engine.h"

#include <GLFW/glfw3.h>

int main() {
    Engine::checkGlfwInit();

    GLFWwindow* window = Engine::createWindow();

    Engine::checkVkExtensions();

    VkInstance instance = Engine::createVkInstance();

    Engine::waitEventsUntilExit(window);
    Engine::destroy(window, instance);
}