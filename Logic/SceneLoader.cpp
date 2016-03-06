#include "stdafx.h"

#include <tchar.h>

#include "SceneLoader.h"
#include "../Shared/Util.h"
#include "GameObjectFactory.h"

#define CHECK_VALID_ITERATOR( iter, container ) \
if ( iter == container.end( ) ) \
		{ \
		DebugWarning( "> SceneLoader - SetModel Fail!!!!!" ); \
		return; \
		} \

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
		else if ( findedKey->GetKey( ) == String( _T( "Name" ) ) )
		{
			CHECK_VALID_ITERATOR( curObject, objectList );

			curObject->get( )->SetName( findedKey->GetString( ) );
		}
		else if ( findedKey->GetKey( ) == String( _T( "Model" ) ) )
		{
			CHECK_VALID_ITERATOR( curObject, objectList );

			curObject->get( )->SetModelMeshName( findedKey->GetString( ).c_str( ) );
		}
		else if ( findedKey->GetKey( ) == String( _T( "Position" ) ) )
		{
			CHECK_VALID_ITERATOR( curObject, objectList );

			std::vector<String> params;

			UTIL::Split( findedKey->GetString( ), params, _T( ' ' ) );

			if ( params.size( ) == 3 )
			{
				float x = static_cast<float>( _ttof( params[0].c_str( ) ) );
				float y = static_cast<float>( _ttof( params[1].c_str( ) ) );
				float z = static_cast<float>( _ttof( params[2].c_str( ) ) );

				curObject->get( )->SetPosition( x, y, z );
			}
		}
		else if ( findedKey->GetKey( ) == String( _T( "Scale" ) ) )
		{
			CHECK_VALID_ITERATOR( curObject, objectList );

			std::vector<String> params;

			UTIL::Split( findedKey->GetString( ), params, _T( ' ' ) );

			if ( params.size( ) == 3 )
			{
				float x = static_cast<float>( _ttof( params[0].c_str( ) ) );
				float y = static_cast<float>( _ttof( params[1].c_str( ) ) );
				float z = static_cast<float>( _ttof( params[2].c_str( ) ) );

				curObject->get( )->SetScale( x, y, z );
			}
		}
		else if ( findedKey->GetKey( ) == String( _T( "Material" ) ) )
		{
			CHECK_VALID_ITERATOR( curObject, objectList );

			curObject->get( )->SetMaterialName( findedKey->GetString( ).c_str( ) );
		}
	}

	if ( curObject != objectList.end( ) &&
		curObject->get( ) &&
		curObject->get( )->NeedInitialize( ) )
	{
		curObject->get( )->Initialize( );
	}
}