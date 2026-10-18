#include "VulkanFlagConverter.h"

namespace agl
{
    VkFormat VulkanFlagConvertor::ConvertToFormat( ResourceFormat format )
    {
        switch ( format )
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
            return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case ResourceFormat::R10G10B10A2_UINT:
            return VK_FORMAT_A2B10G10R10_UINT_PACK32;
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
            return VK_FORMAT_R16G16_SFLOAT;
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
            return VK_FORMAT_R32_SFLOAT;
        case ResourceFormat::R32_UINT:
            break;
        case ResourceFormat::R32_SINT:
            break;
        case ResourceFormat::R24G8_TYPELESS:
            break;
        case ResourceFormat::D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
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

    ResourceFormat VulkanFlagConvertor::ConvertToResourceFormat( VkFormat format )
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

    VkMemoryPropertyFlags VulkanFlagConvertor::ConvertToMemoryPropertyFlags( ResourceAccess resourceAccess )
    {
        if ( HasAnyFlags( resourceAccess, ResourceAccess::CpuRead ) )
        {
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        }

        if ( HasAnyFlags( resourceAccess, ResourceAccess::CpuWrite ) )
        {
            return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        }

        return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }

    VkPipelineStageFlags2 VulkanFlagConvertor::ConvertToPipelineStageFlags2( ResourceState resourceState )
    {
        VkPipelineStageFlags2 vkPipelineStageFlags2 = VK_PIPELINE_STAGE_2_NONE;

        if ( HasAnyFlags( resourceState, ResourceState::RenderTarget ) )
        {
            vkPipelineStageFlags2 |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::CopySource ) )
        {
            vkPipelineStageFlags2 |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::CopyDest ) )
        {
            vkPipelineStageFlags2 |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::Present ) )
        {
            return VK_PIPELINE_STAGE_2_NONE;
        }

        return vkPipelineStageFlags2;
    }

    VkAccessFlags2 VulkanFlagConvertor::ConvertToAccessFlags2( ResourceState resourceState )
    {
        VkAccessFlags2 vkAccessFlags2 = VK_ACCESS_2_NONE;

        if ( HasAnyFlags( resourceState, ResourceState::RenderTarget ) )
        {
            vkAccessFlags2 |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::CopySource ) )
        {
            vkAccessFlags2 |= VK_ACCESS_2_TRANSFER_READ_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::CopyDest ) )
        {
            vkAccessFlags2 |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
        }

        if ( HasAnyFlags( resourceState, ResourceState::Present ) )
        {
            return VK_ACCESS_2_NONE;
        }

        return vkAccessFlags2;
    }

    VkImageLayout VulkanFlagConvertor::ConvertToImageLayout( ResourceState resourceState )
    {
        VkImageLayout vkImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        if ( HasAnyFlags( resourceState, ResourceState::RenderTarget ) )
        {
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        if ( HasAnyFlags( resourceState, ResourceState::NonPixelShaderResource | ResourceState::PixelShaderResource ) )
        {
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }

        if ( HasAnyFlags( resourceState, ResourceState::CopySource ) )
        {
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        }

        if ( HasAnyFlags( resourceState, ResourceState::CopyDest ) )
        {
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        }

        if ( HasAnyFlags( resourceState, ResourceState::Present ) )
        {
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }

        return vkImageLayout;
    }

    VkImageAspectFlags VulkanFlagConvertor::ConvertToImageAspectFlags( ResourceState resourceState )
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

    VkPolygonMode VulkanFlagConvertor::ConvertToPolygonMode( FillMode fillMode )
    {
        switch ( fillMode )
        {
        case FillMode::Wireframe:
            return VK_POLYGON_MODE_LINE;
        case FillMode::Solid:
            return VK_POLYGON_MODE_FILL;
        default:
            assert( false );
            return VK_POLYGON_MODE_LINE;
        }
    }

    VkCullModeFlags VulkanFlagConvertor::ConvertToCullMode( CullMode cullMode )
    {
        switch ( cullMode )
        {
        case CullMode::None:
            return VK_CULL_MODE_NONE;
        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
        default:
            assert( false );
            return VK_CULL_MODE_NONE;
        }
    }

    VkCompareOp VulkanFlagConvertor::ConvertToCompareOp( ComparisonFunc comparisonFunc )
    {
        switch ( comparisonFunc )
        {
        case ComparisonFunc::Never:
            return VK_COMPARE_OP_NEVER;
        case ComparisonFunc::Less:
            return VK_COMPARE_OP_LESS;
        case ComparisonFunc::Equal:
            return VK_COMPARE_OP_EQUAL;
        case ComparisonFunc::LessEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case ComparisonFunc::Greater:
            return VK_COMPARE_OP_GREATER;
        case ComparisonFunc::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case ComparisonFunc::GreaterEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case ComparisonFunc::Always:
            return VK_COMPARE_OP_ALWAYS;
        default:
            assert( false );
            return VK_COMPARE_OP_NEVER;
        }
    }

    VkStencilOp VulkanFlagConvertor::ConvertToStencilOp( StencilOp stencilOp )
    {
        switch ( stencilOp )
        {
        case StencilOp::Keep:
            return VK_STENCIL_OP_KEEP;
        case StencilOp::Zero:
            return VK_STENCIL_OP_ZERO;
        case StencilOp::Replace:
            return VK_STENCIL_OP_REPLACE;
        case StencilOp::IncrSat:
            return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
        case StencilOp::DecrSat:
            return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
        case StencilOp::Invert:
            return VK_STENCIL_OP_INVERT;
        case StencilOp::Incr:
            return VK_STENCIL_OP_INCREMENT_AND_WRAP;
        case StencilOp::Decr:
            return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        default:
            assert( false );
            return VK_STENCIL_OP_KEEP;
        }
    }

    VkBlendFactor VulkanFlagConvertor::ConvertToBlend( Blend blend )
    {
        switch ( blend )
        {
        case Blend::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case Blend::One:
            return VK_BLEND_FACTOR_ONE;
        case Blend::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case Blend::InvSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case Blend::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case Blend::InvSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case Blend::DestAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case Blend::InvDestAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case Blend::DestColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case Blend::InvDestColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case Blend::SrcAlphaSat:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case Blend::BlendFactor:
            return VK_BLEND_FACTOR_CONSTANT_COLOR;
        case Blend::InvBlendFactor:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case Blend::Src1Color:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case Blend::InvSrc1Color:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case Blend::Src1Alpha:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case Blend::InvSrc1Alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            assert( false );
            return VK_BLEND_FACTOR_ZERO;
        }
    }

    VkBlendOp VulkanFlagConvertor::ConvertToBlendOp( BlendOp blendOp )
    {
        switch ( blendOp )
        {
        case BlendOp::Add:
            return VK_BLEND_OP_ADD;
        case BlendOp::Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOp::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOp::Min:
            return VK_BLEND_OP_MIN;
        case BlendOp::Max:
            return VK_BLEND_OP_MAX;
        default:
            assert( false );
            return VK_BLEND_OP_ADD;
        }
    }

    VkColorComponentFlags VulkanFlagConvertor::ConvertToColorComponent( ColorWriteEnable colorWriteEnable )
    {
        VkColorComponentFlags flags = 0;

        if ( HasAnyFlags( colorWriteEnable, ColorWriteEnable::Red ) )
        {
            flags |= VK_COLOR_COMPONENT_R_BIT;
        }

        if ( HasAnyFlags( colorWriteEnable, ColorWriteEnable::Green ) )
        {
            flags |= VK_COLOR_COMPONENT_G_BIT;
        }

        if ( HasAnyFlags( colorWriteEnable, ColorWriteEnable::Blue ) )
        {
            flags |= VK_COLOR_COMPONENT_B_BIT;
        }

        if ( HasAnyFlags( colorWriteEnable, ColorWriteEnable::Alpha ) )
        {
            flags |= VK_COLOR_COMPONENT_A_BIT;
        }

        return flags;
    }

    VkFilter VulkanFlagConvertor::ConvertToMagFilter( TextureFilter filter )
    {
        if ( HasAnyFlags( filter, TextureFilter::MagLinear ) )
        {
            return VK_FILTER_LINEAR;
        }

        return VK_FILTER_NEAREST;
    }

    VkFilter VulkanFlagConvertor::ConvertToMinFilter( TextureFilter filter )
    {
        if ( HasAnyFlags( filter, TextureFilter::MinLinear ) )
        {
            return VK_FILTER_LINEAR;
        }

        return VK_FILTER_NEAREST;
    }

    VkSamplerMipmapMode VulkanFlagConvertor::ConvertToMipmapMode( TextureFilter filter )
    {
        if ( HasAnyFlags( filter, TextureFilter::MipLinear ) )
        {
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        }

        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    }

    VkSamplerAddressMode VulkanFlagConvertor::ConvertToAddressMode( TextureAddressMode addressMode )
    {
        switch ( addressMode )
        {
        case TextureAddressMode::Wrap:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case TextureAddressMode::Mirror:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case TextureAddressMode::Clamp:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case TextureAddressMode::Border:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case TextureAddressMode::MirrorOnce:
            return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
        default:
            assert( false );
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
    }
}
