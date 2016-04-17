#include "stdafx.h"
#include "ShaderListScriptLoader.h"

#include <cstdlib>
#include "../Engine/KeyValueReader.h"
#include "IRenderer.h"

namespace
{
	const TCHAR* SHADER_LIST_SCRIPT_FILE_NAME = _T( "../Script/ShaderList.txt" );
}

bool CShaderListScriptLoader::LoadShadersFromScript( IRenderer* const pRenderer )
{
	CKeyValueReader scrpitReader;
	auto pKeyValue = scrpitReader.LoadKeyValueFromFile( SHADER_LIST_SCRIPT_FILE_NAME );

	if ( pKeyValue )
	{
		auto findedKey = pKeyValue->FindKeyValue( _T( "ShaderList" ) );
		++findedKey;

		for ( ; findedKey != nullptr; ++findedKey )
		{
			ON_FAIL_RETURN( LoadShader( pRenderer, findedKey->GetKey( ), findedKey->GetString( ) ) );
		}

		return true;
	}

	return false;
}

bool CShaderListScriptLoader::LoadShader( IRenderer* const pRenderer, const String& filePath, const String& profile )
{
	if ( pRenderer == nullptr )
	{
		return false;
	}

	const String& shaderType = profile.substr( 0, 2 );
	char mbsProfile[256];
	size_t converted;
	wcstombs_s( &converted, mbsProfile, profile.c_str( ), 256 );

	if ( shaderType.compare( _T( "vs" ) ) == 0 )
	{
		return pRenderer->CreateVertexShader( filePath.c_str( ), mbsProfile ) != nullptr;
	}
	else if ( shaderType.compare( _T( "ps" ) ) == 0 )
	{
		return pRenderer->CreatePixelShader( filePath.c_str( ), mbsProfile ) != nullptr;
	}

	return false;
}

CShaderListScriptLoader::CShaderListScriptLoader( )
{
}


CShaderListScriptLoader::~CShaderListScriptLoader( )
{
}
