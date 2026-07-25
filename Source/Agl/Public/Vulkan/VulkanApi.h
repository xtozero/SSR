#pragma once

#include "GuideTypes.h"

#include <vulkan/vulkan.h>

namespace agl
{
    Owner<IAgl*> CreateVulkanGraphicsApi();

    VkInstance VulkanInstance();
    VkPhysicalDevice VulkanPhysicalDevice();
    VkDevice VulkanDevice();
}
