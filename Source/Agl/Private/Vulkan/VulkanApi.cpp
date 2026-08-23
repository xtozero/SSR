#include "VulkanApi.h"

#include "IAgl.h"
#include "Memory/InlineMemoryAllocator.h"

#include "VulkanCommandList.h"
#include "VulkanSwapchain.h"

#include <set>
#include <vector>
#ifdef _WIN32
#include <vulkan/vulkan_win32.h>
#endif

namespace agl
{
    class Vulkan final : public IAgl
    {
    public:
        virtual AglType GetType() const override;

        virtual bool BootUp( const engine::PlatformWindowContext& windowCtx ) override;
        virtual void OnShutdown() override;

        virtual void HandleDeviceLost() override;
        virtual void AppSizeChanged() override;
        virtual void OnBeginFrameRendering() override;
        virtual void OnEndFrameRendering( uint32 oldFrameIndex, uint32 nextFrameIndex ) override;
        virtual void WaitGPU() override;
        virtual void WaitQueue( QueueType type ) override;

        virtual LockedResource Lock( Buffer* buffer, ResourceLockFlag lockFlag, uint32 subResource ) override;
        virtual void UnLock( Buffer* buffer, uint32 subResource ) override;

        virtual LockedResource Lock( Texture* texture, ResourceLockFlag lockFlag, uint32 subResource ) override;
        virtual void UnLock( Texture* texture, uint32 subResource ) override;

        virtual void GetRendererMultiSampleOption( MultiSampleOption* option ) override;
        virtual ICommandList* GetCommandList() override;
        virtual ICommandList* GetParallelCommandList() override;
        virtual IComputeCommandList* GetComputeCommandList() override;

        virtual BinaryChunk CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const override;
        virtual bool BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const override;

        virtual std::filesystem::path GetShaderCacheFilePath() const override;

        virtual bool SupportsPSOCache() const override;
        virtual bool SupportsPSOLibraryCache() const override;
        virtual std::filesystem::path GetPSOCacheFilePath() const override;

        virtual bool SupportsHardwareRaytracing() const override;

        virtual bool SupportsMeshShader() const override;

        virtual bool SupportsWaveIntrinsics() const override;

        VkInstance GetVulkanInstance() const;
        VkPhysicalDevice GetVulkanPhysicalDevice() const;
        VkDevice GetVulkanDevice() const;

        VkQueue GetVulkanGraphicsQueue() const;
        VkQueue GetVulkanPresentQueue() const;
        VkQueue GetVulkanComputeQueue() const;

        VulkanCommandListResourcePool& GetVulkanCmdPool( CommandListType type );
        VulkanFrameSyncContext GetVulkanFrameSyncContext() const;

        virtual ~Vulkan() override;

    private:
        bool CreateDeviceDependentResource( const engine::PlatformWindowContext& windowCtx );
        bool CreateDeviceIndependentResource();

        VkPhysicalDevice SelectPhysicalDevice( const std::vector<VkPhysicalDevice>& physicalDevices );

        VkInstance m_vkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice m_vkPhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_vkDevice = VK_NULL_HANDLE;

        VkQueue m_vkGraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_vkPresentQueue = VK_NULL_HANDLE;
        VkQueue m_vkComputeQueue = VK_NULL_HANDLE;

        std::vector<VulkanFrameSyncContext, InlineAllocator<VulkanFrameSyncContext, 2>> m_frameSyncContexts;

        uint32 m_frameIndex = 0;

        std::vector<VulkanCommandList, InlineAllocator<VulkanCommandList, 2>> m_commandList;
        std::vector<VulkanComputeCommandList, InlineAllocator<VulkanComputeCommandList, 2>> m_computeCommandList;

        VulkanCommandListResourcePool m_cmdListResourcePools[3];
    };

    AglType Vulkan::GetType() const
    {
        return AglType::Vulkan;
    }

