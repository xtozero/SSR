#include "stdafx.h"
#include "GameObjectFactory.h"

CGameObjectFactory* CGameObjectFactory::GetInstance( )
{
	static CGameObjectFactory gameObjectFactory;
	return &gameObjectFactory;
}

void CGameObjectFactory::RegistGameObjectCreateFunc( const String& className, CCreateGameObjectHelper* helper )
{
	m_createHelpers.emplace( className, helper );
}

std::shared_ptr<CGameObject> CGameObjectFactory::CreateGameObjectByClassName( const String& className )
{
	auto found = m_createHelpers.find( className );

	if ( found != m_createHelpers.end( ) )
	{
		return found->second->Create();
	}
	else
	{
		return nullptr;
	}
}

CGameObjectFactory::CGameObjectFactory( )
{
}


CGameObjectFactory::~CGameObjectFactory( )
{
}
