#pragma once

#include "GuideTypes.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace logic
{
	class GameObject;
	class CreateGameObjectHelper;

	class IGameObjectFactory
	{
	public:
		virtual void RegistGameObjectCreateFunc( const std::string& className, CreateGameObjectHelper* helper ) = 0;
		virtual Owner<GameObject*> CreateGameObjectByClassName( const std::string& className ) const = 0;

		virtual ~IGameObjectFactory() = default;
	};

	IGameObjectFactory& GetGameObjectFactory();

	class CreateGameObjectHelper
	{
	public:
		CreateGameObjectHelper( const std::string& className, const std::function<GameObject*()>& createFunc )
			: m_createFunc( createFunc )
		{
			GetGameObjectFactory().RegistGameObjectCreateFunc( className, this );
		}

		Owner<GameObject*> Create() const
		{
			if ( m_createFunc )
			{
				return m_createFunc();
			}
			else
			{
				return nullptr;
			}
		}

	private:
		std::function<Owner<GameObject*>()> m_createFunc;
	};
}

#define DECLARE_GAME_OBJECT( name, classType ) \
	static Owner<GameObject*> create_##name( ) \
	{ \
		return new classType; \
	} \
	static CreateGameObjectHelper name##_create_heper( #name, create_##name );
