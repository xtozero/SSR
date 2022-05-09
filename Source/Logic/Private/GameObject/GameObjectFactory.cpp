#include "stdafx.h"
#include "GameObject/GameObjectFactory.h"

#include "NameTypes.h"

class CGameObjectFactory : public IGameObjectFactory
{
public:
	virtual void RegistGameObjectCreateFunc( const std::string& className, CCreateGameObjectHelper* helper ) override
	{
		m_createHelpers.emplace( className, helper );
	}

	virtual Owner<CGameObject*> CreateGameObjectByClassName( const std::string& className ) const override
	{
		auto found = m_createHelpers.find( Name( className ) );
		if ( found != m_createHelpers.end() )
		{
			return found->second->Create();
		}
		else
		{
			return nullptr;
		}
	}

private:
	std::map<Name, CCreateGameObjectHelper*> m_createHelpers;
};

IGameObjectFactory& GetGameObjectFactory()
{
	static CGameObjectFactory factory;
	return factory;
}
