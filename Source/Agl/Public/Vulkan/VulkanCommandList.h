#pragma once

#include "ICommandList.h"

#include <vulkan/vulkan.h>

namespace agl
{
    class VulkanCommandListResource final
    {
    public:
        void Initialize( uint32 queueFamilyIndex );
        void Destroy();

        VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        VkFence m_fence = VK_NULL_HANDLE;
        VulkanCommandListResource* m_next = nullptr;
    };

    class VulkanCommandListResourcePool final
    {
    public:
        void Initialize( uint32 queueFamilyIndex );

        void Prepare();
        VulkanCommandListResource& GetCommandList();

        VulkanCommandListResourcePool() = default;
        ~VulkanCommandListResourcePool();

    private:
        uint32 m_queueFamilyIndex = 0;

        FixedBlockMemoryPool<VulkanCommandListResource> m_allocator;
        VulkanCommandListResource* m_freeList = nullptr;
        VulkanCommandListResource* m_runningList = nullptr;
    };

    class VulkanBaseCommandListImpl
    {
    public:
        void Initialize();
        void Prepare();

    protected:
        explicit VulkanBaseCommandListImpl( CommandListType type ) : m_type( type ) {}

        void InitializeCommandList();

        VulkanCommandListResource m_cmdListResource;

        uint32 m_numCommands = 0;

    private:
        CommandListType m_type = CommandListType::General;
    };

    class VulkanCopyCommandListImpl : public VulkanBaseCommandListImpl
    {
    public:
        VulkanCopyCommandListImpl() : VulkanBaseCommandListImpl( CommandListType::Copy ) {}

    protected:
        explicit VulkanCopyCommandListImpl( CommandListType type ) : VulkanBaseCommandListImpl( type ) {}
    };

    class VulkanComputeCommandListImpl : public VulkanCopyCommandListImpl
    {
    public:
        VulkanComputeCommandListImpl() : VulkanCopyCommandListImpl( CommandListType::Compute ) {}

    protected:
        explicit VulkanComputeCommandListImpl( CommandListType type ) : VulkanCopyCommandListImpl( type ) {}
    };

    class VulkanCommandListImpl : public VulkanComputeCommandListImpl
    {
    public:
        VulkanCommandListImpl() : VulkanComputeCommandListImpl( CommandListType::General ) {}
    };

    class VulkanComputeCommandList final : public IComputeCommandList
    {
    public:
        virtual void Prepare() override;

        virtual void AddTransition( const ResourceTransition& transition ) override;
        virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

        virtual void BeginQuery( void* rawQuery ) override;
        virtual void EndQuery( void* rawQuery ) override;

        virtual void BeginEvent( const char* eventName ) override;
        virtual void EndEvent() override;

        virtual void Commit() override;

        virtual void CopyResource( Texture* dest, Texture* src, bool bAsync ) override;
        virtual void CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte ) override;

        virtual void UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource ) override;
        virtual void UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte ) override;

        virtual void BindPipelineState( const ComputePipelineState* pipelineState ) override;
        virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
        virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;

        virtual void Dispatch( uint32 x, uint32 y, uint32 z ) override;

        virtual void DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth ) override;
        virtual void ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset ) override;
    };

    class IVulkanCommandList : public ICommandList
    {
    };

    class VulkanCommandList final : public IVulkanCommandList
    {
    public:
        virtual void Prepare() override;

        virtual void AddTransition( const ResourceTransition& transition ) override;
        virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

        virtual void BeginQuery( void* rawQuery ) override;
        virtual void EndQuery( void* rawQuery ) override;

        virtual void BeginEvent( const char* eventName ) override;
        virtual void EndEvent() override;

        virtual void Commit() override;

        virtual void CopyResource( Texture* dest, Texture* src, bool bAsync ) override;
        virtual void CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte ) override;

        virtual void UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource ) override;
        virtual void UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte ) override;

        virtual void BindPipelineState( const ComputePipelineState* pipelineState ) override;
        virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
        virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;

        virtual void Dispatch( uint32 x, uint32 y, uint32 z ) override;
        virtual void DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth ) override;

        virtual void ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset ) override;

        virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
        virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
        virtual void DispatchMesh( uint32 x, uint32 y, uint32 z ) override;

        virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) override;
        virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) override;

        virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) override;
        virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
        virtual void BindPipelineState( const GraphicsPipelineState* pipelineState ) override;
        virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

        virtual void ClearRenderTarget( RenderTargetView* renderTarget ) override;
        virtual void ClearDepthStencil( DepthStencilView* depthStencil ) override;

        virtual bool CaptureTexture( Texture* texture, DirectX::ScratchImage& outResult ) override;
    };
}