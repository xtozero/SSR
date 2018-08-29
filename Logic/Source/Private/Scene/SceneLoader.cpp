#include "stdafx.h"
#include "Scene/SceneLoader.h"

#include "Core/GameLogic.h"
#include "DataStructure/KeyValueReader.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "Util.h"

#include <tchar.h>

std::unique_ptr<KeyValue> CSceneLoader::LoadSceneFromFile( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& objectList, const String& fileName )
{
	CKeyValueReader scene;

	std::unique_ptr<KeyValue> pKeyValue = scene.LoadKeyValueFromFile( fileName );

	if ( pKeyValue )
	{
		SetSceneObjectProperty( gameLogic, pKeyValue.get(), objectList );

		return std::move( pKeyValue );
	}
	else
	{
		return nullptr;
	}
}

void CSceneLoader::SetSceneObjectProperty( CGameLogic& gameLogic, KeyValue* keyValue, std::vector<std::unique_ptr<CGameObject>>& objectList )
{
	for ( const KeyValue* object = keyValue->Find( _T( "Object" ) ); object != nullptr; object = object->GetNext( ) )
	{
		auto newObject = CGameObjectFactory::GetInstance( ).CreateGameObjectByClassName( object->GetValue( ) );

		if ( newObject )
		{
			newObject->LoadPropertyFromScript( *object );
			objectList.emplace_back( newObject );
		}
	}

	for ( const auto& object : objectList )
	{
		object->Initialize( gameLogic );
	}
}