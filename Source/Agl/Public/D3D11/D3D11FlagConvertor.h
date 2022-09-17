#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <d3d11.h>

namespace agl
{
	inline D3D11_USAGE ConvertAccessFlagToUsage( ResourceAccessFlag accessFlag )
	{
		switch ( static_cast<uint8>( accessFlag ) )
		{
		case static_cast<uint8>( ResourceAccessFlag::GpuRead | ResourceAccessFlag::GpuWrite ):
			return D3D11_USAGE_DEFAULT;
		case static_cast<uint8>( ResourceAccessFlag::GpuRead | ResourceAccessFlag::CpuWrite ):
			return D3D11_USAGE_DYNAMIC;
		case static_cast<uint8>( ResourceAccessFlag::GpuRead ):
			return D3D11_USAGE_IMMUTABLE;
		case static_cast<uint8>( ResourceAccessFlag::GpuRead | ResourceAccessFlag::GpuWrite | ResourceAccessFlag::CpuRead | ResourceAccessFlag::CpuWrite ):
		case static_cast<uint8>( ResourceAccessFlag::GpuRead | ResourceAccessFlag::GpuWrite | ResourceAccessFlag::CpuRead ):
			return D3D11_USAGE_STAGING;
		default:
			assert( false );
			return D3D11_USAGE_DEFAULT;
		}
	}

	inline ResourceAccessFlag ConvertUsageToAccessFlag( D3D11_USAGE accessFlag )
	{
		switch ( accessFlag )
		{
		case D3D11_USAGE_DEFAULT:
			return ResourceAccessFlag::GpuRead | ResourceAccessFlag::GpuWrite;
		case D3D11_USAGE_DYNAMIC:
			return ResourceAccessFlag::GpuRead | ResourceAccessFlag::CpuWrite;
		case D3D11_USAGE_IMMUTABLE:
			return ResourceAccessFlag::GpuRead;
		case D3D11_USAGE_STAGING:
			return ResourceAccessFlag::GpuRead | ResourceAccessFlag::GpuWrite
				| ResourceAccessFlag::CpuRead | ResourceAccessFlag::CpuWrite;
		default:
			assert( false );
			return ResourceAccessFlag::None;
		}
	}

	inline uint32 ConvertTypeToBind( ResourceBindType type )
	{
		uint32 ret = 0;

		if ( HasAnyFlags( type, ResourceBindType::VertexBuffer ) )
		{
			ret |= D3D11_BIND_VERTEX_BUFFER;
		}

		if ( HasAnyFlags( type, ResourceBindType::IndexBuffer ) )
		{
			ret |= D3D11_BIND_INDEX_BUFFER;
		}

		if ( HasAnyFlags( type, ResourceBindType::ConstantBuffer ) )
		{
			ret |= D3D11_BIND_CONSTANT_BUFFER;
		}

		if ( HasAnyFlags( type, ResourceBindType::ShaderResource ) )
		{
			ret |= D3D11_BIND_SHADER_RESOURCE;
		}

		if ( HasAnyFlags( type, ResourceBindType::StreamOutput ) )
		{
			ret |= D3D11_BIND_STREAM_OUTPUT;
		}

		if ( HasAnyFlags( type, ResourceBindType::RenderTarget ) )
		{
			ret |= D3D11_BIND_RENDER_TARGET;
		}

		if ( HasAnyFlags( type, ResourceBindType::DepthStencil ) )
		{
			ret |= D3D11_BIND_DEPTH_STENCIL;
		}

		if ( HasAnyFlags( type, ResourceBindType::RandomAccess ) )
		{
			ret |= D3D11_BIND_UNORDERED_ACCESS;
		}

		return ret;
	}

	inline ResourceBindType ConvertBindToType( uint32 type )
	{
		ResourceBindType ret = ResourceBindType::None;

		if ( type & D3D11_BIND_VERTEX_BUFFER )
		{
			ret |= ResourceBindType::VertexBuffer;
		}

		if ( type & D3D11_BIND_INDEX_BUFFER )
		{
			ret |= ResourceBindType::IndexBuffer;
		}

		if ( type & D3D11_BIND_CONSTANT_BUFFER )
		{
			ret |= ResourceBindType::ConstantBuffer;
		}

		if ( type & D3D11_BIND_SHADER_RESOURCE )
		{
			ret |= ResourceBindType::ShaderResource;
		}

		if ( type & D3D11_BIND_STREAM_OUTPUT )
		{
			ret |= ResourceBindType::StreamOutput;
		}

		if ( type & D3D11_BIND_RENDER_TARGET )
		{
			ret |= ResourceBindType::RenderTarget;
		}

		if ( type & D3D11_BIND_DEPTH_STENCIL )
		{
			ret |= ResourceBindType::DepthStencil;
		}

		if ( type & D3D11_BIND_UNORDERED_ACCESS )
		{
			ret |= ResourceBindType::RandomAccess;
		}

		return ret;
	}

