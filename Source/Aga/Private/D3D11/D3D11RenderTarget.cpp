#include "stdafx.h"
#include "D3D11RenderTarget.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"
#include "D3D11Textures.h"

namespace
{
	D3D11_RENDER_TARGET_VIEW_DESC ConvertTraitToRTV( const TEXTURE_TRAIT& trait )
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtv = {};

		rtv.Format = ConvertFormatToDxgiFormat( trait.m_format );

		if ( IsTexture1D( trait ) )
		{
			if ( trait.m_depth > 1 )
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
				rtv.Texture1DArray.ArraySize = trait.m_depth;
			}
			else
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			}
		}
		else if ( IsTexture2D( trait ) )
		{
			if ( trait.m_depth > 1 )
			{
				if ( trait.m_sampleCount > 1 )
				{
					rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
					rtv.Texture2DMSArray.ArraySize = trait.m_depth;
				}
				else
				{
					rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					rtv.Texture2DArray.ArraySize = trait.m_depth;
				}
			}
			else
			{
				if ( trait.m_sampleCount > 1 )
				{
					rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				}
			}
		}
		else if ( IsTexture3D( trait ) )
		{
			rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			rtv.Texture3D.WSize = trait.m_depth;
		}
		else
		{
			__debugbreak( );
		}

		return rtv;
	}

	D3D11_RENDER_TARGET_VIEW_DESC ConvertDescToRTV( const D3D11_TEXTURE2D_DESC& desc )
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtv = {};

		rtv.Format = desc.Format;

		if ( desc.ArraySize > 1 )
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
				rtv.Texture2DMSArray.ArraySize = desc.ArraySize;
			}
			else
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv.Texture2DArray.ArraySize = desc.ArraySize;
			}
		}
		else
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			}
		}

		return rtv;
	}

}

void CD3D11RenderTarget::InitResource( )
{
	ID3D11Resource* pResource = m_texture->Get( );
	assert( pResource );

	bool result = SUCCEEDED( D3D11Device( ).CreateRenderTargetView( pResource, &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11RenderTarget::CD3D11RenderTarget( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_texture( texture )
{
	SetAppSizeDependency( m_texture->IsAppSizeDependency( ) );

	if ( traitOrNull )
	{
		m_desc = ConvertTraitToRTV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToRTV( texture->GetDesc( ) );
	}
}
