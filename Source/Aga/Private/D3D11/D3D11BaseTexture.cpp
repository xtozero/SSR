#include "stdafx.h"
#include "D3D11BaseTexture.h"

#include "D3D11FlagConvertor.h"

namespace aga
{
	D3D11_TEXTURE1D_DESC ConvertTraitTo1DDesc( const TEXTURE_TRAIT& trait )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
		UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

		return D3D11_TEXTURE1D_DESC{
			trait.m_width,
			trait.m_mipLevels,
			trait.m_depth,
			format,
			usage,
			bindFlag,
			cpuAccessFlag,
			miscFlags
		};
	}

	D3D11_TEXTURE2D_DESC ConvertTraitTo2DDesc( const TEXTURE_TRAIT& trait )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
		DXGI_SAMPLE_DESC SampleDesc = { trait.m_sampleCount, trait.m_sampleQuality };
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
		UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

		return D3D11_TEXTURE2D_DESC{
			trait.m_width,
			trait.m_height,
			trait.m_mipLevels,
			trait.m_depth,
			format,
			SampleDesc,
			usage,
			bindFlag,
			cpuAccessFlag,
			miscFlags
		};
	}

	D3D11_TEXTURE3D_DESC ConvertTraitTo3DDesc( const TEXTURE_TRAIT& trait )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
		UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

		return D3D11_TEXTURE3D_DESC{
			trait.m_width,
			trait.m_height,
			trait.m_depth,
			trait.m_mipLevels,
			format,
			usage,
			bindFlag,
			cpuAccessFlag,
			miscFlags
		};
	}

	TEXTURE_TRAIT ConvertDescToTrait( const D3D11_TEXTURE2D_DESC& desc )
	{
		RESOURCE_FORMAT format = ConvertDxgiFormatToFormat( desc.Format );
		UINT access = ConvertUsageToAccessFlag( desc.Usage );
		UINT bindType = ConvertBindToType( desc.BindFlags );
		UINT miscFlag = ConvertDXMiscToMisc( desc.MiscFlags );


		return TEXTURE_TRAIT{
			desc.Width,
			desc.Height,
			desc.ArraySize,
			desc.SampleDesc.Count,
			desc.SampleDesc.Quality,
			desc.MipLevels,
			format,
			access,
			bindType,
			miscFlag
		};
	}

	D3D11BaseTexture1D::D3D11BaseTexture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData ) 
		: D3D11Texture<ID3D11Texture1D>( trait, initData )
	{
	}

	void D3D11BaseTexture1D::CreateTexture( )
	{
		m_desc = ConvertTraitTo1DDesc( m_trait );

		HRESULT hr = D3D11Device( ).CreateTexture1D( &m_desc, ( m_initData.pSysMem ) ? &m_initData : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );
	}

	D3D11BaseTexture2D::D3D11BaseTexture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData ) 
		: D3D11Texture<ID3D11Texture2D>( trait, initData )
	{
	}

	D3D11BaseTexture2D::D3D11BaseTexture2D( ID3D11Texture2D* texture )
	{
		if ( texture )
		{
			m_texture = texture;
			m_texture->GetDesc( &m_desc );
			m_trait = ConvertDescToTrait( m_desc );
		}
	}

	void D3D11BaseTexture2D::CreateTexture( )
	{
		m_desc = ConvertTraitTo2DDesc( m_trait );

		HRESULT hr = D3D11Device( ).CreateTexture2D( &m_desc, ( m_initData.pSysMem ) ? &m_initData : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );
	}

	D3D11BaseTexture3D::D3D11BaseTexture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D11Texture<ID3D11Texture3D>( trait, initData )
	{
	}

	void D3D11BaseTexture3D::CreateTexture( )
	{
		m_desc = ConvertTraitTo3DDesc( m_trait );

		HRESULT hr = D3D11Device( ).CreateTexture3D( &m_desc, ( m_initData.pSysMem ) ? &m_initData : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );
	}

	bool IsTexture1D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_height <= 1 ) && ( ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT ) == 0 );
	}

	bool IsTexture2D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_height > 1 ) && ( ( trait.m_miscFlag & RESOURCE_MISC::TEXTURE3D ) == 0 );
	}

	bool IsTexture3D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_miscFlag & RESOURCE_MISC::TEXTURE3D ) > 0;
	}
}