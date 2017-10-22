#include "stdafx.h"

#include <tchar.h>

#include "GameLogic.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "SceneLoader.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

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
	const KeyValue* object = keyValue->Find( _T( "Object" ) );
	while ( object )
	{
		auto newObject = CGameObjectFactory::GetInstance( ).CreateGameObjectByClassName( object->GetValue( ) );

		if ( newObject )
		{
			newObject->LoadPropertyFromScript( *object );
			objectList.emplace_back( newObject );
		}

		object = object->GetNext( );
	}

	for ( const auto& object : objectList )
	{
		object->Initialize( gameLogic );
	}
}