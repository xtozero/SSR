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

        VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        };

        result = vkCreateFence( VulkanDevice(), &fenceCreateInfo, nullptr, &m_fence );
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

        if ( m_fence != VK_NULL_HANDLE )
        {
            vkDestroyFence( VulkanDevice(), m_fence, nullptr );
        }
    }

    void VulkanCommandListResourcePool::Initialize( uint32 queueFamilyIndex )
    {
        m_queueFamilyIndex = queueFamilyIndex;
    }

    void VulkanCommandListResourcePool::Destroy()
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

        m_freeList = nullptr;
        m_runningList = nullptr;
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

    bool VulkanBaseCommandListImpl::HasCommands() const
    {
        return m_numCommands > 0;
    }

    VkQueue VulkanBaseCommandListImpl::GetCommandQueue() const
    {
        return ( m_type == CommandListType::Compute ) ? VulkanComputeQueue() : VulkanGraphicsQueue();
    }

    VkCommandBuffer VulkanBaseCommandListImpl::GetBuffer() const
    {
        return m_cmdListResource.m_commandBuffer;
    }

    VkFence VulkanBaseCommandListImpl::GetFence() const
    {
        return m_cmdListResource.m_fence;
    }

    void VulkanBaseCommandListImpl::InitializeCommandList()
    {
        m_cmdListResource = VulkanCmdPool( m_type ).GetCommandList();
        m_numCommands = 0;

        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        };

        VkResult result = vkBeginCommandBuffer( m_cmdListResource.m_commandBuffer, &beginInfo );
        assert( result == VK_SUCCESS );
    }

    void VulkanBaseCommandListImpl::OnCommandRecorded()
    {
        ++m_numCommands;
    }

    void VulkanCopyCommandListImpl::AddTransition( const ResourceTransition& transition )
    {
        m_barrierBatcher.AddTransition( transition );
    }

    void VulkanCopyCommandListImpl::AddUavBarrier( const UavBarrier& uavBarrier )
    {
        m_barrierBatcher.AddUavBarrier( uavBarrier );
    }

    void VulkanCopyCommandListImpl::PipelineBarrier( uint32 numBufferBarriers, const VkBufferMemoryBarrier2* bufferBarriers, uint32 numImageBarriers, const VkImageMemoryBarrier2* imageBarriers )
    {
        VkDependencyInfo dependencyInfo = {
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .bufferMemoryBarrierCount = numBufferBarriers,
            .pBufferMemoryBarriers = bufferBarriers,
            .imageMemoryBarrierCount = numImageBarriers,
            .pImageMemoryBarriers = imageBarriers,
        };

        vkCmdPipelineBarrier2( m_cmdListResource.m_commandBuffer, &dependencyInfo );

        OnCommandRecorded();
    }

    void VulkanCopyCommandListImpl::Close()
    {
        m_barrierBatcher.Commit( *this );

        VkResult result = vkEndCommandBuffer( m_cmdListResource.m_commandBuffer );
        assert( result == VK_SUCCESS );
    }

    void VulkanCopyCommandListImpl::OnCommited()
    {
        InitializeCommandList();
    }

    void VulkanComputeCommandListImpl::OnCommited()
    {
        VulkanCopyCommandListImpl::OnCommited();
    }

    void VulkanCommandListImpl::ClearRenderTarget( RenderTargetView* renderTarget )
    {
        if ( renderTarget == nullptr )
        {
            return;
        }

        m_barrierBatcher.Commit( *this );

        auto vulkanRTV = static_cast<VulkanImageRenderTargetView*>( renderTarget );
        const ColorF& clearValue = vulkanRTV->GetClearColor();

        VkClearValue vkClearValue = {
            .color = { clearValue.R(), clearValue.G(), clearValue.B(), clearValue.A() },
        };

        VkRenderingAttachmentInfo colorAttachmentInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = vulkanRTV->GetImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = vkClearValue,
        };

        VulkanTexture* owner = vulkanRTV->GetOwner();
        const auto& textureDesc = owner->GetDesc();

        VkRenderingInfo renderingInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {
                .offset = {
                    .x = 0,
                    .y = 0,
                },
                .extent = {
                    .width = textureDesc.m_width,
                    .height = textureDesc.m_height,
                },
            },
            .layerCount = textureDesc.m_depth,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentInfo,
        };

        vkCmdBeginRendering( m_cmdListResource.m_commandBuffer, &renderingInfo );
        vkCmdEndRendering( m_cmdListResource.m_commandBuffer );
    }

    void VulkanComputeCommandList::Prepare()
    {
        m_impl.Prepare();
    }

    void VulkanComputeCommandList::AddTransition( const ResourceTransition& transition )
    {
        m_impl.AddTransition( transition );
    }

    void VulkanComputeCommandList::AddUavBarrier( const UavBarrier& uavBarrier )
    {
        m_impl.AddUavBarrier( uavBarrier );
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
        if ( m_impl.HasCommands() == false )
        {
            return;
        }

        m_impl.Close();

        VkCommandBufferSubmitInfo commandBufferSubmitInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = m_impl.GetBuffer(),
            .deviceMask = 0,
        };

        VkSubmitInfo2 submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &commandBufferSubmitInfo
        };

        VkResult result = vkQueueSubmit2( m_impl.GetCommandQueue(), 1, &submitInfo, m_impl.GetFence() );
        assert( result == VK_SUCCESS );

        OnCommited();
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

    void VulkanComputeCommandList::Initialize()
    {
        m_impl.Initialize();
    }

    void VulkanComputeCommandList::OnCommited()
    {
        m_impl.OnCommited();
    }

    void VulkanCommandList::Prepare()
    {
        m_impl.Prepare();

        m_isCommitted = false;
    }

    void VulkanCommandList::AddTransition( const ResourceTransition& transition )
    {
        m_impl.AddTransition( transition );
    }

    void VulkanCommandList::AddUavBarrier( const UavBarrier& uavBarrier )
    {
        m_impl.AddUavBarrier( uavBarrier );
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
        if ( m_impl.HasCommands() == false )
        {
            return;
        }

        m_impl.Close();

        VkSemaphoreSubmitInfo waitInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = VulkanFrameSync().m_imageAvailable,
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .deviceIndex = 0,
        };

        VkCommandBufferSubmitInfo commandBufferSubmitInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = m_impl.GetBuffer(),
            .deviceMask = 0,
        };

        VkSubmitInfo2 submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = m_isCommitted ? 0u : 1u,
            .pWaitSemaphoreInfos = m_isCommitted ? nullptr : &waitInfo,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &commandBufferSubmitInfo
        };

        VkResult result = vkQueueSubmit2( m_impl.GetCommandQueue(), 1, &submitInfo, m_impl.GetFence() );
        assert( result == VK_SUCCESS );

        OnCommited();
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
        m_impl.ClearRenderTarget( renderTarget );
    }

    void VulkanCommandList::ClearDepthStencil( DepthStencilView* depthStencil )
    {
    }

    bool VulkanCommandList::CaptureTexture( Texture* texture, DirectX::ScratchImage& outResult )
    {
        return false;
    }

    void VulkanCommandList::Initialize()
    {
        m_impl.Initialize();
    }

    void VulkanCommandList::OnCommited()
    {
        m_isCommitted = true;

        m_impl.OnCommited();
    }
}