    bool Vulkan::BootUp( const engine::PlatformWindowContext& windowCtx )
    {
        if ( CreateDeviceIndependentResource() == false )
        {
            return false;
        }

        if ( CreateDeviceDependentResource( windowCtx ) == false )
        {
            return false;
        }

        return true;
    }

    void Vulkan::OnShutdown()
    {
        for ( VulkanCommandList& commandList : m_commandList )
        {
            commandList.Prepare();
        }
        m_commandList.clear();

        for ( auto& frameSyncContext : m_frameSyncContexts )
        {
            frameSyncContext.Destroy();
        }
        m_frameSyncContexts.clear();
    }

    void Vulkan::HandleDeviceLost()
    {
    }

    void Vulkan::AppSizeChanged()
    {
    }

    void Vulkan::OnBeginFrameRendering()
    {
        for ( auto& cmdPool : m_cmdListResourcePools )
        {
            cmdPool.Prepare();
        }

        VkFence fence = GetVulkanFrameSyncContext().m_frameComplete;
        VkResult result = vkResetFences( GetVulkanDevice(), 1, &fence );
        assert( result == VK_SUCCESS );
    }

    void Vulkan::OnEndFrameRendering( uint32 oldFrameIndex, uint32 nextFrameIndex )
    {
        if ( m_frameIndex == nextFrameIndex )
        {
            return;
        }

        VkResult result = vkQueueSubmit2( GetVulkanGraphicsQueue(), 0, nullptr, GetVulkanFrameSyncContext().m_frameComplete );
        assert( result == VK_SUCCESS );

        m_frameIndex = nextFrameIndex;

        VkFence waitFence = GetVulkanFrameSyncContext().m_frameComplete;
        constexpr uint64 MaxWaitTime = std::numeric_limits<uint64>::max();
        result = vkWaitForFences( GetVulkanDevice(), 1, &waitFence, VK_TRUE, MaxWaitTime );
        assert( result == VK_SUCCESS );
    }

    void Vulkan::WaitGPU()
    {
        vkDeviceWaitIdle( m_vkDevice );
    }

    void Vulkan::WaitQueue( QueueType type )
    {
    }

    LockedResource Vulkan::Lock( Buffer* buffer, ResourceLockFlag lockFlag, uint32 subResource )
    {
        auto vulkanBuffer = static_cast<VulkanBuffer*>( buffer );
        if ( vulkanBuffer == nullptr )
        {
            return {};
        }

        return vulkanBuffer->Lock( subResource, lockFlag );
    }

    void Vulkan::UnLock( Buffer* buffer, uint32 subResource )
    {
        auto vulkanBuffer = static_cast<VulkanBuffer*>( buffer );
        if ( vulkanBuffer == nullptr )
        {
            return;
        }

        return vulkanBuffer->UnLock( subResource );
    }

    LockedResource Vulkan::Lock( Texture* texture, ResourceLockFlag lockFlag, uint32 subResource )
    {
        return {};
    }

    void Vulkan::UnLock( Texture* texture, uint32 subResource )
    {
    }

    void Vulkan::GetRendererMultiSampleOption( MultiSampleOption* option )
    {
    }

    ICommandList* Vulkan::GetCommandList()
    {
        return &m_commandList[m_frameIndex];
    }

    ICommandList* Vulkan::GetParallelCommandList()
    {
        return nullptr;
    }

    IComputeCommandList* Vulkan::GetComputeCommandList()
    {
        return &m_computeCommandList[m_frameIndex];
    }

