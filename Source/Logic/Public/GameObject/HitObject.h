#pragma once

#include "Common.h"
#include "Proxies/HitProxy.h"

namespace logic
{
	class CGameObject;
	class PrimitiveComponent;

	class HitObject : public rendercore::HitProxy
	{
		GENERATE_CLASS_TYPE_INFO( HitObject )
	public:
		LOGIC_DLL CGameObject* GetObject();

		HitObject() = default;
		HitObject( CGameObject* gameObject, PrimitiveComponent* component )
			: m_gameObject( gameObject )
			, m_component( component )
		{}

	private:
		CGameObject* m_gameObject = nullptr;
		PrimitiveComponent* m_component = nullptr;
	};
}