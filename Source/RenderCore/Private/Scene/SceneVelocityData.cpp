#include "Scene/SceneVelocityData.h"

#include "Scene/Scene.h"

namespace rendercore
{
	void SceneVelocityData::StartFrame( Scene& scene )
	{
		uint64 numFrame = scene.GetNumFrame();

		bool trimOld = ( numFrame % 100 == 0 );

		for ( auto iter = begin( m_componentData ); iter != end( m_componentData ); )
		{
			ComponentVelocityData& data = iter->second;

			uint64 deltaFrame = numFrame - data.m_lastFrameUpdated;
			if ( deltaFrame == 1 )
			{
				data.m_prevTransform = data.m_curTransform;
				scene.AddPrimitiveToUpdate( iter->first );
			}
			
			if ( trimOld && ( deltaFrame > 10 ) )
			{
				iter = m_componentData.erase( iter );
			}
			else
			{
				++iter;
			}
		}
	}

	void SceneVelocityData::UpdateTransform( uint64 numFrame, uint32 primitiveId, const Matrix& curTransform, const Matrix& prevTransform )
	{
		ComponentVelocityData& velocityData = m_componentData[primitiveId];
		velocityData.m_curTransform = curTransform;
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
