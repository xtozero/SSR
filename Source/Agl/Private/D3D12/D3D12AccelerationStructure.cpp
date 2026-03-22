#include "D3D12AccelerationStructure.h"

namespace agl
{
    ID3D12Resource* D3D12BLAS::Resource()
    {
        return m_blas->Resource();
    }

    D3D12BLAS::D3D12BLAS( const BLASDesc& desc, const char* debugName )
        : m_desc( desc )
    {
        m_debugName = Name( debugName );
    }

    void D3D12BLAS::InitResource()
    {
        auto vertexBuffer = static_cast<D3D12Buffer*>( m_desc.m_vertexBuffer.Get() );
        auto indexBuffer = static_cast<D3D12Buffer*>( m_desc.m_indexBuffer.Get() );

        D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {
            .Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES,
            .Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE,
            .Triangles = {
                .IndexFormat = indexBuffer ? indexBuffer->GetFormat() : DXGI_FORMAT_UNKNOWN,
                .VertexFormat = vertexBuffer->GetFormat(),
                .IndexCount = indexBuffer ? indexBuffer->GetTrait().m_count : 0,
                .VertexCount = vertexBuffer->GetTrait().m_count,
                .IndexBuffer = indexBuffer ? indexBuffer->Resource()->GetGPUVirtualAddress() : D3D12_GPU_VIRTUAL_ADDRESS(),
                .VertexBuffer = {
                    .StartAddress = vertexBuffer->Resource()->GetGPUVirtualAddress(),
                    .StrideInBytes = vertexBuffer->GetTrait().m_stride,
                },
            }
        };

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS accelerationStructureInputs = {
            .Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL,
            .Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE,
            .NumDescs = 1,
            .DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY,
            .pGeometryDescs = &geometryDesc,
        };

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
        D3D12Device().GetRaytracingAccelerationStructurePrebuildInfo( &accelerationStructureInputs, &prebuildInfo );

        BufferTrait blasBufferTrait = {
            .m_stride = 1,
            .m_count = static_cast<uint32>( prebuildInfo.ResultDataMaxSizeInBytes ),
            .m_access = ResourceAccess::Default,
            .m_bindType = ResourceBindType::RandomAccess,
            .m_miscFlag = ResourceMisc::WithoutViews,
            .m_format = ResourceFormat::Unknown,
        };

        m_blas = RefStaticCast<D3D12Buffer>( Buffer::Create( blasBufferTrait, "BLAS", ResourceState::RaytracingAccelerationStructure ) );
        m_blas->Init();

        BufferTrait scratchBufferTrait = {
            .m_stride = 1,
            .m_count = static_cast<uint32>( prebuildInfo.ScratchDataSizeInBytes ),
            .m_access = ResourceAccess::Default,
            .m_bindType = ResourceBindType::RandomAccess,
            .m_miscFlag = ResourceMisc::WithoutViews,
            .m_format = ResourceFormat::Unknown,
        };

        auto scratchBuffer = RefStaticCast<D3D12Buffer>( Buffer::Create( scratchBufferTrait, "BLAS.Scratch", ResourceState::UnorderedAccess ) );
        scratchBuffer->Init();

        D3D12FrameResources().RegisterResource( m_blas.Get() );
        D3D12FrameResources().RegisterResource( scratchBuffer.Get() );

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {
            .DestAccelerationStructureData = m_blas->Resource()->GetGPUVirtualAddress(),
            .Inputs = accelerationStructureInputs,
            .ScratchAccelerationStructureData = scratchBuffer->Resource()->GetGPUVirtualAddress(),
        };

        auto& d3d12CommandList = static_cast<ID3D12CommandListEX&>( *GetInterface<IAgl>()->GetCommandList() );
        d3d12CommandList.BuildRaytracingAccelerationStructure( buildDesc );

        UavBarrier uavBarrier = {
            .m_pResource = m_blas->Resource()
        };
        d3d12CommandList.AddUavBarrier( uavBarrier );
    }

    void D3D12BLAS::FreeResource()
    {
        m_blas = nullptr;
    }

    Buffer* D3D12TLAS::Resource() const
    {
        return m_tlas.Get();
    }

    D3D12TLAS::D3D12TLAS( const TLASDesc& desc, const char* debugName )
        : m_desc( desc )
    {
        m_debugName = Name( debugName );
    }

