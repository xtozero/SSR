#pragma once

#include "LibraryTool/Common.h"
#include "Proxies/HitProxy.h"

namespace logic
{
	class GameObject;
	class PrimitiveComponent;

	class HitObject : public rendercore::HitProxy
	{
		GENERATE_CLASS_TYPE_INFO( HitObject )
	public:
		LOGIC_DLL GameObject* GetObject();

		HitObject() = default;
		HitObject( GameObject* gameObject, PrimitiveComponent* component )
			: m_gameObject( gameObject )
			, m_component( component )
		{}

	private:
		GameObject* m_gameObject = nullptr;
		PrimitiveComponent* m_component = nullptr;
	};
}