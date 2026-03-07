#include "Proxies/PrimitiveProxy.h"

#include "Components/PrimitiveComponent.h"
#include "GameObject/HitObject.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "TaskScheduler.h"

namespace rendercore
{
	HitProxy* PrimitiveProxy::CreateHitProxy( logic::PrimitiveComponent* component ) const
	{
		return new logic::HitObject( component->GetOwner(), component );
	}

	void PrimitiveProxy::AddToRaytracingScene( [[maybe_unused]] RaytracingScene& raytracingScene )
	{
	}

	bool PrimitiveProxy::CastShadow() const
	{
		return m_castShadow;
	}

	const Matrix& PrimitiveProxy::WorldTransform() const
	{
		assert( IsInRenderThread() );
		return m_worldTransform;
	}

	const BoxSphereBounds& PrimitiveProxy::Bounds() const
	{
		assert( IsInRenderThread() );
		return m_bounds;
	}

	const BoxSphereBounds& PrimitiveProxy::LocalBounds() const
	{
		assert( IsInRenderThread() );
		return m_localBounds;
	}

	void PrimitiveProxy::UpdateTransformAndBounds( const Matrix& worldTransform, const BoxSphereBounds& bounds, const BoxSphereBounds& localBounds )
	{
		m_worldTransform = worldTransform;
		m_bounds = bounds;
		m_localBounds = localBounds;
	}

	uint32 PrimitiveProxy::PrimitiveId() const
	{
		assert( IsInRenderThread() );
		assert( m_primitiveSceneInfo );
		return m_primitiveSceneInfo->PrimitiveId();
	}
}
