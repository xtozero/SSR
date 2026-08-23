#include "VulkanCommandList.h"

namespace agl
{
    void VulkanCommandListResource::Initialize( uint32 queueFamilyIndex )
    {
        VkCommandPoolCreateInfo poolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = queueFamilyIndex,
        };

        VkResult result = vkCreateCommandPool( VulkanDevice(), &poolCreateInfo, nullptr, &m_commandPool );
        assert( result == VK_SUCCESS );

        VkCommandBufferAllocateInfo allocateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = m_commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        result = vkAllocateCommandBuffers( VulkanDevice(), &allocateInfo, &m_commandBuffer );
        assert( result == VK_SUCCESS );
    }

    void VulkanCommandListResource::Destroy()
    {
        if ( m_commandPool != VK_NULL_HANDLE )
        {
            vkDestroyCommandPool( VulkanDevice(), m_commandPool, nullptr );
            m_commandPool = VK_NULL_HANDLE;
        }

        m_commandBuffer = VK_NULL_HANDLE;
    }

    void VulkanCommandListResourcePool::Initialize( uint32 queueFamilyIndex )
    {
        m_queueFamilyIndex = queueFamilyIndex;
    }

    void VulkanCommandListResourcePool::Prepare()
    {
        VkDevice vulkanDevice = VulkanDevice();

        VulkanCommandListResource* iter = m_runningList;
        while ( iter != nullptr )
        {
            VulkanCommandListResource* next = iter->m_next;

            if ( vkGetFenceStatus( vulkanDevice, iter->m_fence ) == VK_SUCCESS )
            {
                SLinkedList::Remove( m_runningList, iter );
                SLinkedList::AddToHead( m_freeList, iter );
            }

            iter = next;
        }
    }

    VulkanCommandListResource& VulkanCommandListResourcePool::GetCommandList()
    {
        VulkanCommandListResource* ret = nullptr;
        if ( m_freeList == nullptr )
        {
            ret = m_allocator.Allocate();
            std::construct_at( ret );

            ret->Initialize( m_queueFamilyIndex );
        }
        else
        {
            ret = m_freeList;
            SLinkedList::Remove( m_freeList, ret );

            VkDevice vulkanDevice = VulkanDevice();
            VkResult result = vkResetCommandPool( vulkanDevice, ret->m_commandPool, 0 );
            assert( result == VK_SUCCESS );

            result = vkResetFences( vulkanDevice, 1, &ret->m_fence );
            assert( result == VK_SUCCESS );
        }

        SLinkedList::AddToHead( m_runningList, ret );
        return *ret;
    }

    VulkanCommandListResourcePool::~VulkanCommandListResourcePool()
    {
        VulkanCommandListResource* iter = m_freeList;
        while ( iter != nullptr )
        {
            VulkanCommandListResource* next = iter->m_next;
            iter->Destroy();
            iter = next;
        }

        iter = m_runningList;
        while ( iter != nullptr )
        {
            VulkanCommandListResource* next = iter->m_next;
            iter->Destroy();
            iter = next;
        }
    }

    void VulkanBaseCommandListImpl::Initialize()
    {
        InitializeCommandList();
    }

    void VulkanBaseCommandListImpl::Prepare()
    {
        assert( m_cmdListResource.m_fence != VK_NULL_HANDLE );
        if ( vkGetFenceStatus( VulkanDevice(), m_cmdListResource.m_fence ) == VK_SUCCESS )
        {
            InitializeCommandList();
        }
    }

    void VulkanBaseCommandListImpl::InitializeCommandList()
    {
        m_cmdListResource = VulkanCmdPool( m_type ).GetCommandList();
        m_numCommands = 0;
    }

    void VulkanComputeCommandList::Prepare()
    {
    }

    void VulkanComputeCommandList::AddTransition( const ResourceTransition& transition )
    {
    }

    void VulkanComputeCommandList::AddUavBarrier( const UavBarrier& uavBarrier )
    {
    }

    void VulkanComputeCommandList::BeginQuery( void* rawQuery )
    {
    }

    void VulkanComputeCommandList::EndQuery( void* rawQuery )
    {
    }

    void VulkanComputeCommandList::BeginEvent( const char* eventName )
    {
    }

    void VulkanComputeCommandList::EndEvent()
    {
    }

    void VulkanComputeCommandList::Commit()
    {
    }

    void VulkanComputeCommandList::CopyResource( Texture* dest, Texture* src, bool bAsync )
    {
    }

    void VulkanComputeCommandList::CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte )
    {
    }

    void VulkanComputeCommandList::UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource )
    {
    }

    void VulkanComputeCommandList::UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte )
    {
    }

    void VulkanComputeCommandList::BindPipelineState( const ComputePipelineState* pipelineState )
    {
    }

    void VulkanComputeCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
    {
    }

    void VulkanComputeCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
    {
    }

    void VulkanComputeCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
    {
    }

    void VulkanComputeCommandList::DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth )
    {
    }

    void VulkanComputeCommandList::ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset )
    {
    }

    void VulkanCommandList::Prepare()
    {
    }

    void VulkanCommandList::AddTransition( const ResourceTransition& transition )
    {
    }

    void VulkanCommandList::AddUavBarrier( const UavBarrier& uavBarrier )
    {
    }

    void VulkanCommandList::BeginQuery( void* rawQuery )
    {
    }

    void VulkanCommandList::EndQuery( void* rawQuery )
    {
    }

    void VulkanCommandList::BeginEvent( const char* eventName )
    {
    }

    void VulkanCommandList::EndEvent()
    {
    }

    void VulkanCommandList::Commit()
    {
    }

    void VulkanCommandList::CopyResource( Texture* dest, Texture* src, bool bAsync )
    {
    }

    void VulkanCommandList::CopyResource( Buffer* dest, Buffer* src, bool bAsync, uint32 numByte )
    {
    }

    void VulkanCommandList::UpdateSubresource( Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource )
    {
    }

    void VulkanCommandList::UpdateSubresource( Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte )
    {
    }

    void VulkanCommandList::BindPipelineState( const ComputePipelineState* pipelineState )
    {
    }

    void VulkanCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
    {
    }

    void VulkanCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
    {
    }

    void VulkanCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
    {
    }

    void VulkanCommandList::DispatchRays( const RaytracingPipelineState* pipelineState, const ShaderBindings& shaderBindings, uint32 width, uint32 height, uint32 depth )
    {
    }

    void VulkanCommandList::ExecuteIndirect( IndirectCommandType type, Buffer* argument, uint64 argumentOffset )
    {
    }

    void VulkanCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
    {
    }

    void VulkanCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
    {
    }

    void VulkanCommandList::DispatchMesh( uint32 x, uint32 y, uint32 z )
    {
    }

    void VulkanCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
    {
    }

    void VulkanCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
    {
    }

    void VulkanCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets )
    {
    }

    void VulkanCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
    {
    }

    void VulkanCommandList::BindPipelineState( const GraphicsPipelineState* pipelineState )
    {
    }

    void VulkanCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
    {
    }

    void VulkanCommandList::ClearRenderTarget( RenderTargetView* renderTarget )
    {
    }

    void VulkanCommandList::ClearDepthStencil( DepthStencilView* depthStencil )
    {
    }

    bool VulkanCommandList::CaptureTexture( Texture* texture, DirectX::ScratchImage& outResult )
    {
        return false;
    }
}
