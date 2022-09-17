#include "stdafx.h"
#include "GraphicsApiResource.h"

namespace agl
{
	int32 GraphicsApiResource::AddRef()
	{
		++m_refCount;
		return m_refCount;
	}

	int32 GraphicsApiResource::ReleaseRef()
	{
		--m_refCount;

		int32 refCount = m_refCount;
		if ( refCount == 0 )
		{
			Free();
			delete this;
		}

		return refCount;
	}
}
