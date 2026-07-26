#include "VulkanBuffer.h"

#include "VulkanApi.h"
#include "VulkanFlagConverter.h"

using ::agl::BufferDesc;
using ::agl::ResourceBindType;
using ::agl::ResourceMisc;

namespace
{
    VkBufferUsageFlags ConvertToVkBufferUsage( const BufferDesc& desc )
    {
        VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::VertexBuffer ) )
        {
            usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::IndexBuffer ) )
        {
            usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        if ( HasAnyFlags( desc.m_bindType, ResourceBindType::ConstantBuffer ) )
        {
            usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::BufferStructured ) )
        {
            usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        else if ( HasAnyFlags( desc.m_bindType, ResourceBindType::ShaderResource ) )
        {
            usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
        }
        else if ( HasAnyFlags( desc.m_bindType, ResourceBindType::RandomAccess ) )
        {
            usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
        }

        if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::DrawIndirectArgs ) )
        {
            usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        return usage;
    }

    uint32 FindMemoryType( uint32 typeFilter, VkMemoryPropertyFlags properties )
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties( agl::VulkanPhysicalDevice(), &memProperties );

        for ( uint32 i = 0; i < memProperties.memoryTypeCount; ++i )
        {
            if ( ( typeFilter & ( 1 << i ) ) && ( ( memProperties.memoryTypes[i].propertyFlags & properties ) == properties ) )
            {
                return i;
            }
        }

        assert( false );
        return 0;
    }
}

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

    LockedResource VulkanBuffer::Lock( uint32 subResource, ResourceLockFlag lockFlag )
    {
        if ( IsDynamic() )
        {
            if ( m_neverLocked || ( lockFlag != ResourceLockFlag::WriteDiscard ) )
            {
                m_neverLocked = false;
            }
            else
            {
                CreateBuffer();
            }
        }
        else
        {
            // TODO
        }

        void* data = nullptr;
        if ( vkMapMemory( VulkanDevice(), m_deviceMemory, 0, Size(), 0, &data ) != VK_SUCCESS )
        {
            return {};
        }

        LockedResource result = {
            .m_data = data,
            .m_rowPitch = Size(),
            .m_depthPitch = Size()
        };

        return result;
    }

    void VulkanBuffer::UnLock( uint32 subResource )
    {
        if ( m_deviceMemory == VK_NULL_HANDLE )
        {
            return;
        }

        vkUnmapMemory( VulkanDevice(), m_deviceMemory );
    }

    VulkanBuffer::VulkanBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
        : Buffer( initialState )
    {
        m_debugName = Name( debugName );
        m_desc = desc;

        if ( initData != nullptr )
        {
            m_hasInitData = true;

            m_dataStorage = new uint8[Size()];
            std::memcpy( m_dataStorage, initData, Size() );
        }
    }

    VulkanBuffer::~VulkanBuffer()
    {
        delete[] m_dataStorage;
        m_dataStorage = nullptr;
    }

    void VulkanBuffer::CreateBuffer()
    {
        VkDevice device = VulkanDevice();

        VkBufferCreateInfo bufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = Size(),
            .usage = ConvertToVkBufferUsage( m_desc ),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE
        };

        VkResult result = vkCreateBuffer( device, &bufferCreateInfo, nullptr, &m_buffer );
        assert( result == VK_SUCCESS );

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements( device, m_buffer, &memRequirements );

        VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = FindMemoryType( memRequirements.memoryTypeBits, ConvertToVkMemoryPropertyFlags( m_desc.m_access ) )
        };

        result = vkAllocateMemory( device, &allocInfo, nullptr, &m_deviceMemory );
        assert( result == VK_SUCCESS );

        result = vkBindBufferMemory( device, m_buffer, m_deviceMemory, 0 );
        assert( result == VK_SUCCESS );

        if ( m_hasInitData )
        {
            if ( HasAnyFlags( m_desc.m_access, ResourceAccess::CpuWrite ) )
            {
                void* data = nullptr;
                result = vkMapMemory( device, m_deviceMemory, 0, bufferCreateInfo.size, 0, &data );
                assert( result == VK_SUCCESS );

                std::memcpy( data, m_dataStorage, Size() );

                vkUnmapMemory( device, m_deviceMemory );
            }
            else
            {
                // ToDo
            }
        }
    }

    void VulkanBuffer::DestroyBuffer()
    {
        if ( m_buffer != VK_NULL_HANDLE )
        {
            vkDestroyBuffer( VulkanDevice(), m_buffer, nullptr );
        }

        if ( m_deviceMemory != VK_NULL_HANDLE )
        {
            vkFreeMemory( VulkanDevice(), m_deviceMemory, nullptr );
        }
    }

    void VulkanBuffer::InitResource()
    {
        CreateBuffer();
    }

    void VulkanBuffer::FreeResource()
    {
        DestroyBuffer();
    }

    VulkanConstantBuffer::VulkanConstantBuffer( const BufferDesc& desc, const char* debugName, ResourceState initialState, const void* initData )
        : VulkanBuffer( desc, debugName, initialState, initData )
    {
    }

    void VulkanConstantBuffer::CreateBuffer()
    {
    }

    void VulkanConstantBuffer::DestroyBuffer()
    {
    }
}
