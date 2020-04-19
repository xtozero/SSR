#pragma once

#include "Render/IRenderResource.h"

#include <cassert>
#include <D3D11.h>
#include <wrl/client.h>

inline D3D11_USAGE ConvertAccessFlagToUsage( UINT accessFlag )
{
	using namespace RESOURCE_ACCESS_FLAG;

	switch ( accessFlag )
	{
	case ( GPU_READ | GPU_WRITE ):
		return D3D11_USAGE_DEFAULT;
	case ( GPU_READ | CPU_WRITE ):
		return D3D11_USAGE_DYNAMIC;
	case GPU_READ:
		return D3D11_USAGE_IMMUTABLE;
	case ( GPU_READ | GPU_WRITE | CPU_READ | CPU_WRITE ):
	case ( GPU_READ | GPU_WRITE | CPU_READ ):
		return D3D11_USAGE_STAGING;
	default:
		assert( false );
		return D3D11_USAGE_DEFAULT;
	}
}

inline UINT ConvertUsageToAccessFlag( D3D11_USAGE accessFlag )
{
	using namespace RESOURCE_ACCESS_FLAG;

	switch ( accessFlag )
	{
	case D3D11_USAGE_DEFAULT:
		return GPU_READ | GPU_WRITE;
	case D3D11_USAGE_DYNAMIC:
		return GPU_READ | CPU_WRITE;
	case D3D11_USAGE_IMMUTABLE:
		return GPU_READ;
	case D3D11_USAGE_STAGING:
		return GPU_READ | GPU_WRITE | CPU_READ | CPU_WRITE;
	default:
		assert( false );
		return 0;
	}
}