    BinaryChunk Vulkan::CompileShader( const BinaryChunk& source, std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const
    {
        // TODO
        BinaryChunk dummy( 1 );
        return dummy;
    }

    bool Vulkan::BuildShaderMetaData( const BinaryChunk& byteCode, ShaderParameterMap& outParameterMap, ShaderParameterInfo& outParameterInfo ) const
    {
        return false;
    }

    std::filesystem::path Vulkan::GetShaderCacheFilePath() const
    {
        return {};
    }

    bool Vulkan::SupportsPSOCache() const
    {
        return false;
    }

    bool Vulkan::SupportsPSOLibraryCache() const
    {
        return false;
    }

    std::filesystem::path Vulkan::GetPSOCacheFilePath() const
    {
        return {};
    }

    bool Vulkan::SupportsHardwareRaytracing() const
    {
        return false;
    }

    bool Vulkan::SupportsMeshShader() const
    {
        return false;
    }

    bool Vulkan::SupportsWaveIntrinsics() const
    {
        return false;
    }

    VkInstance Vulkan::GetVulkanInstance() const
    {
        return m_vkInstance;
    }

    VkPhysicalDevice Vulkan::GetVulkanPhysicalDevice() const
    {
        return m_vkPhysicalDevice;
    }

    VkDevice Vulkan::GetVulkanDevice() const
    {
        return m_vkDevice;
    }

    VkQueue Vulkan::GetVulkanGraphicsQueue() const
    {
        return m_vkGraphicsQueue;
    }

    VkQueue Vulkan::GetVulkanPresentQueue() const
    {
        return m_vkPresentQueue;
    }

    VkQueue Vulkan::GetVulkanComputeQueue() const
    {
        return m_vkComputeQueue;
    }

    VulkanCommandListResourcePool& Vulkan::GetVulkanCmdPool( CommandListType type )
    {
        switch ( type )
        {
        case CommandListType::General:
        case CommandListType::Compute:
        case CommandListType::Copy:
            return m_cmdListResourcePools[type];
        default:
            break;
        }

        assert( false );
        return m_cmdListResourcePools[0];
    }

    VulkanFrameSyncContext Vulkan::GetVulkanFrameSyncContext() const
    {
        return m_frameSyncContexts[m_frameIndex];
    }

    Vulkan::~Vulkan()
    {
        if ( m_vkDevice != VK_NULL_HANDLE )
        {
            vkDestroyDevice( m_vkDevice, nullptr );
        }

        if ( m_vkInstance != VK_NULL_HANDLE )
        {
            vkDestroyInstance( m_vkInstance, nullptr );
        }
    }

    bool Vulkan::CreateDeviceDependentResource( const engine::PlatformWindowContext& windowCtx )
    {
        uint32 physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices( m_vkInstance, &physicalDeviceCount, nullptr );
        if ( physicalDeviceCount == 0 )
        {
            return false;
        }

        std::vector<VkPhysicalDevice> physicalDevices( physicalDeviceCount );
        VkResult result = vkEnumeratePhysicalDevices( m_vkInstance, &physicalDeviceCount, physicalDevices.data() );
        if ( result != VK_SUCCESS )
        {
            return false;
        }

        m_vkPhysicalDevice = SelectPhysicalDevice( physicalDevices );

        VkSurfaceKHR surface = VulkanSwapchain::CreateSurface( m_vkInstance, windowCtx );
        if ( surface == VK_NULL_HANDLE )
        {
            return false;
        }

        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties( m_vkPhysicalDevice, &queueFamilyCount, nullptr );
        std::vector<VkQueueFamilyProperties> queueFamilyProperties( queueFamilyCount );
        vkGetPhysicalDeviceQueueFamilyProperties( m_vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data() );

        int32 computeFamilyIndex = -1;
        int32 copyFamilyIndex = -1;
        int32 graphicsFamilyIndex = -1;
        int32 presentFamilyIndex = -1;
        for ( uint32 i = 0; i < queueFamilyCount; ++i )
        {
            if ( ( queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT )
                && ( queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT )
                && ( queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT ) )
            {
                graphicsFamilyIndex = i;

                VkBool32 presentSupported = false;
                vkGetPhysicalDeviceSurfaceSupportKHR( m_vkPhysicalDevice, i, surface, &presentSupported );
                if ( presentSupported )
                {
                    presentFamilyIndex = i;
                }
            }
            else if ( queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT )
            {
                computeFamilyIndex = i;
            }
            else if ( queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT )
            {
                copyFamilyIndex = i;
            }

            if ( ( computeFamilyIndex != -1 )
                && ( copyFamilyIndex != -1 )
                && ( graphicsFamilyIndex != -1 )
                && ( presentFamilyIndex != -1 ) )
            {
                break;
            }
        }

        vkDestroySurfaceKHR( m_vkInstance, surface, nullptr );

        if ( ( computeFamilyIndex == -1 )
            || ( copyFamilyIndex == -1 )
            || ( graphicsFamilyIndex == -1 )
            || ( presentFamilyIndex == -1 ) )
        {
            return false;
        }

        if ( graphicsFamilyIndex != presentFamilyIndex )
        {
            return false;
        }

        std::set<int32> uniqueQueueFamilies;
        uniqueQueueFamilies.emplace( computeFamilyIndex );
        uniqueQueueFamilies.emplace( copyFamilyIndex );
        uniqueQueueFamilies.emplace( graphicsFamilyIndex );
        uniqueQueueFamilies.emplace( presentFamilyIndex );

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve( 4 );

        constexpr float queuePriorities = 1.f;
        for ( int32 queueFamily : uniqueQueueFamilies )
        {
            if ( queueFamily < 0 )
            {
                continue;
            }

            VkDeviceQueueCreateInfo queueCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = static_cast<uint32>( queueFamily ),
                .queueCount = 1,
                .pQueuePriorities = &queuePriorities,
            };

            queueCreateInfos.emplace_back( queueCreateInfo );
        }

        VkPhysicalDeviceVulkan13Features vulkan13Features = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .synchronization2 = VK_TRUE,
            .dynamicRendering = VK_TRUE,
        };

