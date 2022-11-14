#pragma once

#include "DxgiFlagConvertor.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
	inline D3D12_HEAP_TYPE ConvertAccessFlagToHeapType( ResourceAccessFlag accessFlag )
	{
		if ( HasAnyFlags( accessFlag, ResourceAccessFlag::CpuRead ) )
		{
			return D3D12_HEAP_TYPE_READBACK;
		}
		else if ( HasAnyFlags( accessFlag, ResourceAccessFlag::CpuWrite ) )
		{
			return D3D12_HEAP_TYPE_UPLOAD;
		}
		else
		{
			return D3D12_HEAP_TYPE_DEFAULT;
		}
	}

	inline ResourceAccessFlag ConvertHeapTypeToAccessFlag( D3D12_HEAP_TYPE heapType )
	{
		switch ( heapType )
		{
		case D3D12_HEAP_TYPE_DEFAULT:
			return ResourceAccessFlag::GpuRead 
				| ResourceAccessFlag::GpuWrite;
		case D3D12_HEAP_TYPE_UPLOAD:
			return ResourceAccessFlag::GpuRead
				| ResourceAccessFlag::GpuWrite
				| ResourceAccessFlag::CpuWrite;
		case D3D12_HEAP_TYPE_READBACK:
			return ResourceAccessFlag::GpuRead
				| ResourceAccessFlag::GpuWrite 
				| ResourceAccessFlag::CpuRead;
		default:
			assert( false );
			return ResourceAccessFlag::None;
		}
	}

	inline D3D12_COMPARISON_FUNC ConvertToComparisionFunc( ComparisonFunc comparisonFunc )
	{
		switch ( comparisonFunc )
		{
		case ComparisonFunc::Never:
			return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::Less:
			return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::Equal:
			return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::LessEqual:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::Greater:
			return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::NotEqual:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case ComparisonFunc::GreaterEqual:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::Always:
			return D3D12_COMPARISON_FUNC_ALWAYS;
		default:
			assert( false );
			return D3D12_COMPARISON_FUNC_NEVER;
		}
	}

	inline D3D12_FILTER ConvertToFilter( TextureFilter filter )
	{
		return static_cast<D3D12_FILTER>( filter );
	}

	inline D3D12_TEXTURE_ADDRESS_MODE ConvertToTextureAddress( TextureAddressMode addressMode )
	{
		switch ( addressMode )
		{
		case TextureAddressMode::Wrap:
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case TextureAddressMode::Mirror:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case TextureAddressMode::Clamp:
			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureAddressMode::Border:
			return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case TextureAddressMode::MirrorOnce:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		default:
			assert( false );
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		}
	}

	inline ResourceBindType ConvertResourceFlagsToBindType( D3D12_RESOURCE_FLAGS flags )
	{
		ResourceBindType bindType = ResourceBindType::None;

		if ( ( flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET ) > 0 )
		{
			bindType |= ResourceBindType::RenderTarget;
		}

		if ( ( flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL ) > 0 )
		{
			bindType |= ResourceBindType::DepthStencil;
		}

		if ( ( flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS ) > 0 )
		{
			bindType |= ResourceBindType::RandomAccess;
		}

		if ( ( flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE ) == 0 )
		{
			bindType |= ResourceBindType::ShaderResource;
		}

		return bindType;
	}

	inline D3D12_RESOURCE_STATES ConvertToResourceStates( ResourceState state )
	{
		D3D12_RESOURCE_STATES d3d12ResourceState = D3D12_RESOURCE_STATE_COMMON;

		if ( HasAnyFlags( state, ResourceState::Common ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_COMMON;
		}

		if ( HasAnyFlags( state, ResourceState::VertexAndConstantBuffer ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		}

		if ( HasAnyFlags( state, ResourceState::Indexbuffer ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		}

		if ( HasAnyFlags( state, ResourceState::RenderTarget ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		}

		if ( HasAnyFlags( state, ResourceState::UnorderedAccess ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		}

		if ( HasAnyFlags( state, ResourceState::DepthWrite ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}

		if ( HasAnyFlags( state, ResourceState::DepthRead ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_DEPTH_READ;
		}

		if ( HasAnyFlags( state, ResourceState::NonPixelShaderResource ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		}

		if ( HasAnyFlags( state, ResourceState::PixelShaderResource ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		}

		if ( HasAnyFlags( state, ResourceState::StreamOut ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_STREAM_OUT;
		}

		if ( HasAnyFlags( state, ResourceState::IndirectArgument ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		}

		if ( HasAnyFlags( state, ResourceState::CopyDest ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_COPY_DEST;
		}

		if ( HasAnyFlags( state, ResourceState::CopySource ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		}

		if ( HasAnyFlags( state, ResourceState::ResolveDest ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_RESOLVE_DEST;
		}

		if ( HasAnyFlags( state, ResourceState::ResolveSource ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		}

		if ( HasAnyFlags( state, ResourceState::RaytracingAccelerationStructure ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		}

		if ( HasAnyFlags( state, ResourceState::ShadingRateSource ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE;
		}

		if ( HasAnyFlags( state, ResourceState::GenericRead ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_GENERIC_READ;
		}

		if ( HasAnyFlags( state, ResourceState::Present ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_PRESENT;
		}

		if ( HasAnyFlags( state, ResourceState::Predication ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_PREDICATION;
		}

		if ( HasAnyFlags( state, ResourceState::VideoDecodeRead ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_VIDEO_DECODE_READ;
		}

		if ( HasAnyFlags( state, ResourceState::VideoDecodeWrite ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE;
		}

		if ( HasAnyFlags( state, ResourceState::VideoProcessRead ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ;
		}

		if ( HasAnyFlags( state, ResourceState::VideoProecssWrite ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE;
		}

		if ( HasAnyFlags( state, ResourceState::VideoEncodeRead ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ;
		}

		if ( HasAnyFlags( state, ResourceState::VideoEncodeWrite ) )
		{
			d3d12ResourceState |= D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE;
		}

		return d3d12ResourceState;
	}

	inline D3D12_RESOURCE_BARRIER ConvertToResourceBarrier( const ResourceTransition& transition )
	{
		D3D12_RESOURCE_BARRIER barrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition =
			{
				.pResource = static_cast<ID3D12Resource*>( transition.m_pResource ),
				.Subresource = transition.m_subResource,
				.StateBefore = ConvertToResourceStates( transition.m_before ),
				.StateAfter  = ConvertToResourceStates( transition.m_after )
			}
		};

		return barrier;
	}
}
