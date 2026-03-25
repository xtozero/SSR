#include "D3D12FrameResourceCollection.h"

namespace agl
{
    void D3D12FrameResourceCollection::RegisterResource( const GraphicsApiResource* resource )
    {
        if ( resource == nullptr )
        {
            return;
        }

        const AllocatedResourceInfo* resourceInfo = nullptr;

        if ( resource->IsBuffer() )
        {
            auto d3d12Buffer = static_cast<const D3D12Buffer*>( resource );
            resourceInfo = &d3d12Buffer->GetResourceInfo();
        }
        else if ( resource->IsTexture() )
        {
            auto d3d12Texture = static_cast<const D3D12Texture*>( resource );
            resourceInfo = &d3d12Texture->GetResourceInfo();
        }

        if ( ( resourceInfo == nullptr )
            || resourceInfo->IsExternalResource() )
        {
            return;
        }

        const ID3D12Resource* d3d12Resource = resourceInfo->GetResource();
        if ( m_allocatedIdentifiers.contains( d3d12Resource ) == false )
        {
            m_allocatedIdentifiers.insert( d3d12Resource );
            m_allocatedInfos.emplace_back( *resourceInfo );
        }
    }

    void D3D12FrameResourceCollection::RegisterResource( IUnknown* resource )
    {
        m_residentResource.emplace_back( resource );
    }

    void D3D12FrameResourceCollection::ReleaseResources()
    {
        for ( auto& allocatedInfo : m_allocatedInfos )
        {
            allocatedInfo.Release();
        }

        m_allocatedInfos.clear();
        m_allocatedIdentifiers.clear();

        m_allocatedIdentifierAllocator.Flush();
        m_allocatedInfoAllocator.Flush();

        std::construct_at( &m_allocatedIdentifiers, m_allocatedIdentifierAllocator );
        std::construct_at( &m_allocatedInfos, m_allocatedInfoAllocator );

        m_residentResource.clear();
    }

    D3D12FrameResourceCollection::D3D12FrameResourceCollection()
        : m_allocatedIdentifiers( m_allocatedIdentifierAllocator )
        , m_allocatedInfos( m_allocatedInfoAllocator )
    {
    }
}
