#include "D3D12FlagConvertor.h"

namespace agl
{
	D3D12_HEAP_TYPE ConvertAccessFlagToHeapType( ResourceAccessFlag accessFlag )
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

	ResourceAccessFlag ConvertHeapTypeToAccessFlag( D3D12_HEAP_TYPE heapType )
	{
		switch ( heapType )
		{
		case D3D12_HEAP_TYPE_DEFAULT:
			return ResourceAccessFlag::Default;
		case D3D12_HEAP_TYPE_UPLOAD:
			return ResourceAccessFlag::Upload;
		case D3D12_HEAP_TYPE_READBACK:
			return ResourceAccessFlag::Download;
		default:
			assert( false );
			return ResourceAccessFlag::None;
		}
	}

	D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimToD3D12PrimType( ResourcePrimitive primitive )
	{
		switch ( primitive )
		{
		case ResourcePrimitive::Undefined:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
		case ResourcePrimitive::Pointlist:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
		case ResourcePrimitive::Linelist:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case ResourcePrimitive::Linestrip:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case ResourcePrimitive::Trianglelist:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case ResourcePrimitive::TriangleStrip:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case ResourcePrimitive::LinelistAdj:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case ResourcePrimitive::LinestripAdj:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		case ResourcePrimitive::TrianglelistAdj:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case ResourcePrimitive::TriangleStripAdj:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		case ResourcePrimitive::ControlPointPatchlist1:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist2:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist3:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist4:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist5:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist6:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist7:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist8:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist9:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist10:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist11:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist12:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist13:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist14:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist15:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist16:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist17:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist18:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist19:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist20:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist21:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist22:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist23:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist24:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist25:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist26:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist27:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist28:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist29:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist30:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist31:
			[[fallthrough]];
		case ResourcePrimitive::ControlPointPatchlist32:
			return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		}

		assert( false );
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}

	D3D12_PRIMITIVE_TOPOLOGY ConvertPrimToD3D12Prim( ResourcePrimitive primitive )
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

	D3D12_DEPTH_WRITE_MASK ConvertToDepthWriteMask( DepthWriteMode depthWriteMode )
	{
		switch ( depthWriteMode )
		{
		case DepthWriteMode::Zero:
			return D3D12_DEPTH_WRITE_MASK_ZERO;
		case DepthWriteMode::All:
			return D3D12_DEPTH_WRITE_MASK_ALL;
		default:
			assert( false );
			return D3D12_DEPTH_WRITE_MASK_ZERO;
		}
	}

	D3D12_COMPARISON_FUNC ConvertToComparisionFunc( ComparisonFunc comparisonFunc )
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

	D3D12_STENCIL_OP ConvertToStencilOp( StencilOp stencilOp )
	{
		switch ( stencilOp )
		{
		case StencilOp::Keep:
			return D3D12_STENCIL_OP_KEEP;
		case StencilOp::Zero:
			return D3D12_STENCIL_OP_ZERO;
		case StencilOp::Replace:
			return D3D12_STENCIL_OP_REPLACE;
		case StencilOp::IncrSat:
			return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOp::DecrSat:
			return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOp::Invert:
			return D3D12_STENCIL_OP_INVERT;
		case StencilOp::Incr:
			return D3D12_STENCIL_OP_INCR;
		case StencilOp::Decr:
			return D3D12_STENCIL_OP_DECR;
		default:
			assert( false );
			return D3D12_STENCIL_OP_KEEP;
		}
	}

	D3D12_FILL_MODE ConvertToFillMode( FillMode fillMode )
	{
		switch ( fillMode )
		{
		case FillMode::Wireframe:
			return D3D12_FILL_MODE_WIREFRAME;
		case FillMode::Solid:
			return D3D12_FILL_MODE_SOLID;
		default:
			assert( false );
			return D3D12_FILL_MODE_WIREFRAME;
		}
	}

	D3D12_CULL_MODE ConvertToCullMode( CullMode cullMode )
	{
		switch ( cullMode )
		{
		case CullMode::None:
			return D3D12_CULL_MODE_NONE;
		case CullMode::Front:
			return D3D12_CULL_MODE_FRONT;
		case CullMode::Back:
			return D3D12_CULL_MODE_BACK;
		default:
			assert( false );
			return D3D12_CULL_MODE_NONE;
		}
	}

	D3D12_FILTER ConvertToFilter( TextureFilter filter )
	{
		return static_cast<D3D12_FILTER>( filter );
	}

	D3D12_TEXTURE_ADDRESS_MODE ConvertToTextureAddress( TextureAddressMode addressMode )
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

