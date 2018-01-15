#include "stdafx.h"
#include "D3D11RandomAccessResource.h"

#include "D3D11Buffer.h"
#include "D3D11Resource.h"

#include <d3d11.h>

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertTextureTraitToUAV( const TEXTURE_TRAIT& trait )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	uav.Format = ConvertFormatToDxgiFormat( trait.m_format );

	if ( IsTexture1D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
			uav.Texture1DArray.ArraySize = trait.m_depth;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
		}
	}
	else if ( IsTexture2D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uav.Texture2DArray.ArraySize = trait.m_depth;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		}
	}
	else if ( IsTexture3D( trait ) )
	{
		__debugbreak( );
	}
	else
	{
		__debugbreak( );
	}

	return uav;
}

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertBufferTraitToUAV( const BUFFER_TRAIT& trait )
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	if ( trait.m_miscFlag == RESOURCE_MISC::BUFFER_ALLOW_RAW_VIEWS )
	{
		if ( ( ( trait.m_stride * trait.m_count ) % 4 ) != 0 )
		{
			__debugbreak( );
		}

		uav.Format = DXGI_FORMAT_R32_TYPELESS;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.NumElements = static_cast<UINT>( ( trait.m_stride * trait.m_count ) / 4 );
		uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	}
	else
	{
		uav.Format = DXGI_FORMAT_UNKNOWN;
		uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uav.Buffer.NumElements = trait.m_count;
	}

	return uav;
}

bool CD3D11RandomAccessResource::CreateRandomAccessResource( ID3D11Device& device, const ITexture& texture, const TEXTURE_TRAIT* traitOrNull )
{
	SetAppSizeDependency( texture.IsAppSizeDependency( ) );

	const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc = nullptr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	if ( traitOrNull )
	{
		uav = ConvertTextureTraitToUAV( *traitOrNull );
		pDesc = &uav;
	}

	ID3D11Resource* pResource = static_cast<ID3D11Resource*>( texture.Get( ) );

	if ( SUCCEEDED( device.CreateUnorderedAccessView( pResource, pDesc, m_pUnorderedAccessView.GetAddressOf( ) ) ) )
	{
		return true;
	}

	return false;
}

bool CD3D11RandomAccessResource::CreateRandomAccessResource( ID3D11Device& device, const IBuffer& buffer, const BUFFER_TRAIT* traitOrNull )
{
	const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc = nullptr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	if ( traitOrNull )
	{
		uav = ConvertBufferTraitToUAV( *traitOrNull );
		pDesc = &uav;
	}

	ID3D11Resource* pResource = static_cast<ID3D11Resource*>( buffer.Get( ) );

	if ( SUCCEEDED( device.CreateUnorderedAccessView( pResource, pDesc, m_pUnorderedAccessView.GetAddressOf( ) ) ) )
	{
		return true;
	}

	return false;
}
