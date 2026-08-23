#pragma once

#include "GuideTypes.h"
#include "ICommandList.h"

#include <vulkan/vulkan.h>

namespace agl
{
    class VulkanCommandListResourcePool;

    class VulkanFrameSyncContext
    {
    public:
        bool Initialize();
        void Destroy();

        VkSemaphore m_imageAvailable = VK_NULL_HANDLE;
        VkSemaphore m_renderFinished = VK_NULL_HANDLE;
        VkFence m_frameComplete = VK_NULL_HANDLE;
    };

    Owner<IAgl*> CreateVulkanGraphicsApi();

    VkInstance VulkanInstance();
    VkPhysicalDevice VulkanPhysicalDevice();
    VkDevice VulkanDevice();

    VkQueue VulkanGraphicsQueue();
    VkQueue VulkanPresentQueue();
    VkQueue VulkanComputeQueue();

    VulkanCommandListResourcePool& VulkanCmdPool( CommandListType type );
    VulkanFrameSyncContext VulkanFrameSync();
}
