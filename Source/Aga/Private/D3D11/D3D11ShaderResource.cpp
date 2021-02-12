//#include "stdafx.h"
//#include "D3D11ShaderResource.h"
//
//#include "D3D11Api.h"
//#include "D3D11Buffer.h"
//#include "D3D11FlagConvertor.h"
//#include "D3D11Textures.h"
//
//namespace
//{
//	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertTraitToSRV( const TEXTURE_TRAIT& trait )
//	{
//		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
//
//		srv.Format = ConvertFormatToDxgiFormat( trait.m_format );
//
//		if ( IsTexture1D( trait ) )
//		{
//			if ( trait.m_depth > 1 )
//			{
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
//				srv.Texture1DArray.ArraySize = trait.m_depth;
//				srv.Texture1DArray.MipLevels = trait.m_mipLevels;
//			}
//			else
//			{
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
//				srv.Texture1D.MipLevels = trait.m_mipLevels;
//			}
//		}
//		else if ( IsTexture2D( trait ) )
//		{
//			if ( trait.m_depth % 6 == 0 )
//			{
//				if ( trait.m_depth == 6 )
//				{
//					srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
//					srv.TextureCube.MipLevels = trait.m_mipLevels;
//				}
//				else
//				{
//					srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
//					srv.TextureCubeArray.MipLevels = trait.m_mipLevels;
//					srv.TextureCubeArray.NumCubes = trait.m_depth / 6;
//				}
//			}
//			else if ( trait.m_depth > 1 )
//			{
//				if ( trait.m_sampleCount > 1 )
//				{
//					srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
//					srv.Texture2DMSArray.ArraySize = trait.m_depth;
//				}
//				else
//				{
//
//					srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
//					srv.Texture2DArray.MipLevels = trait.m_mipLevels;
//					srv.Texture2DArray.ArraySize = trait.m_depth;
//				}
//			}
//			else
//			{
//				if ( trait.m_sampleCount > 1 )
//				{
//					srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
//				}
//				else
//				{
//					srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
//					srv.Texture2D.MipLevels = trait.m_mipLevels;
//				}
//			}
//		}
//		else if ( IsTexture3D( trait ) )
//		{
//			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
//			srv.Texture3D.MipLevels = trait.m_mipLevels;
//		}
//		else
//		{
//			__debugbreak( );
//		}
//
//		return srv;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertTraitToSRV( const BUFFER_TRAIT& trait )
//	{
//		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
//
//		if ( trait.m_miscFlag == RESOURCE_MISC::BUFFER_ALLOW_RAW_VIEWS )
//		{
//			if ( ( ( trait.m_stride * trait.m_count ) % 4 ) != 0 )
//			{
//				__debugbreak( );
//			}
//
//			srv.Format = DXGI_FORMAT_R32_TYPELESS;
//			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
//			srv.BufferEx.NumElements = static_cast<UINT>( ( trait.m_stride * trait.m_count ) / 4 );
//			srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
//		}
//		else
//		{
//			srv.Format = DXGI_FORMAT_UNKNOWN;
//			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
//			srv.Buffer.NumElements = trait.m_count;
//			srv.Buffer.ElementWidth = trait.m_stride;
//		}
//
//		return srv;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE1D_DESC& desc )
//	{
//		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
//
//		srv.Format = desc.Format;
//
//		if ( desc.ArraySize > 1 )
//		{
//			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
//			srv.Texture1DArray.ArraySize = desc.ArraySize;
//			srv.Texture1DArray.MipLevels = desc.MipLevels;
//		}
//		else
//		{
//			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
//			srv.Texture1D.MipLevels = desc.MipLevels;
//		}
//
//		return srv;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE2D_DESC& desc )
//	{
//		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
//
//		srv.Format = desc.Format;
//
//		if ( desc.ArraySize % 6 == 0 )
//		{
//			if ( desc.ArraySize == 6 )
//			{
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
//				srv.TextureCube.MipLevels = desc.MipLevels;
//			}
//			else
//			{
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
//				srv.TextureCubeArray.MipLevels = desc.MipLevels;
//				srv.TextureCubeArray.NumCubes = desc.ArraySize / 6;
//			}
//		}
//		else if ( desc.ArraySize > 1 )
//		{
//			if ( desc.SampleDesc.Count > 1 )
//			{
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
//				srv.Texture2DMSArray.ArraySize = desc.ArraySize;
//			}
//			else
//			{
//
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
//				srv.Texture2DArray.MipLevels = desc.MipLevels;
//				srv.Texture2DArray.ArraySize = desc.ArraySize;
//			}
//		}
//		else
//		{
//			if ( desc.SampleDesc.Count > 1 )
//			{
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
//			}
//			else
//			{
//				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
//				srv.Texture2D.MipLevels = desc.MipLevels;
//			}
//		}
//
//		return srv;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE3D_DESC& desc )
//	{
//		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
//
//		srv.Format = desc.Format;
//		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
//		srv.Texture3D.MipLevels = desc.MipLevels;
//
//		return srv;
//	}
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_BUFFER_DESC& desc )
//	{
//		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
//
//		if ( desc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
//		{
//			if ( ( desc.ByteWidth % 4 ) != 0 )
//			{
//				__debugbreak( );
//			}
//
//			srv.Format = DXGI_FORMAT_R32_TYPELESS;
//			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
//			srv.BufferEx.NumElements = desc.ByteWidth / 4;
//			srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
//		}
//		else
//		{
//			srv.Format = DXGI_FORMAT_UNKNOWN;
//			srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
//			srv.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
//			srv.Buffer.ElementWidth = desc.StructureByteStride;
//		}
//
//		return srv;
//	}
//}
//
//void CD3D11ShaderResource::InitResource( )
//{
//	bool result = SUCCEEDED( D3D11Device( ).CreateShaderResourceView( m_resource->Get( ), &m_desc, m_pResource.GetAddressOf( ) ) );
//	assert( result );
//}
//
//CD3D11ShaderResource::CD3D11ShaderResource( const RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_resource( texture.Get( ) )
//{
//	if ( traitOrNull )
//	{
//		m_desc = ConvertTraitToSRV( *traitOrNull );
//	}
//	else
//	{
//		m_desc = ConvertDescToSRV( texture->GetDesc( ) );
//	}
//}
//
//CD3D11ShaderResource::CD3D11ShaderResource( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_resource( texture.Get( ) )
//{
//	SetAppSizeDependency( texture->IsAppSizeDependency( ) );
//
//	if ( traitOrNull )
//	{
//		m_desc = ConvertTraitToSRV( *traitOrNull );
//	}
//	else
//	{
//		m_desc = ConvertDescToSRV( texture->GetDesc( ) );
//	}
//}
//
//CD3D11ShaderResource::CD3D11ShaderResource( const RefHandle<CD3D11Texture3D>& texture, const TEXTURE_TRAIT* traitOrNull ) : m_resource( texture.Get( ) )
//{
//	if ( traitOrNull )
//	{
//		m_desc = ConvertTraitToSRV( *traitOrNull );
//	}
//	else
//	{
//		m_desc = ConvertDescToSRV( texture->GetDesc( ) );
//	}
//}
//
//CD3D11ShaderResource::CD3D11ShaderResource( const RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT* traitOrNull ) : m_resource( buffer.Get( ) )
//{
//	if ( traitOrNull )
//	{
//		m_desc = ConvertTraitToSRV( *traitOrNull );
//	}
//	else
//	{
//		m_desc = ConvertDescToSRV( buffer->GetDesc( ) );
//	}
//}