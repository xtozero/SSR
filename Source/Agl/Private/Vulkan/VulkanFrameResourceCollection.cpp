#include "VulkanFrameResourceCollection.h"

namespace agl
{
    void VulkanFrameResourceCollection::RegisterResource( const GraphicsApiResource* resource )
    {
        if ( resource == nullptr )
        {
            return;
        }

        m_resources.emplace( resource );
    }

    void VulkanFrameResourceCollection::ReleaseResources()
    {
        m_resources.clear();

        ResetStackContainerScope resetAllocatedInfos( m_resources, m_resourceAllocator );

        m_resourceAllocator.Flush();
    }

    VulkanFrameResourceCollection::VulkanFrameResourceCollection()
        : m_resources( m_resourceAllocator )
    {
    }
}
