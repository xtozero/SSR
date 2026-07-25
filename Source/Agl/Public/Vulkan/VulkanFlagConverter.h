#pragma once

#include <vulkan/vulkan.h>

namespace agl
{
    inline VkFormat ConvertFormatToVkFormat( ResourceFormat format )
    {
        switch( format )
        {
        case ResourceFormat::Unknown:
            break;
        case ResourceFormat::R32G32B32A32_TYPELESS:
            break;
        case ResourceFormat::R32G32B32A32_FLOAT:
            break;
        case ResourceFormat::R32G32B32A32_UINT:
            break;
        case ResourceFormat::R32G32B32A32_SINT:
            break;
        case ResourceFormat::R32G32B32_TYPELESS:
            break;
        case ResourceFormat::R32G32B32_FLOAT:
            break;
        case ResourceFormat::R32G32B32_UINT:
            break;
        case ResourceFormat::R32G32B32_SINT:
            break;
        case ResourceFormat::R16G16B16A16_TYPELESS:
            break;
        case ResourceFormat::R16G16B16A16_FLOAT:
            break;
        case ResourceFormat::R16G16B16A16_UNORM:
            break;
        case ResourceFormat::R16G16B16A16_UINT:
            break;
        case ResourceFormat::R16G16B16A16_SNORM:
            break;
        case ResourceFormat::R16G16B16A16_SINT:
            break;
        case ResourceFormat::R32G32_TYPELESS:
            break;
        case ResourceFormat::R32G32_FLOAT:
            break;
        case ResourceFormat::R32G32_UINT:
            break;
        case ResourceFormat::R32G32_SINT:
            break;
        case ResourceFormat::R32G8X24_TYPELESS:
            break;
        case ResourceFormat::D32_FLOAT_S8X24_UINT:
            break;
        case ResourceFormat::R32_FLOAT_X8X24_TYPELESS:
            break;
        case ResourceFormat::X32_TYPELESS_G8X24_UINT:
            break;
        case ResourceFormat::R10G10B10A2_TYPELESS:
            break;
        case ResourceFormat::R10G10B10A2_UNORM:
            break;
        case ResourceFormat::R10G10B10A2_UINT:
            break;
        case ResourceFormat::R11G11B10_FLOAT:
            break;
        case ResourceFormat::R8G8B8A8_TYPELESS:
            break;
        case ResourceFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case ResourceFormat::R8G8B8A8_UNORM_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case ResourceFormat::R8G8B8A8_UINT:
            break;
        case ResourceFormat::R8G8B8A8_SNORM:
            break;
        case ResourceFormat::R8G8B8A8_SINT:
            break;
        case ResourceFormat::R16G16_TYPELESS:
            break;
        case ResourceFormat::R16G16_FLOAT:
            break;
        case ResourceFormat::R16G16_UNORM:
            break;
        case ResourceFormat::R16G16_UINT:
            break;
        case ResourceFormat::R16G16_SNORM:
            break;
        case ResourceFormat::R16G16_SINT:
            break;
        case ResourceFormat::R32_TYPELESS:
            break;
        case ResourceFormat::D32_FLOAT:
            break;
        case ResourceFormat::R32_FLOAT:
            break;
        case ResourceFormat::R32_UINT:
            break;
        case ResourceFormat::R32_SINT:
            break;
        case ResourceFormat::R24G8_TYPELESS:
            break;
        case ResourceFormat::D24_UNORM_S8_UINT:
            break;
        case ResourceFormat::R24_UNORM_X8_TYPELESS:
            break;
        case ResourceFormat::X24_TYPELESS_G8_UINT:
            break;
        case ResourceFormat::R8G8_TYPELESS:
            break;
        case ResourceFormat::R8G8_UNORM:
            break;
        case ResourceFormat::R8G8_UINT:
            break;
        case ResourceFormat::R8G8_SNORM:
            break;
        case ResourceFormat::R8G8_SINT:
            break;
        case ResourceFormat::R16_TYPELESS:
            break;
        case ResourceFormat::R16_FLOAT:
            break;
        case ResourceFormat::D16_UNORM:
            break;
        case ResourceFormat::R16_UNORM:
            break;
        case ResourceFormat::R16_UINT:
            break;
        case ResourceFormat::R16_SNORM:
            break;
        case ResourceFormat::R16_SINT:
            break;
        case ResourceFormat::R8_TYPELESS:
            break;
        case ResourceFormat::R8_UNORM:
            break;
        case ResourceFormat::R8_UINT:
            break;
        case ResourceFormat::R8_SNORM:
            break;
        case ResourceFormat::R8_SINT:
            break;
        case ResourceFormat::A8_UNORM:
            break;
        case ResourceFormat::R1_UNORM:
            break;
        case ResourceFormat::R9G9B9E5_SHAREDEXP:
            break;
        case ResourceFormat::R8G8_B8G8_UNORM:
            break;
        case ResourceFormat::G8R8_G8B8_UNORM:
            break;
        case ResourceFormat::BC1_TYPELESS:
            break;
        case ResourceFormat::BC1_UNORM:
            break;
        case ResourceFormat::BC1_UNORM_SRGB:
            break;
        case ResourceFormat::BC2_TYPELESS:
            break;
        case ResourceFormat::BC2_UNORM:
            break;
        case ResourceFormat::BC2_UNORM_SRGB:
            break;
        case ResourceFormat::BC3_TYPELESS:
            break;
        case ResourceFormat::BC3_UNORM:
            break;
        case ResourceFormat::BC3_UNORM_SRGB:
            break;
        case ResourceFormat::BC4_TYPELESS:
            break;
        case ResourceFormat::BC4_UNORM:
            break;
        case ResourceFormat::BC4_SNORM:
            break;
        case ResourceFormat::BC5_TYPELESS:
            break;
        case ResourceFormat::BC5_UNORM:
            break;
        case ResourceFormat::BC5_SNORM:
            break;
        case ResourceFormat::B5G6R5_UNORM:
            break;
        case ResourceFormat::B5G5R5A1_UNORM:
            break;
        case ResourceFormat::B8G8R8A8_UNORM:
            break;
        case ResourceFormat::B8G8R8X8_UNORM:
            break;
        case ResourceFormat::R10G10B10_XR_BIAS_A2_UNORM:
            break;
        case ResourceFormat::B8G8R8A8_TYPELESS:
            break;
        case ResourceFormat::B8G8R8A8_UNORM_SRGB:
            break;
        case ResourceFormat::B8G8R8X8_TYPELESS:
            break;
        case ResourceFormat::B8G8R8X8_UNORM_SRGB:
            break;
        case ResourceFormat::BC6H_TYPELESS:
            break;
        case ResourceFormat::BC6H_UF16:
            break;
        case ResourceFormat::BC6H_SF16:
            break;
        case ResourceFormat::BC7_TYPELESS:
            break;
        case ResourceFormat::BC7_UNORM:
            break;
        case ResourceFormat::BC7_UNORM_SRGB:
            break;
        default:
            assert( false );
            return VK_FORMAT_UNDEFINED;
        }
    }
}