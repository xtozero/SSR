#include "stdafx.h"
#include "Buffer.h"

#include "Core/InterfaceFactories.h"
#include "IRenderResourceManager.h"

namespace aga
{
	RefHandle<Buffer> Buffer::Create( const BUFFER_TRAIT& trait, const void* initData )
	{
		return GetInterface<IResourceManager>( )->CreateBuffer( trait, initData );
	}
}
