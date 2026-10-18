#pragma once

#include "DxgiFlagConvertor.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
	class D3D12FlagConvertor
	{
	public:
		static D3D12_HEAP_TYPE ConvertToHeapType( ResourceAccess resourceAccess );

		static ResourceAccess ConvertToResourceAccess( D3D12_HEAP_TYPE heapType );

		static D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimToD3D12PrimType( ResourcePrimitive primitive );

		static D3D12_PRIMITIVE_TOPOLOGY ConvertPrimToD3D12Prim( ResourcePrimitive primitive );

		static D3D12_DEPTH_WRITE_MASK ConvertToDepthWriteMask( DepthWriteMode depthWriteMode );

		static D3D12_COMPARISON_FUNC ConvertToComparisionFunc( ComparisonFunc comparisonFunc );

		static D3D12_STENCIL_OP ConvertToStencilOp( StencilOp stencilOp );

		static D3D12_FILL_MODE ConvertToFillMode( FillMode fillMode );

		static D3D12_CULL_MODE ConvertToCullMode( CullMode cullMode );

		static D3D12_FILTER ConvertToFilter( TextureFilter filter );

		static D3D12_TEXTURE_ADDRESS_MODE ConvertToAddressMode( TextureAddressMode addressMode );

		static D3D12_BLEND ConvertToBlend( Blend blend );

		static D3D12_BLEND_OP ConvertToBlendOp( BlendOp blendOp );

		static ResourceBindType ConvertResourceFlagsToBindType( D3D12_RESOURCE_FLAGS flags );

		static D3D12_RESOURCE_STATES ConvertToResourceStates( ResourceState state );

		static ResourceState ConvertToResourceStates( D3D12_RESOURCE_STATES state );

		static D3D12_RESOURCE_BARRIER ConvertToResourceBarrier( const ResourceTransition& transition );

		static D3D12_INDIRECT_ARGUMENT_TYPE ConvertToIndirectArgumentType( IndirectCommandType commandType );
	};
}
