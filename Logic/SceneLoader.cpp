#include "stdafx.h"

#include <tchar.h>

#include "GameObject.h"
#include "GameObjectFactory.h"
#include "SceneLoader.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

std::shared_ptr<KeyValueGroup> CSceneLoader::LoadSceneFromFile( IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& objectList, const String& fileName )
{
	CKeyValueReader scene;

	auto pKeyValue = scene.LoadKeyValueFromFile( fileName );

	if ( pKeyValue )
	{
		SetSceneObjectProperty( renderer, pKeyValue, objectList );

		return pKeyValue;
	}
	else
	{
		return nullptr;
	}
}

void CSceneLoader::SetSceneObjectProperty( IRenderer& renderer, std::shared_ptr<KeyValueGroup> keyValue, std::vector<std::unique_ptr<CGameObject>>& objectList )
{
	CGameObject* curObject = nullptr;

	for ( auto findedKey = keyValue->FindKeyValue( _T( "Scene" ) ); findedKey != nullptr; ++findedKey )
	{
		if ( findedKey->GetKey( ) == _T( "Object" ) )
		{
			auto newObject = CGameObjectFactory::GetInstance().CreateGameObjectByClassName( findedKey->GetValue( ) );

			if ( newObject )
			{
				objectList.emplace_back( newObject );
				curObject = objectList.back( ).get( );
			}
		}
		else if ( curObject )
		{
			curObject->LoadPropertyFromScript( findedKey );
		}
	}

	for ( const auto& object : objectList )
	{
		object->Initialize( renderer );
	}
}