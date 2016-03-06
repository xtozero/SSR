#include "stdafx.h"

#include "IRenderer.h"
#include "ISampler.h"
#include "Material.h"
#include "MaterialLoader.h"
#include "MaterialSystem.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <vector>

extern IRenderer* g_pRenderer;

namespace
{
	const TCHAR* MATERIAL_SCRIPT_FILE_NAME = _T( "../Script/Materials.txt" );

	SHADER_TYPE TranslateShaderType( const String& shaderType )
	{
		if ( shaderType == _T( "VS" ) )
		{
			return SHADER_TYPE::VS;
		}
		else if ( shaderType == _T( "HS" ) )
		{
			return SHADER_TYPE::HS;
		}
		else if ( shaderType == _T( "DS" ) )
		{
			return SHADER_TYPE::DS;
		}
		else if ( shaderType == _T( "GS" ) )
		{
			return SHADER_TYPE::GS;
		}
		else if ( shaderType == _T( "PS" ) )
		{
			return SHADER_TYPE::PS;
		}
		else if ( shaderType == _T( "CS" ) )
		{
			return SHADER_TYPE::CS;
		}

		return SHADER_TYPE::VS;
	}
}

class CScriptedMaterial : public Material
{
public:
	bool SetShader( const SHADER_TYPE type, const String& shaderName );
	bool SetSamplerState( const SHADER_TYPE type, const String& samplerName );
	bool SetDepthStencilState( const String& dsStateName );
	bool SetRasterizerState( const String& rsStateName );
};

bool CScriptedMaterial::SetShader( const SHADER_TYPE type, const String& shaderName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_pShaders[type] = g_pRenderer->SearchShaderByName( shaderName.c_str( ) );
	return m_pShaders[type] ? true : false;
}

bool CScriptedMaterial::SetSamplerState( const SHADER_TYPE type, const String& samplerName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_pSamplerState[type] = g_pRenderer->CreateSamplerState( samplerName );
	return m_pSamplerState[type] ? true : false;
}

bool CScriptedMaterial::SetDepthStencilState( const String& dsStateName )
{
	m_pDepthStencilState = g_pRenderer->CreateDepthStencilState( dsStateName );
	return m_pDepthStencilState ? true : false;
}

bool CScriptedMaterial::SetRasterizerState( const String& rsStateName )
{
	m_pRenderState = g_pRenderer->CreateRenderState( rsStateName );
	return m_pRenderState ? true : false;
}

bool CMaterialLoader::LoadMaterials( )
{
	CKeyValueReader keyValueReader;

	std::shared_ptr<KeyValueGroup> pKeyValues = keyValueReader.LoadKeyValueFromFile( MATERIAL_SCRIPT_FILE_NAME );

	if ( pKeyValues == nullptr )
	{
		DebugWarning( "Script file not exiest!!!\n" );
		return false;
	}

	auto keyValue = pKeyValues->FindKeyValue( _T( "Materials" ) );

	if ( keyValue == nullptr )
	{
		DebugWarning( "There is No Entering Point!!!\n" );
		return false;
	}

	for ( auto material = keyValue->GetChild( ); material != nullptr; material = material->GetNext( ) )
	{
		CreateScriptedMaterial( material );
	}

	return true;
}

bool CMaterialLoader::CreateScriptedMaterial( std::shared_ptr<KeyValue> pMaterial )
{
	if ( pMaterial == nullptr )
	{
		DebugWarning( "CreateScriptedMaterial Fail!!!\n" );
		return false;
	}

	std::shared_ptr<CScriptedMaterial> newMaterial = std::make_shared<CScriptedMaterial>( );
	MaterialSystem::GetInstance( )->RegisterMaterial( pMaterial->GetString( ).c_str(), newMaterial );

	for ( auto property = pMaterial->GetChild( ); property != nullptr; property = property->GetNext( ) )
	{
		if ( property->GetKey() == _T( "Shader" ) )
		{
			for ( auto shader = property->GetChild( ); shader != nullptr; shader = shader->GetNext() )
			{
				ON_FAIL_RETURN( newMaterial->SetShader( TranslateShaderType( shader->GetKey() ), shader->GetString() ) );
			}
		}
		else if ( property->GetKey( ) == _T( "RS_State" ) )
		{
			ON_FAIL_RETURN( newMaterial->SetRasterizerState( property->GetString( ) ) );
		}
		else if ( property->GetKey( ) == _T( "DS_State" ) )
		{
			ON_FAIL_RETURN( newMaterial->SetDepthStencilState( property->GetString( ) ) );
		}
		else if ( property->GetKey( ) == _T( "Sampler" ) )
		{
			for ( auto sampler = property->GetChild( ); sampler != nullptr; sampler = sampler->GetNext( ) )
			{
				ON_FAIL_RETURN( newMaterial->SetSamplerState( TranslateShaderType( sampler->GetKey( ) ), sampler->GetString( ) ) );
			}
		}
	}

	return true;
}

std::unique_ptr<CMaterialLoader> CreateMaterialLoader( )
{
	return std::make_unique<CMaterialLoader>( );
}