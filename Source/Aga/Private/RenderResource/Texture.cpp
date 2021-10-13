#include "stdafx.h"
#include "Texture.h"

#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace aga
{
	RefHandle<Texture> Texture::Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
	{
		return GetInterface<IResourceManager>( )->CreateTexture( trait, initData );
	}
}
