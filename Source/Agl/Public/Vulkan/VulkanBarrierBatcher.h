#pragma once

#include "SizedTypes.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace agl
{
    class VulkanCopyCommandListImpl;

    struct ResourceTransition;
    struct UavBarrier;

    class VulkanBarrierBatcher
    {
    public:
        void AddTransition( const ResourceTransition& transition );
        void AddUavBarrier( const UavBarrier& uavBarrier );

        void Commit( VulkanCopyCommandListImpl& commandList );

        VulkanBarrierBatcher();

    private:
        static constexpr size_t DefaultCapacity = 16;

        std::vector<VkBufferMemoryBarrier2> m_bufferMemoryBarriers;
        std::vector<VkImageMemoryBarrier2> m_imageMemoryBarriers;
    };
}
