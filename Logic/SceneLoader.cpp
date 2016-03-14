#include "stdafx.h"

#include <tchar.h>

#include "GameObject.h"
#include "GameObjectFactory.h"
#include "SceneLoader.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#define CHECK_VALID_ITERATOR( iter, container ) \
if ( iter == container.end( ) ) \
		{ \
		DebugWarning( "> SceneLoader - SetModel Fail!!!!!" ); \
		return; \
		}

std::shared_ptr<KeyValueGroup> CSceneLoader::LoadSceneFromFile( const String& fileName, std::vector<std::shared_ptr<CGameObject>>& objectList )
{
	CKeyValueReader scene;

	auto pKeyValue = scene.LoadKeyValueFromFile( fileName );

	if ( pKeyValue )
	{
		SetSceneObjectProperty( pKeyValue, objectList );

		return pKeyValue;
	}
	else
	{
		return nullptr;
	}
}

void CSceneLoader::SetSceneObjectProperty( std::shared_ptr<KeyValueGroup> keyValue, std::vector<std::shared_ptr<CGameObject>>& objectList )
{
	auto curObject = objectList.begin( );

	for ( auto findedKey = keyValue->FindKeyValue( _T( "Scene" ) ); findedKey != nullptr; ++findedKey )
	{
		if ( findedKey->GetKey( ) == String( _T( "Object" ) ) )
		{
			if ( curObject != objectList.end( ) &&
				curObject->get( ) &&
				curObject->get( )->NeedInitialize( ) )
			{
				curObject->get( )->Initialize( );
			}

			auto newObject = CGameObjectFactory::GetInstance( )->CreateGameObjectByClassName( findedKey->GetString( ) );

			if ( newObject == nullptr )
			{
			}
			else
			{
				objectList.push_back( newObject );
				curObject = ( objectList.end( ) - 1 );
			}
		}
		else if ( curObject != objectList.end() )
		{
			CHECK_VALID_ITERATOR( curObject, objectList );

			curObject->get()->LoadPropertyFromScript( findedKey );
		}
	}

	if ( curObject != objectList.end( ) &&
		curObject->get( ) &&
		curObject->get( )->NeedInitialize( ) )
	{
		curObject->get( )->Initialize( );
	}
}