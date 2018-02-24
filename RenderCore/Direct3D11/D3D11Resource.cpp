#include "stdafx.h"
#include "D3D11Resource.h"

#include <D3DX11.h>

inline D3D11_BUFFER_DESC ConvertTraitToDesc( const BUFFER_TRAIT& trait )
{
	UINT byteWidth = trait.m_count * trait.m_stride;
	D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
	UINT bindFlag = ConvertTypeToBind( trait.m_bufferType );
	UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
	UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );
	UINT structureByteStride = trait.m_stride;

	return D3D11_BUFFER_DESC{ byteWidth, usage, bindFlag, cpuAccessFlag, miscFlags, structureByteStride };
}

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
		__debugbreak( );
	}

	return rtv;
}

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
		__debugbreak( );
	}

	return dsv;
}

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
		__debugbreak( );
	}

	return srv;
}

inline D3D11_SHADER_RESOURCE_VIEW_DESC ConvertBufferTraitToSRV( const BUFFER_TRAIT& trait )
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( trait.m_miscFlag == RESOURCE_MISC::BUFFER_ALLOW_RAW_VIEWS )
	{
		if ( ( ( trait.m_stride * trait.m_count ) % 4 ) != 0 )
		{
			__debugbreak( );
		}

		srv.Format = DXGI_FORMAT_R32_TYPELESS;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srv.BufferEx.NumElements = static_cast<UINT>( ( trait.m_stride * trait.m_count ) / 4 );
		srv.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	}
	else
	{
		srv.Format = DXGI_FORMAT_UNKNOWN;
		srv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srv.Buffer.NumElements = trait.m_count;
		srv.Buffer.ElementWidth = trait.m_stride;
	}

	return srv;
}

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertTextureTraitToUAV( const TEXTURE_TRAIT& trait )
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

inline D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertBufferTraitToUAV( const BUFFER_TRAIT& trait )
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

bool CD3D11Buffer::Create( ID3D11Device& device, const BUFFER_TRAIT& trait )
{
	D3D11_BUFFER_DESC desc = ConvertTraitToDesc( trait );

	D3D11_SUBRESOURCE_DATA* pSrd = nullptr;
	D3D11_SUBRESOURCE_DATA srd = {};

	if ( trait.m_srcData )
	{
		pSrd = &srd;

		srd.pSysMem = trait.m_srcData;
		srd.SysMemPitch = trait.m_pitch;
		srd.SysMemSlicePitch = trait.m_slicePitch;
	}

	m_bufferSize = trait.m_count * trait.m_stride;
	HRESULT hr = device.CreateBuffer( &desc, pSrd, m_buffer.GetAddressOf( ) );

	return SUCCEEDED( hr );
}

ID3D11Resource* CD3D11Texture1D::Get( ) const
{
	return m_pTexture.Get( );
}

bool CD3D11Texture1D::SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture, bool isAppSizeDependent )
{
	HRESULT hr = pTexture.Get( )->QueryInterface( IID_ID3D11Texture1D, reinterpret_cast<void**>( m_pTexture.GetAddressOf( ) ) );

	if ( SUCCEEDED( hr ) )
	{
		D3D11_TEXTURE1D_DESC desc;
		m_pTexture->GetDesc( &desc );
		m_trait = Convert1DDescToTrait( desc );
		SetAppSizeDependency( isAppSizeDependent );
		return true;
	}

	return false;
}

bool CD3D11Texture1D::Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	m_trait = trait;
	SetAppSizeDependency( ( m_trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT ) > 0 );
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

bool CD3D11Texture1D::LoadFromFile( ID3D11Device& device, const TCHAR* filePath )
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

ID3D11Resource* CD3D11Texture2D::Get( ) const
{
	return m_pTexture.Get( );
}

