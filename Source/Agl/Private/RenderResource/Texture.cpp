#include "Texture.h"

#include "ICommandList.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Texture> Texture::Create( const TextureTrait& trait, const char* debugName )
	{
		return GetInterface<IResourceManager>()->CreateTexture( trait, debugName, ResourceState::Common, nullptr );
	}

	RefHandle<Texture> Texture::Create( const TextureTrait& trait, const char* debugName, ResourceState initialState )
	{
		return GetInterface<IResourceManager>()->CreateTexture( trait, debugName, initialState, nullptr );
	}

	RefHandle<Texture> Texture::Create( const TextureTrait& trait, const char* debugName, const ResourceInitData* initData )
	{
		return GetInterface<IResourceManager>( )->CreateTexture( trait, debugName, ResourceState::Common, initData );
	}

	RefHandle<Texture> Texture::Create( const TextureTrait& trait, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
	{
		return GetInterface<IResourceManager>()->CreateTexture( trait, debugName, initialState, initData );
	}

	UnorderedAccessView* Texture::UAV( uint32 mipSlice )
	{
		assert( mipSlice < m_trait.m_mipLevels );
		return m_uav.empty() ? nullptr : m_uav[mipSlice].Get();
	}

	const UnorderedAccessView* Texture::UAV( uint32 mipSlice ) const
	{
		assert( mipSlice < m_trait.m_mipLevels );
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

	const TextureTrait& Texture::GetTrait() const
	{
		return m_trait;
	}

	bool Texture::IsCubeMap() const
	{
		return HasAnyFlags( m_trait.m_miscFlag, ResourceMisc::TextureCube );
	}

	Texture::Texture( const TextureTrait& trait, ResourceState initialState, const char* debugName ) noexcept
		: m_trait( trait )
		, m_state( initialState )
	{
		m_isTexture = true;

		m_debugName = Name( debugName );
	}

	Texture::Texture() noexcept
	{
		m_isTexture = true;
	}

	bool IsTexture2D( const TextureTrait& trait )
	{
		return ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) == false );
	}

	bool IsTexture3D( const TextureTrait& trait )
	{
		return HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D );
	}
}
