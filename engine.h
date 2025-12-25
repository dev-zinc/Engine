#pragma once

#include <GLFW/glfw3.h>

namespace EngineLoader {

    void checkGlfwInit();

    GLFWwindow *createWindow();

    void checkVkExtensions();

    VkInstance createVkInstance();
}

class Engine {
public:
    Engine(GLFWwindow *m_window, VkInstance m_instance)
        : m_window(m_window),
          m_instance(m_instance) {};

    ~Engine();

    GLFWwindow* getWindow() const;

    VkInstance getInstance() const;

    void waitEventsUntilExit() const;

private:
    GLFWwindow* m_window;
    VkInstance m_instance;
};

namespace EngineLoader {
    Engine createEngine();
}