#include "D3D12BaseTexture.h"

#include "D3D12FlagConvertor.h"
#include "D3D12ResourceViews.h"

namespace agl
{
	TEXTURE_TRAIT ConvertDescToTrait( const D3D12_RESOURCE_DESC& desc, const D3D12_HEAP_PROPERTIES& heapProperties )
	{
		ResourceFormat format = ConvertDxgiFormatToFormat( desc.Format );
		ResourceBindType bindType = ConvertResourceFlagsToBindType( desc.Flags );
		ResourceAccessFlag access = ConvertHeapTypeToAccessFlag( heapProperties.Type );

		return TEXTURE_TRAIT{
			.m_width = static_cast<uint32>( desc.Width ),
			.m_height = static_cast<uint32>( desc.Height ),
			.m_depth = static_cast<uint32>( desc.DepthOrArraySize ),
			.m_sampleCount = desc.SampleDesc.Count,
			.m_sampleQuality = desc.SampleDesc.Quality,
			.m_mipLevels = desc.MipLevels,
			.m_format = format,
			.m_access = access,
			.m_bindType = bindType,
			.m_miscFlag = ResourceMisc::None
		};
	}

	D3D12_RENDER_TARGET_VIEW_DESC ConvertDescToRTV( const D3D12_RESOURCE_DESC& desc )
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtv = {};

		rtv.Format = desc.Format;

		if ( desc.DepthOrArraySize > 1 )
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
				rtv.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
			}
			else
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv.Texture2DArray.ArraySize = desc.DepthOrArraySize;
			}
		}
		else
		{
			if ( desc.SampleDesc.Count > 1 )
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			}
		}

		return rtv;
	}

	D3D12BaseTexture1D::D3D12BaseTexture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D12Texture<ID3D12Resource>( trait, initData )
	{
	}

	void D3D12BaseTexture1D::CreateTexture()
	{
	}

	void D3D12BaseTexture1D::CreateShaderResource()
	{
	}

	void D3D12BaseTexture1D::CreateUnorderedAccess()
	{
	}

	D3D12BaseTexture2D::D3D12BaseTexture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D12Texture<ID3D12Resource>( trait, initData )
	{
	}

	D3D12BaseTexture2D::D3D12BaseTexture2D( ID3D12Resource* texture, const D3D12_RESOURCE_DESC* desc )
	{
		if ( texture )
		{
			m_texture = texture;
			if ( desc == nullptr )
			{
				m_desc = m_texture->GetDesc();
			}
			else
			{
				m_desc = *desc;
			}

			HRESULT hr = m_texture->GetHeapProperties( &m_heapProperties, &m_heapFlags );
			assert( SUCCEEDED( hr ) );

			m_trait = ConvertDescToTrait( m_desc, m_heapProperties );
		}
	}

	void D3D12BaseTexture2D::CreateTexture()
	{
	}

	void D3D12BaseTexture2D::CreateShaderResource()
	{
	}

	void D3D12BaseTexture2D::CreateUnorderedAccess()
	{
	}

	void D3D12BaseTexture2D::CreateRenderTarget()
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertDescToRTV( m_desc );
		m_rtv = new D3D12RenderTargetView( this, m_texture, rtvDesc );
		m_rtv->Init();
	}

	void D3D12BaseTexture2D::CreateDepthStencil()
	{
	}

	D3D12BaseTexture3D::D3D12BaseTexture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D12Texture<ID3D12Resource>( trait, initData )
	{
	}

	void D3D12BaseTexture3D::CreateTexture()
	{
	}

	void D3D12BaseTexture3D::CreateShaderResource()
	{
	}

	void D3D12BaseTexture3D::CreateUnorderedAccess()
	{
	}
}
