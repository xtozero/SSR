#include "stdafx.h"
#include "D3D11Textures.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

namespace
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
}

void CD3D11Texture1D::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateTexture1D( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Texture1D::CD3D11Texture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitTo1DDesc( trait );
}

void CD3D11Texture2D::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateTexture2D( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Texture2D::CD3D11Texture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	SetAppSizeDependency( ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT ) > 0 );

	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitTo2DDesc( trait );
}

CD3D11Texture2D::CD3D11Texture2D( const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture2D, bool appSizeDepentant )
{
	assert( texture2D.Get( ) != nullptr );
	SetAppSizeDependency( appSizeDepentant );

	texture2D->GetDesc( &m_desc );

	m_pResource = texture2D;
}

void CD3D11Texture3D::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateTexture3D( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Texture3D::CD3D11Texture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitTo3DDesc( trait );
}
