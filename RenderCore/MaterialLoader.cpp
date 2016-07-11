#include "stdafx.h"

#include "IRenderer.h"
#include "Material.h"
#include "MaterialLoader.h"
#include "MaterialSystem.h"
#include "../Engine/KeyValueReader.h"
#include "../Shared/Util.h"

#include <vector>

namespace
{
	constexpr TCHAR* MATERIAL_SCRIPT_FILE_NAME = _T( "../Script/Materials.txt" );

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
	bool SetShader( IRenderer* pRenderer, const SHADER_TYPE type, const String& shaderName );
	bool SetSamplerState( IRenderer* pRenderer, const SHADER_TYPE type, const String& samplerName );
	bool SetDepthStencilState( IRenderer* pRenderer, const String& dsStateName );
	bool SetRasterizerState( IRenderer* pRenderer, const String& rsStateName );
};

bool CScriptedMaterial::SetShader( IRenderer* pRenderer, const SHADER_TYPE type, const String& shaderName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_pShaders[type] = pRenderer->SearchShaderByName( shaderName.c_str( ) );
	return m_pShaders[type] ? true : false;
}

bool CScriptedMaterial::SetSamplerState( IRenderer* pRenderer, const SHADER_TYPE type, const String& samplerName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_pSamplerState[type] = pRenderer->CreateSamplerState( samplerName );
	return m_pSamplerState[type] ? true : false;
}

bool CScriptedMaterial::SetDepthStencilState( IRenderer* pRenderer, const String& dsStateName )
{
	m_pDepthStencilState = pRenderer->CreateDepthStencilState( dsStateName );
	return m_pDepthStencilState ? true : false;
}

bool CScriptedMaterial::SetRasterizerState( IRenderer* pRenderer, const String& rsStateName )
{
	m_pRasterizerState = pRenderer->CreateRenderState( rsStateName );
	return m_pRasterizerState ? true : false;
}

bool CMaterialLoader::LoadMaterials( IRenderer* pRenderer )
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
		CreateScriptedMaterial( pRenderer, material );
	}

	return true;
}

bool CMaterialLoader::CreateScriptedMaterial( IRenderer* pRenderer, std::shared_ptr<KeyValue> pMaterial )
{
	if ( pMaterial == nullptr )
	{
		DebugWarning( "CreateScriptedMaterial Fail!!! - pMaterial is nullptr\n" );
		return false;
	}

	if ( pRenderer == nullptr )
	{
		DebugWarning( "CreateScriptedMaterial Fail!!! - pRenderer is nullptr\n" );
		return false;
	}

	std::shared_ptr<CScriptedMaterial> newMaterial = std::make_shared<CScriptedMaterial>( );
	newMaterial->Init( pRenderer );
	MaterialSystem::GetInstance( )->RegisterMaterial( pMaterial->GetKey( ).c_str(), newMaterial );

	for ( auto property = pMaterial->GetChild( ); property != nullptr; property = property->GetNext( ) )
	{
		if ( property->GetKey() == _T( "Shader" ) )
		{
			for ( auto shader = property->GetChild( ); shader != nullptr; shader = shader->GetNext() )
			{
				ON_FAIL_RETURN( newMaterial->SetShader( pRenderer, TranslateShaderType( shader->GetKey() ), shader->GetString() ) );
			}
		}
		else if ( property->GetKey( ) == _T( "RS_State" ) )
		{
			ON_FAIL_RETURN( newMaterial->SetRasterizerState( pRenderer, property->GetString( ) ) );
		}
		else if ( property->GetKey( ) == _T( "DS_State" ) )
		{
			ON_FAIL_RETURN( newMaterial->SetDepthStencilState( pRenderer, property->GetString( ) ) );
		}
		else if ( property->GetKey( ) == _T( "Sampler" ) )
		{
			for ( auto sampler = property->GetChild( ); sampler != nullptr; sampler = sampler->GetNext( ) )
			{
				ON_FAIL_RETURN( newMaterial->SetSamplerState( pRenderer, TranslateShaderType( sampler->GetKey( ) ), sampler->GetString( ) ) );
			}
		}
	}

	return true;
}

std::unique_ptr<CMaterialLoader> CreateMaterialLoader( )
{
	return std::make_unique<CMaterialLoader>( );
}