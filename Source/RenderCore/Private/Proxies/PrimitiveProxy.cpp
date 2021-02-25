#include "stdafx.h"
#include "Proxies/PrimitiveProxy.h"

#include "MultiThread/EngineTaskScheduler.h"

void PrimitiveProxy::SetTransform( const CXMFLOAT4X4& worldTransform )
{
	assert( IsInRenderThread() );
	m_worldTransform = worldTransform;
}

const CXMFLOAT4X4& PrimitiveProxy::GetTransform( ) const
{
	assert( IsInRenderThread( ) );
	return m_worldTransform;
}