        std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &vulkan13Features,
            .queueCreateInfoCount = static_cast<uint32>( queueCreateInfos.size() ),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledExtensionCount = static_cast<uint32>( deviceExtensions.size() ),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &deviceFeatures,
        };

        result = vkCreateDevice( m_vkPhysicalDevice, &createInfo, nullptr, &m_vkDevice );
        if ( result != VK_SUCCESS )
        {
            return false;
        }

        vkGetDeviceQueue( m_vkDevice, graphicsFamilyIndex, 0, &m_vkGraphicsQueue );
        vkGetDeviceQueue( m_vkDevice, presentFamilyIndex, 0, &m_vkPresentQueue );
        vkGetDeviceQueue( m_vkDevice, computeFamilyIndex, 0, &m_vkComputeQueue );

        m_frameSyncContexts.resize( DefaultAgl::GetBufferCount() );
        for ( auto& frameSyncContext : m_frameSyncContexts )
        {
            if ( frameSyncContext.Initialize() == false )
            {
                return false;
            }
        }

        m_commandList.resize( DefaultAgl::GetBufferCount() );
        m_computeCommandList.resize( DefaultAgl::GetBufferCount() );

        m_cmdListResourcePools[CommandListType::General].Initialize( graphicsFamilyIndex );
        m_cmdListResourcePools[CommandListType::Compute].Initialize( computeFamilyIndex );
        m_cmdListResourcePools[CommandListType::Copy].Initialize( copyFamilyIndex );

        return true;
    }

    bool Vulkan::CreateDeviceIndependentResource()
    {
        std::vector<const char*> instanceExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#ifdef _WIN32
        instanceExtensions.emplace_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#endif

        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Vulkan",
            .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
            .pEngineName = "Vulkan",
            .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
            .apiVersion = VK_API_VERSION_1_4,
        };

        VkInstanceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = static_cast<uint32>( instanceExtensions.size() ),
            .ppEnabledExtensionNames = instanceExtensions.data(),
        };