	D3D12_BLEND ConvertToBlend( Blend blend )
	{
		switch ( blend )
		{
		case Blend::Zero:
			return D3D12_BLEND_ZERO;
		case Blend::One:
			return D3D12_BLEND_ONE;
		case Blend::SrcColor:
			return D3D12_BLEND_SRC_COLOR;
		case Blend::InvSrcColor:
			return D3D12_BLEND_INV_SRC_COLOR;
		case Blend::SrcAlpha:
			return D3D12_BLEND_SRC_ALPHA;
		case Blend::InvSrcAlpha:
			return D3D12_BLEND_INV_SRC_ALPHA;
		case Blend::DestAlpha:
			return D3D12_BLEND_DEST_ALPHA;
		case Blend::InvDestAlpha:
			return D3D12_BLEND_INV_DEST_ALPHA;
		case Blend::DestColor:
			return D3D12_BLEND_DEST_COLOR;
		case Blend::InvDestColor:
			return D3D12_BLEND_INV_DEST_COLOR;
		case Blend::SrcAlphaSat:
			return D3D12_BLEND_SRC_ALPHA_SAT;
		case Blend::BlendFactor:
			return D3D12_BLEND_BLEND_FACTOR;
		case Blend::InvBlendFactor:
			return D3D12_BLEND_INV_BLEND_FACTOR;
		case Blend::Src1Color:
			return D3D12_BLEND_SRC1_COLOR;
		case Blend::InvSrc1Color:
			return D3D12_BLEND_INV_SRC1_COLOR;
		case Blend::Src1Alpha:
			return D3D12_BLEND_SRC1_ALPHA;
		case Blend::InvSrc1Alpha:
			return D3D12_BLEND_INV_SRC1_ALPHA;
		default:
			assert( false );
			return D3D12_BLEND_ZERO;
		}
	}

	D3D12_BLEND_OP ConvertToBlendOp( BlendOp blendOp )
	{
		switch ( blendOp )
		{
		case BlendOp::Add:
			return D3D12_BLEND_OP_ADD;
		case BlendOp::Subtract:
			return D3D12_BLEND_OP_SUBTRACT;
		case BlendOp::ReverseSubtract:
			return D3D12_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Min:
			return D3D12_BLEND_OP_MIN;
		case BlendOp::Max:
			return D3D12_BLEND_OP_MAX;
		default:
			assert( false );
			return D3D12_BLEND_OP_ADD;
		}
	}

	ResourceBindType ConvertResourceFlagsToBindType( D3D12_RESOURCE_FLAGS flags )
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

	D3D12_RESOURCE_STATES ConvertToResourceStates( ResourceState state )
	{
		D3D12_RESOURCE_STATES d3d12ResourceState = D3D12_RESOURCE_STATE_COMMON;

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

	ResourceState ConvertToResourceStates( D3D12_RESOURCE_STATES state )
	{
		ResourceState resourceState = ResourceState::Common;

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER ) )
		{
			resourceState |= ResourceState::VertexAndConstantBuffer;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_INDEX_BUFFER ) )
		{
			resourceState |= ResourceState::Indexbuffer;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_RENDER_TARGET ) )
		{
			resourceState |= ResourceState::RenderTarget;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_UNORDERED_ACCESS ) )
		{
			resourceState |= ResourceState::UnorderedAccess;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_DEPTH_WRITE ) )
		{
			resourceState |= ResourceState::DepthWrite;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_DEPTH_READ ) )
		{
			resourceState |= ResourceState::DepthRead;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE ) )
		{
			resourceState |= ResourceState::NonPixelShaderResource;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE ) )
		{
			resourceState |= ResourceState::PixelShaderResource;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_STREAM_OUT ) )
		{
			resourceState |= ResourceState::StreamOut;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT ) )
		{
			resourceState |= ResourceState::IndirectArgument;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_COPY_DEST ) )
		{
			resourceState |= ResourceState::CopyDest;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_COPY_SOURCE ) )
		{
			resourceState |= ResourceState::CopySource;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_RESOLVE_DEST ) )
		{
			resourceState |= ResourceState::ResolveDest;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_RESOLVE_SOURCE ) )
		{
			resourceState |= ResourceState::ResolveSource;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE ) )
		{
			resourceState |= ResourceState::RaytracingAccelerationStructure;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_SHADING_RATE_SOURCE ) )
		{
			resourceState |= ResourceState::ShadingRateSource;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_PRESENT ) )
		{
			resourceState |= ResourceState::Present;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_PREDICATION ) )
		{
			resourceState |= ResourceState::Predication;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_VIDEO_DECODE_READ ) )
		{
			resourceState |= ResourceState::VideoDecodeRead;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_VIDEO_DECODE_WRITE ) )
		{
			resourceState |= ResourceState::VideoDecodeWrite;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ ) )
		{
			resourceState |= ResourceState::VideoProcessRead;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE ) )
		{
			resourceState |= ResourceState::VideoProecssWrite;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_VIDEO_ENCODE_READ ) )
		{
			resourceState |= ResourceState::VideoEncodeRead;
		}

		if ( HasAnyFlags( state, D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE ) )
		{
			resourceState |= ResourceState::VideoEncodeWrite;
		}

		return resourceState;
	}

	D3D12_RESOURCE_BARRIER ConvertToResourceBarrier( const ResourceTransition& transition )
	{
		D3D12_RESOURCE_BARRIER barrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
			.Transition =
			{
				.pResource = static_cast<ID3D12Resource*>( transition.m_pResource ),
				.Subresource = transition.m_subResource,
				.StateBefore = ConvertToResourceStates( transition.m_pTransitionable->GetResourceState() ),
				.StateAfter = ConvertToResourceStates( transition.m_state )
			}
		};

		if ( barrier.Transition.StateBefore == barrier.Transition.StateAfter )
		{
			barrier.Transition.StateBefore = ConvertToResourceStates( transition.m_pTransitionable->GetResourceState() );
			barrier.Transition.StateAfter = ConvertToResourceStates( transition.m_state );
		}

		return barrier;
	}
}