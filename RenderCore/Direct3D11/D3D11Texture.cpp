#include "stdafx.h"
#include "D3D11Texture.h"

#include <cassert>
#include <D3D11.h>
#include <D3DX11.h>
#include <wrl/client.h>

#include "../Direct3D11/D3D11Resource.h"

inline D3D11_TEXTURE1D_DESC ConvertTraitTo1DDesc( const TEXTURE_TRAIT& trait )
{
	DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_type );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

	return D3D11_TEXTURE1D_DESC{ trait.m_width,
								trait.m_mipLevels,
								trait.m_depth,
								format,
								usage,
								bindFlag,
								cpuAccessFlag,
								miscFlags };
}


inline D3D11_TEXTURE2D_DESC ConvertTraitTo2DDesc( const TEXTURE_TRAIT& trait )
{
	DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
	DXGI_SAMPLE_DESC SampleDesc = { trait.m_sampleCount, trait.m_sampleQuality };
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_type );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

	return D3D11_TEXTURE2D_DESC{ trait.m_width,
								trait.m_height,
								trait.m_mipLevels,
								trait.m_depth,
								format,
								SampleDesc,
								usage,
								bindFlag,
								cpuAccessFlag,
								miscFlags };
}

inline D3D11_TEXTURE3D_DESC ConvertTraitTo3DDesc( const TEXTURE_TRAIT& trait )
{
	DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_type );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

	return D3D11_TEXTURE3D_DESC{ trait.m_width,
								trait.m_height,
								trait.m_depth,
								trait.m_mipLevels,
								format,
								usage,
								bindFlag,
								cpuAccessFlag,
								miscFlags };
}

inline TEXTURE_TRAIT Convert1DDescToTrait( const D3D11_TEXTURE1D_DESC& desc )
{
	UINT format = ConvertDxgiFormatToFormat( desc.Format );
	UINT type = ConvertBindToType( desc.BindFlags );
	UINT access = ConvertUsageToAccessFlag( desc.Usage );
	UINT misc = ConvertDXMiscToMisc( desc.MiscFlags );

	return TEXTURE_TRAIT{ desc.Width,
						1,
						desc.ArraySize,
						0,
						0,
						desc.MipLevels,
						format,
						access,
						type,
						misc };
}


inline TEXTURE_TRAIT Convert2DDescToTrait( const D3D11_TEXTURE2D_DESC& desc )
{
	UINT format = ConvertDxgiFormatToFormat( desc.Format );
	UINT type = ConvertBindToType( desc.BindFlags );
	UINT access = ConvertUsageToAccessFlag( desc.Usage );
	UINT misc = ConvertDXMiscToMisc( desc.MiscFlags );

	return TEXTURE_TRAIT{ desc.Width,
						desc.Height,
						desc.ArraySize,
						desc.SampleDesc.Count,
						desc.SampleDesc.Quality,
						desc.MipLevels,
						format,
						access,
						type,
						misc };
}

inline TEXTURE_TRAIT Convert3DDescToTrait( const D3D11_TEXTURE3D_DESC& desc )
{
	UINT format = ConvertDxgiFormatToFormat( desc.Format );
	UINT type = ConvertBindToType( desc.BindFlags );
	UINT access = ConvertUsageToAccessFlag( desc.Usage );
	UINT misc = ConvertDXMiscToMisc( desc.MiscFlags );

	return TEXTURE_TRAIT{ desc.Width,
						desc.Height,
						desc.Depth,
						0,
						0,
						desc.MipLevels,
						format,
						access,
						type,
						misc };
}

void* CD3D11Texture1D::Get( ) const
{
	return m_pTexture.Get( );
}

bool CD3D11Texture1D::SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture )
{
	HRESULT hr = pTexture.Get( )->QueryInterface( IID_ID3D11Texture1D, reinterpret_cast<void**>( m_pTexture.GetAddressOf( ) ) );

	if ( SUCCEEDED( hr ) )
	{
		D3D11_TEXTURE1D_DESC desc;
		m_pTexture->GetDesc( &desc );
		m_trait = Convert1DDescToTrait( desc );
		return true;
	}

	return false;
}

