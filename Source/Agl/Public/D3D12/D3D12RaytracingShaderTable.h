#pragma once

#include "D3D12Shaders.h"
#include "RaytracingShaderTable.h"

#include <vector>

namespace agl
{
    class D3D12StateSubobjects;

    class D3D12HitGroup : public GraphicsApiResource
    {
    public:
        D3D12HitGroup( const HitGroupDesc& desc );

        D3D12IntersectionShader* GetIntersection() const;
        D3D12AnyHitShader* GetAnyHit() const;
        D3D12ClosestHitShader* GetClosestHit() const;

        const wchar_t* GetExportName() const;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        RefHandle<D3D12IntersectionShader> m_intersection;
        RefHandle<D3D12AnyHitShader> m_anyHit;
        RefHandle<D3D12ClosestHitShader> m_closestHit;

        wchar_t m_wExportName[RaytracingShader::ExportNameBufferSize] = {};
    };

    class D3D12RaytracingShaderTable : public RaytracingShaderTable
    {
    public:
        void AddToStateSubobjects( D3D12StateSubobjects& outSubobjects ) const;
        void WriteShaderRecords( ID3D12StateObjectProperties& properties ) const;

        const ShaderParameterInfo& GetParameterInfo() const;
        D3D12_DISPATCH_RAYS_DESC GetDispatchRaysDesc( uint32 width, uint32 height, uint32 depth ) const;
        D3D12RootSignature* GetRootSignature() const;

        D3D12RaytracingShaderTable( const RaytracingShaderTableDesc& desc );

        static const uint32 ShaderRecordSize = CalcAlignment( D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT );

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        void MergeShaderParameterInfo();

        RefHandle<D3D12RayGenerationShader> m_rayGeneration;
        std::vector<RefHandle<D3D12HitGroup>> m_hitGroups;
        std::vector<RefHandle<D3D12MissShader>> m_misses;

        ShaderParameterInfo m_shaderParameterInfo;

        RefHandle<D3D12RootSignature> m_rootSignature;

        RefHandle<D3D12Buffer> m_shaderRecords;
    };
}
