#pragma once
#include "Texture.h"

#include "D3D11Api.h"

#include <d3d11.h>

namespace aga
{
	class D3D11BaseTexture : public Texture
	{
	public:
		virtual std::pair<UINT, UINT> Size( ) const override
		{
			return { m_trait.m_width, m_trait.m_height };
		}

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
		virtual void FreeResource( ) override
		{
			m_srv = nullptr;
			m_uav = nullptr;
			m_rtv = nullptr;
			m_dsv = nullptr;
		}

		virtual void CreateTexture( ) = 0;

		TEXTURE_TRAIT m_trait = {};
		D3D11_SUBRESOURCE_DATA m_initData = {};
	private:
		void* m_dataStorage = nullptr;
	};

	template <typename T>
	class D3D11Texture : public D3D11BaseTexture
	{
	public:
		D3D11Texture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData ) : D3D11BaseTexture( trait, initData ) { }
		D3D11Texture( ) = default;
		D3D11Texture( const D3D11BaseTexture& ) = delete;
		D3D11Texture& operator=( const D3D11BaseTexture& ) = delete;
		D3D11Texture( D3D11BaseTexture&& ) = delete;
		D3D11Texture& operator=( D3D11BaseTexture&& ) = delete;

	protected:
		T* m_texture = nullptr;

		virtual void CreateShaderResource( ) = 0;
		virtual void CreateRenderTarget( ) = 0;
		virtual void CreateDepthStencil( ) = 0;

	private:
		virtual void InitResource( ) override
		{
			if ( m_texture == nullptr )
			{
				CreateTexture( );
			}

			if ( m_texture )
			{
				if ( ( m_trait.m_bindType & RESOURCE_BIND_TYPE::SHADER_RESOURCE ) > 0 )
				{
					CreateShaderResource( );
				}

				if ( ( m_trait.m_bindType & RESOURCE_BIND_TYPE::RENDER_TARGET ) > 0 )
				{
					CreateRenderTarget( );
				}

				if ( ( m_trait.m_bindType & RESOURCE_BIND_TYPE::DEPTH_STENCIL ) > 0 )
				{
					CreateDepthStencil( );
				}
			}
		}

		virtual void FreeResource( ) override
		{
			D3D11BaseTexture::FreeResource( );

			if ( m_texture )
			{
				m_texture->Release( );
				m_texture = nullptr;
			}
		}
	};

	class D3D11BaseTexture1D final : public D3D11Texture<ID3D11Texture1D>
	{
	public:
		D3D11BaseTexture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );

	protected:
		virtual void CreateTexture( ) override;

		virtual void CreateShaderResource( ) override;
		virtual void CreateRenderTarget( ) override {};
		virtual void CreateDepthStencil( ) override {};

	private:
		D3D11_TEXTURE1D_DESC m_desc = {};
	};

	class D3D11BaseTexture2D final : public D3D11Texture<ID3D11Texture2D>
	{
	public:
		D3D11BaseTexture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );
		explicit D3D11BaseTexture2D( ID3D11Texture2D* texture );

	protected:
		virtual void CreateTexture( ) override;

		virtual void CreateShaderResource( ) override;
		virtual void CreateRenderTarget( ) override;
		virtual void CreateDepthStencil( ) override;

	private:
		D3D11_TEXTURE2D_DESC m_desc = {};
	};

	class D3D11BaseTexture3D final : public D3D11Texture<ID3D11Texture3D>
	{
	public:
		D3D11BaseTexture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );

	protected:
		virtual void CreateTexture( ) override;

		virtual void CreateShaderResource( ) override;
		virtual void CreateRenderTarget( ) override {};
		virtual void CreateDepthStencil( ) override {};

	private:
		D3D11_TEXTURE3D_DESC m_desc = {};
	};

	bool IsTexture1D( const TEXTURE_TRAIT& trait );
	bool IsTexture2D( const TEXTURE_TRAIT& trait );
	bool IsTexture3D( const TEXTURE_TRAIT& trait );
}