bool CD3D11Texture2D::SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture, bool isAppSizeDependent )
{
	HRESULT hr = pTexture.Get( )->QueryInterface( IID_ID3D11Texture2D, reinterpret_cast<void**>( m_pTexture.GetAddressOf( ) ) );

	if ( SUCCEEDED( hr ) )
	{
		D3D11_TEXTURE2D_DESC desc;
		m_pTexture->GetDesc( &desc );
		m_trait = Convert2DDescToTrait( desc );
		m_trait.m_miscFlag |= isAppSizeDependent ? RESOURCE_MISC::APP_SIZE_DEPENDENT : RESOURCE_MISC::NONE;
		SetAppSizeDependency( isAppSizeDependent );
		return true;
	}

	return false;
}

bool CD3D11Texture2D::Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	m_trait = trait;
	SetAppSizeDependency( ( m_trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT ) > 0 );
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

	return SUCCEEDED( device.CreateTexture2D( &desc, pSrd, m_pTexture.GetAddressOf( ) ) );
}

bool CD3D11Texture2D::LoadFromFile( ID3D11Device& device, const TCHAR* filePath )
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

ID3D11Resource* CD3D11Texture3D::Get( ) const
{
	return m_pTexture.Get( );
}

bool CD3D11Texture3D::SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture, bool isAppSizeDependent )
{
	HRESULT hr = pTexture.Get( )->QueryInterface( IID_ID3D11Texture3D, reinterpret_cast<void**>( m_pTexture.GetAddressOf( ) ) );

	if ( SUCCEEDED( hr ) )
	{
		D3D11_TEXTURE3D_DESC desc;
		m_pTexture->GetDesc( &desc );
		m_trait = Convert3DDescToTrait( desc );
		SetAppSizeDependency( isAppSizeDependent );
		return true;
	}

	return false;
}

bool CD3D11Texture3D::Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
{
	m_trait = trait;
	SetAppSizeDependency( ( m_trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT ) > 0 );
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

bool CD3D11Texture3D::LoadFromFile( ID3D11Device& device, const TCHAR* filePath )
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

bool CRenderTarget::CreateRenderTarget( ID3D11Device& device, const CD3D11Texture& texture, const TEXTURE_TRAIT* traitOrNull )
{
	SetAppSizeDependency( texture.IsAppSizeDependency( ) );

	D3D11_RENDER_TARGET_VIEW_DESC* pRtvDesc = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};

	if ( traitOrNull )
	{
		rtvDesc = ConvertTextureTraitToRTV( *traitOrNull );
		pRtvDesc = &rtvDesc;
	}

	ID3D11Resource* pResource = texture.Get( );

	if ( pResource == nullptr )
	{
		return false;
	}

	return SUCCEEDED( device.CreateRenderTargetView( pResource, pRtvDesc, &m_pRenderTargetView ) );
}

ID3D11RenderTargetView* CRenderTarget::Get( ) const
{
	return m_pRenderTargetView.Get( );
}

void CRenderTarget::SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView )
{
	m_pRenderTargetView = renderTargetView;
}

ID3D11DepthStencilView* CDepthStencil::Get( ) const
{
	return m_pDepthStencilVeiw.Get( );
}

