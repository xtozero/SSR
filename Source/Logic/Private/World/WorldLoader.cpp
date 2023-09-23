#include "stdafx.h"
#include "World/WorldLoader.h"

#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"

#include <tchar.h>

namespace logic
{
	bool CWorldLoader::Load( CGameLogic& gameLogic, const char* worldAsset, size_t assetSize )
	{
		json::Value root( json::DataType::Empty );
		json::Reader reader;

		if ( reader.Parse( worldAsset, assetSize, root ) )
		{
			const json::Value* pWorld = root.Find( "World" );
			if ( pWorld == nullptr )
			{
				return false;
			}

			for ( const json::Value& elem : *pWorld )
			{
				if ( const json::Value* pObject = elem.Find( "Object" ) )
				{
					Owner<CGameObject*> newObject = GetGameObjectFactory().CreateGameObjectByClassName( ( *pObject )["Class"].AsString() );

					if ( newObject )
					{
						newObject->LoadProperty( *pObject );
						gameLogic.SpawnObject( newObject );
					}
				}
			}

			return true;
		}

		return false;
	}
}