	inline uint32 ConvertMicsToDXMisc( ResourceMisc miscFlag )
	{
		uint32 ret = 0;

		if ( HasAnyFlags( miscFlag, ResourceMisc::GenerateMips ) )
		{
			ret |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::Shared ) )
		{
			ret |= D3D11_RESOURCE_MISC_SHARED;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::TextureCube ) )
		{
			ret |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::DrawIndirectArgs ) )
		{
			ret |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::BufferAllowRawViews ) )
		{
			assert( ( miscFlag & ResourceMisc::BufferStructured ) == ResourceMisc::None );
			ret |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::BufferStructured ) )
		{
			assert( ( miscFlag & ResourceMisc::BufferAllowRawViews ) == ResourceMisc::None );
			ret |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::ResourceClamp ) )
		{
			ret |= D3D11_RESOURCE_MISC_RESOURCE_CLAMP;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::SharedKeyedMutex ) )
		{
			ret |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
		}

		if ( HasAnyFlags( miscFlag, ResourceMisc::GdiCompatible ) )
		{
			ret |= D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
		}

		return ret;
	}

	inline ResourceMisc ConvertDXMiscToMisc( uint32 miscFlag )
	{
		ResourceMisc ret = ResourceMisc::None;

		if ( miscFlag & D3D11_RESOURCE_MISC_GENERATE_MIPS )
		{
			ret |= ResourceMisc::GenerateMips;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_SHARED )
		{
			ret |= ResourceMisc::Shared;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_TEXTURECUBE )
		{
			ret |= ResourceMisc::TextureCube;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS )
		{
			ret |= ResourceMisc::DrawIndirectArgs;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
		{
			ret |= ResourceMisc::BufferAllowRawViews;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED )
		{
			ret |= ResourceMisc::BufferStructured;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_RESOURCE_CLAMP )
		{
			ret |= ResourceMisc::ResourceClamp;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX )
		{
			ret |= ResourceMisc::SharedKeyedMutex;
		}

		if ( miscFlag & D3D11_RESOURCE_MISC_GDI_COMPATIBLE )
		{
			ret |= ResourceMisc::GdiCompatible;
		}

		return ret;
	}

	inline uint32 ConvertAccessFlagToCpuFlag( ResourceAccessFlag accessFlag )
	{
		uint32 ret = 0;

		if ( HasAnyFlags( accessFlag, ResourceAccessFlag::CpuRead ) )
		{
			ret |= D3D11_CPU_ACCESS_READ;
		}

		if ( HasAnyFlags( accessFlag, ResourceAccessFlag::CpuWrite ) )
		{
			ret |= D3D11_CPU_ACCESS_WRITE;
		}

		return ret;
	}

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
			return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
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

	inline ResourcePrimitive Convert3D11PrimToPrim( D3D_PRIMITIVE_TOPOLOGY primitive )
	{
		switch ( primitive )
		{
		case D3D_PRIMITIVE_TOPOLOGY_UNDEFINED:
			return ResourcePrimitive::Undefined;
		case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
			return ResourcePrimitive::Pointlist;
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
			return ResourcePrimitive::Linelist;
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
			return ResourcePrimitive::Linestrip;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			return ResourcePrimitive::Trianglelist;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			return ResourcePrimitive::TriangleStrip;
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
			return ResourcePrimitive::LinelistAdj;
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
			return ResourcePrimitive::LinestripAdj;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
			return ResourcePrimitive::TrianglelistAdj;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
			return ResourcePrimitive::TriangleStripAdj;
		case D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist1;
		case D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist2;
		case D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist3;
		case D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist4;
		case D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist5;
		case D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist6;
		case D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist7;
		case D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist8;
		case D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist9;
		case D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist10;
		case D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist11;
		case D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist12;
		case D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist13;
		case D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist14;
		case D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist15;
		case D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist16;
		case D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist17;
		case D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist18;
		case D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist19;
		case D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist20;
		case D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist21;
		case D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist22;
		case D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist23;
		case D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist24;
		case D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist25;
		case D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist26;
		case D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist27;
		case D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist28;
		case D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist29;
		case D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist30;
		case D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist31;
		case D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
			return ResourcePrimitive::ControlPointPatchlist32;
		default:
			assert( false );
			return ResourcePrimitive::Undefined;
		}
	}

	inline D3D_PRIMITIVE_TOPOLOGY ConvertPrimToD3D11Prim( ResourcePrimitive primitive )
	{
		switch ( primitive )
		{
		case ResourcePrimitive::Undefined:
			return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case ResourcePrimitive::Pointlist:
			return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		case ResourcePrimitive::Linelist:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case ResourcePrimitive::Linestrip:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case ResourcePrimitive::Trianglelist:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case ResourcePrimitive::TriangleStrip:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case ResourcePrimitive::LinelistAdj:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
		case ResourcePrimitive::LinestripAdj:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;
		case ResourcePrimitive::TrianglelistAdj:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		case ResourcePrimitive::TriangleStripAdj:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ;
		case ResourcePrimitive::ControlPointPatchlist1:
			return D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist2:
			return D3D_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist3:
			return D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist4:
			return D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist5:
			return D3D_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist6:
			return D3D_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist7:
			return D3D_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist8:
			return D3D_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist9:
			return D3D_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist10:
			return D3D_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist11:
			return D3D_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist12:
			return D3D_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist13:
			return D3D_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist14:
			return D3D_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist15:
			return D3D_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist16:
			return D3D_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist17:
			return D3D_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist18:
			return D3D_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist19:
			return D3D_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist20:
			return D3D_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist21:
			return D3D_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist22:
			return D3D_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist23:
			return D3D_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist24:
			return D3D_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist25:
			return D3D_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist26:
			return D3D_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist27:
			return D3D_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist28:
			return D3D_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist29:
			return D3D_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist30:
			return D3D_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist31:
			return D3D_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST;
		case ResourcePrimitive::ControlPointPatchlist32:
			return D3D_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST;
		default:
			assert( false );
			return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}

	inline D3D11_MAP ConvertLockFlagToD3D11Map( ResourceLockFlag lockFlag )
	{
		switch ( lockFlag )
		{
		case ResourceLockFlag::Read:
			return D3D11_MAP_READ;
		case ResourceLockFlag::ReadWrite:
			return D3D11_MAP_READ_WRITE;
		case ResourceLockFlag::Write:
			return D3D11_MAP_WRITE;
		case ResourceLockFlag::WriteDiscard:
			return D3D11_MAP_WRITE_DISCARD;
		case ResourceLockFlag::WriteNoOverwrite:
			return D3D11_MAP_WRITE_NO_OVERWRITE;
		default:
			__debugbreak();
			return static_cast<D3D11_MAP>( 0 );
		}
	}

	inline D3D11_DEPTH_WRITE_MASK ConvertToDepthWriteMask( DepthWriteMode depthWriteMode )
	{
		switch ( depthWriteMode )
		{
		case DepthWriteMode::Zero:
			return D3D11_DEPTH_WRITE_MASK_ZERO;
		case DepthWriteMode::All:
			return D3D11_DEPTH_WRITE_MASK_ALL;
		default:
			assert( false );
			return D3D11_DEPTH_WRITE_MASK_ZERO;
		}
	}

	inline D3D11_COMPARISON_FUNC ConvertToComparisionFunc( ComparisonFunc comparisonFunc )
	{
		switch ( comparisonFunc )
		{
		case ComparisonFunc::Never:
			return D3D11_COMPARISON_NEVER;
		case ComparisonFunc::Less:
			return D3D11_COMPARISON_LESS;
		case ComparisonFunc::Equal:
			return D3D11_COMPARISON_EQUAL;
		case ComparisonFunc::LessEqual:
			return D3D11_COMPARISON_LESS_EQUAL;
		case ComparisonFunc::Greater:
			return D3D11_COMPARISON_GREATER;
		case ComparisonFunc::NotEqual:
			return D3D11_COMPARISON_NOT_EQUAL;
		case ComparisonFunc::GreaterEqual:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case ComparisonFunc::Always:
			return D3D11_COMPARISON_ALWAYS;
		default:
			assert( false );
			return D3D11_COMPARISON_NEVER;
		}
	}

	inline D3D11_STENCIL_OP ConvertToStencilOp( StencilOp stencilOp )
	{
		switch ( stencilOp )
		{
		case StencilOp::Keep:
			return D3D11_STENCIL_OP_KEEP;
		case StencilOp::Zero:
			return D3D11_STENCIL_OP_ZERO;
		case StencilOp::Replace:
			return D3D11_STENCIL_OP_REPLACE;
		case StencilOp::IncrSat:
			return D3D11_STENCIL_OP_INCR_SAT;
		case StencilOp::DecrSat:
			return D3D11_STENCIL_OP_DECR_SAT;
		case StencilOp::Invert:
			return D3D11_STENCIL_OP_INVERT;
		case StencilOp::Incr:
			return D3D11_STENCIL_OP_INCR;
		case StencilOp::Decr:
			return D3D11_STENCIL_OP_DECR;
		default:
			assert( false );
			return D3D11_STENCIL_OP_KEEP;
		}
	}

	inline D3D11_FILL_MODE ConvertToFillMode( FillMode fillMode )
	{
		switch ( fillMode )
		{
		case FillMode::Wireframe:
			return D3D11_FILL_WIREFRAME;
		case FillMode::Solid:
			return D3D11_FILL_SOLID;
		default:
			assert( false );
			return D3D11_FILL_WIREFRAME;
		}
	}

	inline D3D11_CULL_MODE ConvertToCullMode( CullMode cullMode )
	{
		switch ( cullMode )
		{
		case CullMode::None:
			return D3D11_CULL_NONE;
		case CullMode::Front:
			return D3D11_CULL_FRONT;
		case CullMode::Back:
			return D3D11_CULL_BACK;
		default:
			assert( false );
			return D3D11_CULL_NONE;
		}
	}

	inline D3D11_FILTER ConvertToFilter( TextureFilter filter )
	{
		return static_cast<D3D11_FILTER>( filter );
	}

	inline D3D11_TEXTURE_ADDRESS_MODE ConvertToTextureAddress( TextureAddressMode addressMode )
	{
		switch ( addressMode )
		{
		case TextureAddressMode::Wrap:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case TextureAddressMode::Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case TextureAddressMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case TextureAddressMode::Border:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		case TextureAddressMode::MirrorOnce:
			return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
		default:
			assert( false );
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}
	}

	inline D3D11_BLEND ConvertToBlend( Blend blend )
	{
		switch ( blend )
		{
		case Blend::Zero:
			return D3D11_BLEND_ZERO;
		case Blend::One:
			return D3D11_BLEND_ONE;
		case Blend::SrcColor:
			return D3D11_BLEND_SRC_COLOR;
		case Blend::InvSrcColor:
			return D3D11_BLEND_INV_SRC_COLOR;
		case Blend::SrcAlpha:
			return D3D11_BLEND_SRC_ALPHA;
		case Blend::InvSrcAlpha:
			return D3D11_BLEND_INV_SRC_ALPHA;
		case Blend::DestAlpha:
			return D3D11_BLEND_DEST_ALPHA;
		case Blend::InvDestAlpha:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case Blend::DestColor:
			return D3D11_BLEND_DEST_COLOR;
		case Blend::InvDestColor:
			return D3D11_BLEND_INV_DEST_COLOR;
		case Blend::SrcAlphaSat:
			return D3D11_BLEND_SRC_ALPHA_SAT;
		case Blend::BlendFactor:
			return D3D11_BLEND_BLEND_FACTOR;
		case Blend::InvBlendFactor:
			return D3D11_BLEND_INV_BLEND_FACTOR;
		case Blend::Src1Color:
			return D3D11_BLEND_SRC1_COLOR;
		case Blend::InvSrc1Color:
			return D3D11_BLEND_INV_SRC1_COLOR;
		case Blend::Src1Alpha:
			return D3D11_BLEND_SRC1_ALPHA;
		case Blend::InvSrc1Alpha:
			return D3D11_BLEND_INV_SRC1_ALPHA;
		default:
			assert( false );
			return D3D11_BLEND_ZERO;
		}
	}

	inline D3D11_BLEND_OP ConvertToBlendOp( BlendOp blendOp )
	{
		switch ( blendOp )
		{
		case BlendOp::Add:
			return D3D11_BLEND_OP_ADD;
		case BlendOp::Subtract:
			return D3D11_BLEND_OP_SUBTRACT;
		case BlendOp::ReverseSubtract:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Min:
			return D3D11_BLEND_OP_MIN;
		case BlendOp::Max:
			return D3D11_BLEND_OP_MAX;
		default:
			assert( false );
			return D3D11_BLEND_OP_ADD;
		}
	}

	inline unsigned char ConvertToColorWriteEnable( ColorWriteEnable colorWriteEnable )
	{
		return static_cast<unsigned char>( colorWriteEnable );
	}
}
