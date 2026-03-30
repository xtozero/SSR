#include "D3D12RaytracingShaderTable.h"

#include "StringUtility.h"

namespace agl
{
    D3D12HitGroup::D3D12HitGroup( const HitGroupDesc& desc )
        : m_intersection( static_cast<D3D12IntersectionShader*>( desc.m_intersection ) )
        , m_anyHit( static_cast<D3D12AnyHitShader*>( desc.m_anyHit ) )
        , m_closestHit( static_cast<D3D12ClosestHitShader*>( desc.m_closestHit ) )
    {
        SetHash( desc.GetHash() );
    }

    D3D12IntersectionShader* D3D12HitGroup::GetIntersection() const
    {
        return m_intersection.Get();
    }

    D3D12AnyHitShader* D3D12HitGroup::GetAnyHit() const
    {
        return m_anyHit.Get();
    }

    D3D12ClosestHitShader* D3D12HitGroup::GetClosestHit() const
    {
        return m_closestHit.Get();
    }

    const wchar_t* D3D12HitGroup::GetExportName() const
    {
        return m_wExportName;
    }

    void D3D12HitGroup::InitResource()
    {
        char exportName[RaytracingShader::ExportNameBufferSize] = {};
        SPrintf( exportName, RaytracingShader::ExportNameBufferSize, "HitGroup_%llu", GetHash() );
        ToWideChar( m_wExportName, RaytracingShader::ExportNameBufferSize, exportName );
    }

    void D3D12HitGroup::FreeResource()
    {
        m_intersection = nullptr;
        m_anyHit = nullptr;
        m_closestHit = nullptr;
    }

    void D3D12RaytracingShaderTable::AddToStateSubobjects( D3D12StateSubobjects& outSubobjects ) const
    {
        outSubobjects.AddDXIL( m_rayGeneration->ByteCode(), m_rayGeneration->ByteCodeSize(), m_rayGeneration->GetExportName() );

        for ( const auto& hitGroup : m_hitGroups )
        {
            if ( D3D12IntersectionShader* intersection = hitGroup->GetIntersection() )
            {
                outSubobjects.AddDXIL( intersection->ByteCode(), intersection->ByteCodeSize(), intersection->GetExportName() );
            }

            if ( D3D12AnyHitShader* anyHit = hitGroup->GetAnyHit() )
            {
                outSubobjects.AddDXIL( anyHit->ByteCode(), anyHit->ByteCodeSize(), anyHit->GetExportName() );
            }

            if ( D3D12ClosestHitShader* closestHit = hitGroup->GetClosestHit() )
            {
                outSubobjects.AddDXIL( closestHit->ByteCode(), closestHit->ByteCodeSize(), closestHit->GetExportName() );
            }
        }

        for ( const auto& miss : m_misses )
        {
            outSubobjects.AddDXIL( miss->ByteCode(), miss->ByteCodeSize(), miss->GetExportName() );
        }

        for ( const auto& hitGroup : m_hitGroups )
        {
            outSubobjects.AddHitGroup( *hitGroup.Get() );
        }

        outSubobjects.AddGlobalRootSignature( *m_rootSignature.Get() );
    }

    void D3D12RaytracingShaderTable::WriteShaderRecords( ID3D12StateObjectProperties& properties ) const
    {
        LockedResource lockedResource = m_shaderRecords->Lock( 0, ResourceLockFlag::WriteDiscard );
        assert( lockedResource.m_data );

        auto lockedData = static_cast<uint8*>( lockedResource.m_data );

        {
            void* identifier = properties.GetShaderIdentifier( m_rayGeneration->GetExportName() );
            std::memcpy( lockedData, identifier, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES );
        }

        for ( const auto& hitGroup : m_hitGroups )
        {
            lockedData += ShaderRecordSize;

            void* identifier = properties.GetShaderIdentifier( hitGroup->GetExportName() );
            std::memcpy( lockedData, identifier, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES );
        }

        for ( const auto& miss : m_misses )
        {
            lockedData += ShaderRecordSize;

            void* identifier = properties.GetShaderIdentifier( miss->GetExportName() );
            std::memcpy( lockedData, identifier, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES );
        }

        m_shaderRecords->UnLock();
    }

    const ShaderParameterInfo& D3D12RaytracingShaderTable::GetParameterInfo() const
    {
        return m_shaderParameterInfo;
    }