#ifdef _DEBUG
        const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        {
            uint32 instanceLayerCount = 0;
            vkEnumerateInstanceLayerProperties( &instanceLayerCount, nullptr );

            std::vector<VkLayerProperties> instanceLayerProperties( instanceLayerCount );
            vkEnumerateInstanceLayerProperties( &instanceLayerCount, instanceLayerProperties.data() );
            for ( const VkLayerProperties& layerProperties : instanceLayerProperties )
            {
                if ( std::strcmp( layerProperties.layerName, validationLayerName ) == 0 )
                {
                    createInfo.enabledLayerCount = 1;
                    createInfo.ppEnabledLayerNames = &validationLayerName;
                    break;
                }
            }
        }
#endif

        VkResult result = vkCreateInstance( &createInfo, nullptr, &m_vkInstance );
        if ( result != VK_SUCCESS )
        {
            return false;
        }

        return true;
    }

    VkPhysicalDevice Vulkan::SelectPhysicalDevice( const std::vector<VkPhysicalDevice>& physicalDevices )
    {
        for ( VkPhysicalDevice physicalDevice : physicalDevices )
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties( physicalDevice, &properties);

            if ( properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
            {
                return physicalDevice;
            }
        }

        return VK_NULL_HANDLE;
    }

    bool VulkanFrameSyncContext::Initialize()
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        VkResult result = vkCreateSemaphore( VulkanDevice(), &semaphoreCreateInfo, nullptr, &m_imageAvailable );
        if ( result != VK_SUCCESS )
        {
            return false;
        }

        result = vkCreateSemaphore( VulkanDevice(), &semaphoreCreateInfo, nullptr, &m_renderFinished );
        if ( result != VK_SUCCESS )
        {
            return false;
        }

        VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };

        result = vkCreateFence( VulkanDevice(), &fenceCreateInfo, nullptr, &m_frameComplete );
        if ( result != VK_SUCCESS )
        {
            return false;
        }

        return true;
    }

    void VulkanFrameSyncContext::Destroy()
    {
        if ( m_imageAvailable != VK_NULL_HANDLE )
        {
            vkDestroySemaphore( VulkanDevice(), m_imageAvailable, nullptr );
            m_imageAvailable = VK_NULL_HANDLE;
        }

        if ( m_renderFinished != VK_NULL_HANDLE )
        {
            vkDestroySemaphore( VulkanDevice(), m_renderFinished, nullptr );
            m_renderFinished = VK_NULL_HANDLE;
        }

        if ( m_frameComplete != VK_NULL_HANDLE )
        {
            vkDestroyFence( VulkanDevice(), m_frameComplete, nullptr );
            m_frameComplete = VK_NULL_HANDLE;
        }
    }

    Owner<IAgl*> CreateVulkanGraphicsApi()
    {
        return new Vulkan();
    }

    static Vulkan& GetVulkan()
    {
        auto vulkanApi = static_cast<Vulkan*>( GetInterface<IAgl>() );
        return *vulkanApi;
    }

    VkInstance VulkanInstance()
    {
        return GetVulkan().GetVulkanInstance();
    }

    VkPhysicalDevice VulkanPhysicalDevice()
    {
        return GetVulkan().GetVulkanPhysicalDevice();
    }

    VkDevice VulkanDevice()
    {
        return GetVulkan().GetVulkanDevice();
    }

    VkQueue VulkanGraphicsQueue()
    {
        return GetVulkan().GetVulkanGraphicsQueue();
    }

    VkQueue VulkanPresentQueue()
    {
        return GetVulkan().GetVulkanPresentQueue();
    }

    VkQueue VulkanComputeQueue()
    {
        return GetVulkan().GetVulkanComputeQueue();
    }

    VulkanCommandListResourcePool& VulkanCmdPool( CommandListType type )
    {
        return GetVulkan().GetVulkanCmdPool( type );
    }

    VulkanFrameSyncContext VulkanFrameSync()
    {
        return GetVulkan().GetVulkanFrameSyncContext();
    }
}
