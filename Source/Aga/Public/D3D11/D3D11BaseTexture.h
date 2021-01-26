#pragma once
#include "Texture.h"

#include "D3D11Api.h"

#include <d3d11.h>

namespace aga
{
	template <typename T>
	class D3D11BaseTexture : public Texture
	{
	public:
		virtual void InitResource( ) override
		{
			if ( m_texture == nullptr )
			{
				CreateTexture( );
			}
			
			CreateShaderResource( );
			CreateRenderTarget( );
			CreateDepthStencil( );
		}

		virtual void Free( ) override
		{
			if ( m_srv )
			{
				m_srv->Release( );
				m_srv = nullptr;
			}

			if ( m_rtv )
			{
				m_rtv->Release( );
				m_rtv = nullptr;
			}

			if ( m_dsv )
			{
				m_dsv->Release( );
				m_dsv = nullptr;
			}

			if ( m_texture )
			{
				m_texture->Release( );
				m_texture = nullptr;
			}
		}

		ID3D11RenderTargetView* RenderTargetView( ) { return m_rtv; }

		D3D11BaseTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData ) : m_trait( trait ) 
		{
			if ( initData )
			{
				m_dataStorage = new unsigned char[initData->m_srcSize];
				std::memcpy( m_dataStorage, initData->m_srcData, initData->m_srcSize );

				m_initData.pSysMem = m_dataStorage;
				m_initData.SysMemPitch = initData->m_pitch;
				m_initData.SysMemSlicePitch = initData->m_slicePitch;
			}
		}
		D3D11BaseTexture( ) = default;
		D3D11BaseTexture( const D3D11BaseTexture& ) = delete;
		D3D11BaseTexture& operator=( const D3D11BaseTexture& ) = delete;
		D3D11BaseTexture( D3D11BaseTexture&& ) = delete;
		D3D11BaseTexture& operator=( D3D11BaseTexture&& ) = delete;
		~D3D11BaseTexture( )
		{
			Free( );
			delete[] m_dataStorage;
			m_dataStorage = nullptr;
		}

	protected:
		void CreateShaderResource( )
		{
			if ( ( m_trait.m_bindType & RESOURCE_BIND_TYPE::SHADER_RESOURCE ) > 0 )
			{
				HRESULT hr = D3D11Device( ).CreateShaderResourceView( m_texture, nullptr, &m_srv );
				assert( SUCCEEDED( hr ) );
			}
		}

		void CreateRenderTarget( )
		{
			if ( ( m_trait.m_bindType & RESOURCE_BIND_TYPE::RENDER_TARGET ) > 0 )
			{
				HRESULT hr = D3D11Device( ).CreateRenderTargetView( m_texture, nullptr, &m_rtv );
				assert( SUCCEEDED( hr ) );
			}
		}

		void CreateDepthStencil( )
		{
			if ( ( m_trait.m_bindType & RESOURCE_BIND_TYPE::DEPTH_STENCIL ) > 0 )
			{
				HRESULT hr = D3D11Device( ).CreateDepthStencilView( m_texture, nullptr, &m_dsv );
				assert( SUCCEEDED( hr ) );
			}
		}

		T* m_texture = nullptr;
		TEXTURE_TRAIT m_trait = {};
		D3D11_SUBRESOURCE_DATA m_initData = {};

	private:
		virtual void CreateTexture( ) = 0;

		void* m_dataStorage = nullptr;

		ID3D11ShaderResourceView* m_srv = nullptr;
		ID3D11RenderTargetView* m_rtv = nullptr;
		ID3D11DepthStencilView* m_dsv = nullptr;
	};

	class D3D11BaseTexture1D final : public D3D11BaseTexture<ID3D11Texture1D>
	{
	public:
		D3D11BaseTexture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );

	private:
		virtual void CreateTexture( ) override;

		D3D11_TEXTURE1D_DESC m_desc = {};
	};

	class D3D11BaseTexture2D final : public D3D11BaseTexture<ID3D11Texture2D>
	{
	public:
		D3D11BaseTexture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );
		explicit D3D11BaseTexture2D( ID3D11Texture2D* texture );

	private:
		virtual void CreateTexture( ) override;

		D3D11_TEXTURE2D_DESC m_desc = {};
	};

	class D3D11BaseTexture3D final : public D3D11BaseTexture<ID3D11Texture3D>
	{
	public:
		D3D11BaseTexture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );

	private:
		virtual void CreateTexture( ) override;

		D3D11_TEXTURE3D_DESC m_desc = {};
	};

	bool IsTexture1D( const TEXTURE_TRAIT& trait );
	bool IsTexture2D( const TEXTURE_TRAIT& trait );
	bool IsTexture3D( const TEXTURE_TRAIT& trait );
}
