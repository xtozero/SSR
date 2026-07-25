#include "VulkanBuffer.h"

namespace agl
{
    void VulkanBuffer::CreateShaderResource()
    {
    }

    void VulkanBuffer::CreateUnorderedAccess()
    {
    }

    void* VulkanBuffer::Resource() const
    {
        return nullptr;
    }

    VulkanBuffer::VulkanBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
        : Buffer( initialState )
    {
    }

    VulkanBuffer::~VulkanBuffer()
    {
    }

    void VulkanBuffer::InitResource()
    {
    }

    void VulkanBuffer::FreeResource()
    {
    }
}
