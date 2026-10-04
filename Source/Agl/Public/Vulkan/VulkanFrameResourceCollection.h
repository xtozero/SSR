#pragma once

#include "StackMemoryAllocator.h"

#include <set>

namespace agl
{
    class VulkanFrameResourceCollection final
    {
    public:
        void RegisterResource( const GraphicsApiResource* resource );

        void ReleaseResources();

        VulkanFrameResourceCollection();

    private:
        using ResourceHandle = RefHandle<const GraphicsApiResource>;

        TypedStackAllocator<ResourceHandle> m_resourceAllocator;
        std::set<ResourceHandle, std::less<ResourceHandle>, TypedStackAllocator<ResourceHandle>> m_resources;
    };
}