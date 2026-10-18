#pragma once

#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <d3d11.h>

namespace agl
{
	class D3D11FlagConvertor
	{
	public:
		static D3D11_USAGE ConvertToUsage( ResourceAccess resourceAccess );

		static ResourceAccess ConvertToResourceAccess( D3D11_USAGE usage );

		static uint32 ConvertTypeToBind( ResourceBindType type );

		static ResourceBindType ConvertBindToType( uint32 type );

		static uint32 ConvertMicsToDXMisc( ResourceMisc miscFlag );

		static ResourceMisc ConvertDXMiscToMisc( uint32 miscFlag );

		static uint32 ConvertToCpuFlag( ResourceAccess resourceAccess );

		static ResourcePrimitive Convert3D11PrimToPrim( D3D_PRIMITIVE_TOPOLOGY primitive );

		static D3D11_PRIMITIVE_TOPOLOGY ConvertPrimToD3D11Prim( ResourcePrimitive primitive );

		static D3D11_MAP ConvertToD3D11Map( ResourceLockFlag lockFlag );

		static D3D11_DEPTH_WRITE_MASK ConvertToDepthWriteMask( DepthWriteMode depthWriteMode );

		static D3D11_COMPARISON_FUNC ConvertToComparisionFunc( ComparisonFunc comparisonFunc );

		static D3D11_STENCIL_OP ConvertToStencilOp( StencilOp stencilOp );

		static D3D11_FILL_MODE ConvertToFillMode( FillMode fillMode );

		static D3D11_CULL_MODE ConvertToCullMode( CullMode cullMode );

		static D3D11_FILTER ConvertToFilter( TextureFilter filter );

		static D3D11_TEXTURE_ADDRESS_MODE ConvertToAddressMode( TextureAddressMode addressMode );

		static D3D11_BLEND ConvertToBlend( Blend blend );

		static D3D11_BLEND_OP ConvertToBlendOp( BlendOp blendOp );
	};
}
