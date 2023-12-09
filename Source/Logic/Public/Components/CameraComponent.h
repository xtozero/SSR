#pragma once

#include "SceneComponent.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"

namespace json
{
	class Value;
}

namespace logic
{
	class CGameLogic;
	class IRenderer;

	class CameraComponent : public SceneComponent
	{
		GENERATE_CLASS_TYPE_INFO( CameraComponent )

	public:
		using SceneComponent::SceneComponent;

		virtual void LoadProperty( const json::Value& json ) override;
	};
}
