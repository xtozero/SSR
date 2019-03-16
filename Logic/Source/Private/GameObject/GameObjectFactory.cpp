#include "stdafx.h"
#include "GameObject/GameObjectFactory.h"

class CGameObjectFactory : public IGameObjectFactory
{
public:
	virtual void RegistGameObjectCreateFunc( const String& className, CCreateGameObjectHelper* helper ) override
	{
		m_createHelpers.emplace( className, helper );
	}

	virtual Owner<CGameObject*> CreateGameObjectByClassName( const String& className ) const override
	{
		auto found = m_createHelpers.find( className );

		if ( found != m_createHelpers.end( ) )
		{
			return found->second->Create( );
		}
		else
		{
			return nullptr;
		}
	}

private:
	std::map<String, CCreateGameObjectHelper*> m_createHelpers;
};

IGameObjectFactory& GetGameObjectFactory( )
{
	static CGameObjectFactory factory;
	return factory;
}
