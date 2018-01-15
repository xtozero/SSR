#include "stdafx.h"
#include "D3D11DepthStencil.h"

#include "D3D11Resource.h"

#include <d3d11.h>

inline D3D11_DEPTH_STENCIL_VIEW_DESC ConvertTextureTraitToDSV( const TEXTURE_TRAIT& trait )
{
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};

	dsv.Format = ConvertFormatToDxgiFormat( trait.m_format );

	if ( IsTexture1D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
			dsv.Texture1DArray.ArraySize = trait.m_depth;
		}
		else
		{
			dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
		}
	}
	else if ( IsTexture2D( trait ) )
	{
		if ( trait.m_depth > 1 )
		{
			if ( trait.m_sampleCount != 0 )
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
				dsv.Texture2DMSArray.ArraySize = trait.m_depth;
			}
			else
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				dsv.Texture2DArray.ArraySize = trait.m_depth;
			}
		}
		else
		{
			if ( trait.m_sampleCount != 0 )
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
		}
	}
	else
	{
		assert( false );
	}

	return dsv;
}

void* CDepthStencil::Get( ) const
{
	return m_pDepthStencilVeiw.Get( );
}

bool CDepthStencil::CreateDepthStencil( ID3D11Device& device, const ITexture& texture, const TEXTURE_TRAIT* traitOrNull )
{
	SetAppSizeDependency( texture.IsAppSizeDependency( ) );

	D3D11_DEPTH_STENCIL_VIEW_DESC* pDsvDesc = nullptr;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	if ( traitOrNull )
	{
		dsvDesc = ConvertTextureTraitToDSV( *traitOrNull );
		pDsvDesc = &dsvDesc;
	}

	ID3D11Resource* pResource = static_cast<ID3D11Resource*>( texture.Get( ) );

	if ( pResource == nullptr )
	{
		return false;
	}

	return SUCCEEDED( device.CreateDepthStencilView( pResource, pDsvDesc, m_pDepthStencilVeiw.GetAddressOf( ) ) );
}

void CDepthStencil::SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView )
{
	m_pDepthStencilVeiw = depthStencilView;
}

void CDepthStencil::Clear( ID3D11DeviceContext& deviceContext, unsigned int clearFlag, float depth, unsigned char stencil )
{
	if ( m_pDepthStencilVeiw )
	{
		deviceContext.ClearDepthStencilView( m_pDepthStencilVeiw.Get( ), clearFlag, depth, stencil );
	}
}
