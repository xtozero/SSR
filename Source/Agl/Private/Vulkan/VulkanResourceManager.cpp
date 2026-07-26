#include "VulkanResourceManager.h"

#include "VulkanBuffer.h"
#include "VulkanFlagConverter.h"
#include "VulkanSamplerState.h"
#include "VulkanSwapchain.h"
#include "VulkanTexture.h"
#include "VulkanViewport.h"

namespace agl
{
    void VulkanResourceManager::Shutdown()
    {
    }

    Texture* VulkanResourceManager::CreateTexture( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData ) const
    {
        Texture* newTexture = nullptr;

        // TODO
        newTexture = new VulkanTexture2D( desc, debugName, initialState, initData );

        return newTexture;
    }

    Buffer* VulkanResourceManager::CreateBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData ) const
    {
        // TODO
        return new VulkanBuffer( desc, debugName, initialState, initData );
    }

    VertexLayout* VulkanResourceManager::CreateVertexLayout( const VertexShader* vs, const VertexLayoutData* layoutData, uint32 size ) const
    {
        return nullptr;
    }

    ComputeShader* VulkanResourceManager::CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        return nullptr;
    }

    VertexShader* VulkanResourceManager::CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        return nullptr;
    }

    GeometryShader* VulkanResourceManager::CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        return nullptr;
    }

    PixelShader* VulkanResourceManager::CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        return nullptr;
    }

    MeshShader* VulkanResourceManager::CreateMeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        return nullptr;
    }

    AmplificationShader* VulkanResourceManager::CreateAmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        return nullptr;
    }

    RayGenerationShader* VulkanResourceManager::CreateRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        return nullptr;
    }

    IntersectionShader* VulkanResourceManager::CreateIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        return nullptr;
    }

    AnyHitShader* VulkanResourceManager::CreateAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        return nullptr;
    }

    ClosestHitShader* VulkanResourceManager::CreateClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        return nullptr;
    }

    MissShader* VulkanResourceManager::CreateMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        return nullptr;
    }

    CallableShader* VulkanResourceManager::CreateCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        return nullptr;
    }

    BlendState* VulkanResourceManager::CreateBlendState( const BlendStateDesc& desc ) const
    {
        return nullptr;
    }

    DepthStencilState* VulkanResourceManager::CreateDepthStencilState( const DepthStencilStateDesc& desc ) const
    {
        return nullptr;
    }

    RasterizerState* VulkanResourceManager::CreateRasterizerState( const RasterizerStateDesc& desc ) const
    {
        return nullptr;
    }

    SamplerState* VulkanResourceManager::CreateSamplerState( const SamplerStateDesc& desc ) const
    {
        // TODO
        return new VulkanSamplerState( desc );
    }

    GraphicsPipelineState* VulkanResourceManager::CreatePipelineState( const GraphicsPipelineStateDesc& desc )
    {
        return nullptr;
    }

    ComputePipelineState* VulkanResourceManager::CreatePipelineState( const ComputePipelineStateDesc& desc )
    {
        return nullptr;
    }

    Canvas* VulkanResourceManager::CreateCanvas( uint32 width, uint32 height, const engine::PlatformWindowContext& windowCtx, ResourceFormat format, const float4& clearColor ) const
    {
        return new VulkanSwapchain( width, height, DefaultAgl::GetBufferCount(), windowCtx, ConvertFormatToVkFormat( format ), clearColor );
    }

    Viewport* VulkanResourceManager::CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) const
    {
        // ToDo
        return new VulkanViewport( width, height, ConvertFormatToVkFormat( format ), bgColor );
    }

    Viewport* VulkanResourceManager::CreateViewport( Canvas& canvas ) const
    {
        // ToDo
        return new VulkanViewport( *reinterpret_cast<VulkanSwapchain*>( &canvas ) );
    }

    GpuTimer* VulkanResourceManager::CreateGpuTimer() const
    {
        return nullptr;
    }

    OcclusionQuery* VulkanResourceManager::CreateOcclusionQuery() const
    {
        return nullptr;
    }

    PipelineStatistics* VulkanResourceManager::CreatePipelineStatistics() const
    {
        return nullptr;
    }

    void VulkanResourceManager::SetPSOCache( std::map<uint64, BinaryChunk>& psoCache )
    {
    }

    void VulkanResourceManager::SetPSOCache( const BinaryChunk& psoCache )
    {
    }

    BinaryChunk VulkanResourceManager::SerializePSOLibraryCache()
    {
        return {};
    }

    void VulkanResourceManager::PostReloadShaders()
    {
    }

    BLAS* VulkanResourceManager::CreateBLAS( const BLASDesc& desc, const char* debugName ) const
    {
        return nullptr;
    }

    TLAS* VulkanResourceManager::CreateTLAS( const TLASDesc& desc, const char* debugName ) const
    {
        return nullptr;
    }

    RaytracingPipelineState* VulkanResourceManager::CreateRaytracingPipelineState( const RaytracingPipelineStateDesc& desc )
    {
        return nullptr;
    }

    VulkanResourceManager::~VulkanResourceManager()
    {
    }

    Owner<IResourceManager*> CreateVulkanResourceManager()
    {
        return new VulkanResourceManager();
    }
}
