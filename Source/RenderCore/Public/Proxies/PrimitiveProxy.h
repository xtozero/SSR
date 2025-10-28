#pragma once

#include "DrawSnapshot.h"
#include "Math/Matrix.h"
#include "Mesh/MeshDrawInfo.h"
#include "Physics/BoxSphereBounds.h"
#include "SizedTypes.h"

#include <optional>

namespace logic
{
	class PrimitiveComponent;
}

namespace rendercore
{
	class DrawSnapshot;
	class HitProxy;
	class PrimitiveSceneInfo;
	class Scene;
	class ScenePrimitiveBuffer;
	class SceneViewConstantBuffer;

	struct RenderViewInfo;

	class PrimitiveProxy
	{
	public:
		virtual void CreateRenderData() = 0;
		virtual void PrepareSubMeshs() = 0;
		virtual void TakeSnapshot( RenderFrameArray<DrawSnapshot>& outSnapshotStorage, RenderFrameArray<VisibleDrawSnapshot>& outVisibleSnapshot ) const = 0;
		virtual std::optional<DrawSnapshot> TakeSnapshot( uint32 lod, uint32 sectionIndex ) const = 0;
		virtual std::optional<MeshDrawInfo> GatherMeshDrawInfo( uint32 lod, uint32 sectionIndex ) const = 0;
		virtual void GatherDynamicMeshDrawInfo( [[maybe_unused]] RenderViewInfo& viewInfo ) const {};

		virtual HitProxy* CreateHitProxy( logic::PrimitiveComponent* component ) const;

		bool CastShadow() const;
		const Matrix& WorldTransform() const;
		const BoxSphereBounds& Bounds() const;
		const BoxSphereBounds& LocalBounds() const;

		void UpdateTransformAndBounds( const Matrix& worldTransform, const BoxSphereBounds& bounds, const BoxSphereBounds& localBounds );

		uint32 PrimitiveId() const;

		virtual ~PrimitiveProxy() = default;

	protected:
		friend Scene;

		bool m_castShadow = true;

		PrimitiveSceneInfo* m_primitiveSceneInfo = nullptr;
		Matrix m_worldTransform;
		BoxSphereBounds m_bounds;
		BoxSphereBounds m_localBounds;
	};
}
