#include "VulkanResourceUploader.h"

#include "VulkanApi.h"

namespace agl
{
    void VulkanUploadContext::Prepare()
    {
        m_cmdListResource = VulkanCmdPool( CommandListType::Copy ).GetCommandList();
    }

    void VulkanResourceUploader::Upload( VulkanBuffer& dest, const void* data, uint32 dsetOffset, uint32 numByte )
    {

    }
}