    void D3D12TLAS::InitResource()
    {
        auto numInstances = static_cast<uint32>( m_desc.instanceDescs.size() );

        BufferTrait instanceDescBufferTrait = {
            .m_stride = sizeof( D3D12_RAYTRACING_INSTANCE_DESC ),
            .m_count = numInstances,
            .m_access = ResourceAccess::Upload,
            .m_bindType = ResourceBindType::None,
            .m_miscFlag = ResourceMisc::Intermediate,
            .m_format = ResourceFormat::Unknown,
        };

        auto instanceDescBuffer = RefStaticCast<D3D12Buffer>( Buffer::Create( instanceDescBufferTrait, "TLAS.InstanceDesc" ) );
        instanceDescBuffer->Init();

        auto dest = static_cast<D3D12_RAYTRACING_INSTANCE_DESC*>( GetInterface<IAgl>()->Lock( instanceDescBuffer.Get() ).m_data );
        for ( size_t i = 0; i < numInstances; ++i )
        {
            const auto& instanceDesc = m_desc.instanceDescs[i];
            auto d3d12BLAS = RefStaticCast<D3D12BLAS>( instanceDesc.m_blas );

            std::memcpy( &dest[i].Transform, &instanceDesc.m_worldTransform, sizeof( Matrix3X4 ) );
            dest[i].InstanceID = instanceDesc.m_instanceId;
            dest[i].InstanceMask = 0xFF;
            dest[i].InstanceContributionToHitGroupIndex = 0;
            dest[i].Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
            dest[i].AccelerationStructure = d3d12BLAS->Resource()->GetGPUVirtualAddress();
        }

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS accelerationStructureInputs = {
            .Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL,
            .Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE,
            .NumDescs = numInstances,
            .DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY,
            .InstanceDescs = instanceDescBuffer->Resource()->GetGPUVirtualAddress()
        };

        D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
        D3D12Device().GetRaytracingAccelerationStructurePrebuildInfo( &accelerationStructureInputs, &prebuildInfo );

        BufferTrait tlasBufferTrait = {
            .m_stride = 1,
            .m_count = static_cast<uint32>( prebuildInfo.ResultDataMaxSizeInBytes ),
            .m_access = ResourceAccess::Default,
            .m_bindType = ResourceBindType::ShaderResource | ResourceBindType::RandomAccess,
            .m_miscFlag = ResourceMisc::WithoutViews | ResourceMisc::RaytracingAccelerationStructure,
            .m_format = ResourceFormat::Unknown,
        };

        m_tlas = RefStaticCast<D3D12Buffer>( Buffer::Create( tlasBufferTrait, "TLAS", ResourceState::RaytracingAccelerationStructure ) );
        m_tlas->Init();
        m_tlas->CreateShaderResource();

        BufferTrait scratchBufferTrait = {
            .m_stride = 1,
            .m_count = static_cast<uint32>( prebuildInfo.ScratchDataSizeInBytes ),
            .m_access = ResourceAccess::Default,
            .m_bindType = ResourceBindType::RandomAccess,
            .m_miscFlag = ResourceMisc::WithoutViews,
            .m_format = ResourceFormat::Unknown,
        };

        auto scratchBuffer = RefStaticCast<D3D12Buffer>( Buffer::Create( scratchBufferTrait, "TLAS.Scratch" ) );
        scratchBuffer->Init();

        D3D12FrameResources().RegisterResource( instanceDescBuffer.Get() );
        D3D12FrameResources().RegisterResource( m_tlas.Get() );
        D3D12FrameResources().RegisterResource( scratchBuffer.Get() );

        D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {
            .DestAccelerationStructureData = m_tlas->Resource()->GetGPUVirtualAddress(),
            .Inputs = accelerationStructureInputs,
            .ScratchAccelerationStructureData = scratchBuffer->Resource()->GetGPUVirtualAddress(),
        };

        auto& d3d12CommandList = static_cast<ID3D12CommandListEX&>( *GetInterface<IAgl>()->GetCommandList() );
        d3d12CommandList.BuildRaytracingAccelerationStructure( buildDesc );

        UavBarrier uavBarrier = {
            .m_pResource = m_tlas->Resource()
        };
        d3d12CommandList.AddUavBarrier( uavBarrier );
    }

    void D3D12TLAS::FreeResource()
    {
        m_tlas = nullptr;
    }
}
