#pragma once

#include "D3D12RaytracingShaderTable.h"
#include "RaytracingPipelineState.h"

#include <wrl/client.h>

namespace agl
{
    class D3D12StateSubobjects
    {
    public:
        void AddDXIL( const void* byteCode, size_t byteCodeSize, const wchar_t* exportName );
        void AddHitGroup( const D3D12HitGroup& hitGroup );
        void AddShaderConfig( uint32 maxPayloadSizeInBytes, uint32 maxAttributeSizeInBytes );
        void AddPipelineConfig( uint32 maxTraceRecursionDepth );
        void AddGlobalRootSignature( const D3D12RootSignature& rootSignature );

        D3D12_STATE_OBJECT_DESC GetSubobjectDesc();

    private:
        template <typename T>
        T& AllocDesc()
        {
            return *reinterpret_cast<T*>( m_transientAllocator.allocate( sizeof( T ) ) );
        }

        wchar_t* AllocWCharBuffer( uint32 size );

        RenderFrameArray<D3D12_STATE_SUBOBJECT> m_subobjects;
        TransientAllocator<uint8, ThreadType::RenderThread> m_transientAllocator;
    };

    class D3D12RaytracingPipelineState : public RaytracingPipelineState
    {
    public:
        virtual const ShaderParameterInfo& ParameterInfo() const override;
        D3D12_DISPATCH_RAYS_DESC GetDispatchRaysDesc( uint32 width, uint32 height, uint32 depth ) const;
        D3D12RootSignature* GetRootSignature() const;

        ID3D12StateObject* GetStateObject() const;

        D3D12RaytracingPipelineState( const RaytracingPipelineStateDesc& desc );

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        RefHandle<D3D12RaytracingShaderTable> m_shaderTable;
        uint32 m_maxPayloadSizeInBytes = 0;
        uint32 m_maxAttributeSizeInBytes = 0;
        uint32 m_maxTraceRecursionDepth = 0;

        Microsoft::WRL::ComPtr<ID3D12StateObject> m_stateObject;
    };
}
