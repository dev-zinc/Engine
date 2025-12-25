
cmake_minimum_required(VERSION 4.0)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

if (APPLE)
    set(VULKAN_SDK_VERSION "1.4.335.1")
    set(VULKAN_SDK_PATH "$ENV{HOME}/VulkanSDK/${VULKAN_SDK_VERSION}/macOS")
    list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK_PATH}")
else ()
    set(VULKAN_SDK_VERSION "1.4.335.0")
    set(VULKAN_SDK_PATH "C:/VulkanSDK/${VULKAN_SDK_VERSION}")
    list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK_PATH}")
endif ()

find_package(Vulkan REQUIRED)

if(APPLE)
    find_package(glfw3 REQUIRED)
    find_package(glm REQUIRED)
else ()
    include(FetchContent)

    FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw
            GIT_TAG        3.4
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm
            GIT_TAG        1.0.2
    )
    FetchContent_MakeAvailable(glfw glm)
endif ()

add_executable(Engine main.cpp
        platform.h)

target_compile_definitions(Engine PRIVATE GLFW_INCLUDE_VULKAN)

target_link_libraries(Engine
        PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)