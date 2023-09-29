#pragma once

#include "GraphicsApiResource.h"

#include <dxgi.h>

namespace agl
{
	inline DXGI_FORMAT ConvertFormatToDxgiFormat( ResourceFormat format )
	{
		switch ( format )
		{
		case ResourceFormat::Unknown:
			return DXGI_FORMAT_UNKNOWN;
		case ResourceFormat::R32G32B32A32_TYPELESS:
			return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		case ResourceFormat::R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ResourceFormat::R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case ResourceFormat::R32G32B32A32_SINT:
			return DXGI_FORMAT_R32G32B32A32_SINT;
		case ResourceFormat::R32G32B32_TYPELESS:
			return DXGI_FORMAT_R32G32B32_TYPELESS;
		case ResourceFormat::R32G32B32_FLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case ResourceFormat::R32G32B32_UINT:
			return DXGI_FORMAT_R32G32B32_UINT;
		case ResourceFormat::R32G32B32_SINT:
			return DXGI_FORMAT_R32G32B32_SINT;
		case ResourceFormat::R16G16B16A16_TYPELESS:
			return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case ResourceFormat::R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case ResourceFormat::R16G16B16A16_UNORM:
			return DXGI_FORMAT_R16G16B16A16_UNORM;
		case ResourceFormat::R16G16B16A16_UINT:
			return DXGI_FORMAT_R16G16B16A16_UINT;
		case ResourceFormat::R16G16B16A16_SNORM:
			return DXGI_FORMAT_R16G16B16A16_SNORM;
		case ResourceFormat::R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_SINT;
		case ResourceFormat::R32G32_TYPELESS:
			return DXGI_FORMAT_R32G32_TYPELESS;
		case ResourceFormat::R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
		case ResourceFormat::R32G32_UINT:
			return DXGI_FORMAT_R32G32_UINT;
		case ResourceFormat::R32G32_SINT:
			return DXGI_FORMAT_R32G32_SINT;
		case ResourceFormat::R32G8X24_TYPELESS:
			return DXGI_FORMAT_R32G8X24_TYPELESS;
		case ResourceFormat::D32_FLOAT_S8X24_UINT:
			return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		case ResourceFormat::R32_FLOAT_X8X24_TYPELESS:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		case ResourceFormat::X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		case ResourceFormat::R10G10B10A2_TYPELESS:
			return DXGI_FORMAT_R10G10B10A2_TYPELESS;
		case ResourceFormat::R10G10B10A2_UNORM:
			return DXGI_FORMAT_R10G10B10A2_UNORM;
		case ResourceFormat::R10G10B10A2_UINT:
			return DXGI_FORMAT_R10G10B10A2_UINT;
		case ResourceFormat::R11G11B10_FLOAT:
			return DXGI_FORMAT_R11G11B10_FLOAT;
		case ResourceFormat::R8G8B8A8_TYPELESS:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case ResourceFormat::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ResourceFormat::R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		case ResourceFormat::R8G8B8A8_UINT:
			return DXGI_FORMAT_R8G8B8A8_UINT;
		case ResourceFormat::R8G8B8A8_SNORM:
			return DXGI_FORMAT_R8G8B8A8_SNORM;
		case ResourceFormat::R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_SINT;
		case ResourceFormat::R16G16_TYPELESS:
			return DXGI_FORMAT_R16G16_TYPELESS;
		case ResourceFormat::R16G16_FLOAT:
			return DXGI_FORMAT_R16G16_FLOAT;
		case ResourceFormat::R16G16_UNORM:
			return DXGI_FORMAT_R16G16_UNORM;
		case ResourceFormat::R16G16_UINT:
			return DXGI_FORMAT_R16G16_UINT;
		case ResourceFormat::R16G16_SNORM:
			return DXGI_FORMAT_R16G16_SNORM;
		case ResourceFormat::R16G16_SINT:
			return DXGI_FORMAT_R16G16_SINT;
		case ResourceFormat::R32_TYPELESS:
			return DXGI_FORMAT_R32_TYPELESS;
		case ResourceFormat::D32_FLOAT:
			return DXGI_FORMAT_D32_FLOAT;
		case ResourceFormat::R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;
		case ResourceFormat::R32_UINT:
			return DXGI_FORMAT_R32_UINT;
		case ResourceFormat::R32_SINT:
			return DXGI_FORMAT_R32_SINT;
		case ResourceFormat::R24G8_TYPELESS:
			return DXGI_FORMAT_R24G8_TYPELESS;
		case ResourceFormat::D24_UNORM_S8_UINT:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		case ResourceFormat::R24_UNORM_X8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case ResourceFormat::X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		case ResourceFormat::R8G8_TYPELESS:
			return DXGI_FORMAT_R8G8_TYPELESS;
		case ResourceFormat::R8G8_UNORM:
			return DXGI_FORMAT_R8G8_UNORM;
		case ResourceFormat::R8G8_UINT:
			return DXGI_FORMAT_R8G8_UINT;
		case ResourceFormat::R8G8_SNORM:
			return DXGI_FORMAT_R8G8_SNORM;
		case ResourceFormat::R8G8_SINT:
			return DXGI_FORMAT_R8G8_SINT;
		case ResourceFormat::R16_TYPELESS:
			return DXGI_FORMAT_R16_TYPELESS;
		case ResourceFormat::R16_FLOAT:
			return DXGI_FORMAT_R16_FLOAT;
		case ResourceFormat::D16_UNORM:
			return DXGI_FORMAT_D16_UNORM;
		case ResourceFormat::R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;
		case ResourceFormat::R16_UINT:
			return DXGI_FORMAT_R16_UINT;
		case ResourceFormat::R16_SNORM:
			return DXGI_FORMAT_R16_SNORM;
		case ResourceFormat::R16_SINT:
			return DXGI_FORMAT_R16_SINT;
		case ResourceFormat::R8_TYPELESS:
			return DXGI_FORMAT_R8_TYPELESS;
		case ResourceFormat::R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
		case ResourceFormat::R8_UINT:
			return DXGI_FORMAT_R8_UINT;
		case ResourceFormat::R8_SNORM:
			return DXGI_FORMAT_R8_SNORM;
		case ResourceFormat::R8_SINT:
			return DXGI_FORMAT_R8_SINT;
		case ResourceFormat::A8_UNORM:
			return DXGI_FORMAT_A8_UNORM;
		case ResourceFormat::R1_UNORM:
			return DXGI_FORMAT_R1_UNORM;
		case ResourceFormat::R9G9B9E5_SHAREDEXP:
			return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
		case ResourceFormat::R8G8_B8G8_UNORM:
			return DXGI_FORMAT_R8G8_B8G8_UNORM;
		case ResourceFormat::G8R8_G8B8_UNORM:
			return DXGI_FORMAT_G8R8_G8B8_UNORM;
		case ResourceFormat::BC1_TYPELESS:
			return DXGI_FORMAT_BC1_TYPELESS;
		case ResourceFormat::BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM;
		case ResourceFormat::BC1_UNORM_SRGB:
			return DXGI_FORMAT_BC1_UNORM_SRGB;
		case ResourceFormat::BC2_TYPELESS:
			return DXGI_FORMAT_BC2_TYPELESS;
		case ResourceFormat::BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM;
		case ResourceFormat::BC2_UNORM_SRGB:
			return DXGI_FORMAT_BC2_UNORM_SRGB;
		case ResourceFormat::BC3_TYPELESS:
			return DXGI_FORMAT_BC3_TYPELESS;
		case ResourceFormat::BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM;
		case ResourceFormat::BC3_UNORM_SRGB:
			return DXGI_FORMAT_BC3_UNORM_SRGB;
		case ResourceFormat::BC4_TYPELESS:
			return DXGI_FORMAT_BC4_TYPELESS;
		case ResourceFormat::BC4_UNORM:
			return DXGI_FORMAT_BC4_UNORM;
		case ResourceFormat::BC4_SNORM:
			return DXGI_FORMAT_BC4_SNORM;
		case ResourceFormat::BC5_TYPELESS:
			return DXGI_FORMAT_BC5_TYPELESS;
		case ResourceFormat::BC5_UNORM:
			return DXGI_FORMAT_BC5_UNORM;
		case ResourceFormat::BC5_SNORM:
			return DXGI_FORMAT_BC5_SNORM;
		case ResourceFormat::B5G6R5_UNORM:
			return DXGI_FORMAT_B5G6R5_UNORM;
		case ResourceFormat::B5G5R5A1_UNORM:
			return DXGI_FORMAT_B5G5R5A1_UNORM;
		case ResourceFormat::B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case ResourceFormat::B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM;
		case ResourceFormat::R10G10B10_XR_BIAS_A2_UNORM:
			return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
		case ResourceFormat::B8G8R8A8_TYPELESS:
			return DXGI_FORMAT_B8G8R8A8_TYPELESS;
		case ResourceFormat::B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
		case ResourceFormat::B8G8R8X8_TYPELESS:
			return DXGI_FORMAT_B8G8R8X8_TYPELESS;
		case ResourceFormat::B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
		case ResourceFormat::BC6H_TYPELESS:
			return DXGI_FORMAT_BC6H_TYPELESS;
		case ResourceFormat::BC6H_UF16:
			return DXGI_FORMAT_BC6H_UF16;
		case ResourceFormat::BC6H_SF16:
			return DXGI_FORMAT_BC6H_SF16;
		case ResourceFormat::BC7_TYPELESS:
			return DXGI_FORMAT_BC7_TYPELESS;
		case ResourceFormat::BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM;
		case ResourceFormat::BC7_UNORM_SRGB:
			return DXGI_FORMAT_BC7_UNORM_SRGB;
		default:
			assert( false );
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	inline ResourceFormat ConvertDxgiFormatToFormat( DXGI_FORMAT format )
	{
		switch ( format )
		{
		case DXGI_FORMAT_UNKNOWN:
			return ResourceFormat::Unknown;
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			return ResourceFormat::R32G32B32A32_TYPELESS;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return ResourceFormat::R32G32B32A32_FLOAT;
		case DXGI_FORMAT_R32G32B32A32_UINT:
			return ResourceFormat::R32G32B32A32_UINT;
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return ResourceFormat::R32G32B32A32_SINT;
		case DXGI_FORMAT_R32G32B32_TYPELESS:
			return ResourceFormat::R32G32B32_TYPELESS;
		case DXGI_FORMAT_R32G32B32_FLOAT:
			return ResourceFormat::R32G32B32_FLOAT;
		case DXGI_FORMAT_R32G32B32_UINT:
			return ResourceFormat::R32G32B32_UINT;
		case DXGI_FORMAT_R32G32B32_SINT:
			return ResourceFormat::R32G32B32_SINT;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			return ResourceFormat::R16G16B16A16_TYPELESS;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return ResourceFormat::R16G16B16A16_FLOAT;
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			return ResourceFormat::R16G16B16A16_UNORM;
		case DXGI_FORMAT_R16G16B16A16_UINT:
			return ResourceFormat::R16G16B16A16_UINT;
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			return ResourceFormat::R16G16B16A16_SNORM;
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return ResourceFormat::R16G16B16A16_SINT;
		case DXGI_FORMAT_R32G32_TYPELESS:
			return ResourceFormat::R32G32_TYPELESS;
		case DXGI_FORMAT_R32G32_FLOAT:
			return ResourceFormat::R32G32_FLOAT;
		case DXGI_FORMAT_R32G32_UINT:
			return ResourceFormat::R32G32_UINT;
		case DXGI_FORMAT_R32G32_SINT:
			return ResourceFormat::R32G32_SINT;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			return ResourceFormat::R32G8X24_TYPELESS;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			return ResourceFormat::D32_FLOAT_S8X24_UINT;
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			return ResourceFormat::R32_FLOAT_X8X24_TYPELESS;
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return ResourceFormat::X32_TYPELESS_G8X24_UINT;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			return ResourceFormat::R10G10B10A2_TYPELESS;
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			return ResourceFormat::R10G10B10A2_UNORM;
		case DXGI_FORMAT_R10G10B10A2_UINT:
			return ResourceFormat::R10G10B10A2_UINT;
		case DXGI_FORMAT_R11G11B10_FLOAT:
			return ResourceFormat::R11G11B10_FLOAT;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			return ResourceFormat::R8G8B8A8_TYPELESS;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return ResourceFormat::R8G8B8A8_UNORM;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return ResourceFormat::R8G8B8A8_UNORM_SRGB;
		case DXGI_FORMAT_R8G8B8A8_UINT:
			return ResourceFormat::R8G8B8A8_UINT;
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			return ResourceFormat::R8G8B8A8_SNORM;
		case DXGI_FORMAT_R8G8B8A8_SINT:
			return ResourceFormat::R8G8B8A8_SINT;
		case DXGI_FORMAT_R16G16_TYPELESS:
			return ResourceFormat::R16G16_TYPELESS;
		case DXGI_FORMAT_R16G16_FLOAT:
			return ResourceFormat::R16G16_FLOAT;
		case DXGI_FORMAT_R16G16_UNORM:
			return ResourceFormat::R16G16_UNORM;
		case DXGI_FORMAT_R16G16_UINT:
			return ResourceFormat::R16G16_UINT;
		case DXGI_FORMAT_R16G16_SNORM:
			return ResourceFormat::R16G16_SNORM;
		case DXGI_FORMAT_R16G16_SINT:
			return ResourceFormat::R16G16_SINT;
		case DXGI_FORMAT_R32_TYPELESS:
			return ResourceFormat::R32_TYPELESS;
		case DXGI_FORMAT_D32_FLOAT:
			return ResourceFormat::D32_FLOAT;
		case DXGI_FORMAT_R32_FLOAT:
			return ResourceFormat::R32_FLOAT;
		case DXGI_FORMAT_R32_UINT:
			return ResourceFormat::R32_UINT;
		case DXGI_FORMAT_R32_SINT:
			return ResourceFormat::R32_SINT;
		case DXGI_FORMAT_R24G8_TYPELESS:
			return ResourceFormat::R24G8_TYPELESS;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			return ResourceFormat::D24_UNORM_S8_UINT;
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			return ResourceFormat::R24_UNORM_X8_TYPELESS;
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return ResourceFormat::X24_TYPELESS_G8_UINT;
		case DXGI_FORMAT_R8G8_TYPELESS:
			return ResourceFormat::R8G8_TYPELESS;
		case DXGI_FORMAT_R8G8_UNORM:
			return ResourceFormat::R8G8_UNORM;
		case DXGI_FORMAT_R8G8_UINT:
			return ResourceFormat::R8G8_UINT;
		case DXGI_FORMAT_R8G8_SNORM:
			return ResourceFormat::R8G8_SNORM;
		case DXGI_FORMAT_R8G8_SINT:
			return ResourceFormat::R8G8_SINT;
		case DXGI_FORMAT_R16_TYPELESS:
			return ResourceFormat::R16_TYPELESS;
		case DXGI_FORMAT_R16_FLOAT:
			return ResourceFormat::R16_FLOAT;
		case DXGI_FORMAT_D16_UNORM:
			return ResourceFormat::D16_UNORM;
		case DXGI_FORMAT_R16_UNORM:
			return ResourceFormat::R16_UNORM;
		case DXGI_FORMAT_R16_UINT:
			return ResourceFormat::R16_UINT;
		case DXGI_FORMAT_R16_SNORM:
			return ResourceFormat::R16_SNORM;
		case DXGI_FORMAT_R16_SINT:
			return ResourceFormat::R16_SINT;
		case DXGI_FORMAT_R8_TYPELESS:
			return ResourceFormat::R8_TYPELESS;
		case DXGI_FORMAT_R8_UNORM:
			return ResourceFormat::R8_UNORM;
		case DXGI_FORMAT_R8_UINT:
			return ResourceFormat::R8_UINT;
		case DXGI_FORMAT_R8_SNORM:
			return ResourceFormat::R8_SNORM;
		case DXGI_FORMAT_R8_SINT:
			return ResourceFormat::R8_SINT;
		case DXGI_FORMAT_A8_UNORM:
			return ResourceFormat::A8_UNORM;
		case DXGI_FORMAT_R1_UNORM:
			return ResourceFormat::R1_UNORM;
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			return ResourceFormat::R9G9B9E5_SHAREDEXP;
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
			return ResourceFormat::R8G8_B8G8_UNORM;
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			return ResourceFormat::G8R8_G8B8_UNORM;
		case DXGI_FORMAT_BC1_TYPELESS:
			return ResourceFormat::BC1_TYPELESS;
		case DXGI_FORMAT_BC1_UNORM:
			return ResourceFormat::BC1_UNORM;
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return ResourceFormat::BC1_UNORM_SRGB;
		case DXGI_FORMAT_BC2_TYPELESS:
			return ResourceFormat::BC2_TYPELESS;
		case DXGI_FORMAT_BC2_UNORM:
			return ResourceFormat::BC2_UNORM;
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			return ResourceFormat::BC2_UNORM_SRGB;
		case DXGI_FORMAT_BC3_TYPELESS:
			return ResourceFormat::BC3_TYPELESS;
		case DXGI_FORMAT_BC3_UNORM:
			return ResourceFormat::BC3_UNORM;
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return ResourceFormat::BC3_UNORM_SRGB;
		case DXGI_FORMAT_BC4_TYPELESS:
			return ResourceFormat::BC4_TYPELESS;
		case DXGI_FORMAT_BC4_UNORM:
			return ResourceFormat::BC4_UNORM;
		case DXGI_FORMAT_BC4_SNORM:
			return ResourceFormat::BC4_SNORM;
		case DXGI_FORMAT_BC5_TYPELESS:
			return ResourceFormat::BC5_TYPELESS;
		case DXGI_FORMAT_BC5_UNORM:
			return ResourceFormat::BC5_UNORM;
		case DXGI_FORMAT_BC5_SNORM:
			return ResourceFormat::BC5_SNORM;
		case DXGI_FORMAT_B5G6R5_UNORM:
			return ResourceFormat::B5G6R5_UNORM;
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return ResourceFormat::B5G5R5A1_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return ResourceFormat::B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return ResourceFormat::B8G8R8X8_UNORM;
		case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			return ResourceFormat::R10G10B10_XR_BIAS_A2_UNORM;
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			return ResourceFormat::B8G8R8A8_TYPELESS;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return ResourceFormat::B8G8R8A8_UNORM_SRGB;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			return ResourceFormat::B8G8R8X8_TYPELESS;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return ResourceFormat::B8G8R8X8_UNORM_SRGB;
		case DXGI_FORMAT_BC6H_TYPELESS:
			return ResourceFormat::BC6H_TYPELESS;
		case DXGI_FORMAT_BC6H_UF16:
			return ResourceFormat::BC6H_UF16;
		case DXGI_FORMAT_BC6H_SF16:
			return ResourceFormat::BC6H_SF16;
		case DXGI_FORMAT_BC7_TYPELESS:
			return ResourceFormat::BC7_TYPELESS;
		case DXGI_FORMAT_BC7_UNORM:
			return ResourceFormat::BC7_UNORM;
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return ResourceFormat::BC7_UNORM_SRGB;
		default:
			assert( false );
			return ResourceFormat::Unknown;
		}
	}

	inline DXGI_FORMAT ConvertToDxgiGammaFormat( DXGI_FORMAT format )
	{
		switch ( format )
		{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC1_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC1_UNORM:
			return DXGI_FORMAT_BC1_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC2_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC2_UNORM:
			return DXGI_FORMAT_BC2_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC3_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC3_UNORM:
			return DXGI_FORMAT_BC3_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_BC7_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC7_UNORM:
			return DXGI_FORMAT_BC7_UNORM_SRGB;
			break;
		default:
			break;
		}

		return format;
	}

	inline DXGI_FORMAT ConvertToDxgiLinearFormat( DXGI_FORMAT format )
	{
		switch ( format )
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return DXGI_FORMAT_BC1_UNORM;
			break;
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			return DXGI_FORMAT_BC2_UNORM;
			break;
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return DXGI_FORMAT_BC3_UNORM;
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
			break;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM;
			break;
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return DXGI_FORMAT_BC7_UNORM;
			break;
		default:
			break;
		}

		return format;
	}

	inline DXGI_FORMAT ConvertDxgiFormatForDSV( DXGI_FORMAT format )
	{
		switch ( format )
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		}

		return format;
	}

