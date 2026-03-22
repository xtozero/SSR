#pragma once

#include "GraphicsApiResource.h"
#include "RaytracingShaderTable.h"

namespace agl
{
    class RaytracingShaderTable;

    struct RaytracingPipelineStateDesc
    {
        size_t GetHash() const;

        RaytracingShaderTableDesc m_shaderTableDesc;
        uint32 m_maxPayloadSizeInBytes = 0;
        uint32 m_maxAttributeSizeInBytes = 0;
        uint32 m_maxTraceRecursionDepth = 0;
    };

    class RaytracingPipelineState : public GraphicsApiResource
    {
    public:
        AGL_DLL static RefHandle<RaytracingPipelineState> Create( const RaytracingPipelineStateDesc& desc );

        virtual const ShaderParameterInfo& ParameterInfo() const = 0;
    };

    AGL_DLL [[nodiscard]] ShaderBindings CreateShaderBindings( const RaytracingPipelineState* pipelineState );
}