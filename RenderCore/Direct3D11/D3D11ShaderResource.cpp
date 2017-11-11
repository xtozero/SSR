#include "stdafx.h"
#include "D3D11ShaderResource.h"

#include "D3D11Buffer.h"
#include "D3D11Resource.h"

#include <D3DX11.h>

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertTextureTraitToSRV( const TEXTURE_TRAIT& trait )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	srv.Format = ConvertFormatToDxgiFormat( trait.m_format );

	if ( IsTexture1D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
			srv.Texture1DArray.ArraySize = trait.m_depth;
			srv.Texture1DArray.MipLevels = trait.m_mipLevels;
		}
		else
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
			srv.Texture1D.MipLevels = trait.m_mipLevels;
		}
	}
	else if ( IsTexture2D( trait ) )
	{
		if ( trait.m_depth % 6 == 0 )
		{
			if ( trait.m_depth == 6 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
				srv.TextureCube.MipLevels = trait.m_mipLevels;
			}
			else
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
				srv.TextureCubeArray.MipLevels = trait.m_mipLevels;
				srv.TextureCubeArray.NumCubes = trait.m_depth / 6;
			}
		}
		else if ( trait.m_depth > 1 )
		{
			if ( trait.m_sampleQuality != 0 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
				srv.Texture2DMSArray.ArraySize = trait.m_depth;
			}
			else
			{
				
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
				srv.Texture2DArray.MipLevels = trait.m_mipLevels;
				srv.Texture2DArray.ArraySize = trait.m_depth;
			}
		}
		else
		{
			if ( trait.m_sampleQuality != 0 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
				srv.Texture2D.MipLevels = trait.m_mipLevels;
			}
		}
	}
	else if ( IsTexture3D( trait ) )
	{
		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		srv.Texture3D.MipLevels = trait.m_mipLevels;
	}
	else
	{
		__debugbreak( );
	}

	return srv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertBufferTraitToSRV( const BUFFER_TRAIT& trait )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( trait.m_miscFlag == RESOURCE_MISC::BUFFER_ALLOW_RAW_VIEWS )
	{
		if ( ( ( trait.m_stride * trait.m_count ) % 4 ) != 0 )
		{
			__debugbreak( );
		}

		srv.Format = DXGI_FORMAT_R32_TYPELESS;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srv.BufferEx.NumElements = static_cast<UINT>( ( trait.m_stride * trait.m_count ) / 4 );
		srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	}
	else
	{
		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.NumElements = trait.m_count;
		srv.Buffer.ElementWidth = trait.m_stride;
	}

	return srv;
}

bool CD3D11ShaderResource::LoadShaderResourceFromFile( ID3D11Device& device, const String& fileName )
{
	HRESULT hr;
	D3DX11CreateShaderResourceViewFromFile( &device, fileName.c_str( ), nullptr, nullptr, &m_pShaderResourceView, &hr );

	if ( SUCCEEDED( hr ) )
	{
		return true;
	}

	return false;
}

bool CD3D11ShaderResource::CreateShaderResource( ID3D11Device& device, const ITexture& texture, const TEXTURE_TRAIT* traitOrNull )
{
	const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( traitOrNull )
	{
		srv = ConvertTextureTraitToSRV( *traitOrNull );
		pDesc = &srv;
	}

	ID3D11Resource* pResource = static_cast<ID3D11Resource*>( texture.Get( ) );

	if ( SUCCEEDED( device.CreateShaderResourceView( pResource, pDesc, &m_pShaderResourceView ) ) )
	{
		return true;
	}
	return false;
}

bool CD3D11ShaderResource::CreateShaderResource( ID3D11Device& device, const IBuffer& buffer, const BUFFER_TRAIT* traitOrNull )
{
	const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( traitOrNull )
	{
		srv = ConvertBufferTraitToSRV( *traitOrNull );
		pDesc = &srv;
	}

	ID3D11Resource* pResource = static_cast<ID3D11Resource*>( buffer.Get( ) );

	if ( SUCCEEDED( device.CreateShaderResourceView( pResource, pDesc, &m_pShaderResourceView ) ) )
	{
		return true;
	}
	return false;
}
