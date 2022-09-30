#pragma once

#include "Math/Matrix.h"
#include "SizedTypes.h"

#include <map>
#include <optional>

namespace rendercore
{
	class Scene;

	struct ComponentVelocityData
	{
		Matrix m_prevTransform;
		uint64 m_lastFrameUpdated = 0;
	};

	class SceneVelocityData
	{
	public:
		void StartFrame( Scene& scene );
		void UpdateTransform( uint64 numframe, uint32 primitiveId, const Matrix& prevTransform );
		void RemoveFromScene( uint32 primitiveId );
		std::optional<Matrix> GetPreviousTransform( uint32 primitiveId ) const;

	private:
		std::map<uint32, ComponentVelocityData> m_componentData;
	};
}
