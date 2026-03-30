#include "Texture.h"

#include "ICommandList.h"
#include "InterfaceFactories.h"
#include "IResourceManager.h"

namespace agl
{
	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName )
	{
		auto newTexture = GetInterface<IResourceManager>()->CreateTexture( desc, debugName, ResourceState::Common, nullptr );
		EnqueueRenderTask(
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName, ResourceState initialState )
	{
		auto newTexture = GetInterface<IResourceManager>()->CreateTexture( desc, debugName, initialState, nullptr );
		EnqueueRenderTask(
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName, const ResourceInitData* initData )
	{
		auto newTexture = GetInterface<IResourceManager>( )->CreateTexture( desc, debugName, ResourceState::Common, initData );
		EnqueueRenderTask(
			[texture = newTexture]()
			{
				texture->Init();
			} );

		return newTexture;
	}

	RefHandle<Texture> Texture::Create( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
	{
		auto newTexture = GetInterface<IResourceManager>()->CreateTexture( desc, debugName, initialState, initData );
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

	bool IsTexture2D( const TextureDesc& desc )
	{
		return ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) == false );
	}

	bool IsTexture3D( const TextureDesc& desc )
	{
		return HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D );
	}
}
