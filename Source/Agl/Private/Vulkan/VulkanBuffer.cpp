#include "VulkanBuffer.h"

#include "VulkanApi.h"
#include "VulkanFlagConverter.h"
#include "VulkanResourceVeiws.h"

using ::agl::BufferDesc;
using ::agl::ResourceBindType;
using ::agl::ResourceMisc;

namespace
{
    VkBufferUsageFlags ConvertToVkBufferUsage( const BufferDesc& desc )
    {
        VkBufferUsageFlags usage = 0;
        if ( HasAllFlags( desc.m_access, agl::ResourceAccess::Default ) )
        {
            usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        if ( HasAllFlags( desc.m_access, agl::ResourceAccess::Upload ) )
        {
            usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }

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
}

namespace agl
{
    void VulkanBuffer::CreateShaderResource()
    {
        m_srv = new VulkanBufferShaderResourceView( this );
        m_srv->Init();
    }

    void VulkanBuffer::CreateUnorderedAccess()
    {
        m_uav = new VulkanBufferUnorderedAccessView( this );
        m_uav->Init();
    }

    void* VulkanBuffer::Resource() const
    {
        return m_buffer;
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

        VkMemoryMapInfo mapInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO,
            .memory = m_deviceMemory,
            .size = Size(),
        };
        void* data = nullptr;
        if ( vkMapMemory2( VulkanDevice(), &mapInfo, &data ) != VK_SUCCESS )
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

        VkMemoryUnmapInfo unmapInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO,
            .memory = m_deviceMemory,
        };
        vkUnmapMemory2( VulkanDevice(), &unmapInfo );
    }

    VkBuffer VulkanBuffer::Resource()
    {
        return m_buffer;
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
            .memoryTypeIndex = FindVulkanMemoryType( memRequirements.memoryTypeBits, ConvertToVkMemoryPropertyFlags( m_desc.m_access ) )
        };

        result = vkAllocateMemory( device, &allocInfo, nullptr, &m_deviceMemory );
        assert( result == VK_SUCCESS );

        VkBindBufferMemoryInfo bindInfo = {
            .sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
            .buffer = m_buffer,
            .memory = m_deviceMemory,
            .memoryOffset = 0,
        };

        result = vkBindBufferMemory2( device, 1, &bindInfo );
        assert( result == VK_SUCCESS );

        if ( m_hasInitData )
        {
            if ( HasAnyFlags( m_desc.m_access, ResourceAccess::CpuWrite ) )
            {
                VkMemoryMapInfo mapInfo = {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_MAP_INFO,
                    .memory = m_deviceMemory,
                    .size = bufferCreateInfo.size,
                };
                void* data = nullptr;
                result = vkMapMemory2( device, &mapInfo, &data );
                assert( result == VK_SUCCESS );

                std::memcpy( data, m_dataStorage, Size() );

                VkMemoryUnmapInfo unmapInfo = {
                    .sType = VK_STRUCTURE_TYPE_MEMORY_UNMAP_INFO,
                    .memory = m_deviceMemory,
                };
                vkUnmapMemory2( device, &unmapInfo );
            }
            else
            {
                VulkanUploader().Upload( *this, m_dataStorage );
            }
        }

        if ( HasAnyFlags( m_desc.m_miscFlag, ResourceMisc::Intermediate | ResourceMisc::WithoutViews ) )
        {
            return;
        }

        if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::ShaderResource ) )
        {
            CreateShaderResource();
        }

        if ( HasAnyFlags( m_desc.m_bindType, ResourceBindType::RandomAccess ) )
        {
            CreateUnorderedAccess();
        }
    }

    void VulkanBuffer::DestroyBuffer()
    {
        m_srv = nullptr;
        m_uav = nullptr;

        if ( m_buffer != VK_NULL_HANDLE )
        {
            vkDestroyBuffer( VulkanDevice(), m_buffer, nullptr );
            m_buffer = VK_NULL_HANDLE;
        }

        if ( m_deviceMemory != VK_NULL_HANDLE )
        {
            vkFreeMemory( VulkanDevice(), m_deviceMemory, nullptr );
            m_deviceMemory = VK_NULL_HANDLE;
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
}
