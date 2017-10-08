#include "stdafx.h"
#include "ShaderListScriptLoader.h"

#include <cstdlib>
#include "../Engine/KeyValueReader.h"
#include "CommonRenderer/IRenderer.h"

namespace
{
	constexpr TCHAR* SHADER_LIST_SCRIPT_FILE_NAME = _T( "../Script/ShaderList.txt" );
}

bool CShaderListScriptLoader::LoadShadersFromScript( IRenderer& renderer )
{
	CKeyValueReader scrpitReader;
	std::unique_ptr<KeyValueGroupImpl> pKeyValues = scrpitReader.LoadKeyValueFromFile( SHADER_LIST_SCRIPT_FILE_NAME );

	if ( pKeyValues )
	{
		auto findedKey = pKeyValues->FindKeyValue( _T( "ShaderList" ) );
		++findedKey;

		for ( ; findedKey != nullptr; ++findedKey )
		{
			ON_FAIL_RETURN( LoadShader( renderer, findedKey->GetKey( ), findedKey->GetValue( ) ) );
		}

		return true;
	}

	return false;
}

bool CShaderListScriptLoader::LoadShader( IRenderer& renderer, const String& filePath, const String& profile )
{
	const String& shaderType = profile.substr( 0, 2 );
	char mbsProfile[256];
	size_t converted;
	wcstombs_s( &converted, mbsProfile, profile.c_str( ), 256 );

	if ( shaderType.compare( _T( "vs" ) ) == 0 )
	{
		return renderer.CreateVertexShader( filePath.c_str( ), mbsProfile ) != nullptr;
	}
	else if ( shaderType.compare( _T( "ps" ) ) == 0 )
	{
		return renderer.CreatePixelShader( filePath.c_str( ), mbsProfile ) != nullptr;
	}

	return false;
}

CShaderListScriptLoader::CShaderListScriptLoader( )
{
}


CShaderListScriptLoader::~CShaderListScriptLoader( )
{
}
