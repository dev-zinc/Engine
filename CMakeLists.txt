cmake_minimum_required(VERSION 4.0)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    set(VULKAN_SDK_VERSION "1.4.335.1")
    set(VULKAN_SDK_PATH "$ENV{HOME}/VulkanSDK/${VULKAN_SDK_VERSION}/macOS")
    list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK_PATH}")
endif()

add_compile_definitions(GLFW_INCLUDE_VULKAN)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(Engine main.cpp)

target_link_libraries(Engine
        PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)