    D3D12_DISPATCH_RAYS_DESC D3D12RaytracingShaderTable::GetDispatchRaysDesc( uint32 width, uint32 height, uint32 depth ) const
    {
        D3D12_GPU_VIRTUAL_ADDRESS shaderRecordAddress = m_shaderRecords->Resource()->GetGPUVirtualAddress();
        D3D12_GPU_VIRTUAL_ADDRESS hitShaderRecordStart = shaderRecordAddress + ShaderRecordSize;
        D3D12_GPU_VIRTUAL_ADDRESS missShaderRecordStart = hitShaderRecordStart + ShaderRecordSize * m_hitGroups.size();

        return D3D12_DISPATCH_RAYS_DESC{
            .RayGenerationShaderRecord = {
                .StartAddress = shaderRecordAddress,
                .SizeInBytes = ShaderRecordSize
            },
            .MissShaderTable = {
                .StartAddress = missShaderRecordStart,
                .SizeInBytes = ShaderRecordSize * m_misses.size(),
                .StrideInBytes = ShaderRecordSize,
            },
            .HitGroupTable = {
                .StartAddress = hitShaderRecordStart,
                .SizeInBytes = ShaderRecordSize * m_hitGroups.size(),
                .StrideInBytes = ShaderRecordSize,
            },
            .CallableShaderTable = {},
            .Width = width,
            .Height = height,
            .Depth = depth,
        };
    }

    D3D12RootSignature* D3D12RaytracingShaderTable::GetRootSignature() const
    {
        return m_rootSignature.Get();
    }

    D3D12RaytracingShaderTable::D3D12RaytracingShaderTable( const RaytracingShaderTableDesc& desc )
        : m_rayGeneration( static_cast<D3D12RayGenerationShader*>( desc.m_rayGeneration ) )
    {
        auto& d3d12ResourceManager = *static_cast<D3D12ResourceManager*>( GetInterface<IResourceManager>() );

        m_hitGroups.reserve( desc.m_hitGroupDescs.size() );
        for ( const auto& hitGroupDesc : desc.m_hitGroupDescs )
        {
            m_hitGroups.emplace_back( d3d12ResourceManager.CreateHitGroup( hitGroupDesc ) );
        }

        m_misses.reserve( desc.m_misses.size() );
        for ( const auto& miss : desc.m_misses )
        {
            m_misses.emplace_back( static_cast<D3D12MissShader*>( miss ) );
        }

        MergeShaderParameterInfo();

        m_rootSignature = new D3D12RootSignature( *this );
    }

    void D3D12RaytracingShaderTable::InitResource()
    {
        m_rootSignature->Init();

        auto shaderRecordCount = static_cast<uint32>( 1/*RayGen*/ + m_hitGroups.size() + m_misses.size() );

        BufferTrait trait = {
            .m_stride = ShaderRecordSize,
            .m_count = shaderRecordCount,
            .m_access = ResourceAccess::Upload,
            .m_bindType = ResourceBindType::None,
            .m_miscFlag = ResourceMisc::None,
            .m_format = ResourceFormat::Unknown
        };

        m_shaderRecords = RefStaticCast<D3D12Buffer>( Buffer::Create( trait, "ShaderRecords", ResourceState::NonPixelShaderResource ) );
    }

    void D3D12RaytracingShaderTable::FreeResource()
    {
        m_rayGeneration = nullptr;
        m_hitGroups.clear();
        m_misses.clear();
        m_shaderParameterInfo.Clear();
        m_rootSignature = nullptr;
        m_shaderRecords = nullptr;
    }

    void D3D12RaytracingShaderTable::MergeShaderParameterInfo()
    {
        std::set<ShaderParameter> shaderParameterSet;

        if ( m_rayGeneration.Get() )
        {
            m_rayGeneration->GetParameterInfo().Merge( shaderParameterSet );
        }

        for ( const RefHandle<D3D12HitGroup>& hitGroup : m_hitGroups )
        {
            if ( hitGroup.Get() == nullptr )
            {
                continue;
            }

            if ( const D3D12IntersectionShader* intersection = hitGroup->GetIntersection() )
            {
                intersection->GetParameterInfo().Merge( shaderParameterSet );
            }

            if ( const D3D12AnyHitShader* anyHit = hitGroup->GetAnyHit() )
            {
                anyHit->GetParameterInfo().Merge( shaderParameterSet );
            }

            if ( const D3D12ClosestHitShader* closestHit = hitGroup->GetClosestHit() )
            {
                closestHit->GetParameterInfo().Merge( shaderParameterSet );
            }
        }

        for ( const RefHandle<D3D12MissShader>& miss : m_misses )
        {
            if ( miss.Get() )
            {
                miss->GetParameterInfo().Merge( shaderParameterSet );
            }
        }

        for ( const auto& shaderParameter : shaderParameterSet )
        {
            if ( shaderParameter.m_type == ShaderParameterType::ConstantBuffer )
            {
                m_shaderParameterInfo.m_constantBuffers.emplace_back( shaderParameter );
            }
            else if ( shaderParameter.m_type == ShaderParameterType::SRV )
            {
                m_shaderParameterInfo.m_srvs.emplace_back( shaderParameter );
            }
            else if ( shaderParameter.m_type == ShaderParameterType::UAV )
            {
                m_shaderParameterInfo.m_uavs.emplace_back( shaderParameter );
            }
            else if ( shaderParameter.m_type == ShaderParameterType::Sampler )
            {
                m_shaderParameterInfo.m_samplers.emplace_back( shaderParameter );
            }
            else if ( shaderParameter.m_type == ShaderParameterType::Bindless )
            {
                m_shaderParameterInfo.m_bindless.emplace_back( shaderParameter );
            }
        }
    }
}