	inline DXGI_FORMAT ConvertDxgiFormatForSRV( DXGI_FORMAT format )
	{
		switch ( format )
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		}

		return format;
	}

	inline ResourceFormat ConvertToGammaFormat( ResourceFormat format )
	{
		switch ( format )
		{
		case ResourceFormat::R8G8B8A8_TYPELESS:
			[[fallthrough]];
		case ResourceFormat::R8G8B8A8_UNORM:
			return ResourceFormat::R8G8B8A8_UNORM_SRGB;
			break;
		case ResourceFormat::BC1_TYPELESS:
			[[fallthrough]];
		case ResourceFormat::BC1_UNORM:
			return ResourceFormat::BC1_UNORM_SRGB;
			break;
		case ResourceFormat::BC2_TYPELESS:
			[[fallthrough]];
		case ResourceFormat::BC2_UNORM:
			return ResourceFormat::BC2_UNORM_SRGB;
			break;
		case ResourceFormat::BC3_TYPELESS:
			[[fallthrough]];
		case ResourceFormat::BC3_UNORM:
			return ResourceFormat::BC3_UNORM_SRGB;
			break;
		case ResourceFormat::B8G8R8A8_TYPELESS:
			[[fallthrough]];
		case ResourceFormat::B8G8R8A8_UNORM:
			return ResourceFormat::B8G8R8A8_UNORM_SRGB;
			break;
		case ResourceFormat::B8G8R8X8_TYPELESS:
			[[fallthrough]];
		case ResourceFormat::B8G8R8X8_UNORM:
			return ResourceFormat::B8G8R8X8_UNORM_SRGB;
			break;
		case ResourceFormat::BC7_TYPELESS:
			[[fallthrough]];
		case ResourceFormat::BC7_UNORM:
			return ResourceFormat::BC7_UNORM_SRGB;
			break;
		default:
			break;
		}

		return format;
	}

	inline ResourceFormat ConvertToLinearFormat( ResourceFormat format )
	{
		switch ( format )
		{
		case ResourceFormat::R8G8B8A8_UNORM_SRGB:
			return ResourceFormat::R8G8B8A8_UNORM;
			break;
		case ResourceFormat::BC1_UNORM_SRGB:
			return ResourceFormat::BC1_UNORM;
			break;
		case ResourceFormat::BC2_UNORM_SRGB:
			return ResourceFormat::BC2_UNORM;
			break;
		case ResourceFormat::BC3_UNORM_SRGB:
			return ResourceFormat::BC3_UNORM;
			break;
		case ResourceFormat::B8G8R8A8_UNORM_SRGB:
			return ResourceFormat::B8G8R8A8_UNORM;
			break;
		case ResourceFormat::B8G8R8X8_UNORM_SRGB:
			return ResourceFormat::B8G8R8X8_UNORM;
			break;
		case ResourceFormat::BC7_UNORM_SRGB:
			return ResourceFormat::BC7_UNORM;
			break;
		default:
			break;
		}
	}

	inline DXGI_FORMAT ConvertDxgiFormatToDxgiTypelessFormat( DXGI_FORMAT format )
	{
		switch ( format )
		{
		case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32B32A32_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32B32A32_SINT:
			return DXGI_FORMAT_R32G32B32A32_TYPELESS;
		case DXGI_FORMAT_R32G32B32_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32B32_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32B32_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32B32_SINT:
			return DXGI_FORMAT_R32G32B32_TYPELESS;
		case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16B16A16_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case DXGI_FORMAT_R32G32_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R32G32_SINT:
			return DXGI_FORMAT_R32G32_TYPELESS;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32G8X24_TYPELESS;
		case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R10G10B10A2_UINT:
			return DXGI_FORMAT_R10G10B10A2_TYPELESS;
		/* case DXGI_FORMAT_R11G11B10_FLOAT:
			return ResourceFormat::R11G11B10_FLOAT; */
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8B8A8_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8B8A8_SINT:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case DXGI_FORMAT_R16G16_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16_SNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R16G16_SINT:
			return DXGI_FORMAT_R16G16_TYPELESS;
		case DXGI_FORMAT_R32_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_D32_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_R32_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_R32_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R32_SINT:
			return DXGI_FORMAT_R32_TYPELESS;
		case DXGI_FORMAT_R24G8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24G8_TYPELESS;
		case DXGI_FORMAT_R8G8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8_SNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R8G8_SINT:
			return DXGI_FORMAT_R8G8_TYPELESS;
		case DXGI_FORMAT_R16_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R16_FLOAT:
			[[fallthrough]];
		case DXGI_FORMAT_D16_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R16_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R16_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R16_SNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R16_SINT:
			return DXGI_FORMAT_R16_TYPELESS;
		case DXGI_FORMAT_R8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_R8_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R8_UINT:
			[[fallthrough]];
		case DXGI_FORMAT_R8_SNORM:
			[[fallthrough]];
		case DXGI_FORMAT_R8_SINT:
			return DXGI_FORMAT_R8_TYPELESS;
		/* case DXGI_FORMAT_A8_UNORM:
			return ResourceFormat::A8_UNORM;
		case DXGI_FORMAT_R1_UNORM:
			return ResourceFormat::R1_UNORM;
		case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
			return ResourceFormat::R9G9B9E5_SHAREDEXP;
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
			return ResourceFormat::R8G8_B8G8_UNORM;
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
			return ResourceFormat::G8R8_G8B8_UNORM; */
		case DXGI_FORMAT_BC1_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC1_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			return DXGI_FORMAT_BC1_TYPELESS;
		case DXGI_FORMAT_BC2_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC2_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			return DXGI_FORMAT_BC2_TYPELESS;
		case DXGI_FORMAT_BC3_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC3_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			return DXGI_FORMAT_BC3_TYPELESS;
		case DXGI_FORMAT_BC4_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC4_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_BC4_SNORM:
			return DXGI_FORMAT_BC4_TYPELESS;
		case DXGI_FORMAT_BC5_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC5_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_BC5_SNORM:
			return DXGI_FORMAT_BC5_TYPELESS;
		/* case DXGI_FORMAT_B5G6R5_UNORM:
			return ResourceFormat::B5G6R5_UNORM;
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			return ResourceFormat::B5G5R5A1_UNORM; */
		/* case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
			return ResourceFormat::R10G10B10_XR_BIAS_A2_UNORM; */
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_TYPELESS;
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_TYPELESS;
		case DXGI_FORMAT_BC6H_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC6H_UF16:
			[[fallthrough]];
		case DXGI_FORMAT_BC6H_SF16:
			return DXGI_FORMAT_BC6H_TYPELESS;
		case DXGI_FORMAT_BC7_TYPELESS:
			[[fallthrough]];
		case DXGI_FORMAT_BC7_UNORM:
			[[fallthrough]];
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			return DXGI_FORMAT_BC7_TYPELESS;
		default:
			break;
		}

		return DXGI_FORMAT_UNKNOWN;
	}
}