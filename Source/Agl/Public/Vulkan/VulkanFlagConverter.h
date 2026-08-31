#pragma once

#include <vulkan/vulkan.h>

namespace agl
{
    inline VkFormat ConvertToVkFormat( ResourceFormat format )
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

    inline ResourceFormat ConvertVkFormatToFormat( VkFormat format )
    {
        switch ( format )
        {
        case VK_FORMAT_UNDEFINED:
            break;
        case VK_FORMAT_R4G4_UNORM_PACK8:
            break;
        case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
            break;
        case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
            break;
        case VK_FORMAT_R5G6B5_UNORM_PACK16:
            break;
        case VK_FORMAT_B5G6R5_UNORM_PACK16:
            break;
        case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
            break;
        case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
            break;
        case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
            break;
        case VK_FORMAT_R8_UNORM:
            break;
        case VK_FORMAT_R8_SNORM:
            break;
        case VK_FORMAT_R8_USCALED:
            break;
        case VK_FORMAT_R8_SSCALED:
            break;
        case VK_FORMAT_R8_UINT:
            break;
        case VK_FORMAT_R8_SINT:
            break;
        case VK_FORMAT_R8_SRGB:
            break;
        case VK_FORMAT_R8G8_UNORM:
            break;
        case VK_FORMAT_R8G8_SNORM:
            break;
        case VK_FORMAT_R8G8_USCALED:
            break;
        case VK_FORMAT_R8G8_SSCALED:
            break;
        case VK_FORMAT_R8G8_UINT:
            break;
        case VK_FORMAT_R8G8_SINT:
            break;
        case VK_FORMAT_R8G8_SRGB:
            break;
        case VK_FORMAT_R8G8B8_UNORM:
            break;
        case VK_FORMAT_R8G8B8_SNORM:
            break;
        case VK_FORMAT_R8G8B8_USCALED:
            break;
        case VK_FORMAT_R8G8B8_SSCALED:
            break;
        case VK_FORMAT_R8G8B8_UINT:
            break;
        case VK_FORMAT_R8G8B8_SINT:
            break;
        case VK_FORMAT_R8G8B8_SRGB:
            break;
        case VK_FORMAT_B8G8R8_UNORM:
            break;
        case VK_FORMAT_B8G8R8_SNORM:
            break;
        case VK_FORMAT_B8G8R8_USCALED:
            break;
        case VK_FORMAT_B8G8R8_SSCALED:
            break;
        case VK_FORMAT_B8G8R8_UINT:
            break;
        case VK_FORMAT_B8G8R8_SINT:
            break;
        case VK_FORMAT_B8G8R8_SRGB:
            break;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return ResourceFormat::R8G8B8A8_UNORM;
        case VK_FORMAT_R8G8B8A8_SNORM:
            break;
        case VK_FORMAT_R8G8B8A8_USCALED:
            break;
        case VK_FORMAT_R8G8B8A8_SSCALED:
            break;
        case VK_FORMAT_R8G8B8A8_UINT:
            break;
        case VK_FORMAT_R8G8B8A8_SINT:
            break;
        case VK_FORMAT_R8G8B8A8_SRGB:
            break;
        case VK_FORMAT_B8G8R8A8_UNORM:
            break;
        case VK_FORMAT_B8G8R8A8_SNORM:
            break;
        case VK_FORMAT_B8G8R8A8_USCALED:
            break;
        case VK_FORMAT_B8G8R8A8_SSCALED:
            break;
        case VK_FORMAT_B8G8R8A8_UINT:
            break;
        case VK_FORMAT_B8G8R8A8_SINT:
            break;
        case VK_FORMAT_B8G8R8A8_SRGB:
            break;
        case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
            break;
        case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
            break;
        case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
            break;
        case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
            break;
        case VK_FORMAT_A8B8G8R8_UINT_PACK32:
            break;
        case VK_FORMAT_A8B8G8R8_SINT_PACK32:
            break;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            break;
        case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
            break;
        case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
            break;
        case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
            break;
        case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
            break;
        case VK_FORMAT_A2R10G10B10_UINT_PACK32:
            break;
        case VK_FORMAT_A2R10G10B10_SINT_PACK32:
            break;
        case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
            break;
        case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
            break;
        case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
            break;
        case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
            break;
        case VK_FORMAT_A2B10G10R10_UINT_PACK32:
            break;
        case VK_FORMAT_A2B10G10R10_SINT_PACK32:
            break;
        case VK_FORMAT_R16_UNORM:
            break;
        case VK_FORMAT_R16_SNORM:
            break;
        case VK_FORMAT_R16_USCALED:
            break;
        case VK_FORMAT_R16_SSCALED:
            break;
        case VK_FORMAT_R16_UINT:
            break;
        case VK_FORMAT_R16_SINT:
            break;
        case VK_FORMAT_R16_SFLOAT:
            break;
        case VK_FORMAT_R16G16_UNORM:
            break;
        case VK_FORMAT_R16G16_SNORM:
            break;
        case VK_FORMAT_R16G16_USCALED:
            break;
        case VK_FORMAT_R16G16_SSCALED:
            break;
        case VK_FORMAT_R16G16_UINT:
            break;
        case VK_FORMAT_R16G16_SINT:
            break;
        case VK_FORMAT_R16G16_SFLOAT:
            break;
        case VK_FORMAT_R16G16B16_UNORM:
            break;
        case VK_FORMAT_R16G16B16_SNORM:
            break;
        case VK_FORMAT_R16G16B16_USCALED:
            break;
        case VK_FORMAT_R16G16B16_SSCALED:
            break;
        case VK_FORMAT_R16G16B16_UINT:
            break;
        case VK_FORMAT_R16G16B16_SINT:
            break;
        case VK_FORMAT_R16G16B16_SFLOAT:
            break;
        case VK_FORMAT_R16G16B16A16_UNORM:
            break;
        case VK_FORMAT_R16G16B16A16_SNORM:
            break;
        case VK_FORMAT_R16G16B16A16_USCALED:
            break;
        case VK_FORMAT_R16G16B16A16_SSCALED:
            break;
        case VK_FORMAT_R16G16B16A16_UINT:
            break;
        case VK_FORMAT_R16G16B16A16_SINT:
            break;
        case VK_FORMAT_R16G16B16A16_SFLOAT:
            break;
        case VK_FORMAT_R32_UINT:
            break;
        case VK_FORMAT_R32_SINT:
            break;
        case VK_FORMAT_R32_SFLOAT:
            break;
        case VK_FORMAT_R32G32_UINT:
            break;
        case VK_FORMAT_R32G32_SINT:
            break;
        case VK_FORMAT_R32G32_SFLOAT:
            break;
        case VK_FORMAT_R32G32B32_UINT:
            break;
        case VK_FORMAT_R32G32B32_SINT:
            break;
        case VK_FORMAT_R32G32B32_SFLOAT:
            break;
        case VK_FORMAT_R32G32B32A32_UINT:
            break;
        case VK_FORMAT_R32G32B32A32_SINT:
            break;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            break;
        case VK_FORMAT_R64_UINT:
            break;
        case VK_FORMAT_R64_SINT:
            break;
        case VK_FORMAT_R64_SFLOAT:
            break;
        case VK_FORMAT_R64G64_UINT:
            break;
        case VK_FORMAT_R64G64_SINT:
            break;
        case VK_FORMAT_R64G64_SFLOAT:
            break;
        case VK_FORMAT_R64G64B64_UINT:
            break;
        case VK_FORMAT_R64G64B64_SINT:
            break;
        case VK_FORMAT_R64G64B64_SFLOAT:
            break;
        case VK_FORMAT_R64G64B64A64_UINT:
            break;
        case VK_FORMAT_R64G64B64A64_SINT:
            break;
        case VK_FORMAT_R64G64B64A64_SFLOAT:
            break;
        case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
            break;
        case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            break;
        case VK_FORMAT_D16_UNORM:
            break;
        case VK_FORMAT_X8_D24_UNORM_PACK32:
            break;
        case VK_FORMAT_D32_SFLOAT:
            break;
        case VK_FORMAT_S8_UINT:
            break;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            break;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            break;
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            break;
        case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
            break;
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
            break;
        case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
            break;
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
            break;
        case VK_FORMAT_BC2_UNORM_BLOCK:
            break;
        case VK_FORMAT_BC2_SRGB_BLOCK:
            break;
        case VK_FORMAT_BC3_UNORM_BLOCK:
            break;
        case VK_FORMAT_BC3_SRGB_BLOCK:
            break;
        case VK_FORMAT_BC4_UNORM_BLOCK:
            break;
        case VK_FORMAT_BC4_SNORM_BLOCK:
            break;
        case VK_FORMAT_BC5_UNORM_BLOCK:
            break;
        case VK_FORMAT_BC5_SNORM_BLOCK:
            break;
        case VK_FORMAT_BC6H_UFLOAT_BLOCK:
            break;
        case VK_FORMAT_BC6H_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_BC7_UNORM_BLOCK:
            break;
        case VK_FORMAT_BC7_SRGB_BLOCK:
            break;
        case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            break;
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            break;
        case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            break;
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            break;
        case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            break;
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            break;
        case VK_FORMAT_EAC_R11_UNORM_BLOCK:
            break;
        case VK_FORMAT_EAC_R11_SNORM_BLOCK:
            break;
        case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
            break;
        case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            break;
        case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
            break;
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            break;
        case VK_FORMAT_G8B8G8R8_422_UNORM:
            break;
        case VK_FORMAT_B8G8R8G8_422_UNORM:
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
            break;
        case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
            break;
        case VK_FORMAT_R10X6_UNORM_PACK16:
            break;
        case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
            break;
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
            break;
        case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
            break;
        case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            break;
        case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            break;
        case VK_FORMAT_R12X4_UNORM_PACK16:
            break;
        case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
            break;
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
            break;
        case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
            break;
        case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            break;
        case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            break;
        case VK_FORMAT_G16B16G16R16_422_UNORM:
            break;
        case VK_FORMAT_B16G16R16G16_422_UNORM:
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
            break;
        case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            break;
        case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
            break;
        case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
            break;
        case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
            break;
        case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
            break;
        case VK_FORMAT_A4R4G4B4_UNORM_PACK16:
            break;
        case VK_FORMAT_A4B4G4R4_UNORM_PACK16:
            break;
        case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
            break;
        case VK_FORMAT_A1B5G5R5_UNORM_PACK16:
            break;
        case VK_FORMAT_A8_UNORM:
            break;
        case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
            break;
        case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
            break;
        case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
            break;
        case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
            break;
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
            break;
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
            break;
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
            break;
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            break;
        case VK_FORMAT_ASTC_3x3x3_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_3x3x3_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_3x3x3_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x3x3_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x3x3_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x3x3_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x4x3_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x4x3_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x4x3_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x4x4_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x4x4_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_4x4x4_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x4x4_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x4x4_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x4x4_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x5x4_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x5x4_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x5x4_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x5x5_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x5x5_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_5x5x5_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x5x5_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x5x5_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x5x5_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x6x5_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x6x5_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x6x5_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x6x6_UNORM_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x6x6_SRGB_BLOCK_EXT:
            break;
        case VK_FORMAT_ASTC_6x6x6_SFLOAT_BLOCK_EXT:
            break;
        case VK_FORMAT_R8_BOOL_ARM:
            break;
        case VK_FORMAT_R16_SFLOAT_FPENCODING_BFLOAT16_ARM:
            break;
        case VK_FORMAT_R8_SFLOAT_FPENCODING_FLOAT8E4M3_ARM:
            break;
        case VK_FORMAT_R8_SFLOAT_FPENCODING_FLOAT8E5M2_ARM:
            break;
        case VK_FORMAT_R16G16_SFIXED5_NV:
            break;
        case VK_FORMAT_R10X6_UINT_PACK16_ARM:
            break;
        case VK_FORMAT_R10X6G10X6_UINT_2PACK16_ARM:
            break;
        case VK_FORMAT_R10X6G10X6B10X6A10X6_UINT_4PACK16_ARM:
            break;
        case VK_FORMAT_R12X4_UINT_PACK16_ARM:
            break;
        case VK_FORMAT_R12X4G12X4_UINT_2PACK16_ARM:
            break;
        case VK_FORMAT_R12X4G12X4B12X4A12X4_UINT_4PACK16_ARM:
            break;
        case VK_FORMAT_R14X2_UINT_PACK16_ARM:
            break;
        case VK_FORMAT_R14X2G14X2_UINT_2PACK16_ARM:
            break;
        case VK_FORMAT_R14X2G14X2B14X2A14X2_UINT_4PACK16_ARM:
            break;
        case VK_FORMAT_R14X2_UNORM_PACK16_ARM:
            break;
        case VK_FORMAT_R14X2G14X2_UNORM_2PACK16_ARM:
            break;
        case VK_FORMAT_R14X2G14X2B14X2A14X2_UNORM_4PACK16_ARM:
            break;
        case VK_FORMAT_G14X2_B14X2R14X2_2PLANE_420_UNORM_3PACK16_ARM:
            break;
        case VK_FORMAT_G14X2_B14X2R14X2_2PLANE_422_UNORM_3PACK16_ARM:
            break;
        case VK_FORMAT_MAX_ENUM:
            break;
        default:
            assert( false );
            return ResourceFormat::Unknown;
        }
    }