bool CD3D11Texture1D::Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	m_trait = trait;
	D3D11_TEXTURE1D_DESC desc = ConvertTraitTo1DDesc( trait );

	D3D11_SUBRESOURCE_DATA* pSrd = nullptr;
	D3D11_SUBRESOURCE_DATA srd = {};

	if ( initData )
	{
		srd.pSysMem = initData->m_srcData;
		srd.SysMemPitch = initData->m_pitch;
		srd.SysMemSlicePitch = initData->m_slicePitch;
		pSrd = &srd;
	}

	return SUCCEEDED( device.CreateTexture1D( &desc, pSrd, m_pTexture.GetAddressOf( ) ) );
}

bool CD3D11Texture1D::LoadFromFile( ID3D11Device& device, const TCHAR * filePath )
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	D3DX11CreateTextureFromFile( &device, filePath, nullptr, nullptr, resource.GetAddressOf( ), &hr );

	if ( SUCCEEDED( hr ) == false )
	{
		return false;
	}

	return SetTexture( resource );
}

void* CD3D11Texture2D::Get( ) const
{
	return m_pTexture.Get( );
}

bool CD3D11Texture2D::SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture )
{
	HRESULT hr = pTexture.Get( )->QueryInterface( IID_ID3D11Texture2D, reinterpret_cast<void**>( m_pTexture.GetAddressOf( ) ) );

	if ( SUCCEEDED( hr ) )
	{
		D3D11_TEXTURE2D_DESC desc;
		m_pTexture->GetDesc( &desc );
		m_trait = Convert2DDescToTrait( desc );
		return true;
	}

	return false;
}

bool CD3D11Texture2D::Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	m_trait = trait;
	D3D11_TEXTURE2D_DESC desc = ConvertTraitTo2DDesc( trait );

	D3D11_SUBRESOURCE_DATA* pSrd = nullptr;
	D3D11_SUBRESOURCE_DATA srd = {};

	if ( initData )
	{
		srd.pSysMem = initData->m_srcData;
		srd.SysMemPitch = initData->m_pitch;
		srd.SysMemSlicePitch = initData->m_slicePitch;
		pSrd = &srd;
	}

	return SUCCEEDED( device.CreateTexture2D( &desc, pSrd, m_pTexture.GetAddressOf() ) );
}

bool CD3D11Texture2D::LoadFromFile( ID3D11Device& device, const TCHAR* filePath )
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	D3DX11CreateTextureFromFile( &device, filePath, nullptr, nullptr, resource.GetAddressOf(), &hr );

	if ( SUCCEEDED( hr ) == false )
	{
		return false;
	}

	return SetTexture( resource );
}

void* CD3D11Texture3D::Get( ) const
{
	return m_pTexture.Get( );
}

bool CD3D11Texture3D::SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture )
{
	HRESULT hr = pTexture.Get( )->QueryInterface( IID_ID3D11Texture3D, reinterpret_cast<void**>( m_pTexture.GetAddressOf( ) ) );

	if ( SUCCEEDED( hr ) )
	{
		D3D11_TEXTURE3D_DESC desc;
		m_pTexture->GetDesc( &desc );
		m_trait = Convert3DDescToTrait( desc );
		return true;
	}

	return false;
}

bool CD3D11Texture3D::Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	m_trait = trait;
	D3D11_TEXTURE3D_DESC desc = ConvertTraitTo3DDesc( trait );

	D3D11_SUBRESOURCE_DATA* pSrd = nullptr;
	D3D11_SUBRESOURCE_DATA srd = {};

	if ( initData )
	{
		srd.pSysMem = initData->m_srcData;
		srd.SysMemPitch = initData->m_pitch;
		srd.SysMemSlicePitch = initData->m_slicePitch;
		pSrd = &srd;
	}

	return SUCCEEDED( device.CreateTexture3D( &desc, pSrd, m_pTexture.GetAddressOf( ) ) );
}

bool CD3D11Texture3D::LoadFromFile( ID3D11Device& device, const TCHAR * filePath )
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	D3DX11CreateTextureFromFile( &device, filePath, nullptr, nullptr, resource.GetAddressOf( ), &hr );

	if ( SUCCEEDED( hr ) == false )
	{
		return false;
	}

	return SetTexture( resource );
}