bool CDepthStencil::CreateDepthStencil( ID3D11Device& device, const CD3D11Texture& texture, const TEXTURE_TRAIT* traitOrNull )
{
	SetAppSizeDependency( texture.IsAppSizeDependency( ) );

	D3D11_DEPTH_STENCIL_VIEW_DESC* pDsvDesc = nullptr;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	if ( traitOrNull )
	{
		dsvDesc = ConvertTextureTraitToDSV( *traitOrNull );
		pDsvDesc = &dsvDesc;
	}

	ID3D11Resource* pResource = texture.Get( );

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

bool CD3D11ShaderResource::CreateShaderResourceFromFile( ID3D11Device& device, const String& fileName )
{
	HRESULT hr;
	D3DX11CreateShaderResourceViewFromFile( &device, fileName.c_str( ), nullptr, nullptr, &m_pShaderResourceView, &hr );

	if ( SUCCEEDED( hr ) )
	{
		return true;
	}

	return false;
}

bool CD3D11ShaderResource::CreateShaderResource( ID3D11Device& device, const CD3D11Texture& texture, const TEXTURE_TRAIT* traitOrNull )
{
	SetAppSizeDependency( texture.IsAppSizeDependency( ) );

	const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( traitOrNull )
	{
		srv = ConvertTextureTraitToSRV( *traitOrNull );
		pDesc = &srv;
	}

	ID3D11Resource* pResource = texture.Get( );

	if ( SUCCEEDED( device.CreateShaderResourceView( pResource, pDesc, &m_pShaderResourceView ) ) )
	{
		return true;
	}
	return false;
}

bool CD3D11ShaderResource::CreateShaderResource( ID3D11Device& device, const CD3D11Buffer& buffer, const BUFFER_TRAIT* traitOrNull )
{
	const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

	if ( traitOrNull )
	{
		srv = ConvertBufferTraitToSRV( *traitOrNull );
		pDesc = &srv;
	}

	if ( SUCCEEDED( device.CreateShaderResourceView( buffer.Get( ), pDesc, &m_pShaderResourceView ) ) )
	{
		return true;
	}
	return false;
}

bool CD3D11RandomAccessResource::CreateRandomAccessResource( ID3D11Device& device, const CD3D11Texture& texture, const TEXTURE_TRAIT* traitOrNull )
{
	SetAppSizeDependency( texture.IsAppSizeDependency( ) );

	const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc = nullptr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	if ( traitOrNull )
	{
		uav = ConvertTextureTraitToUAV( *traitOrNull );
		pDesc = &uav;
	}

	ID3D11Resource* pResource = texture.Get( );

	if ( SUCCEEDED( device.CreateUnorderedAccessView( pResource, pDesc, m_pUnorderedAccessView.GetAddressOf( ) ) ) )
	{
		return true;
	}

	return false;
}

bool CD3D11RandomAccessResource::CreateRandomAccessResource( ID3D11Device& device, const CD3D11Buffer& buffer, const BUFFER_TRAIT* traitOrNull )
{
	const D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc = nullptr;
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

	if ( traitOrNull )
	{
		uav = ConvertBufferTraitToUAV( *traitOrNull );
		pDesc = &uav;
	}

	if ( SUCCEEDED( device.CreateUnorderedAccessView( buffer.Get( ), pDesc, m_pUnorderedAccessView.GetAddressOf( ) ) ) )
	{
		return true;
	}

	return false;
}

bool CD3D11VertexShader::CreateShader( ID3D11Device& device, const void* byteCodePtr, const size_t byteCodeSize, const D3D11_INPUT_ELEMENT_DESC* layout, int numLayout )
{
	bool result = SUCCEEDED( device.CreateInputLayout( layout,
		numLayout,
		byteCodePtr,
		byteCodeSize,
		&m_pInputLayout ) );

	if ( result )
	{
		result = SUCCEEDED( device.CreateVertexShader(
			byteCodePtr,
			byteCodeSize,
			nullptr,
			&m_pVertexShader ) );
	}

	return result;
}

bool CD3D11PixelShader::CreateShader( ID3D11Device& device, const void* byteCodePtr, size_t byteCodeSize )
{
	bool result = SUCCEEDED( device.CreatePixelShader(
		byteCodePtr,
		byteCodeSize,
		nullptr,
		&m_pPixelShader ) );

	return result;
}

bool CD3D11ComputeShader::CreateShader( ID3D11Device& device, const void* byteCodePtr, size_t byteCodeSize )
{
	bool result = SUCCEEDED( device.CreateComputeShader(
		byteCodePtr,
		byteCodeSize,
		nullptr,
		&m_pComputeShader ) );

	return result;
}
