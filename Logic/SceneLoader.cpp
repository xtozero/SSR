#include "stdafx.h"
#include "SceneLoader.h"
#include <tchar.h>

#include "..\Engine\KeyValueReader.h"

void CSceneLoader::LoadSceneFromFile( const String& fileName )
{
	CKeyValueReader scene;

	auto pKeyValue = scene.LoadKeyValueFromFile( fileName );

	if ( pKeyValue )
	{
		auto findedKey = pKeyValue->FindKeyValue( _T( "Test" ) );

		DebugMsg( "test find : %ls, %ls\n", findedKey->GetKey( ).c_str( ), findedKey->GetString( ).c_str( ) );
	}
}

CSceneLoader::CSceneLoader( )
{
}

CSceneLoader::~CSceneLoader( )
{
}
