#include "stdafx.h"
#include "Texture.h"

#include "ICommandList.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Texture> Texture::Create( const TextureTrait& trait, const char* debugName, const ResourceInitData* initData )
	{
		return GetInterface<IResourceManager>( )->CreateTexture( trait, debugName, initData );
	}

	ResourceState Texture::GetState() const
	{
		return m_state;
	}

	void Texture::SetState( ResourceState state )
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

	Texture::Texture( const TextureTrait& trait, const char* debugName ) noexcept
		: m_trait( trait )
	{
		m_isTexture = true;

		m_debugName = Name( debugName );
	}

	Texture::Texture() noexcept
	{
		m_isTexture = true;
	}

	bool IsTexture1D( const TextureTrait& trait )
	{
		return trait.m_height <= 1;
	}

	bool IsTexture2D( const TextureTrait& trait )
	{
		return ( trait.m_height > 1 ) && ( ( trait.m_miscFlag & ResourceMisc::Texture3D ) == ResourceMisc::None );
	}

	bool IsTexture3D( const TextureTrait& trait )
	{
		return ( trait.m_miscFlag & ResourceMisc::Texture3D ) != ResourceMisc::None;
	}
}
