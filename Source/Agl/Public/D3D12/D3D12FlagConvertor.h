#pragma once

#include "DxgiFlagConvertor.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
	D3D12_HEAP_TYPE ConvertAccessFlagToHeapType( ResourceAccessFlag accessFlag );

	ResourceAccessFlag ConvertHeapTypeToAccessFlag( D3D12_HEAP_TYPE heapType );

	D3D12_PRIMITIVE_TOPOLOGY_TYPE ConvertPrimToD3D12PrimType( ResourcePrimitive primitive );

	D3D12_PRIMITIVE_TOPOLOGY ConvertPrimToD3D12Prim( ResourcePrimitive primitive );

	D3D12_DEPTH_WRITE_MASK ConvertToDepthWriteMask( DepthWriteMode depthWriteMode );

	D3D12_COMPARISON_FUNC ConvertToComparisionFunc( ComparisonFunc comparisonFunc );

	D3D12_STENCIL_OP ConvertToStencilOp( StencilOp stencilOp );

	D3D12_FILL_MODE ConvertToFillMode( FillMode fillMode );

	D3D12_CULL_MODE ConvertToCullMode( CullMode cullMode );

	D3D12_FILTER ConvertToFilter( TextureFilter filter );

	D3D12_TEXTURE_ADDRESS_MODE ConvertToTextureAddress( TextureAddressMode addressMode );

	D3D12_BLEND ConvertToBlend( Blend blend );

	D3D12_BLEND_OP ConvertToBlendOp( BlendOp blendOp );

	ResourceBindType ConvertResourceFlagsToBindType( D3D12_RESOURCE_FLAGS flags );

	D3D12_RESOURCE_STATES ConvertToResourceStates( ResourceState state );

	ResourceState ConvertToResourceStates( D3D12_RESOURCE_STATES state );

	D3D12_RESOURCE_BARRIER ConvertToResourceBarrier( const ResourceTransition& transition );
}
