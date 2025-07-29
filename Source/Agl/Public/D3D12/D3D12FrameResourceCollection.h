#pragma once

#include "D3D12ResourceAllocator.h"
#include "StackMemoryAllocator.h"

#include <set>

namespace agl
{
    class D3D12FrameResourceCollection
    {
    public:
        void RegisterResource( GraphicsApiResource* resource );
        void RegisterResource( IUnknown* resource );

        void ReleaseResources();

        D3D12FrameResourceCollection();

    private:
        TypedStackAllocator<const ID3D12Resource*> m_allocatedIdentifierAllocator;
        std::set<const ID3D12Resource*, std::less<const ID3D12Resource*>, TypedStackAllocator<const ID3D12Resource*>> m_allocatedIdentifiers;

        TypedStackAllocator<AllocatedResourceInfo> m_allocatedInfoAllocator;
        std::vector<AllocatedResourceInfo, TypedStackAllocator<AllocatedResourceInfo>> m_allocatedInfos;

        std::vector<Microsoft::WRL::ComPtr<IUnknown>, TypedStackAllocator<Microsoft::WRL::ComPtr<IUnknown>>> m_residentResource;
    };
}
