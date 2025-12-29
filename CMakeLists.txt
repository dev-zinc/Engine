
cmake_minimum_required(VERSION 4.0)
project(Engine)

set(CMAKE_CXX_STANDARD 20)

include(cmake/Dependencies.cmake)

add_executable(Engine main.cpp
        engine/util/platform.h
        engine/engine.h
        engine/engine.cpp
        engine/util/validations.h
        engine/util/validations.cpp
        engine/engine_component_factory.cpp
        engine/engine_component_factory.h
        engine/queue/queue_factory.cpp
        engine/queue/queue_factory.h
        engine/swapchain/swapchain_supports.cpp
        engine/swapchain/swapchain_supports.h
        engine/util/binary_file_utils.cpp
        engine/util/binary_file_utils.h
        engine/shader/shaders.h
        engine/shader/render_pass.cpp
        engine/shader/render_pass_supports.h
        engine/pipeline/graphics_pipeline_supports.cpp
        engine/pipeline/graphics_pipeline_supports.h
)

target_compile_definitions(Engine PRIVATE GLFW_INCLUDE_VULKAN)

target_link_libraries(Engine
        PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)

include(cmake/CompileShaders.cmake)
add_dependencies(Engine Shaders)