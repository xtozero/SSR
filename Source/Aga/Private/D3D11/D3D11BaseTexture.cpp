#include "stdafx.h"
#include "D3D11BaseTexture.h"

#include "D3D11FlagConvertor.h"
#include "D3D11ResourceViews.h"

namespace aga
{
	D3D11_TEXTURE1D_DESC ConvertTraitTo1DDesc( const TEXTURE_TRAIT& trait )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		uint32 bindFlag = ConvertTypeToBind( trait.m_bindType );
		uint32 cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

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
		uint32 bindFlag = ConvertTypeToBind( trait.m_bindType );
		uint32 cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

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
		uint32 bindFlag = ConvertTypeToBind( trait.m_bindType );
		uint32 cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

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
		RESOURCE_ACCESS_FLAG access = ConvertUsageToAccessFlag( desc.Usage );
		RESOURCE_BIND_TYPE bindType = ConvertBindToType( desc.BindFlags );
		RESOURCE_MISC miscFlag = ConvertDXMiscToMisc( desc.MiscFlags );


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

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE1D_DESC& desc )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

		srv.Format = desc.Format;

		if ( desc.ArraySize > 1 )
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1DARRAY;
			srv.Texture1DArray.ArraySize = desc.ArraySize;
			srv.Texture1DArray.MipLevels = desc.MipLevels;
		}
		else
		{
			srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE1D;
			srv.Texture1D.MipLevels = desc.MipLevels;
		}

		return srv;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE2D_DESC& desc )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

		srv.Format = desc.Format;

		if ( desc.ArraySize % 6 == 0 )
		{
			if ( desc.ArraySize == 6 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
				srv.TextureCube.MipLevels = desc.MipLevels;
			}
			else
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
				srv.TextureCubeArray.MipLevels = desc.MipLevels;
				srv.TextureCubeArray.NumCubes = desc.ArraySize / 6;
			}
		}
		else if ( desc.ArraySize > 1 )
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMSARRAY;
				srv.Texture2DMSArray.ArraySize = desc.ArraySize;
			}
			else
			{

				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
				srv.Texture2DArray.MipLevels = desc.MipLevels;
				srv.Texture2DArray.ArraySize = desc.ArraySize;
			}
		}
		else
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
				srv.Texture2D.MipLevels = desc.MipLevels;
			}
		}

		return srv;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE3D_DESC& desc )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

		srv.Format = desc.Format;
		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		srv.Texture3D.MipLevels = desc.MipLevels;

		return srv;
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

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertDescToUAV( const D3D11_TEXTURE3D_DESC& desc )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

		uav.Format = desc.Format;
		uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uav.Texture3D.WSize = desc.Depth;

		return uav;
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

	D3D11_DEPTH_STENCIL_VIEW_DESC ConvertDescToDSV( const D3D11_TEXTURE2D_DESC& desc )
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};

		dsv.Format = desc.Format;

		if ( desc.ArraySize > 1 )
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
				dsv.Texture2DMSArray.ArraySize = desc.ArraySize;
			}
			else
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				dsv.Texture2DArray.ArraySize = desc.ArraySize;
			}
		}
		else
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
		}

		return dsv;
	}

	D3D11BaseTexture1D::D3D11BaseTexture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D11Texture<ID3D11Texture1D>( trait, initData )
	{
	}

	void D3D11BaseTexture1D::CreateTexture()
	{
		m_desc = ConvertTraitTo1DDesc( m_trait );

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateTexture1D( &m_desc, m_dataStorage ? m_initData.data() : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );
	}

	void D3D11BaseTexture1D::CreateShaderResource()
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_desc );
		m_srv = new D3D11ShaderResourceView( this, m_texture, srvDesc );
		m_srv->Init();
	}

	void D3D11BaseTexture1D::CreateUnorderedAccess()
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_desc );
		m_uav = new D3D11UnorderedAccessView( this, m_texture, uavDesc );
		m_uav->Init();
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

	void D3D11BaseTexture2D::CreateTexture()
	{
		m_desc = ConvertTraitTo2DDesc( m_trait );

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateTexture2D( &m_desc, m_dataStorage ? m_initData.data() : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );
	}

	void D3D11BaseTexture2D::CreateShaderResource()
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_desc );
		m_srv = new D3D11ShaderResourceView( this, m_texture, srvDesc );
		m_srv->Init();
	}

	void D3D11BaseTexture2D::CreateUnorderedAccess()
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_desc );
		m_uav = new D3D11UnorderedAccessView( this, m_texture, uavDesc );
		m_uav->Init();
	}

	void D3D11BaseTexture2D::CreateRenderTarget()
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertDescToRTV( m_desc );
		m_rtv = new D3D11RenderTargetView( this, m_texture, rtvDesc );
		m_rtv->Init();
	}

	void D3D11BaseTexture2D::CreateDepthStencil()
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = ConvertDescToDSV( m_desc );
		m_dsv = new D3D11DepthStencilView( this, m_texture, dsvDesc );
		m_dsv->Init();
	}

	D3D11BaseTexture3D::D3D11BaseTexture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D11Texture<ID3D11Texture3D>( trait, initData )
	{
	}

	void D3D11BaseTexture3D::CreateTexture()
	{
		m_desc = ConvertTraitTo3DDesc( m_trait );

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateTexture3D( &m_desc, m_dataStorage ? m_initData.data() : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );
	}

	void D3D11BaseTexture3D::CreateShaderResource()
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_desc );
		m_srv = new D3D11ShaderResourceView( this, m_texture, srvDesc );
		m_srv->Init();
	}

	void D3D11BaseTexture3D::CreateUnorderedAccess()
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_desc );
		m_uav = new D3D11UnorderedAccessView( this, m_texture, uavDesc );
		m_uav->Init();
	}

	bool IsTexture1D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_height <= 1 ) && ( ( trait.m_miscFlag & RESOURCE_MISC::APP_SIZE_DEPENDENT ) == RESOURCE_MISC::NONE );
	}

	bool IsTexture2D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_height > 1 ) && ( ( trait.m_miscFlag & RESOURCE_MISC::TEXTURE3D ) == RESOURCE_MISC::NONE );
	}

	bool IsTexture3D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_miscFlag & RESOURCE_MISC::TEXTURE3D ) != RESOURCE_MISC::NONE;
	}
}
