#pragma once

#include <vulkan/vulkan.h>

namespace GraphicsPipelineSupports {

    VkPipelineShaderStageCreateInfo createPipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shaderModule);
    VkPipelineVertexInputStateCreateInfo createPipelineVertexInputStateCreateInfo();
    VkPipelineInputAssemblyStateCreateInfo createPipelineInputAssemblyStateCreateInfo();
    VkPipelineViewportStateCreateInfo createPipelineViewportStateCreateInfo(const VkViewport *viewport, const VkRect2D *scissor);
    VkPipelineRasterizationStateCreateInfo createPipelineRasterizationStateCreateInfo();
    VkPipelineMultisampleStateCreateInfo createPipelineMultisampleStateCreateInfo();
    VkPipelineColorBlendAttachmentState createPipelineColorBlendAttachmentState();
    VkPipelineColorBlendStateCreateInfo createPipelineColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState *colorBlendAttachment);
    VkPipelineLayoutCreateInfo createPipelineLayoutCreateInfo();
}
