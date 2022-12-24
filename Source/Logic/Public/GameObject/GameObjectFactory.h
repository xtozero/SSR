#pragma once

#include "GuideTypes.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

class CGameObject;
class CCreateGameObjectHelper;

class IGameObjectFactory
{
public:
	virtual void RegistGameObjectCreateFunc( const std::string& className, CCreateGameObjectHelper* helper ) = 0;
	virtual Owner<CGameObject*> CreateGameObjectByClassName( const std::string& className ) const = 0;

	virtual ~IGameObjectFactory( ) = default;
};

IGameObjectFactory& GetGameObjectFactory( );

class CCreateGameObjectHelper
{
public:
	CCreateGameObjectHelper( const std::string& className, std::function<CGameObject*( )> createFunc )
		: m_createFunc( createFunc )
	{
		GetGameObjectFactory( ).RegistGameObjectCreateFunc( className, this );
	}

	Owner<CGameObject*> Create( )
	{
		if ( m_createFunc )
		{
			return m_createFunc( );
		}
		else
		{
			return nullptr;
		}
	}

private:
	std::function<Owner<CGameObject*>( )> m_createFunc;
};

#define DECLARE_GAME_OBJECT( name, classType ) \
	static Owner<CGameObject*> create_##name( ) \
	{ \
		return new classType; \
	} \
	static CCreateGameObjectHelper name##_create_heper( #name, create_##name );