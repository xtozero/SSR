#pragma once

#include "Texture.h"
#include "D3D11Api.h"
#include "SizedTypes.h"

#include <d3d11.h>
#include <vector>

namespace agl
{
	template <typename T>
	class D3D11Texture : public TextureBase
	{
	public:
		virtual void* Resource() const
		{
			return m_texture;
		}

		D3D11Texture( const TextureTrait& trait, const ResourceInitData* initData ) : TextureBase( trait, initData ) 
		{
			if ( initData )
			{
				size_t numSections = initData->m_sections.size();

				m_initData.resize( numSections );
				for ( size_t i = 0; i < numSections; ++i )
				{
					const ResourceSectionData& section = initData->m_sections[i];

					m_initData[i].pSysMem = static_cast<unsigned char*>( m_dataStorage ) + section.m_offset;
					m_initData[i].SysMemPitch = static_cast<uint32>( section.m_pitch );
					m_initData[i].SysMemSlicePitch = static_cast<uint32>( section.m_slicePitch );
				}
			}
		}
		D3D11Texture() = default;
		D3D11Texture( const D3D11Texture& ) = delete;
		D3D11Texture& operator=( const D3D11Texture& ) = delete;
		D3D11Texture( D3D11Texture&& ) = delete;
		D3D11Texture& operator=( D3D11Texture&& ) = delete;

	protected:
		T* m_texture = nullptr;
		std::vector<D3D11_SUBRESOURCE_DATA> m_initData;

		virtual void CreateShaderResource() = 0;
		virtual void CreateUnorderedAccess() = 0;
		virtual void CreateRenderTarget() = 0;
		virtual void CreateDepthStencil() = 0;

	private:
		virtual void InitResource() override
		{
			if ( m_texture == nullptr )
			{
				CreateTexture();
			}

			if ( HasAnyFlags( m_trait.m_miscFlag, ResourceMisc::Intermediate ) )
			{
				return;
			}

			if ( m_texture )
			{
				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::ShaderResource ) )
				{
					CreateShaderResource();
				}

				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::RandomAccess ) )
				{
					CreateUnorderedAccess();
				}

				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::RenderTarget ) )
				{
					CreateRenderTarget();
				}

				if ( HasAnyFlags( m_trait.m_bindType, ResourceBindType::DepthStencil ) )
				{
					CreateDepthStencil();
				}
			}
		}

		virtual void FreeResource() override
		{
			TextureBase::FreeResource();

			if ( m_texture )
			{
				m_texture->Release();
				m_texture = nullptr;
			}
		}
	};

	class D3D11BaseTexture1D final : public D3D11Texture<ID3D11Texture1D>
	{
	public:
		D3D11BaseTexture1D( const TextureTrait& trait, const ResourceInitData* initData );

	protected:
		virtual void CreateTexture() override;

		virtual void CreateShaderResource() override;
		virtual void CreateUnorderedAccess() override;
		virtual void CreateRenderTarget() override {};
		virtual void CreateDepthStencil() override {};

	private:
		D3D11_TEXTURE1D_DESC m_desc = {};
	};

	class D3D11BaseTexture2D final : public D3D11Texture<ID3D11Texture2D>
	{
	public:
		D3D11BaseTexture2D( const TextureTrait& trait, const ResourceInitData* initData );
		explicit D3D11BaseTexture2D( ID3D11Texture2D* texture );

	protected:
		virtual void CreateTexture() override;

		virtual void CreateShaderResource() override;
		virtual void CreateUnorderedAccess() override;
		virtual void CreateRenderTarget() override;
		virtual void CreateDepthStencil() override;

	private:
		D3D11_TEXTURE2D_DESC m_desc = {};
	};

	class D3D11BaseTexture3D final : public D3D11Texture<ID3D11Texture3D>
	{
	public:
		D3D11BaseTexture3D( const TextureTrait& trait, const ResourceInitData* initData );

	protected:
		virtual void CreateTexture() override;

		virtual void CreateShaderResource() override;
		virtual void CreateUnorderedAccess() override;
		virtual void CreateRenderTarget() override {};
		virtual void CreateDepthStencil() override {};

	private:
		D3D11_TEXTURE3D_DESC m_desc = {};
	};
}
