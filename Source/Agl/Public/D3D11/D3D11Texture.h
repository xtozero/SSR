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
		virtual void* Resource() const override
		{
			return m_texture;
		}

		virtual void CreateShaderResource( std::optional<ResourceFormat> overrideFormat = {} ) = 0;
		virtual void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat = {} ) = 0;
		virtual void CreateRenderTarget( std::optional<ResourceFormat> overrideFormat = {} ) = 0;
		virtual void CreateDepthStencil( std::optional<ResourceFormat> overrideFormat = {} ) = 0;

		void Reconstruct( const TextureTrait& trait, const ResourceInitData* initData )
		{
			delete[] m_dataStorage;

			m_trait = trait;
			ConvertToDesc( trait );
			m_initData.clear();

			if ( initData )
			{
				m_dataStorage = new unsigned char[initData->m_srcSize];
				std::memcpy( m_dataStorage, initData->m_srcData, initData->m_srcSize );

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

		D3D11Texture( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
			: TextureBase( trait, debugName, initialState, initData )
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

	private:
		virtual void InitResource() override
		{
			if ( m_texture == nullptr )
			{
				CreateTexture();
			}

			if ( HasAnyFlags( m_trait.m_miscFlag, ResourceMisc::Intermediate | ResourceMisc::WithoutViews ) )
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

		virtual void ConvertToDesc( const TextureTrait& trait ) = 0;
	};

	class D3D11Texture2D final : public D3D11Texture<ID3D11Texture2D>
	{
	public:
		virtual void CreateShaderResource( std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateRenderTarget( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateDepthStencil( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override;

		D3D11Texture2D( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData );
		D3D11Texture2D( ID3D11Texture2D* texture, const char* debugName, const D3D11_TEXTURE2D_DESC* desc = nullptr );

	protected:
		virtual void CreateTexture() override;

	private:
		virtual void ConvertToDesc( const TextureTrait& trait ) override;

		D3D11_TEXTURE2D_DESC m_desc = {};
	};

	class D3D11Texture3D final : public D3D11Texture<ID3D11Texture3D>
	{
	public:
		virtual void CreateShaderResource( std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateRenderTarget( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override;
		virtual void CreateDepthStencil( [[maybe_unused]] std::optional<ResourceFormat> overrideFormat = {} ) override {};

		D3D11Texture3D( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData );

	protected:
		virtual void CreateTexture() override;

	private:
		virtual void ConvertToDesc( const TextureTrait& trait ) override;

		D3D11_TEXTURE3D_DESC m_desc = {};
	};
}
