#pragma once

#include "Texture.h"

#include <d3d12.h>
#include <vector>
#include <d3d11.h>

namespace agl
{
	template <typename T>
	class D3D12Texture : public TextureBase
	{
	public:
		virtual void* Resource() const
		{
			return m_texture;
		}

		D3D12Texture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData ) : TextureBase( trait, initData )
		{
			if ( initData )
			{
				size_t numSections = initData->m_sections.size();

				m_initData.resize( numSections );
				for ( size_t i = 0; i < numSections; ++i )
				{
					const RESOURCE_SECTION_DATA& section = initData->m_sections[i];

					m_initData[i].pData = static_cast<unsigned char*>( m_dataStorage ) + section.m_offset;
					m_initData[i].RowPitch = section.m_pitch;
					m_initData[i].SlicePitch = section.m_slicePitch;
				}
			}
		}
		D3D12Texture() = default;
		D3D12Texture( const D3D12Texture& ) = delete;
		D3D12Texture& operator=( const D3D12Texture& ) = delete;
		D3D12Texture( D3D12Texture&& ) = delete;
		D3D12Texture& operator=( D3D12Texture&& ) = delete;

	protected:
		T* m_texture = nullptr;
		D3D12_RESOURCE_DESC m_desc;
		D3D12_HEAP_PROPERTIES m_heapProperties;
		D3D12_HEAP_FLAGS m_heapFlags;
		std::vector<D3D12_SUBRESOURCE_DATA> m_initData;

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

	class D3D12BaseTexture1D final : public D3D12Texture<ID3D12Resource>
	{
	public:
		D3D12BaseTexture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );

	protected:
		virtual void CreateTexture() override;

		virtual void CreateShaderResource() override;
		virtual void CreateUnorderedAccess() override;
		virtual void CreateRenderTarget() override {};
		virtual void CreateDepthStencil() override {};

	private:
	};

	class D3D12BaseTexture2D final : public D3D12Texture<ID3D12Resource>
	{
	public:
		D3D12BaseTexture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );
		D3D12BaseTexture2D( ID3D12Resource* texture, const D3D12_RESOURCE_DESC* desc = nullptr );

	protected:
		virtual void CreateTexture() override;

		virtual void CreateShaderResource() override;
		virtual void CreateUnorderedAccess() override;
		virtual void CreateRenderTarget() override;
		virtual void CreateDepthStencil() override;

	private:
	};

	class D3D12BaseTexture3D final : public D3D12Texture<ID3D12Resource>
	{
	public:
		D3D12BaseTexture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData );

	protected:
		virtual void CreateTexture() override;

		virtual void CreateShaderResource() override;
		virtual void CreateUnorderedAccess() override;
		virtual void CreateRenderTarget() override {};
		virtual void CreateDepthStencil() override {};

	private:
	};
}
