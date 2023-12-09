#include "stdafx.h"
#include "Components/CameraComponent.h"

#include "common.h"
#include "Core/Timer.h"
#include "Json/json.hpp"
#include "Math/TransformationMatrix.h"

#include <type_traits>

namespace logic
{
	void CameraComponent::LoadProperty( const json::Value& json )
	{
		if ( const json::Value* pPos = json.Find( "Position" ) )
		{
			const json::Value& pos = *pPos;

			if ( pos.Size() == 3 )
			{
				Vector origin( static_cast<float>( pos[0].AsReal() ),
					static_cast<float>( pos[1].AsReal() ),
					static_cast<float>( pos[2].AsReal() ) );
				SetRelativePosition( origin );
			}
		}
	}
}
