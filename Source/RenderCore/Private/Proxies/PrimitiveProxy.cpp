#include "stdafx.h"
#include "Proxies/PrimitiveProxy.h"

#include "MultiThread/EngineTaskScheduler.h"
#include "Scene/PrimitiveSceneInfo.h"

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

std::size_t PrimitiveProxy::GetId( ) const
{
	assert( IsInRenderThread( ) );
	assert( m_primitiveSceneInfo );
	return m_primitiveSceneInfo->m_id;
}
