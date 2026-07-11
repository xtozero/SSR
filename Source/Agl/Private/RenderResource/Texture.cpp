#include "Texture.h"

#include "ICommandList.h"
#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName )
	{
		RefHandle<Texture> newTexture = GetInterface<IResourceManager>()->CreateTexture( desc, debugName, ResourceState::Common, nullptr );
		EnqueueRenderTask(
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName, ResourceState initialState )
	{
		RefHandle<Texture> newTexture = GetInterface<IResourceManager>()->CreateTexture( desc, debugName, initialState, nullptr );
		EnqueueRenderTask(
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName, const ResourceInitData* initData )
	{
		RefHandle<Texture> newTexture = GetInterface<IResourceManager>( )->CreateTexture( desc, debugName, ResourceState::Common, initData );
		EnqueueRenderTask(
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
	{
		RefHandle<Texture> newTexture = GetInterface<IResourceManager>()->CreateTexture( desc, debugName, initialState, initData );
		EnqueueRenderTask(
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	UnorderedAccessView* Texture::UAV( uint32 mipSlice )
	{
		assert( mipSlice < m_desc.m_mipLevels );
		return m_uav.empty() ? nullptr : m_uav[mipSlice].Get();
	}

	const UnorderedAccessView* Texture::UAV( uint32 mipSlice ) const
	{
		assert( mipSlice < m_desc.m_mipLevels );
		return  m_uav.empty() ? nullptr : m_uav[mipSlice].Get();
	}

	ResourceState Texture::GetResourceState() const
	{
		return m_state;
	}

	void Texture::SetResourceState( ResourceState state )
	{
		m_state = state;
	}

	const TextureDesc& Texture::GetDesc() const
	{
		return m_desc;
	}

	bool Texture::IsCubeMap() const
	{
		return HasAnyFlags( m_desc.m_miscFlag, ResourceMisc::TextureCube );
	}

	Texture::Texture( const TextureDesc& desc, ResourceState initialState, const char* debugName ) noexcept
		: m_desc( desc )
		, m_state( initialState )
	{
		m_isTexture = true;

		m_debugName = Name( debugName );
	}

	Texture::Texture() noexcept
	{
		m_isTexture = true;
	}

	void TextureBase::UpdateTextureMips( uint32 width, uint32 height, int32 mipLevels, const ResourceInitData& initData )
	{
		// Mip streaming is only available for textures used exclusively as shader resources.
		assert( m_desc.m_bindType == ResourceBindType::ShaderResource );

		m_desc.m_width = width;
		m_desc.m_height = height;
		m_desc.m_mipLevels = mipLevels;

		delete[] m_dataStorage;

		m_dataStorage = new uint8[initData.m_srcSize];
		std::memcpy( m_dataStorage, initData.m_srcData, initData.m_srcSize );
	}

	bool IsTexture2D( const TextureDesc& desc )
	{
		return ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) == false );
	}

	bool IsTexture3D( const TextureDesc& desc )
	{
		return HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D );
	}
}
