#pragma once

#include <vulkan/vulkan_core.h>

namespace RenderPassSupports {

    VkAttachmentDescription createAttachmentDescription(VkFormat format);
    VkAttachmentReference createAttachmentReference();
    VkSubpassDescription createSubpassDescription(const VkAttachmentReference *attachmentReference);
    VkSubpassDependency createSubpassDependency();
}
