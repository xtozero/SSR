#pragma once

#include <vulkan/vulkan_core.h>

#include "Buffer.h"

namespace agl
{
    class VulkanBuffer : public Buffer
    {
    public:
        virtual void CreateShaderResource() override;
        virtual void CreateUnorderedAccess() override;

        virtual void* Resource() const override;

        virtual LockedResource Lock( uint32 subResource = 0, ResourceLockFlag lockFlag = ResourceLockFlag::WriteDiscard );
        virtual void UnLock( uint32 subResource = 0 );

        VulkanBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData );
        virtual ~VulkanBuffer() override;
        VulkanBuffer( const VulkanBuffer& ) = delete;
        VulkanBuffer& operator=( const VulkanBuffer& ) = delete;
        VulkanBuffer( VulkanBuffer&& ) = delete;
        VulkanBuffer& operator=( VulkanBuffer&& ) = delete;

    protected:
        virtual void CreateBuffer();
        virtual void DestroyBuffer();

        uint8* m_dataStorage = nullptr;
        bool m_hasInitData = false;

        bool m_neverLocked = true;

    private:
        virtual void InitResource() override;
        virtual void FreeResource() override;

        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_deviceMemory = VK_NULL_HANDLE;
    };
}
