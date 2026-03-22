#include "RaytracingPipelineState.h"

namespace agl
{
    size_t RaytracingPipelineStateDesc::GetHash() const
    {
        static size_t typeHash = typeid( RaytracingPipelineStateDesc ).hash_code();
        size_t hash = typeHash;

        HashCombine( hash, m_shaderTableDesc.GetHash() );
        HashCombine( hash, m_maxPayloadSizeInBytes );
        HashCombine( hash, m_maxAttributeSizeInBytes );
        HashCombine( hash, m_maxTraceRecursionDepth );

        return hash;
    }

    RefHandle<RaytracingPipelineState> RaytracingPipelineState::Create( const RaytracingPipelineStateDesc& desc )
    {
        return GetInterface<IResourceManager>()->CreateRaytracingPipelineState( desc );
    }

    ShaderBindingsInitializer CreateShaderBindingsInitializer( const RaytracingPipelineState* pipelineState )
    {
        ShaderBindingsInitializer initializer;

        if ( pipelineState )
        {
            initializer[ShaderType::Compute] = &pipelineState->ParameterInfo();
        }

        return initializer;
    }

    ShaderBindings CreateShaderBindings( const RaytracingPipelineState* pipelineState )
    {
        auto initializer = CreateShaderBindingsInitializer( pipelineState );
        ShaderBindings shaderBindings;
        shaderBindings.Initialize( initializer );

        return shaderBindings;
    }
}