    inline VkMemoryPropertyFlags ConvertToVkMemoryPropertyFlags( ResourceAccess resourceAccess )
    {
        VkMemoryPropertyFlags flags = 0;

        if ( HasAnyFlags( resourceAccess, ResourceAccess::CpuWrite ) )
        {
            flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }
        else if ( HasAnyFlags( resourceAccess, ResourceAccess::CpuRead ) )
        {
            flags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            flags |= VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        }
        else if ( HasAnyFlags( resourceAccess, ResourceAccess::Default ) )
        {
            flags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }

        return flags;
    }

    inline VkPipelineStageFlags2 ConvertToVkPipelineStageFlags2( ResourceState resourceState )
    {
        VkPipelineStageFlags2 vkPipelineStageFlags2 = VK_PIPELINE_STAGE_2_NONE;

        if ( HasAnyFlags( resourceState, ResourceState::RenderTarget ) )
        {
            vkPipelineStageFlags2 |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::Present ) )
        {
            return VK_PIPELINE_STAGE_2_NONE;
        }

        return vkPipelineStageFlags2;
    }

    inline VkAccessFlags2 ConvertToVkAccessFlags2( ResourceState resourceState )
    {
        VkAccessFlags2 vkAccessFlags2 = VK_ACCESS_2_NONE;

        if ( HasAnyFlags( resourceState, ResourceState::RenderTarget ) )
        {
            vkAccessFlags2 |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::Present ) )
        {
            return VK_ACCESS_2_NONE;
        }

        return vkAccessFlags2;
    }

    inline VkImageLayout ConvertToVkImageLayout( ResourceState resourceState )
    {
        VkImageLayout vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if ( HasAnyFlags( resourceState, ResourceState::RenderTarget ) )
        {
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        if ( HasAnyFlags( resourceState, ResourceState::Present ) )
        {
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        return vkImageLayout;
    }

    inline VkImageAspectFlags ConvertToVkImageAspectFlags( ResourceState resourceState )
    {
        VkImageAspectFlags vkImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

        if ( HasAnyFlags( resourceState, ResourceState::DepthRead )
            || HasAnyFlags( resourceState, ResourceState::DepthWrite ) )
        {
            // TODO
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        return vkImageAspectFlags;
    }
}