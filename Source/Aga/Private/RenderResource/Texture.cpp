#include "stdafx.h"
#include "Texture.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

namespace aga
{
	RefHandle<Texture> Texture::Create( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
	{
		return GetInterface<IAga>( )->CreateTexture( trait, initData );
	}
}
