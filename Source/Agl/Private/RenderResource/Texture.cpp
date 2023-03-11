#include "stdafx.h"
#include "Texture.h"

#include "ICommandList.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Texture> Texture::Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
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

	const TEXTURE_TRAIT& Texture::GetTrait() const
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

	bool IsTexture1D( const TEXTURE_TRAIT& trait )
	{
		return trait.m_height <= 1;
	}

	bool IsTexture2D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_height > 1 ) && ( ( trait.m_miscFlag & ResourceMisc::Texture3D ) == ResourceMisc::None );
	}

	bool IsTexture3D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_miscFlag & ResourceMisc::Texture3D ) != ResourceMisc::None;
	}
}
