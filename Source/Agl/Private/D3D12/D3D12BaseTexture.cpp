#include "D3D12BaseTexture.h"

namespace agl
{
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

	D3D12BaseTexture2D::D3D12BaseTexture2D( ID3D12Resource* texture )
	{
		if ( texture )
		{
			m_texture = texture;
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
