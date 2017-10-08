#include "stdafx.h"
#include "D3D11ShaderResource.h"

#include "../common.h"

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
		assert( false );
	}

	return srv;
}

void* CD3D11ShaderResource::Get( ) const
{
	return m_pShaderResourceView.Get( );
}

void CD3D11ShaderResource::SetShaderResourceView( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& shaderResourceView )
{
	m_pShaderResourceView = shaderResourceView;
}

bool CD3D11ShaderResource::LoadShaderResourceFromFile( ID3D11Device* pDevice, const String& fileName )
{
	HRESULT hr;
	D3DX11CreateShaderResourceViewFromFile( pDevice, fileName.c_str( ), nullptr, nullptr, &m_pShaderResourceView, &hr );

	if ( SUCCEEDED( hr ) )
	{
		return true;
	}

	return false;
}

bool CD3D11ShaderResource::CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const TEXTURE_TRAIT* traitOrNull )
{
	const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( traitOrNull )
	{
		srv = ConvertTextureTraitToSRV( *traitOrNull );
		pDesc = &srv;
	}

	if ( SUCCEEDED( pDevice->CreateShaderResourceView( static_cast<ID3D11Resource*>(pTexture->Get( )), pDesc, &m_pShaderResourceView ) ) )
	{
		return true;
	}
	return false;
}
