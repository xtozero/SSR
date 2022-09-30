#include "Scene/SceneVelocityData.h"

#include "Scene/Scene.h"

namespace rendercore
{
	void SceneVelocityData::StartFrame( Scene& scene )
	{
		uint64 numFrame = scene.GetNumFrame();

		for ( auto iter = begin( m_componentData ); iter != end( m_componentData ); ++iter )
		{
			const ComponentVelocityData& data = iter->second;

			if ( numFrame - data.m_lastFrameUpdated == 1 )
			{
				scene.AddPrimitiveToUpdate( iter->first );
			}
		}
	}

	void SceneVelocityData::UpdateTransform( uint64 numFrame, uint32 primitiveId, const Matrix& prevTransform )
	{
		ComponentVelocityData& velocityData = m_componentData[primitiveId];
		velocityData.m_prevTransform = prevTransform;
		velocityData.m_lastFrameUpdated = numFrame;
	}

	void SceneVelocityData::RemoveFromScene( uint32 primitiveId )
	{
		m_componentData.erase( primitiveId );
	}

	std::optional<Matrix> SceneVelocityData::GetPreviousTransform( uint32 primitiveId ) const
	{
		auto found = m_componentData.find( primitiveId );
		if ( found != std::end( m_componentData ) )
		{
			return found->second.m_prevTransform;
		}

		return {};
	}
}
