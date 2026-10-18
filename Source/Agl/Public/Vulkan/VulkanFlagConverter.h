#pragma once

#include <vulkan/vulkan.h>

namespace agl
{
    class VulkanFlagConvertor
    {
    public:
        static VkFormat ConvertToFormat( ResourceFormat format );

        static ResourceFormat ConvertToResourceFormat( VkFormat format );

        static VkMemoryPropertyFlags ConvertToMemoryPropertyFlags( ResourceAccess resourceAccess );

        static VkPipelineStageFlags2 ConvertToPipelineStageFlags2( ResourceState resourceState );

        static VkAccessFlags2 ConvertToAccessFlags2( ResourceState resourceState );

        static VkImageLayout ConvertToImageLayout( ResourceState resourceState );

        static VkImageAspectFlags ConvertToImageAspectFlags( ResourceState resourceState );

        static VkPolygonMode ConvertToPolygonMode( FillMode fillMode );

        static VkCullModeFlags ConvertToCullMode( CullMode cullMode );

        static VkCompareOp ConvertToCompareOp( ComparisonFunc comparisonFunc );

        static VkStencilOp ConvertToStencilOp( StencilOp stencilOp );

        static VkBlendFactor ConvertToBlend( Blend blend );

        static VkBlendOp ConvertToBlendOp( BlendOp blendOp );

        static VkColorComponentFlags ConvertToColorComponent( ColorWriteEnable colorWriteEnable );

        static VkFilter ConvertToMagFilter( TextureFilter filter );

        static VkFilter ConvertToMinFilter( TextureFilter filter );

        static VkSamplerMipmapMode ConvertToMipmapMode( TextureFilter filter );

        static VkSamplerAddressMode ConvertToAddressMode( TextureAddressMode addressMode );
    };
}