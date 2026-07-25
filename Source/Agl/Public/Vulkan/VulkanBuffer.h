#pragma once

#include "Buffer.h"

namespace agl
{
    class VulkanBuffer : public Buffer
    {
    public:
        virtual void CreateShaderResource() override;
        virtual void CreateUnorderedAccess() override;

        virtual void* Resource() const override;

        VulkanBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData );
        virtual ~VulkanBuffer() override;
        VulkanBuffer( const VulkanBuffer& ) = delete;
        VulkanBuffer& operator=( const VulkanBuffer& ) = delete;
        VulkanBuffer( VulkanBuffer&& ) = delete;
        VulkanBuffer& operator=( VulkanBuffer&& ) = delete;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;
    };
}
