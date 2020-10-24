#include "stdafx.h"
#include "Common/GraphicsApiResource.h"

int GraphicsApiResource::AddRef( )
{
	++m_refCount;
	return m_refCount;
}

int GraphicsApiResource::ReleaseRef( )
{
	--m_refCount;

	int refCount = m_refCount;
	if ( refCount == 0 )
	{
		Free( );
		delete this;
	}

	return refCount;
}