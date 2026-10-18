#include "VulkanSamplerState.h"

#include "VulkanFlagConverter.h"

using ::agl::SamplerStateDesc;
using ::agl::TextureFilter;
using ::agl::VulkanFlagConvertor;

namespace
{
    VkSamplerCreateInfo ConverToCreateInfo( const SamplerStateDesc& desc )
    {
        bool anisotropyEnable = HasAllFlags( desc.m_filter, TextureFilter::Anisotropic );
        bool compareEnable = HasAnyFlags( desc.m_filter, TextureFilter::Comparison );

        return VkSamplerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VulkanFlagConvertor::ConvertToMagFilter( desc.m_filter ),
            .minFilter = VulkanFlagConvertor::ConvertToMinFilter( desc.m_filter ),
            .mipmapMode = VulkanFlagConvertor::ConvertToMipmapMode( desc.m_filter ),
            .addressModeU = VulkanFlagConvertor::ConvertToAddressMode( desc.m_addressU ),
            .addressModeV = VulkanFlagConvertor::ConvertToAddressMode( desc.m_addressV ),
            .addressModeW = VulkanFlagConvertor::ConvertToAddressMode( desc.m_addressW ),
            .mipLodBias = desc.m_mipLODBias,
            .anisotropyEnable = anisotropyEnable,
            .maxAnisotropy = static_cast<float>( desc.m_maxAnisotropy ),
            .compareEnable = compareEnable,
            .compareOp = VulkanFlagConvertor::ConvertToCompareOp( desc.m_comparisonFunc ),
            .minLod = desc.m_minLOD,
            .maxLod = desc.m_maxLOD,
            // .borderColor = desc.m_borderColor, // TODO
            .unnormalizedCoordinates = VK_FALSE,
        };
    }
}

namespace agl
{
    const VkSampler& VulkanSamplerState::Resource() const
    {
        return m_sampler;
    }

    VulkanSamplerState::VulkanSamplerState( const SamplerStateDesc& desc )
        : m_createInfo( ConverToCreateInfo( desc ) )
    {
    }

    void VulkanSamplerState::InitResource()
    {
        VkResult result = vkCreateSampler( VulkanDevice(), &m_createInfo, nullptr, &m_sampler );
        assert( result == VK_SUCCESS );
    }

    void VulkanSamplerState::FreeResource()
    {
        if ( m_sampler != VK_NULL_HANDLE )
        {
            vkDestroySampler( VulkanDevice(), m_sampler, nullptr );
        }
    }
}
