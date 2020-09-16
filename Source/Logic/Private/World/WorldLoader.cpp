#include "stdafx.h"
#include "World/WorldLoader.h"

#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"
#include "Util.h"

#include <tchar.h>

bool CWorldLoader::Load( CGameLogic& gameLogic, const char* worldAsset, size_t assetSize )
{
	JSON::Value root( JSON::DataType::EMPTY );
	JSON::Reader reader;

	if ( reader.Parse( worldAsset, assetSize, root ) )
	{
		const JSON::Value* pWorld = root.Find( "World" );
		if ( pWorld == nullptr )
		{
			return false;
		}

		for ( const JSON::Value& elem : *pWorld )
		{
			if ( const JSON::Value* pObject = elem.Find( "Object" ) )
			{
				Owner<CGameObject*> newObject = GetGameObjectFactory( ).CreateGameObjectByClassName( ( *pObject )["Class"].AsString( ) );

				if ( newObject )
				{
					newObject->LoadProperty( *pObject );
					newObject->Initialize( gameLogic );
					// gameLogic.SpawnObject( newObject );
				}
			}
			else if ( const JSON::Value* pCamera = elem.Find( "Camera" ) )
			{
				gameLogic.GetLocalPlayer( )->GetCamera( ).LoadProperty( *pCamera );
			}
		}

		return true;
	}

	return false;
}
