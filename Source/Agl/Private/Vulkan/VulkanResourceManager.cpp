#include "VulkanResourceManager.h"

#include "VulkanBlendState.h"
#include "VulkanBuffer.h"
#include "VulkanDepthStencilState.h"
#include "VulkanFlagConverter.h"
#include "VulkanPipelineState.h"
#include "VulkanQuery.h"
#include "VulkanRasterizerState.h"
#include "VulkanSamplerState.h"
#include "VulkanShaders.h"
#include "VulkanSwapchain.h"
#include "VulkanTexture.h"
#include "VulkanVertexLayout.h"
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
        // TODO
        return new VulkanVertexLayout();
    }

    ComputeShader* VulkanResourceManager::CreateComputeShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        // TODO
        return new VulkanComputeShader( byteCode, byteCodeSize, paramInfo );
    }

    VertexShader* VulkanResourceManager::CreateVertexShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        // TODO
        return new VulkanVertexShader( byteCode, byteCodeSize, paramInfo );
    }

    GeometryShader* VulkanResourceManager::CreateGeometryShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        // TODO
        return new VulkanGeometryShader( byteCode, byteCodeSize, paramInfo );
    }

    PixelShader* VulkanResourceManager::CreatePixelShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        // TODO
        return new VulkanPixelShader( byteCode, byteCodeSize, paramInfo );
    }

    MeshShader* VulkanResourceManager::CreateMeshShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        // TODO
        return new VulkanMeshShader( byteCode, byteCodeSize, paramInfo );
    }

    AmplificationShader* VulkanResourceManager::CreateAmplificationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo ) const
    {
        // TODO
        return new VulkanAmplificationShader( byteCode, byteCodeSize, paramInfo );
    }

    RayGenerationShader* VulkanResourceManager::CreateRayGenerationShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        // TODO
        return new VulkanRayGenerationShader( byteCode, byteCodeSize, paramInfo, exportName );
    }

    IntersectionShader* VulkanResourceManager::CreateIntersectionShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        // TODO
        return new VulkanIntersectionShader( byteCode, byteCodeSize, paramInfo, exportName );
    }

    AnyHitShader* VulkanResourceManager::CreateAnyHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        // TODO
        return new VulkanAnyHitShader( byteCode, byteCodeSize, paramInfo, exportName );
    }

    ClosestHitShader* VulkanResourceManager::CreateClosestHitShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        // TODO
        return new VulkanClosestHitShader( byteCode, byteCodeSize, paramInfo, exportName );
    }

    MissShader* VulkanResourceManager::CreateMissShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        // TODO
        return new VulkanMissShader( byteCode, byteCodeSize, paramInfo, exportName );
    }

    CallableShader* VulkanResourceManager::CreateCallableShader( const void* byteCode, size_t byteCodeSize, const ShaderParameterInfo& paramInfo, Name exportName ) const
    {
        // TODO
        return new VulkanCallableShader( byteCode, byteCodeSize, paramInfo, exportName );
    }

    BlendState* VulkanResourceManager::CreateBlendState( const BlendStateDesc& desc ) const
    {
        // TODO
        return new VulkanBlendState();
    }

    DepthStencilState* VulkanResourceManager::CreateDepthStencilState( const DepthStencilStateDesc& desc ) const
    {
        // TODO
        return new VulkanDepthStencilState();
    }

    RasterizerState* VulkanResourceManager::CreateRasterizerState( const RasterizerStateDesc& desc ) const
    {
        // TODO
        return new VulkanRasterizerState();
    }

    SamplerState* VulkanResourceManager::CreateSamplerState( const SamplerStateDesc& desc ) const
    {
        // TODO
        return new VulkanSamplerState( desc );
    }

    GraphicsPipelineState* VulkanResourceManager::CreatePipelineState( const GraphicsPipelineStateDesc& desc )
    {
        // TODO
        return new VulkanGraphicsPipelineState();
    }

    ComputePipelineState* VulkanResourceManager::CreatePipelineState( const ComputePipelineStateDesc& desc )
    {
        // TODO
        return new VulkanComputePipelineState();
    }

    Canvas* VulkanResourceManager::CreateCanvas( uint32 width, uint32 height, const engine::PlatformWindowContext& windowCtx, ResourceFormat format, const float4& clearColor ) const
    {
        return new VulkanSwapchain( width, height, DefaultAgl::GetBufferCount(), windowCtx, ConvertToVkFormat( format ), clearColor );
    }

    Viewport* VulkanResourceManager::CreateViewport( uint32 width, uint32 height, ResourceFormat format, const float4& bgColor ) const
    {
        // ToDo
        return new VulkanViewport( width, height, ConvertToVkFormat( format ), bgColor );
    }

    Viewport* VulkanResourceManager::CreateViewport( Canvas& canvas ) const
    {
        // ToDo
        return new VulkanViewport( *reinterpret_cast<VulkanSwapchain*>( &canvas ) );
    }

    GpuTimer* VulkanResourceManager::CreateGpuTimer() const
    {
        // TODO
        return new VulkanGpuTimer();
    }

    OcclusionQuery* VulkanResourceManager::CreateOcclusionQuery() const
    {
        // TODO
        return new VulkanOcclusionTest();
    }

    PipelineStatistics* VulkanResourceManager::CreatePipelineStatistics() const
    {
        // TODO
        return new VulkanPipelineStatistics();
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
