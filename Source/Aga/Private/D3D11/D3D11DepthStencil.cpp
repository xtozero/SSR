//#include "stdafx.h"
//#include "D3D11DepthStencil.h"
//
//#include "D3D11Api.h"
//#include "D3D11FlagConvertor.h"
//#include "D3D11Textures.h"
//
//namespace
//{
//	D3D11_DEPTH_STENCIL_VIEW_DESC ConvertTraitToDSV( const TEXTURE_TRAIT& trait )
//	{
//		D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
//
//		dsv.Format = ConvertFormatToDxgiFormat( trait.m_format );
//
//		if ( IsTexture1D( trait ) )
//		{
//			if ( trait.m_depth > 1 )
//			{
//				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
//				dsv.Texture1DArray.ArraySize = trait.m_depth;
//			}
//			else
//			{
//				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
//			}
//		}
//		else if ( IsTexture2D( trait ) )
//		{
//			if ( trait.m_depth > 1 )
//			{
//				if ( trait.m_sampleCount > 1 )
//				{
//					dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
//					dsv.Texture2DMSArray.ArraySize = trait.m_depth;
//				}
//				else
//				{
//					dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
//					dsv.Texture2DArray.ArraySize = trait.m_depth;
//				}
//			}
//			else
//			{
//				if ( trait.m_sampleCount > 1 )
//				{
//					dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
//				}
//				else
//				{
//					dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//				}
//			}
//		}
//		else
//		{
//			__debugbreak( );
//		}
//
//		return dsv;
//	}
//
//	D3D11_DEPTH_STENCIL_VIEW_DESC ConvertTraitToDSV( const D3D11_TEXTURE2D_DESC& desc )
//	{
//		D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
//
//		dsv.Format = desc.Format;
//
//		if ( desc.ArraySize > 1 )
//		{
//			if ( desc.SampleDesc.Count > 1 )
//			{
//				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
//				dsv.Texture2DMSArray.ArraySize = desc.ArraySize;
//			}
//			else
//			{
//				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
//				dsv.Texture2DArray.ArraySize = desc.ArraySize;
//			}
//		}
//		else
//		{
//			if ( desc.SampleDesc.Count > 1 )
//			{
//				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
//			}
//			else
//			{
//				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//			}
//		}
//
//		return dsv;
//	}
//
//}
//
//void CD3D11DepthStencil::InitResource( )
//{
//	ID3D11Resource* pResource = m_texture->Get( );
//	assert( pResource );
//
//	bool result = SUCCEEDED( D3D11Device( ).CreateDepthStencilView( pResource, &m_desc, m_pResource.GetAddressOf( ) ) );
//	assert( result );
//}
//
//void CD3D11DepthStencil::Clear( unsigned int clearFlag, float depth, unsigned char stencil )
//{
//	if ( m_pResource )
//	{
//		D3D11Context( ).ClearDepthStencilView( m_pResource.Get( ), clearFlag, depth, stencil );
//	}
//}
//
//CD3D11DepthStencil::CD3D11DepthStencil( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_texture( texture )
//{
//	SetAppSizeDependency( m_texture->IsAppSizeDependency( ) );
//
//	if ( traitOrNull )
//	{
//		m_desc = ConvertTraitToDSV( *traitOrNull );
//	}
//	else
//	{
//		m_desc = ConvertTraitToDSV( m_texture->GetDesc( ) );
//	}
//}