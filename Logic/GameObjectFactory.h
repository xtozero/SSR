#pragma once

#include <functional>
#include <map>
#include <memory>

#include "../Shared/Util.h"

class CGameObject;
class CCreateGameObjectHelper;

class CGameObjectFactory
{
public:
	static CGameObjectFactory& GetInstance( );

	void RegistGameObjectCreateFunc( const String& className, CCreateGameObjectHelper* helper );
	Owner<CGameObject*> CreateGameObjectByClassName( const String& className );

	CGameObjectFactory( );
	~CGameObjectFactory( );

private:
	std::map<String, CCreateGameObjectHelper*> m_createHelpers;
};

class CCreateGameObjectHelper
{
public:
	CCreateGameObjectHelper( const String& className, std::function<CGameObject*( )> createFunc )
		: m_createFunc( createFunc )
	{
		CGameObjectFactory::GetInstance( ).RegistGameObjectCreateFunc( className, this );
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
	std::function<Owner<CGameObject*> ( )> m_createFunc;
};

#define DECLARE_GAME_OBJECT( name, classType ) \
	static Owner<CGameObject*> create_##name( ) \
	{ \
		return new classType; \
	} \
	static CCreateGameObjectHelper name##_create_heper( _T( #name ), create_##name );