inline UINT ConvertTypeToBind( UINT type )
{
	using namespace RESOURCE_BIND_TYPE;

	UINT ret = 0;

	if ( type & VERTEX_BUFFER )
	{
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if ( type & INDEX_BUFFER )
	{
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if ( type & CONSTANT_BUFFER )
	{
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	if ( type & SHADER_RESOURCE )
	{
		ret |= D3D11_BIND_SHADER_RESOURCE;
	}

	if ( type & STREAM_OUTPUT )
	{
		ret |= D3D11_BIND_STREAM_OUTPUT;
	}

	if ( type & RENDER_TARGET )
	{
		ret |= D3D11_BIND_RENDER_TARGET;
	}

	if ( type & DEPTH_STENCIL )
	{
		ret |= D3D11_BIND_DEPTH_STENCIL;
	}

	if ( type & UNORDERED_ACCESS )
	{
		ret |= D3D11_BIND_UNORDERED_ACCESS;
	}

	return ret;
}

inline UINT ConvertBindToType( UINT type )
{
	using namespace RESOURCE_BIND_TYPE;

	UINT ret = 0;

	if ( type & D3D11_BIND_VERTEX_BUFFER )
	{
		ret |= VERTEX_BUFFER;
	}

	if ( type & D3D11_BIND_INDEX_BUFFER )
	{
		ret |= INDEX_BUFFER;
	}

	if ( type & D3D11_BIND_CONSTANT_BUFFER )
	{
		ret |= CONSTANT_BUFFER;
	}

	if ( type & D3D11_BIND_SHADER_RESOURCE )
	{
		ret |= SHADER_RESOURCE;
	}

	if ( type & D3D11_BIND_STREAM_OUTPUT )
	{
		ret |= STREAM_OUTPUT;
	}

	if ( type & D3D11_BIND_RENDER_TARGET )
	{
		ret |= RENDER_TARGET;
	}

	if ( type & D3D11_BIND_DEPTH_STENCIL )
	{
		ret |= DEPTH_STENCIL;
	}

	if ( type & D3D11_BIND_UNORDERED_ACCESS )
	{
		ret |= UNORDERED_ACCESS;
	}

	return ret;
}

inline UINT ConvertMicsToDXMisc( UINT miscFlag )
{
	using namespace RESOURCE_MISC;

	UINT ret = 0;

	if ( miscFlag & GENERATE_MIPS )
	{
		ret |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	if ( miscFlag & SHARED )
	{
		ret |= D3D11_RESOURCE_MISC_SHARED;
	}

	if ( miscFlag & TEXTURECUBE )
	{
		ret |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	if ( miscFlag & DRAWINDIRECT_ARGS )
	{
		ret |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	}

	if ( miscFlag & BUFFER_ALLOW_RAW_VIEWS )
	{
		assert( ( miscFlag & BUFFER_STRUCTURED ) == 0 );
		ret |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	}

	if ( miscFlag & BUFFER_STRUCTURED )
	{
		assert( ( miscFlag & BUFFER_ALLOW_RAW_VIEWS ) == 0 );
		ret |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	if ( miscFlag & RESOURCE_CLAMP )
	{
		ret |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP;
	}

	if ( miscFlag & SHARED_KEYEDMUTEX )
	{
		ret |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	}

	if ( miscFlag & GDI_COMPATIBLE )
	{
		ret |= D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
	}

	return ret;
}

inline UINT ConvertDXMiscToMisc( UINT miscFlag )
{
	using namespace RESOURCE_MISC;

	UINT ret = 0;

	if ( miscFlag & D3D11_RESOURCE_MISC_GENERATE_MIPS )
	{
		ret |= GENERATE_MIPS;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_SHARED )
	{
		ret |= SHARED;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE )
	{
		ret |= TEXTURECUBE;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS )
	{
		ret |= DRAWINDIRECT_ARGS;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
	{
		ret |= BUFFER_ALLOW_RAW_VIEWS;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
	{
		ret |= BUFFER_STRUCTURED;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_RESOURCE_CLAMP )
	{
		ret |= RESOURCE_CLAMP;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX )
	{
		ret |= SHARED_KEYEDMUTEX;
	}

	if ( miscFlag & D3D11_RESOURCE_MISC_GDI_COMPATIBLE )
	{
		ret |= GDI_COMPATIBLE;
	}

	return ret;
}

inline UINT ConvertAccessFlagToCpuFlag( UINT accessFlag )
{
	using namespace RESOURCE_ACCESS_FLAG;

	UINT ret = 0;

	if ( accessFlag & CPU_READ )
	{
		ret |= D3D11_CPU_ACCESS_READ;
	}

	if ( accessFlag & CPU_WRITE )
	{
		ret |= D3D11_CPU_ACCESS_WRITE;
	}

	return ret;
}

inline DXGI_FORMAT ConvertFormatToDxgiFormat( UINT format )
{
	using namespace RESOURCE_FORMAT;

	switch ( format )
	{
	case UNKNOWN :
		return DXGI_FORMAT_UNKNOWN;
	case R32G32B32A32_TYPELESS :
		return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case R32G32B32A32_FLOAT :
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case R32G32B32A32_UINT :
		return DXGI_FORMAT_R32G32B32A32_UINT;
	case R32G32B32A32_SINT :
		return DXGI_FORMAT_R32G32B32A32_SINT;
	case R32G32B32_TYPELESS :
		return DXGI_FORMAT_R32G32B32_TYPELESS;
	case R32G32B32_FLOAT :
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case R32G32B32_UINT :
		return DXGI_FORMAT_R32G32B32_UINT;
	case R32G32B32_SINT :
		return DXGI_FORMAT_R32G32B32_SINT;
	case R16G16B16A16_TYPELESS :
		return DXGI_FORMAT_R16G16B16A16_TYPELESS;
	case R16G16B16A16_FLOAT :
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case R16G16B16A16_UNORM :
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	case R16G16B16A16_UINT :
		return DXGI_FORMAT_R16G16B16A16_UINT;
	case R16G16B16A16_SNORM :
		return DXGI_FORMAT_R16G16B16A16_SNORM;
	case R16G16B16A16_SINT :
		return DXGI_FORMAT_R16G16B16A16_SINT;
	case R32G32_TYPELESS :
		return DXGI_FORMAT_R32G32_TYPELESS;
	case R32G32_FLOAT :
		return DXGI_FORMAT_R32G32_FLOAT;
	case R32G32_UINT :
		return DXGI_FORMAT_R32G32_UINT;
	case R32G32_SINT :
		return DXGI_FORMAT_R32G32_SINT;
	case R32G8X24_TYPELESS :
		return DXGI_FORMAT_R32G8X24_TYPELESS;
	case D32_FLOAT_S8X24_UINT :
		return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	case R32_FLOAT_X8X24_TYPELESS :
		return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	case X32_TYPELESS_G8X24_UINT :
		return DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
	case R10G10B10A2_TYPELESS :
		return DXGI_FORMAT_R10G10B10A2_TYPELESS;
	case R10G10B10A2_UNORM :
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	case R10G10B10A2_UINT :
		return DXGI_FORMAT_R10G10B10A2_UINT;
	case R11G11B10_FLOAT :
		return DXGI_FORMAT_R11G11B10_FLOAT;
	case R8G8B8A8_TYPELESS :
		return DXGI_FORMAT_R8G8B8A8_TYPELESS;
	case R8G8B8A8_UNORM :
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case R8G8B8A8_UNORM_SRGB :
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	case R8G8B8A8_UINT :
		return DXGI_FORMAT_R8G8B8A8_UINT;
	case R8G8B8A8_SNORM :
		return DXGI_FORMAT_R8G8B8A8_SNORM;
	case R8G8B8A8_SINT :
		return DXGI_FORMAT_R8G8B8A8_SINT;
	case R16G16_TYPELESS :
		return DXGI_FORMAT_R16G16_TYPELESS;
	case R16G16_FLOAT :
		return DXGI_FORMAT_R16G16_FLOAT;
	case R16G16_UNORM :
		return DXGI_FORMAT_R16G16_UNORM;
	case R16G16_UINT :
		return DXGI_FORMAT_R16G16_UINT;
	case R16G16_SNORM :
		return DXGI_FORMAT_R16G16_SNORM;
	case R16G16_SINT :
		return DXGI_FORMAT_R16G16_SINT;
	case R32_TYPELESS :
		return DXGI_FORMAT_R32_TYPELESS;
	case D32_FLOAT :
		return DXGI_FORMAT_D32_FLOAT;
	case R32_FLOAT :
		return DXGI_FORMAT_R32_FLOAT;
	case R32_UINT :
		return DXGI_FORMAT_R32_UINT;
	case R32_SINT :
		return DXGI_FORMAT_R32_SINT;
	case R24G8_TYPELESS :
		return DXGI_FORMAT_R24G8_TYPELESS;
	case D24_UNORM_S8_UINT :
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case R24_UNORM_X8_TYPELESS :
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	case X24_TYPELESS_G8_UINT :
		return DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	case R8G8_TYPELESS :
		return DXGI_FORMAT_R8G8_TYPELESS;
	case R8G8_UNORM :
		return DXGI_FORMAT_R8G8_UNORM;
	case R8G8_UINT :
		return DXGI_FORMAT_R8G8_UINT;
	case R8G8_SNORM :
		return DXGI_FORMAT_R8G8_SNORM;
	case R8G8_SINT :
		return DXGI_FORMAT_R8G8_SINT;
	case R16_TYPELESS :
		return DXGI_FORMAT_R16_TYPELESS;
	case R16_FLOAT :
		return DXGI_FORMAT_R16_FLOAT;
	case D16_UNORM :
		return DXGI_FORMAT_D16_UNORM;
	case R16_UNORM :
		return DXGI_FORMAT_R16_UNORM;
	case R16_UINT :
		return DXGI_FORMAT_R16_UINT;
	case R16_SNORM :
		return DXGI_FORMAT_R16_SNORM;
	case R16_SINT :
		return DXGI_FORMAT_R16_SINT;
	case R8_TYPELESS :
		return DXGI_FORMAT_R8_TYPELESS;
	case R8_UNORM :
		return DXGI_FORMAT_R8_UNORM;
	case R8_UINT :
		return DXGI_FORMAT_R8_UINT;
	case R8_SNORM :
		return DXGI_FORMAT_R8_SNORM;
	case R8_SINT :
		return DXGI_FORMAT_R8_SINT;
	case A8_UNORM :
		return DXGI_FORMAT_A8_UNORM;
	case R1_UNORM :
		return DXGI_FORMAT_R1_UNORM;
	case R9G9B9E5_SHAREDEXP :
		return DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
	case R8G8_B8G8_UNORM :
		return DXGI_FORMAT_R8G8_B8G8_UNORM;
	case G8R8_G8B8_UNORM :
		return DXGI_FORMAT_G8R8_G8B8_UNORM;
	case BC1_TYPELESS :
		return DXGI_FORMAT_BC1_TYPELESS;
	case BC1_UNORM :
		return DXGI_FORMAT_BC1_UNORM;
	case BC1_UNORM_SRGB :
		return DXGI_FORMAT_BC1_UNORM_SRGB;
	case BC2_TYPELESS :
		return DXGI_FORMAT_BC2_TYPELESS;
	case BC2_UNORM :
		return DXGI_FORMAT_BC2_UNORM;
	case BC2_UNORM_SRGB :
		return DXGI_FORMAT_BC2_UNORM_SRGB;
	case BC3_TYPELESS :
		return DXGI_FORMAT_BC3_TYPELESS;
	case BC3_UNORM :
		return DXGI_FORMAT_BC3_UNORM;
	case BC3_UNORM_SRGB :
		return DXGI_FORMAT_BC3_UNORM_SRGB;
	case BC4_TYPELESS :
		return DXGI_FORMAT_BC4_TYPELESS;
	case BC4_UNORM :
		return DXGI_FORMAT_BC4_UNORM;
	case BC4_SNORM :
		return DXGI_FORMAT_BC4_SNORM;
	case BC5_TYPELESS :
		return DXGI_FORMAT_BC5_TYPELESS;
	case BC5_UNORM :
		return DXGI_FORMAT_BC5_UNORM;
	case BC5_SNORM :
		return DXGI_FORMAT_BC5_SNORM;
	case B5G6R5_UNORM :
		return DXGI_FORMAT_B5G6R5_UNORM;
	case B5G5R5A1_UNORM :
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	case B8G8R8A8_UNORM :
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	case B8G8R8X8_UNORM :
		return DXGI_FORMAT_B8G8R8X8_UNORM;
	case R10G10B10_XR_BIAS_A2_UNORM :
		return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
	case B8G8R8A8_TYPELESS :
		return DXGI_FORMAT_B8G8R8A8_TYPELESS;
	case B8G8R8A8_UNORM_SRGB :
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	case B8G8R8X8_TYPELESS :
		return DXGI_FORMAT_B8G8R8X8_TYPELESS;
	case B8G8R8X8_UNORM_SRGB :
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
	case BC6H_TYPELESS :
		return DXGI_FORMAT_BC6H_TYPELESS;
	case BC6H_UF16 :
		return DXGI_FORMAT_BC6H_UF16;
	case BC6H_SF16 :
		return DXGI_FORMAT_BC6H_SF16;
	case BC7_TYPELESS :
		return DXGI_FORMAT_BC7_TYPELESS;
	case BC7_UNORM :
		return DXGI_FORMAT_BC7_UNORM;
	case BC7_UNORM_SRGB :
		return DXGI_FORMAT_BC7_UNORM_SRGB;
	default:
		assert( false );
		return DXGI_FORMAT_UNKNOWN;
	}
}

inline UINT ConvertDxgiFormatToFormat( DXGI_FORMAT format )
{
	using namespace RESOURCE_FORMAT;

	switch ( format )
	{
	case DXGI_FORMAT_UNKNOWN:
		return UNKNOWN;
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
		return R32G32B32A32_TYPELESS;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return R32G32B32A32_FLOAT;
	case DXGI_FORMAT_R32G32B32A32_UINT:
		return R32G32B32A32_UINT;
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return R32G32B32A32_SINT;
	case DXGI_FORMAT_R32G32B32_TYPELESS:
		return R32G32B32_TYPELESS;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return R32G32B32_FLOAT;
	case DXGI_FORMAT_R32G32B32_UINT:
		return R32G32B32_UINT;
	case DXGI_FORMAT_R32G32B32_SINT:
		return R32G32B32_SINT;
	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
		return R16G16B16A16_TYPELESS;
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return R16G16B16A16_FLOAT;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return R16G16B16A16_UNORM;
	case DXGI_FORMAT_R16G16B16A16_UINT:
		return R16G16B16A16_UINT;
	case DXGI_FORMAT_R16G16B16A16_SNORM:
		return R16G16B16A16_SNORM;
	case DXGI_FORMAT_R16G16B16A16_SINT:
		return R16G16B16A16_SINT;
	case DXGI_FORMAT_R32G32_TYPELESS:
		return R32G32_TYPELESS;
	case DXGI_FORMAT_R32G32_FLOAT:
		return R32G32_FLOAT;
	case DXGI_FORMAT_R32G32_UINT:
		return R32G32_UINT;
	case DXGI_FORMAT_R32G32_SINT:
		return R32G32_SINT;
	case DXGI_FORMAT_R32G8X24_TYPELESS:
		return R32G8X24_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		return D32_FLOAT_S8X24_UINT;
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		return R32_FLOAT_X8X24_TYPELESS;
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return X32_TYPELESS_G8X24_UINT;
	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
		return R10G10B10A2_TYPELESS;
	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return R10G10B10A2_UNORM;
	case DXGI_FORMAT_R10G10B10A2_UINT:
		return R10G10B10A2_UINT;
	case DXGI_FORMAT_R11G11B10_FLOAT:
		return R11G11B10_FLOAT;
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		return R8G8B8A8_TYPELESS;
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return R8G8B8A8_UNORM;
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return R8G8B8A8_UNORM_SRGB;
	case DXGI_FORMAT_R8G8B8A8_UINT:
		return R8G8B8A8_UINT;
	case DXGI_FORMAT_R8G8B8A8_SNORM:
		return R8G8B8A8_SNORM;
	case DXGI_FORMAT_R8G8B8A8_SINT:
		return R8G8B8A8_SINT;
	case DXGI_FORMAT_R16G16_TYPELESS:
		return R16G16_TYPELESS;
	case DXGI_FORMAT_R16G16_FLOAT:
		return R16G16_FLOAT;
	case DXGI_FORMAT_R16G16_UNORM:
		return R16G16_UNORM;
	case DXGI_FORMAT_R16G16_UINT:
		return R16G16_UINT;
	case DXGI_FORMAT_R16G16_SNORM:
		return R16G16_SNORM;
	case DXGI_FORMAT_R16G16_SINT:
		return R16G16_SINT;
	case DXGI_FORMAT_R32_TYPELESS:
		return R32_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT:
		return D32_FLOAT;
	case DXGI_FORMAT_R32_FLOAT:
		return R32_FLOAT;
	case DXGI_FORMAT_R32_UINT:
		return R32_UINT;
	case DXGI_FORMAT_R32_SINT:
		return R32_SINT;
	case DXGI_FORMAT_R24G8_TYPELESS:
		return R24G8_TYPELESS;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return D24_UNORM_S8_UINT;
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		return R24_UNORM_X8_TYPELESS;
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
		return X24_TYPELESS_G8_UINT;
	case DXGI_FORMAT_R8G8_TYPELESS:
		return R8G8_TYPELESS;
	case DXGI_FORMAT_R8G8_UNORM:
		return R8G8_UNORM;
	case DXGI_FORMAT_R8G8_UINT:
		return R8G8_UINT;
	case DXGI_FORMAT_R8G8_SNORM:
		return R8G8_SNORM;
	case DXGI_FORMAT_R8G8_SINT:
		return R8G8_SINT;
	case DXGI_FORMAT_R16_TYPELESS:
		return R16_TYPELESS;
	case DXGI_FORMAT_R16_FLOAT:
		return R16_FLOAT;
	case DXGI_FORMAT_D16_UNORM:
		return D16_UNORM;
	case DXGI_FORMAT_R16_UNORM:
		return R16_UNORM;
	case DXGI_FORMAT_R16_UINT:
		return R16_UINT;
	case DXGI_FORMAT_R16_SNORM:
		return R16_SNORM;
	case DXGI_FORMAT_R16_SINT:
		return R16_SINT;
	case DXGI_FORMAT_R8_TYPELESS:
		return R8_TYPELESS;
	case DXGI_FORMAT_R8_UNORM:
		return R8_UNORM;
	case DXGI_FORMAT_R8_UINT:
		return R8_UINT;
	case DXGI_FORMAT_R8_SNORM:
		return R8_SNORM;
	case DXGI_FORMAT_R8_SINT:
		return R8_SINT;
	case DXGI_FORMAT_A8_UNORM:
		return A8_UNORM;
	case DXGI_FORMAT_R1_UNORM:
		return R1_UNORM;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return R9G9B9E5_SHAREDEXP;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
		return R8G8_B8G8_UNORM;
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		return G8R8_G8B8_UNORM;
	case DXGI_FORMAT_BC1_TYPELESS:
		return BC1_TYPELESS;
	case DXGI_FORMAT_BC1_UNORM:
		return BC1_UNORM;
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return BC1_UNORM_SRGB;
	case DXGI_FORMAT_BC2_TYPELESS:
		return BC2_TYPELESS;
	case DXGI_FORMAT_BC2_UNORM:
		return BC2_UNORM;
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		return BC2_UNORM_SRGB;
	case DXGI_FORMAT_BC3_TYPELESS:
		return BC3_TYPELESS;
	case DXGI_FORMAT_BC3_UNORM:
		return BC3_UNORM;
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		return BC3_UNORM_SRGB;
	case DXGI_FORMAT_BC4_TYPELESS:
		return BC4_TYPELESS;
	case DXGI_FORMAT_BC4_UNORM:
		return BC4_UNORM;
	case DXGI_FORMAT_BC4_SNORM:
		return BC4_SNORM;
	case DXGI_FORMAT_BC5_TYPELESS:
		return BC5_TYPELESS;
	case DXGI_FORMAT_BC5_UNORM:
		return BC5_UNORM;
	case DXGI_FORMAT_BC5_SNORM:
		return BC5_SNORM;
	case DXGI_FORMAT_B5G6R5_UNORM:
		return B5G6R5_UNORM;
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return B5G5R5A1_UNORM;
	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return B8G8R8A8_UNORM;
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return B8G8R8X8_UNORM;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return R10G10B10_XR_BIAS_A2_UNORM;
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		return B8G8R8A8_TYPELESS;
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return B8G8R8A8_UNORM_SRGB;
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		return B8G8R8X8_TYPELESS;
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return B8G8R8X8_UNORM_SRGB;
	case DXGI_FORMAT_BC6H_TYPELESS:
		return BC6H_TYPELESS;
	case DXGI_FORMAT_BC6H_UF16:
		return BC6H_UF16;
	case DXGI_FORMAT_BC6H_SF16:
		return BC6H_SF16;
	case DXGI_FORMAT_BC7_TYPELESS:
		return BC7_TYPELESS;
	case DXGI_FORMAT_BC7_UNORM:
		return BC7_UNORM;
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return BC7_UNORM_SRGB;
	default:
		assert( false );
		return UNKNOWN;
	}
}

inline bool IsTexture1D( const TEXTURE_TRAIT& trait )
{
	return trait.m_height < 2;
}

inline bool IsTexture2D( const TEXTURE_TRAIT& trait )
{
	return trait.m_sampleCount > 0;
}

inline bool IsTexture3D( const TEXTURE_TRAIT& trait )
{
	return trait.m_height > 1 && trait.m_depth > 1 && trait.m_sampleCount < 2;
}

inline UINT Convert3D11PrimToPrim( D3D_PRIMITIVE_TOPOLOGY primitive )
{
	using namespace RESOURCE_PRIMITIVE;

	switch ( primitive )
	{
	case D3D_PRIMITIVE_TOPOLOGY_UNDEFINED:
		return UNDEFINED;
	case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
		return POINTLIST;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
		return LINELIST;
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
		return LINESTRIP;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		return TRIANGLELIST;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		return TRIANGLESTRIP;
	case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		return LINELIST_ADJ;
	case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		return LINESTRIP_ADJ;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		return TRIANGLELIST_ADJ;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		return TRIANGLESTRIP_ADJ;
	case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 1;
	case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 2;
	case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 3;
	case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 4;
	case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 5;
	case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 6;
	case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 7;
	case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 8;
	case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 9;
	case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 10;
	case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 11;
	case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 12;
	case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 13;
	case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 14;
	case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 15;
	case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 16;
	case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 17;
	case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 18;
	case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 19;
	case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 20;
	case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 21;
	case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 22;
	case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 23;
	case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 24;
	case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 25;
	case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 26;
	case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 27;
	case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 28;
	case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 29;
	case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 30;
	case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 31;
	case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		return CONTROL_POINT_PATCHLIST + 32;
	default:
		assert( false );
		return UNDEFINED;
	}
}

inline D3D_PRIMITIVE_TOPOLOGY ConvertPrimToD3D11Prim( UINT primitive )
{
	using namespace RESOURCE_PRIMITIVE;

	switch ( primitive )
	{
	case UNDEFINED:
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	case POINTLIST:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	case LINELIST:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case LINESTRIP:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case TRIANGLELIST:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case TRIANGLESTRIP:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case LINELIST_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
	case LINESTRIP_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
	case TRIANGLELIST_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
	case TRIANGLESTRIP_ADJ:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
	case CONTROL_POINT_PATCHLIST + 1:
		return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 2:
		return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 3:
		return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 4:
		return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 5:
		return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 6:
		return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 7:
		return D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 8:
		return D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 9:
		return D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 10:
		return D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 11:
		return D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 12:
		return D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 13:
		return D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 14:
		return D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 15:
		return D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 16:
		return D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 17:
		return D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 18:
		return D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 19:
		return D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 20:
		return D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 21:
		return D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 22:
		return D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 23:
		return D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 24:
		return D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 25:
		return D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 26:
		return D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 27:
		return D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 28:
		return D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 29:
		return D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 30:
		return D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 31:
		return D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
	case CONTROL_POINT_PATCHLIST + 32:
		return D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
	default:
		assert( false );
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}
}

inline D3D11_MAP ConvertLockFlagToD3D11Map( UINT lockFlag )
{
	switch ( lockFlag )
	{
	case BUFFER_LOCKFLAG::READ:
		return D3D11_MAP_READ;
	case BUFFER_LOCKFLAG::READ_WRITE:
		return D3D11_MAP_READ_WRITE;
	case BUFFER_LOCKFLAG::WRITE:
		return D3D11_MAP_WRITE;
	case BUFFER_LOCKFLAG::WRITE_DISCARD:
		return D3D11_MAP_WRITE_DISCARD;
	case BUFFER_LOCKFLAG::WRITE_NO_OVERWRITE:
		return D3D11_MAP_WRITE_NO_OVERWRITE;
	default:
		__debugbreak( );
		return static_cast<D3D11_MAP>( 0 );
	}
}

inline void ConvertVertexLayoutToInputLayout( D3D11_INPUT_ELEMENT_DESC* iL, const VERTEX_LAYOUT* vL, int numLayout )
{
	for ( int i = 0; i < numLayout; ++i )
	{
		iL[i].SemanticName = vL[i].m_name.c_str();
		iL[i].SemanticIndex = vL[i].m_index;
		iL[i].Format = ConvertFormatToDxgiFormat( vL[i].m_format );
		iL[i].InputSlot = vL[i].m_slot;
		iL[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		iL[i].InputSlotClass = vL[i].m_isInstanceData ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		iL[i].InstanceDataStepRate = vL[i].m_instanceDataStep;
	}
}

inline D3D11_DEPTH_WRITE_MASK ConvertToDepthWriteMask( DEPTH_WRITE_MODE depthWriteMode )
{
	switch ( depthWriteMode )
	{
	case DEPTH_WRITE_MODE::ZERO:
		return D3D11_DEPTH_WRITE_MASK_ZERO;
	case DEPTH_WRITE_MODE::ALL:
		return D3D11_DEPTH_WRITE_MASK_ALL;
	default:
		assert( false );
		return D3D11_DEPTH_WRITE_MASK_ZERO;
	}
}

inline D3D11_COMPARISON_FUNC ConvertToComparisionFunc( COMPARISON_FUNC comparisonFunc )
{
	switch ( comparisonFunc )
	{
	case COMPARISON_FUNC::NEVER:
		return D3D11_COMPARISON_NEVER;
	case COMPARISON_FUNC::LESS:
		return D3D11_COMPARISON_LESS;
	case COMPARISON_FUNC::EQUAL:
		return D3D11_COMPARISON_EQUAL;
	case COMPARISON_FUNC::LESS_EQUAL:
		return D3D11_COMPARISON_LESS_EQUAL;
	case COMPARISON_FUNC::GREATER:
		return D3D11_COMPARISON_GREATER;
	case COMPARISON_FUNC::NOT_EQUAL:
		return D3D11_COMPARISON_NOT_EQUAL;
	case COMPARISON_FUNC::GREATER_EQUAL:
		return D3D11_COMPARISON_GREATER_EQUAL;
	case COMPARISON_FUNC::ALWAYS:
		return D3D11_COMPARISON_ALWAYS;
	default:
		assert( false );
		return D3D11_COMPARISON_NEVER;
	}
}

inline D3D11_STENCIL_OP ConvertToStencilOp( STENCIL_OP stencilOp )
{
	switch ( stencilOp )
	{
	case STENCIL_OP::KEEP:
		return D3D11_STENCIL_OP_KEEP;
	case STENCIL_OP::ZERO:
		return D3D11_STENCIL_OP_ZERO;
	case STENCIL_OP::REPLACE:
		return D3D11_STENCIL_OP_REPLACE;
	case STENCIL_OP::INCR_SAT:
		return D3D11_STENCIL_OP_INCR_SAT;
	case STENCIL_OP::DECR_SAT:
		return D3D11_STENCIL_OP_DECR_SAT;
	case STENCIL_OP::INVERT:
		return D3D11_STENCIL_OP_INVERT;
	case STENCIL_OP::INCR:
		return D3D11_STENCIL_OP_INCR;
	case STENCIL_OP::DECR:
		return D3D11_STENCIL_OP_DECR;
	default:
		assert( false );
		return D3D11_STENCIL_OP_KEEP;
	}
}

inline D3D11_FILL_MODE ConvertToFillMode( FILL_MODE fillMode )
{
	switch ( fillMode )
	{
	case FILL_MODE::WIREFRAME:
		return D3D11_FILL_WIREFRAME;
	case FILL_MODE::SOLID:
		return D3D11_FILL_SOLID;
	default:
		assert( false );
		return D3D11_FILL_WIREFRAME;
	}
}

inline D3D11_CULL_MODE ConvertToCullMode( CULL_MODE cullMode )
{
	switch ( cullMode )
	{
	case CULL_MODE::NONE:
		return D3D11_CULL_NONE;
	case CULL_MODE::FRONT:
		return D3D11_CULL_FRONT;
	case CULL_MODE::BACK:
		return D3D11_CULL_BACK;
	default:
		assert( false );
		return D3D11_CULL_NONE;
	}
}

inline D3D11_FILTER ConvertToFilter( TEXTURE_FILTER::Type filter )
{
	return static_cast<D3D11_FILTER>( filter );
}

inline D3D11_TEXTURE_ADDRESS_MODE ConvertToTextureAddress( TEXTURE_ADDRESS_MODE addressMode )
{
	switch ( addressMode )
	{
	case TEXTURE_ADDRESS_MODE::WRAP:
		return D3D11_TEXTURE_ADDRESS_WRAP;
	case TEXTURE_ADDRESS_MODE::MIRROR:
		return D3D11_TEXTURE_ADDRESS_MIRROR;
	case TEXTURE_ADDRESS_MODE::CLAMP:
		return D3D11_TEXTURE_ADDRESS_CLAMP;
	case TEXTURE_ADDRESS_MODE::BORDER:
		return D3D11_TEXTURE_ADDRESS_BORDER;
	case TEXTURE_ADDRESS_MODE::MIRROR_ONCE:
		return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
	default:
		assert( false );
		return D3D11_TEXTURE_ADDRESS_WRAP;
	}
}

inline D3D11_BLEND ConvertToBlend( BLEND blend )
{
	switch ( blend )
	{
	case BLEND::ZERO:
		return D3D11_BLEND_ZERO;
	case BLEND::ONE:
		return D3D11_BLEND_ONE;
	case BLEND::SRC_COLOR:
		return D3D11_BLEND_SRC_COLOR;
	case BLEND::INV_SRC_COLOR:
		return D3D11_BLEND_INV_SRC_COLOR;
	case BLEND::SRC_ALPHA:
		return D3D11_BLEND_SRC_ALPHA;
	case BLEND::INV_SRC_ALPHA:
		return D3D11_BLEND_INV_SRC_ALPHA;
	case BLEND::DEST_ALPHA:
		return D3D11_BLEND_DEST_ALPHA;
	case BLEND::INV_DEST_ALPHA:
		return D3D11_BLEND_INV_DEST_ALPHA;
	case BLEND::DEST_COLOR:
		return D3D11_BLEND_DEST_COLOR;
	case BLEND::INV_DEST_COLOR:
		return D3D11_BLEND_INV_DEST_COLOR;
	case BLEND::SRC_ALPHA_SAT:
		return D3D11_BLEND_SRC_ALPHA_SAT;
	case BLEND::BLEND_FACTOR:
		return D3D11_BLEND_BLEND_FACTOR;
	case BLEND::INV_BLEND_FACTOR:
		return D3D11_BLEND_INV_BLEND_FACTOR;
	case BLEND::SRC1_COLOR:
		return D3D11_BLEND_SRC1_COLOR;
	case BLEND::INV_SRC1_COLOR:
		return D3D11_BLEND_INV_SRC1_COLOR;
	case BLEND::SRC1_ALPHA:
		return D3D11_BLEND_SRC1_ALPHA;
	case BLEND::INV_SRC1_ALPHA:
		return D3D11_BLEND_INV_SRC1_ALPHA;
	default:
		assert( false );
		return D3D11_BLEND_ZERO;
	}
}

inline D3D11_BLEND_OP ConvertToBlendOp( BLEND_OP blendOp )
{
	switch ( blendOp )
	{
	case BLEND_OP::ADD:
		return D3D11_BLEND_OP_ADD;
	case BLEND_OP::SUBTRACT:
		return D3D11_BLEND_OP_SUBTRACT;
	case BLEND_OP::REV_SUBTRACT:
		return D3D11_BLEND_OP_REV_SUBTRACT;
	case BLEND_OP::MIN:
		return D3D11_BLEND_OP_MIN;
	case BLEND_OP::MAX:
		return D3D11_BLEND_OP_MAX;
	default:
		assert( false );
		return D3D11_BLEND_OP_ADD;
	}
}

inline unsigned char ConvertToColorWriteEnable( COLOR_WRITE_ENABLE colorWriteEnable )
{
	return static_cast<unsigned char>( colorWriteEnable );
}

class CD3D11Buffer : public GraphicsResourceBase<ID3D11Buffer>
{
public:
	virtual void InitResource( ) override;

	UINT Size( ) const { return m_desc.ByteWidth; }
	UINT Stride( ) const { return m_desc.StructureByteStride; }
	const D3D11_BUFFER_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Buffer( const BUFFER_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Buffer( )
	{
		delete m_dataStorage;
	}
	CD3D11Buffer( const CD3D11Buffer& ) = delete;
	CD3D11Buffer( CD3D11Buffer&& ) = default;
	CD3D11Buffer& operator=( const CD3D11Buffer& ) = delete;
	CD3D11Buffer& operator=( CD3D11Buffer&& ) = default;

private:
	D3D11_BUFFER_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};


class CD3D11Texture1D : public GraphicsResourceBase<ID3D11Texture1D>
{
public:
	virtual void InitResource( ) override;

	const D3D11_TEXTURE1D_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Texture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Texture1D( )
	{
		delete m_dataStorage;
	}
	CD3D11Texture1D( const CD3D11Texture1D& ) = delete;
	CD3D11Texture1D( CD3D11Texture1D&& ) = default;
	CD3D11Texture1D& operator=( const CD3D11Texture1D& ) = delete;
	CD3D11Texture1D& operator=( CD3D11Texture1D&& ) = delete;
	
private:
	D3D11_TEXTURE1D_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};

class CD3D11Texture2D : public GraphicsResourceBase<ID3D11Texture2D>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	const D3D11_TEXTURE2D_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Texture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Texture2D( )
	{
		delete m_dataStorage;
	}
	CD3D11Texture2D( const CD3D11Texture2D& ) = delete;
	CD3D11Texture2D( CD3D11Texture2D&& ) = default;
	CD3D11Texture2D& operator=( const CD3D11Texture2D& ) = delete;
	CD3D11Texture2D& operator=( CD3D11Texture2D&& ) = delete;

private:
	D3D11_TEXTURE2D_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};

class CD3D11Texture3D : public GraphicsResourceBase<ID3D11Texture3D>
{
public:
	virtual void InitResource( ) override;

	const D3D11_TEXTURE3D_DESC& GetDesc( ) const { return m_desc; }

	CD3D11Texture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	~CD3D11Texture3D( )
	{
		delete m_dataStorage;
	}
	CD3D11Texture3D( const CD3D11Texture3D& ) = delete;
	CD3D11Texture3D( CD3D11Texture3D&& ) = default;
	CD3D11Texture3D& operator=( const CD3D11Texture3D& ) = delete;
	CD3D11Texture3D& operator=( CD3D11Texture3D&& ) = delete;

private:
	D3D11_TEXTURE3D_DESC m_desc;
	D3D11_SUBRESOURCE_DATA m_initData;
	void* m_dataStorage = nullptr;
};

class CD3D11RenderTarget : public GraphicsViewBase<ID3D11RenderTargetView>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	CD3D11RenderTarget( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
	~CD3D11RenderTarget( ) = default;
	CD3D11RenderTarget( const CD3D11RenderTarget& ) = delete;
	CD3D11RenderTarget( CD3D11RenderTarget&& ) = default;
	CD3D11RenderTarget& operator=( const CD3D11RenderTarget& ) = delete;
	CD3D11RenderTarget& operator=( CD3D11RenderTarget&& ) = default;

private:
	RefHandle<CD3D11Texture2D> m_texture;
	D3D11_RENDER_TARGET_VIEW_DESC m_desc;
};

class CD3D11DepthStencil : public GraphicsViewBase<ID3D11DepthStencilView>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;
	void Clear( unsigned int clearFlag, float depth, unsigned char stencil );

	CD3D11DepthStencil( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
	~CD3D11DepthStencil( ) = default;
	CD3D11DepthStencil( const CD3D11DepthStencil& ) = delete;
	CD3D11DepthStencil( CD3D11DepthStencil&& ) = default;
	CD3D11DepthStencil& operator=( const CD3D11DepthStencil& ) = delete;
	CD3D11DepthStencil& operator=( CD3D11DepthStencil&& ) = default;

private:
	RefHandle<CD3D11Texture2D> m_texture;
	D3D11_DEPTH_STENCIL_VIEW_DESC m_desc;
};

class CD3D11ShaderResource : public GraphicsViewBase<ID3D11ShaderResourceView>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	CD3D11ShaderResource( RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11ShaderResource( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11ShaderResource( RefHandle<CD3D11Texture3D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11ShaderResource( RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT* traitOrNull );
	~CD3D11ShaderResource( ) = default;
	CD3D11ShaderResource( const CD3D11ShaderResource& ) = delete;
	CD3D11ShaderResource( CD3D11ShaderResource&& ) = default;
	CD3D11ShaderResource& operator=( const CD3D11ShaderResource& ) = delete;
	CD3D11ShaderResource& operator=( CD3D11ShaderResource&& ) = default;

private:
	RefHandle<IGraphicsResource> m_resource;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_desc;
};

class CD3D11RandomAccessResource : public GraphicsViewBase<ID3D11UnorderedAccessView>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	CD3D11RandomAccessResource( RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11RandomAccessResource( RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11RandomAccessResource( RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT* traitOrNull );
	~CD3D11RandomAccessResource( ) = default;
	CD3D11RandomAccessResource( const CD3D11RandomAccessResource& ) = delete;
	CD3D11RandomAccessResource( CD3D11RandomAccessResource&& ) = default;
	CD3D11RandomAccessResource& operator=( const CD3D11RandomAccessResource& ) = delete;
	CD3D11RandomAccessResource& operator=( CD3D11RandomAccessResource&& ) = default;

private:
	RefHandle<IGraphicsResource> m_resource;
	D3D11_UNORDERED_ACCESS_VIEW_DESC m_desc;
};

template <typename ResourceType>
class CD3D11ShaderBase : public DeviceDependantBase<ResourceType>
{
public:
	const void* ByteCode( ) const { return m_byteCodePtr; }
	std::size_t ByteCodeSize( ) const { return m_byteCodeSize; }

	CD3D11ShaderBase( const void* byteCodePtr, std::size_t byteCodeSize ) : m_byteCodeSize( byteCodeSize )
	{
		m_byteCodePtr = new unsigned char[m_byteCodeSize];
		std::memcpy( m_byteCodePtr, byteCodePtr, m_byteCodeSize );
	}

	virtual ~CD3D11ShaderBase( )
	{
		delete m_byteCodePtr;
	}

	CD3D11ShaderBase( const CD3D11ShaderBase& ) = delete;
	CD3D11ShaderBase( CD3D11ShaderBase&& other ) = default;
	CD3D11ShaderBase& operator=( const CD3D11ShaderBase& ) = delete;
	CD3D11ShaderBase& operator=( CD3D11ShaderBase&& other ) = default;

protected:
	void* m_byteCodePtr = nullptr;
	std::size_t m_byteCodeSize = 0;
};

class CD3D11VertexShader : public CD3D11ShaderBase<ID3D11VertexShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11VertexShader( const void* byteCodePtr, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11VertexShader>( byteCodePtr, byteCodeSize ) {}
	CD3D11VertexShader( const CD3D11VertexShader& ) = delete;
	CD3D11VertexShader( CD3D11VertexShader&& ) = default;
	CD3D11VertexShader& operator=( const CD3D11VertexShader& ) = delete;
	CD3D11VertexShader& operator=( CD3D11VertexShader&& ) = default;
	~CD3D11VertexShader( ) = default;
};

class CD3D11GeometryShader : public CD3D11ShaderBase<ID3D11GeometryShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11GeometryShader( const void* byteCodePtr, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11GeometryShader>( byteCodePtr, byteCodeSize ) {}
	CD3D11GeometryShader( const CD3D11GeometryShader& ) = delete;
	CD3D11GeometryShader( CD3D11GeometryShader&& ) = default;
	CD3D11GeometryShader& operator=( const CD3D11GeometryShader& ) = delete;
	CD3D11GeometryShader& operator=( CD3D11GeometryShader&& ) = default;
	~CD3D11GeometryShader( ) = default;
};

class CD3D11PixelShader : public CD3D11ShaderBase<ID3D11PixelShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11PixelShader( const void* byteCodePtr, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11PixelShader>( byteCodePtr, byteCodeSize ) {}
	CD3D11PixelShader( const CD3D11PixelShader& ) = delete;
	CD3D11PixelShader( CD3D11PixelShader&& ) = default;
	CD3D11PixelShader& operator=( const CD3D11PixelShader& ) = delete;
	CD3D11PixelShader& operator=( CD3D11PixelShader&& ) = default;
	~CD3D11PixelShader( ) = default;
};

class CD3D11ComputeShader : public CD3D11ShaderBase<ID3D11ComputeShader>
{
public:
	virtual void InitResource( ) override;

	CD3D11ComputeShader( const void* byteCodePtr, std::size_t byteCodeSize ) : CD3D11ShaderBase<ID3D11ComputeShader>( byteCodePtr, byteCodeSize ) {}
	CD3D11ComputeShader( const CD3D11ComputeShader& ) = delete;
	CD3D11ComputeShader( CD3D11ComputeShader&& ) = default;
	CD3D11ComputeShader& operator=( const CD3D11ComputeShader& ) = delete;
	CD3D11ComputeShader& operator=( CD3D11ComputeShader&& ) = default;
	~CD3D11ComputeShader( ) = default;
};

class CD3D11VertexLayout : public DeviceDependantBase<ID3D11InputLayout>
{
public:
	virtual void InitResource( ) override;

	CD3D11VertexLayout( RefHandle<CD3D11VertexShader>& vs, const VERTEX_LAYOUT* layout, int layoutSize );
	CD3D11VertexLayout( const CD3D11VertexLayout& ) = delete;
	CD3D11VertexLayout( CD3D11VertexLayout&& ) = default;
	CD3D11VertexLayout& operator=( const CD3D11VertexLayout& ) = delete;
	CD3D11VertexLayout& operator=( CD3D11VertexLayout&& ) = default;
	~CD3D11VertexLayout( );

private:
	D3D11_INPUT_ELEMENT_DESC* m_imputDesc;
	unsigned int m_layoutSize;
	RefHandle<CD3D11VertexShader> m_vs;
};

struct CD3D11DepthStencilDesc : public CD3D11_DEPTH_STENCIL_DESC
{
public:
	using CD3D11_DEPTH_STENCIL_DESC::CD3D11_DEPTH_STENCIL_DESC;
	unsigned int StencilRef = 0;
};

class CD3D11DepthStencilState : public DeviceDependantBase<ID3D11DepthStencilState>
{
public:
	virtual void InitResource( ) override;

	unsigned int GetStencilRef( ) const { return m_stencilRef; }
	void SetStencilRef( unsigned int stencilRef ) { m_stencilRef = stencilRef; }

	CD3D11DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait );
	CD3D11DepthStencilState( const CD3D11DepthStencilState& ) = delete;
	CD3D11DepthStencilState( CD3D11DepthStencilState&& ) = default;
	CD3D11DepthStencilState& operator=( const CD3D11DepthStencilState& ) = delete;
	CD3D11DepthStencilState& operator=( CD3D11DepthStencilState&& ) = default;
	~CD3D11DepthStencilState( ) = default;

private:
	unsigned int m_stencilRef = 0;
	D3D11_DEPTH_STENCIL_DESC m_desc;
};

class CD3D11RasterizerState : public DeviceDependantBase<ID3D11RasterizerState>
{
public:
	virtual void InitResource( ) override;

	CD3D11RasterizerState( const RASTERIZER_STATE_TRAIT& trait );
	CD3D11RasterizerState( const CD3D11RasterizerState& ) = delete;
	CD3D11RasterizerState( CD3D11RasterizerState&& ) = default;
	CD3D11RasterizerState& operator=( const CD3D11RasterizerState& ) = delete;
	CD3D11RasterizerState& operator=( CD3D11RasterizerState&& ) = default;
	~CD3D11RasterizerState( ) = default;

private:
	D3D11_RASTERIZER_DESC m_desc;
};

class CD3D11SamplerState : public DeviceDependantBase<ID3D11SamplerState>
{
public:
	virtual void InitResource( ) override;

	CD3D11SamplerState( const SAMPLER_STATE_TRAIT& trait );
	CD3D11SamplerState( const CD3D11SamplerState& ) = default;
	CD3D11SamplerState( CD3D11SamplerState&& ) = default;
	CD3D11SamplerState& operator=( const CD3D11SamplerState& ) = default;
	CD3D11SamplerState& operator=( CD3D11SamplerState&& ) = default;
	~CD3D11SamplerState( ) = default;

private:
	D3D11_SAMPLER_DESC m_desc;
};

struct CD3D_BLEND_DESC
{
	CD3D11_BLEND_DESC m_desc = CD3D11_BLEND_DESC( CD3D11_DEFAULT( ) );
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
};

class CD3D11BlendState : public DeviceDependantBase<ID3D11BlendState>
{
public:
	virtual void InitResource( ) override;

	const float* GetBlendFactor( ) const { return m_blendFactor.data( ); }

	unsigned int GetSamplerMask( ) const { return m_sampleMask; }

	void SetBlendFactor( const std::array<float, 4>& blendFactor ) noexcept
	{
		m_blendFactor = blendFactor;
	}

	void SetSampleMask( unsigned int sampleMask ) noexcept { m_sampleMask = sampleMask; }

	CD3D11BlendState( const BLEND_STATE_TRAIT& trait );
	CD3D11BlendState( const CD3D11BlendState& ) = delete;
	CD3D11BlendState( CD3D11BlendState&& ) = default;
	CD3D11BlendState& operator=( const CD3D11BlendState& ) = delete;
	CD3D11BlendState& operator=( CD3D11BlendState&& ) = default;
	~CD3D11BlendState( ) = default;

private:
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
	D3D11_BLEND_DESC m_desc;
};