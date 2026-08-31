#pragma once

#include "ICommandList.h"
#include "VulkanBarrierBatcher.h"

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
        void Destroy();

        void Prepare();
        VulkanCommandListResource& GetCommandList();

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

        bool HasCommands() const;

        VkQueue GetCommandQueue() const;
        VkCommandBuffer GetBuffer() const;
        VkFence GetFence() const;

    protected:
        explicit VulkanBaseCommandListImpl( CommandListType type ) : m_type( type ) {}

        void InitializeCommandList();

        void OnCommandRecorded();

        VulkanCommandListResource m_cmdListResource;

        uint32 m_numCommands = 0;

    private:
        CommandListType m_type = CommandListType::General;
    };

    class VulkanCopyCommandListImpl : public VulkanBaseCommandListImpl
    {
    public:
        void AddTransition( const ResourceTransition& transition );
        void AddUavBarrier( const UavBarrier& uavBarrier );

        void PipelineBarrier( uint32 numBufferBarriers, const VkBufferMemoryBarrier2* bufferBarriers, uint32 numImageBarriers, const VkImageMemoryBarrier2* imageBarriers );

        void Close();

        void OnCommited();

        VulkanCopyCommandListImpl() : VulkanBaseCommandListImpl( CommandListType::Copy ) {}

    protected:
        VulkanBarrierBatcher m_barrierBatcher;

        explicit VulkanCopyCommandListImpl( CommandListType type ) : VulkanBaseCommandListImpl( type ) {}
    };

    class VulkanComputeCommandListImpl : public VulkanCopyCommandListImpl
    {
    public:
        VulkanComputeCommandListImpl() : VulkanCopyCommandListImpl( CommandListType::Compute ) {}

        void OnCommited();

    protected:
        explicit VulkanComputeCommandListImpl( CommandListType type ) : VulkanCopyCommandListImpl( type ) {}
    };

    class VulkanCommandListImpl : public VulkanComputeCommandListImpl
    {
    public:
        void ClearRenderTarget( RenderTargetView* renderTarget );

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

        void Initialize();

    private:
        void OnCommited();

        VulkanComputeCommandListImpl m_impl;
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

        void Initialize();

    private:
        void OnCommited();

        VulkanCommandListImpl m_impl;

        bool m_isCommitted = false;
    };
}