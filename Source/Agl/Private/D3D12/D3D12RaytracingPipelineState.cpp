#include "D3D12RaytracingPipelineState.h"

#include "TransientAllocator.h"

namespace agl
{
    void D3D12StateSubobjects::AddDXIL( const void* byteCode, size_t byteCodeSize, const wchar_t* exportName )
    {
        auto& desc = AllocDesc<D3D12_DXIL_LIBRARY_DESC>();
        desc.DXILLibrary.pShaderBytecode = byteCode;
        desc.DXILLibrary.BytecodeLength = byteCodeSize;
        desc.NumExports = 1;

        auto& exportDesc = AllocDesc<D3D12_EXPORT_DESC>();
        exportDesc.Name = exportName;
        exportDesc.ExportToRename = nullptr;
        exportDesc.Flags = D3D12_EXPORT_FLAG_NONE;

        desc.pExports = &exportDesc;

        D3D12_STATE_SUBOBJECT& subobject = m_subobjects.emplace_back();
        subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        subobject.pDesc = &desc;
    }

    void D3D12StateSubobjects::AddHitGroup( const D3D12HitGroup& hitGroup )
    {
        auto& desc = AllocDesc<D3D12_HIT_GROUP_DESC>();
        desc = {};
        desc.HitGroupExport = hitGroup.GetExportName();
        desc.Type = D3D12_HIT_GROUP_TYPE_TRIANGLES;

        if ( D3D12IntersectionShader* intersection = hitGroup.GetIntersection() )
        {
            desc.IntersectionShaderImport = intersection->GetExportName();
        }

        if ( D3D12ClosestHitShader* closestHit = hitGroup.GetClosestHit() )
        {
            desc.ClosestHitShaderImport = closestHit->GetExportName();
        }

        if ( D3D12AnyHitShader* anyHit = hitGroup.GetAnyHit() )
        {
            desc.AnyHitShaderImport = anyHit->GetExportName();
        }

        D3D12_STATE_SUBOBJECT& subobject = m_subobjects.emplace_back();
        subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
        subobject.pDesc = &desc;
    }

    void D3D12StateSubobjects::AddShaderConfig( uint32 maxPayloadSizeInBytes, uint32 maxAttributeSizeInBytes )
    {
        auto& desc = AllocDesc<D3D12_RAYTRACING_SHADER_CONFIG>();
        desc.MaxPayloadSizeInBytes = maxPayloadSizeInBytes;
        desc.MaxAttributeSizeInBytes = maxAttributeSizeInBytes;

        D3D12_STATE_SUBOBJECT& subobject = m_subobjects.emplace_back();
        subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        subobject.pDesc = &desc;
    }

    void D3D12StateSubobjects::AddPipelineConfig( uint32 maxTraceRecursionDepth )
    {
        auto& desc = AllocDesc<D3D12_RAYTRACING_PIPELINE_CONFIG>();
        desc.MaxTraceRecursionDepth = maxTraceRecursionDepth;

        D3D12_STATE_SUBOBJECT& subobject = m_subobjects.emplace_back();
        subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
        subobject.pDesc = &desc;
    }

    void D3D12StateSubobjects::AddGlobalRootSignature( const D3D12RootSignature& rootSignature )
    {
        auto& desc = AllocDesc<D3D12_GLOBAL_ROOT_SIGNATURE>();
        desc.pGlobalRootSignature = rootSignature.Resource();

        D3D12_STATE_SUBOBJECT& subobject = m_subobjects.emplace_back();
        subobject.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
        subobject.pDesc = &desc;
    }

    D3D12_STATE_OBJECT_DESC D3D12StateSubobjects::GetSubobjectDesc()
    {
        D3D12_STATE_OBJECT_DESC subobjectDesc = {
            .Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE,
            .NumSubobjects = static_cast<uint32>( m_subobjects.size() ),
            .pSubobjects = m_subobjects.data(),
        };

        return subobjectDesc;
    }

    wchar_t* D3D12StateSubobjects::AllocWCharBuffer( uint32 size )
    {
        return reinterpret_cast<wchar_t*>( m_transientAllocator.allocate( sizeof( wchar_t ) * size ) );
    }

    const ShaderParameterInfo& D3D12RaytracingPipelineState::ParameterInfo() const
    {
        return m_shaderTable->GetParameterInfo();
    }

    D3D12_DISPATCH_RAYS_DESC D3D12RaytracingPipelineState::GetDispatchRaysDesc( uint32 width, uint32 height, uint32 depth ) const
    {
        return m_shaderTable->GetDispatchRaysDesc( width, height, depth );
    }

    D3D12RootSignature* D3D12RaytracingPipelineState::GetRootSignature() const
    {
        return m_shaderTable->GetRootSignature();
    }

    ID3D12StateObject* D3D12RaytracingPipelineState::GetStateObject() const
    {
        return m_stateObject.Get();
    }

    D3D12RaytracingPipelineState::D3D12RaytracingPipelineState( const RaytracingPipelineStateDesc& desc )
        : m_maxPayloadSizeInBytes( desc.m_maxPayloadSizeInBytes )
        , m_maxAttributeSizeInBytes( desc.m_maxAttributeSizeInBytes )
        , m_maxTraceRecursionDepth( desc.m_maxTraceRecursionDepth )
    {
        auto& d3d12ResourceManager = *static_cast<D3D12ResourceManager*>( GetInterface<IResourceManager>() );
        m_shaderTable = d3d12ResourceManager.CreateRaytracingShaderTable( desc.m_shaderTableDesc );
    }

    void D3D12RaytracingPipelineState::InitResource()
    {
        if ( m_shaderTable.Get() == nullptr )
        {
            return;
        }

        D3D12StateSubobjects stateSubObjects;

        m_shaderTable->AddToStateSubobjects( stateSubObjects );

        stateSubObjects.AddShaderConfig( m_maxPayloadSizeInBytes, m_maxAttributeSizeInBytes );

        stateSubObjects.AddPipelineConfig( m_maxTraceRecursionDepth );

        D3D12_STATE_OBJECT_DESC subobjectDesc = stateSubObjects.GetSubobjectDesc();

        [[maybe_unused]] HRESULT hr = D3D12Device().CreateStateObject( &subobjectDesc, IID_PPV_ARGS( m_stateObject.GetAddressOf() ) );
        assert( SUCCEEDED( hr ) );

        Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> stateObjectProps;
        m_stateObject->QueryInterface( IID_PPV_ARGS( stateObjectProps.GetAddressOf() ) );

        m_shaderTable->WriteShaderRecords( *stateObjectProps.Get() );
    }

    void D3D12RaytracingPipelineState::FreeResource()
    {
        m_shaderTable = nullptr;
        m_stateObject.Reset();
    }
}
