#include "stdafx.h"
#include "D3D11RandomAccessResource.h"

#include "D3D11Api.h"
#include "D3D11Buffer.h"
#include "D3D11FlagConvertor.h"
#include "D3D11Textures.h"

namespace
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertTraitToUAV( const TEXTURE_TRAIT& trait )
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

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertTraitToUAV( const BUFFER_TRAIT& trait )
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

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_TEXTURE1D_DESC& desc )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

		uav.Format = desc.Format;

		if ( desc.ArraySize > 1 )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
			uav.Texture1DArray.ArraySize = desc.ArraySize;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
		}

		return uav;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_TEXTURE2D_DESC& desc )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

		uav.Format = desc.Format;

		if ( desc.ArraySize > 1 )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uav.Texture2DArray.ArraySize = desc.ArraySize;
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		}

		return uav;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_BUFFER_DESC& desc )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

		if ( desc.MiscFlags == D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS )
		{
			if ( ( desc.ByteWidth % 4 ) != 0 )
			{
				__debugbreak( );
			}

			uav.Format = DXGI_FORMAT_R32_TYPELESS;
			uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav.Buffer.NumElements = desc.ByteWidth / 4;
			uav.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		}
		else
		{
			uav.Format = DXGI_FORMAT_UNKNOWN;
			uav.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uav.Buffer.NumElements = ( desc.StructureByteStride != 0 ) ? ( desc.ByteWidth / desc.StructureByteStride ) : desc.ByteWidth;
		}

		return uav;
	}
}

void CD3D11RandomAccessResource::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateUnorderedAccessView( m_resource->Get( ), &m_desc, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11RandomAccessResource::CD3D11RandomAccessResource( const RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT * traitOrNull ) : m_resource( texture.Get( ) )
{
	if ( traitOrNull )
	{
		m_desc = ConvertTraitToUAV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToUAV( texture->GetDesc( ) );
	}
}

CD3D11RandomAccessResource::CD3D11RandomAccessResource( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT * traitOrNull ) : m_resource( texture.Get( ) )
{
	SetAppSizeDependency( texture->IsAppSizeDependency( ) );

	if ( traitOrNull )
	{
		m_desc = ConvertTraitToUAV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToUAV( texture->GetDesc( ) );
	}
}

CD3D11RandomAccessResource::CD3D11RandomAccessResource( const RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT * traitOrNull ) : m_resource( buffer.Get( ) )
{
	if ( traitOrNull )
	{
		m_desc = ConvertTraitToUAV( *traitOrNull );
	}
	else
	{
		m_desc = ConvertDescToUAV( buffer->GetDesc( ) );
	}
}