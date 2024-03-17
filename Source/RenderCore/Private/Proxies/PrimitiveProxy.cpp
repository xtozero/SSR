#include "Proxies/PrimitiveProxy.h"

#include "Scene/PrimitiveSceneInfo.h"
#include "TaskScheduler.h"

namespace rendercore
{
	Matrix& PrimitiveProxy::WorldTransform()
	{
		assert( IsInRenderThread() );
		return m_worldTransform;
	}

	const Matrix& PrimitiveProxy::WorldTransform() const
	{
		assert( IsInRenderThread() );
		return m_worldTransform;
	}

	BoxSphereBounds& PrimitiveProxy::Bounds()
	{
		assert( IsInRenderThread() );
		return m_bounds;
	}

	const BoxSphereBounds& PrimitiveProxy::Bounds() const
	{
		assert( IsInRenderThread() );
		return m_bounds;
	}

	BoxSphereBounds& PrimitiveProxy::LocalBounds()
	{
		assert( IsInRenderThread() );
		return m_localBounds;
	}

	const BoxSphereBounds& PrimitiveProxy::LocalBounds() const
	{
		assert( IsInRenderThread() );
		return m_localBounds;
	}

	uint32 PrimitiveProxy::PrimitiveId() const
	{
		assert( IsInRenderThread() );
		assert( m_primitiveSceneInfo );
		return m_primitiveSceneInfo->PrimitiveId();
	}
}
