#include "stdafx.h"
#include "D3D11Texture.h"

#include "D3D11FlagConvertor.h"
#include "D3D11ResourceViews.h"

namespace agl
{
	D3D11_TEXTURE2D_DESC ConvertTraitTo2DDesc( const TextureTrait& trait )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
		DXGI_SAMPLE_DESC SampleDesc = {
			.Count = trait.m_sampleCount,
			.Quality = trait.m_sampleQuality 
		};
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		uint32 bindFlag = ConvertTypeToBind( trait.m_bindType );
		uint32 cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

		if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Intermediate ) )
		{
			bindFlag |= D3D11_BIND_SHADER_RESOURCE;
		}

		return D3D11_TEXTURE2D_DESC{
			.Width = trait.m_width,
			.Height = trait.m_height,
			.MipLevels = trait.m_mipLevels,
			.ArraySize = trait.m_depth,
			.Format = format,
			.SampleDesc = SampleDesc,
			.Usage = usage,
			.BindFlags = bindFlag,
			.CPUAccessFlags = cpuAccessFlag,
			.MiscFlags = miscFlags
		};
	}

	D3D11_TEXTURE3D_DESC ConvertTraitTo3DDesc( const TextureTrait& trait )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( trait.m_format );
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		uint32 bindFlag = ConvertTypeToBind( trait.m_bindType );
		uint32 cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );

		if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Intermediate ) )
		{
			bindFlag |= D3D11_BIND_SHADER_RESOURCE;
		}

		return D3D11_TEXTURE3D_DESC{
			.Width = trait.m_width,
			.Height = trait.m_height,
			.Depth = trait.m_depth,
			.MipLevels = trait.m_mipLevels,
			.Format = format,
			.Usage = usage,
			.BindFlags = bindFlag,
			.CPUAccessFlags = cpuAccessFlag,
			.MiscFlags = miscFlags
		};
	}

	TextureTrait ConvertDescToTrait( const D3D11_TEXTURE2D_DESC& desc )
	{
		ResourceFormat format = ConvertDxgiFormatToFormat( desc.Format );
		ResourceAccessFlag access = ConvertUsageToAccessFlag( desc.Usage );
		ResourceBindType bindType = ConvertBindToType( desc.BindFlags );
		ResourceMisc miscFlag = ConvertDXMiscToMisc( desc.MiscFlags );

		return TextureTrait{
			.m_width = desc.Width,
			.m_height = desc.Height,
			.m_depth = desc.ArraySize,
			.m_sampleCount = desc.SampleDesc.Count,
			.m_sampleQuality = desc.SampleDesc.Quality,
			.m_mipLevels = desc.MipLevels,
			.m_format = format,
			.m_access = access,
			.m_bindType = bindType,
			.m_miscFlag = miscFlag
		};
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertDescToSRV( const D3D11_TEXTURE2D_DESC& desc )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

		srv.Format = ConvertDxgiFormatForSRV( desc.Format );

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

		srv.Format = ConvertDxgiFormatForSRV( desc.Format );
		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		srv.Texture3D.MipLevels = desc.MipLevels;

		return srv;
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

		dsv.Format = ConvertDxgiFormatForDSV( desc.Format );

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

	void D3D11Texture2D::CreateShaderResource( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_desc );
		if ( overrideFormat.has_value() )
		{
			srvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_srv = new D3D11ShaderResourceView( this, m_texture, srvDesc );
		m_srv->Init();
	}

	void D3D11Texture2D::CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_desc );
		if ( overrideFormat.has_value() )
		{
			uavDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_uav = new D3D11UnorderedAccessView( this, m_texture, uavDesc );
		m_uav->Init();
	}

	void D3D11Texture2D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertDescToRTV( m_desc );
		if ( overrideFormat.has_value() )
		{
			rtvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_rtv = new D3D11RenderTargetView( this, m_texture, rtvDesc );
		m_rtv->Init();
	}

	void D3D11Texture2D::CreateDepthStencil( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = ConvertDescToDSV( m_desc );
		if ( overrideFormat.has_value() )
		{
			dsvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_dsv = new D3D11DepthStencilView( this, m_texture, dsvDesc );
		m_dsv->Init();
	}

	D3D11Texture2D::D3D11Texture2D( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
		: D3D11Texture<ID3D11Texture2D>( trait, debugName, initialState, initData )
	{
	}

	D3D11Texture2D::D3D11Texture2D( ID3D11Texture2D* texture, const char* debugName, const D3D11_TEXTURE2D_DESC* desc )
	{
		if ( texture )
		{
			m_debugName = Name( debugName );

			auto dataSize = static_cast<uint32>( m_debugName.Str().size() );
			texture->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, m_debugName.Str().data() );

			m_texture = texture;
			if ( desc == nullptr )
			{
				m_texture->GetDesc( &m_desc );
			}
			else
			{
				m_desc = *desc;
			}

			m_trait = ConvertDescToTrait( m_desc );
		}
	}

	void D3D11Texture2D::CreateTexture()
	{
		ConvertToDesc( m_trait );

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateTexture2D( &m_desc, m_dataStorage ? m_initData.data() : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );

		if ( m_texture )
		{
			auto dataSize = static_cast<uint32>( m_debugName.Str().size() );
			m_texture->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, m_debugName.Str().data() );
		}
	}

	void D3D11Texture2D::ConvertToDesc( const TextureTrait& trait )
	{
		m_desc = ConvertTraitTo2DDesc( trait );
	}

	void D3D11Texture3D::CreateShaderResource( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertDescToSRV( m_desc );
		if ( overrideFormat.has_value() )
		{
			srvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_srv = new D3D11ShaderResourceView( this, m_texture, srvDesc );
		m_srv->Init();
	}

	void D3D11Texture3D::CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertDescToUAV( m_desc );
		if ( overrideFormat.has_value() )
		{
			uavDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_uav = new D3D11UnorderedAccessView( this, m_texture, uavDesc );
		m_uav->Init();
	}

	D3D11Texture3D::D3D11Texture3D( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
		: D3D11Texture<ID3D11Texture3D>( trait, debugName, initialState, initData )
	{
	}

	void D3D11Texture3D::CreateTexture()
	{
		ConvertToDesc( m_trait );

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateTexture3D( &m_desc, m_dataStorage ? m_initData.data() : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );

		if ( m_texture )
		{
			auto dataSize = static_cast<uint32>( m_debugName.Str().size() );
			m_texture->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, m_debugName.Str().data() );
		}
	}

	void D3D11Texture3D::ConvertToDesc( const TextureTrait& trait )
	{
		m_desc = ConvertTraitTo3DDesc( trait );
	}
}
