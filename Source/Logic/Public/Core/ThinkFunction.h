#pragma once

#include "SizedTypes.h"

#include <memory>

namespace logic
{
	class CGameObject;
	class Component;
	class World;

	enum class ThinkingGroup : uint8
	{
		PrePhysics = 0,
		StartPhysics,
		DuringPhysics,
		EndPhysics,
		PostPhysics,
		Max,
	};

	class ThinkFunction
	{
	public:
		virtual void ExecuteThink( float elapsedTime ) = 0;

		ThinkingGroup m_thinkGroup;

		bool m_canEverTick;

		float m_thinkInterval;

		struct InternalData
		{
			InternalData( World& world ) :
				m_world( world ) {}

			bool m_registered = false;

			float m_lastThinkTime = -1;

			World& m_world;
		};

		std::unique_ptr<InternalData> m_internalData;

		bool IsThinkFunctionRegistered();

		void RegisterThinkFunction( World& world );
		void UnRegisterThinkFunction();
	};

	class GameObjectThinkFunction : public ThinkFunction
	{
	public:
		virtual void ExecuteThink( float elapsedTime ) override;

		CGameObject* m_target;
	};

	class ComponentThinkFunction : public ThinkFunction
	{
	public:
		virtual void ExecuteThink( float elapsedTime ) override;

		Component* m_target;
	};
}
