#include "stdafx.h"
#include "Buffer.h"

#include "Core/InterfaceFactories.h"
#include "IAga.h"

namespace aga
{
	RefHandle<Buffer> aga::Buffer::Create( const BUFFER_TRAIT& trait, const void* initData )
	{
		return GetInterface<IAga>( )->CreateBuffer( trait, initData );
	}
}
