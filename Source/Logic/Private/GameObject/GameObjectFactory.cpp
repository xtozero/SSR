#include "GameObject/GameObjectFactory.h"

#include "NameTypes.h"

namespace logic
{
	class GameObjectFactory : public IGameObjectFactory
	{
	public:
		virtual void RegistGameObjectCreateFunc( const std::string& className, CreateGameObjectHelper* helper ) override
		{
			m_createHelpers.emplace( className, helper );
		}

		virtual Owner<GameObject*> CreateGameObjectByClassName( const std::string& className ) const override
		{
			auto found = m_createHelpers.find( Name( className ) );
			if ( found != std::end( m_createHelpers ) )
			{
				return found->second->Create();
			}
			else
			{
				return nullptr;
			}
		}

	private:
		std::map<Name, CreateGameObjectHelper*> m_createHelpers;
	};

	IGameObjectFactory& GetGameObjectFactory()
	{
		static GameObjectFactory factory;
		return factory;
	}
}
