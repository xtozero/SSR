#include "stdafx.h"
#include "World/WorldLoader.h"

#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

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
					newObject->LoadProperty( gameLogic, *pObject );
					gameLogic.SpawnObject( newObject );
				}
			}
		}

		return true;
	}

	return false;
}
