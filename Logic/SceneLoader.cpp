#include "stdafx.h"
#include "SceneLoader.h"
#include <tchar.h>
#include "../Shared/Util.h"

#define CHECK_VALID_ITERATOR( iter, container ) \
		if ( iter == container.end() ) \
		{ \
			DebugMsg( "SceneLoad - SetModel Fail!!!!!" ); \
			return; \
		} \

std::shared_ptr<KeyValueGroup> CSceneLoader::LoadSceneFromFile( const String& fileName, std::vector<std::shared_ptr<CGameObject>>& objectList )
{
	CKeyValueReader scene;

	auto pKeyValue = scene.LoadKeyValueFromFile( fileName );

	if ( pKeyValue )
	{
		for ( auto findedKey = pKeyValue->FindKeyValue( _T( "Scene" ) ); findedKey != nullptr; ++findedKey )
		{
			DebugMsg( "%ls, %ls\n", findedKey->GetKey( ).c_str( ), findedKey->GetString( ).c_str( ) );
		}

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
	auto curObject = objectList.begin();

	for ( auto findedKey = keyValue->FindKeyValue( _T( "Scene" ) ); findedKey != nullptr; ++findedKey )
	{
		if ( findedKey->GetKey( ) == String( _T( "Object" ) ) )
		{
			objectList.push_back( std::make_shared<CGameObject>( ) );

			curObject = ( objectList.end( ) - 1 );
		}
		else if ( findedKey->GetKey( ) == String( _T( "Model" ) ) )
		{
			CHECK_VALID_ITERATOR( curObject, objectList );

			curObject->get( )->LoadModelMesh( findedKey->GetString( ).c_str( ) );
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
	}
}