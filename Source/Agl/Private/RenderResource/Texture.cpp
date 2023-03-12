#include "stdafx.h"
#include "Texture.h"

#include "ICommandList.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Texture> Texture::Create( const TextureTrait& trait, const ResourceInitData* initData )
	{
		return GetInterface<IResourceManager>( )->CreateTexture( trait, initData );
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

	void Texture::Transition( ICommandListBase& commandList, ResourceState state )
	{
		if ( m_state == state )
		{
			return;
		}

		ResourceTransition transition = {
			.m_pResource = Resource(),
			.m_pTransitionable = *this,
			.m_subResource = AllSubResource,
			.m_state = state
		};

		commandList.Transition( 1, &transition );
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
