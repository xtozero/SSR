#include "stdafx.h"
#include "Texture.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace agl
{
	RefHandle<Texture> Texture::Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
	{
		return GetInterface<IResourceManager>( )->CreateTexture( trait, initData );
	}

	bool IsTexture1D( const TEXTURE_TRAIT& trait )
	{
		return ( trait.m_height <= 1 ) && ( ( trait.m_miscFlag & ResourceMisc::AppSizeDependent ) == ResourceMisc::None );
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
