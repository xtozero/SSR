#include "D3D11Texture.h"

#include "D3D11FlagConvertor.h"
#include "D3D11ResourceViews.h"

namespace agl
{
	D3D11_TEXTURE2D_DESC ConvertToTex2DDesc( const TextureDesc& desc )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( desc.m_format );
		DXGI_SAMPLE_DESC SampleDesc = {
			.Count = desc.m_sampleCount,
			.Quality = desc.m_sampleQuality
		};
		D3D11_USAGE usage = ConvertToUsage( desc.m_access );
		uint32 bindFlag = ConvertTypeToBind( desc.m_bindType );
		uint32 cpuAccessFlag = ConvertToCpuFlag( desc.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( desc.m_miscFlag );

		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Intermediate ) )
		{
			bindFlag |= D3D11_BIND_SHADER_RESOURCE;
		}

		return D3D11_TEXTURE2D_DESC{
			.Width = desc.m_width,
			.Height = desc.m_height,
			.MipLevels = desc.m_mipLevels,
			.ArraySize = desc.m_depth,
			.Format = format,
			.SampleDesc = SampleDesc,
			.Usage = usage,
			.BindFlags = bindFlag,
			.CPUAccessFlags = cpuAccessFlag,
			.MiscFlags = miscFlags
		};
	}

	D3D11_TEXTURE3D_DESC ConvertToTex3DDesc( const TextureDesc& desc )
	{
		DXGI_FORMAT format = ConvertFormatToDxgiFormat( desc.m_format );
		D3D11_USAGE usage = ConvertToUsage( desc.m_access );
		uint32 bindFlag = ConvertTypeToBind( desc.m_bindType );
		uint32 cpuAccessFlag = ConvertToCpuFlag( desc.m_access );
		uint32 miscFlags = ConvertMicsToDXMisc( desc.m_miscFlag );

		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Intermediate ) )
		{
			bindFlag |= D3D11_BIND_SHADER_RESOURCE;
		}

		return D3D11_TEXTURE3D_DESC{
			.Width = desc.m_width,
			.Height = desc.m_height,
			.Depth = desc.m_depth,
			.MipLevels = desc.m_mipLevels,
			.Format = format,
			.Usage = usage,
			.BindFlags = bindFlag,
			.CPUAccessFlags = cpuAccessFlag,
			.MiscFlags = miscFlags
		};
	}

	TextureDesc ConvertToTextureDesc( const D3D11_TEXTURE2D_DESC& desc )
	{
		ResourceFormat format = ConvertDxgiFormatToFormat( desc.Format );
		ResourceAccess access = ConvertToResourceAccess( desc.Usage );
		ResourceBindType bindType = ConvertBindToType( desc.BindFlags );
		ResourceMisc miscFlag = ConvertDXMiscToMisc( desc.MiscFlags );

		return TextureDesc{
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

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertToSRVDesc( const D3D11_TEXTURE2D_DESC& desc )
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

	D3D11_SHADER_RESOURCE_VIEW_DESC ConvertToSRVDesc( const D3D11_TEXTURE3D_DESC& desc )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};

		srv.Format = ConvertDxgiFormatForSRV( desc.Format );
		srv.ViewDimension = D3D_SRV_DIMENSION_TEXTURE3D;
		srv.Texture3D.MipLevels = desc.MipLevels;

		return srv;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertToUAVDesc( const D3D11_TEXTURE2D_DESC& desc, uint32 mipSlice )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

		uav.Format = desc.Format;

		if ( desc.ArraySize > 1 )
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
			uav.Texture2DArray = {
				.MipSlice = mipSlice,
				.FirstArraySlice = 0,
				.ArraySize = desc.ArraySize
			};
		}
		else
		{
			uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uav.Texture2D = {
				.MipSlice = mipSlice
			};
		}

		return uav;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC ConvertToUAVDesc( const D3D11_TEXTURE3D_DESC& desc, uint32 mipSlice )
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uav = {};

		uav.Format = desc.Format;
		uav.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
		uav.Texture3D = {
			.MipSlice = mipSlice,
			.FirstWSlice = 0,
			.WSize = desc.Depth
		};

		return uav;
	}

	D3D11_RENDER_TARGET_VIEW_DESC ConvertToRTVDesc( const D3D11_TEXTURE2D_DESC& desc )
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

	D3D11_RENDER_TARGET_VIEW_DESC ConvertToRTVDesc( const D3D11_TEXTURE3D_DESC& desc )
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtv = {};

		rtv.Format = desc.Format;
		rtv.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		rtv.Texture3D = {
			.MipSlice = 0,
			.FirstWSlice = 0,
			.WSize = desc.Depth
		};

		return rtv;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC ConvertToDSVDesc( const D3D11_TEXTURE2D_DESC& desc )
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
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertToSRVDesc( m_d3dDesc );
		if ( overrideFormat.has_value() )
		{
			srvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_srv = new D3D11ShaderResourceView( this, m_texture, srvDesc );
		m_srv->Init();
	}

	void D3D11Texture2D::CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat )
	{
		m_uav.resize( m_desc.m_mipLevels );
		for ( uint32 mipSlice = 0; mipSlice < m_desc.m_mipLevels; ++mipSlice )
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertToUAVDesc( m_d3dDesc, mipSlice );
			if ( overrideFormat.has_value() )
			{
				uavDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
			}

			m_uav[mipSlice] = new D3D11UnorderedAccessView( this, m_texture, uavDesc );
			m_uav[mipSlice]->Init();
		}
	}

	void D3D11Texture2D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertToRTVDesc( m_d3dDesc );
		if ( overrideFormat.has_value() )
		{
			rtvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}

		ColorF clearColor = ColorF::Black;
		if ( m_desc.m_clearValue )
		{
			clearColor = m_desc.m_clearValue->m_color;
		}

		m_rtv = new D3D11RenderTargetView( this, m_texture, rtvDesc, clearColor );
		m_rtv->Init();
	}

	void D3D11Texture2D::CreateDepthStencil( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = ConvertToDSVDesc( m_d3dDesc );
		if ( overrideFormat.has_value() )
		{
			dsvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}

		float depthClearValue = 0;
		uint8 stencilClearValue = 0;
		if ( m_desc.m_clearValue )
		{
			depthClearValue = m_desc.m_clearValue->m_depthStencil.m_depth;
			stencilClearValue = m_desc.m_clearValue->m_depthStencil.m_stencil;
		}

		m_dsv = new D3D11DepthStencilView( this, m_texture, dsvDesc, depthClearValue, stencilClearValue );
		m_dsv->Init();
	}

	void D3D11Texture2D::UpdateTextureMips( uint32 width, uint32 height, int32 mipLevels, const ResourceInitData& initData )
	{
		TextureBase::UpdateTextureMips( width, height, mipLevels, initData );

		SetInitData( initData );

		CreateTexture();

		if ( m_srv.Get() )
		{
			static_cast<D3D11ShaderResourceView*>( m_srv.Get() )->UpdateTextureMips( m_texture, m_desc.m_mipLevels );
		}
	}

	D3D11Texture2D::D3D11Texture2D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
		: D3D11Texture<ID3D11Texture2D>( desc, debugName, initialState, initData )
	{
	}

	D3D11Texture2D::D3D11Texture2D( ID3D11Texture2D* texture, const char* debugName, const float4& clearColor, const D3D11_TEXTURE2D_DESC* desc )
	{
		if ( texture )
		{
			m_texture = texture;

			Rename( Name( debugName ) );

			if ( desc == nullptr )
			{
				m_texture->GetDesc( &m_d3dDesc );
			}
			else
			{
				m_d3dDesc = *desc;
			}

			m_desc = ConvertToTextureDesc( m_d3dDesc );
			m_desc.m_clearValue = ResourceClearValue{
				.m_format = m_desc.m_format,
				.m_color = { clearColor[0], clearColor[1], clearColor[2], clearColor[3] },
			};
		}
	}

	void D3D11Texture2D::CreateTexture()
	{
		BuildD3DDesc( m_desc );

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateTexture2D( &m_d3dDesc, m_dataStorage ? m_initData.data() : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );

		SetDebugObjectName();
	}

	void D3D11Texture2D::BuildD3DDesc( const TextureDesc& desc )
	{
		m_d3dDesc = ConvertToTex2DDesc( desc );
	}

	void D3D11Texture3D::CreateShaderResource( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertToSRVDesc( m_d3dDesc );
		if ( overrideFormat.has_value() )
		{
			srvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_srv = new D3D11ShaderResourceView( this, m_texture, srvDesc );
		m_srv->Init();
	}

	void D3D11Texture3D::CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat )
	{
		m_uav.resize( m_desc.m_mipLevels );
		for ( uint32 mipSlice = 0; mipSlice < m_desc.m_mipLevels; ++mipSlice )
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertToUAVDesc( m_d3dDesc, mipSlice );
			if ( overrideFormat.has_value() )
			{
				uavDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
			}

			m_uav[mipSlice] = new D3D11UnorderedAccessView( this, m_texture, uavDesc );
			m_uav[mipSlice]->Init();
		}
	}

	void D3D11Texture3D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertToRTVDesc( m_d3dDesc );
		if ( overrideFormat.has_value() )
		{
			rtvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}

		ColorF clearColor = ColorF::Black;
		if ( m_desc.m_clearValue )
		{
			clearColor = m_desc.m_clearValue->m_color;
		}

		m_rtv = new D3D11RenderTargetView( this, m_texture, rtvDesc, clearColor );
		m_rtv->Init();
	}

	void D3D11Texture3D::UpdateTextureMips( uint32 width, uint32 height, int32 mipLevels, const ResourceInitData& initData )
	{
		TextureBase::UpdateTextureMips( width, height, mipLevels, initData );

		SetInitData( initData );

		CreateTexture();

		if ( m_srv.Get() )
		{
			static_cast<D3D11ShaderResourceView*>( m_srv.Get() )->UpdateTextureMips( m_texture, m_desc.m_mipLevels );
		}
	}

	D3D11Texture3D::D3D11Texture3D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
		: D3D11Texture<ID3D11Texture3D>( desc, debugName, initialState, initData )
	{
	}

	void D3D11Texture3D::CreateTexture()
	{
		BuildD3DDesc( m_desc );

		[[maybe_unused]] HRESULT hr = D3D11Device().CreateTexture3D( &m_d3dDesc, m_dataStorage ? m_initData.data() : nullptr, &m_texture );
		assert( SUCCEEDED( hr ) );

		SetDebugObjectName();
	}

	void D3D11Texture3D::BuildD3DDesc( const TextureDesc& desc )
	{
		m_d3dDesc = ConvertToTex3DDesc( desc );
	}
}
