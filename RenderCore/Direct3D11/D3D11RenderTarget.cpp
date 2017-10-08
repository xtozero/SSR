#include "stdafx.h"
#include "D3D11RenderTarget.h"

#include "D3D11Resource.h"

#include <d3d11.h>

inline D3D11_RENDER_TARGET_VIEW_DESC ConvertTextureTraitToRTV( const TEXTURE_TRAIT& trait )
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
			if ( trait.m_sampleCount != 0 )
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
			if ( trait.m_sampleCount != 0 )
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
		assert( false );
	}

	return rtv;
}

bool CRenderTarget::CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const TEXTURE_TRAIT* traitOrNull )
{
	D3D11_RENDER_TARGET_VIEW_DESC* pRtvDesc = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};

	if ( traitOrNull )
	{
		rtvDesc = ConvertTextureTraitToRTV( *traitOrNull );
		pRtvDesc = &rtvDesc;
	}

	return SUCCEEDED( pDevice->CreateRenderTargetView( pResource, pRtvDesc, &m_pRenderTargetView ) );
}

void* CRenderTarget::Get( ) const
{
	return m_pRenderTargetView.Get( );
}

void CRenderTarget::SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView )
{
	m_pRenderTargetView = renderTargetView;
}
