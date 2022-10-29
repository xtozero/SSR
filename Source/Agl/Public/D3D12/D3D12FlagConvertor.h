#pragma once

#include "DxgiFlagConvertor.h"
#include "GraphicsApiResource.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
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
}
