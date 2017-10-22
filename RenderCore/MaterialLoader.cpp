#include "stdafx.h"

#include "CommonRenderer/IRenderer.h"
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
	bool SetShader( IRenderer& renderer, const SHADER_TYPE type, const String& shaderName );
	bool SetSamplerState( IRenderer& renderer, const SHADER_TYPE type, const String& samplerName );
	bool SetDepthStencilState( IRenderer& renderer, const String& dsStateName );
	bool SetRasterizerState( IRenderer& renderer, const String& rsStateName );
	bool SetBlendState( IRenderer& renderer, const String& blendStateName );
};

bool CScriptedMaterial::SetShader( IRenderer& renderer, const SHADER_TYPE type, const String& shaderName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_pShaders[type] = renderer.SearchShaderByName( shaderName.c_str( ) );
	return m_pShaders[type] ? true : false;
}

bool CScriptedMaterial::SetSamplerState( IRenderer& renderer, const SHADER_TYPE type, const String& samplerName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_pSamplerState[type] = renderer.CreateSamplerState( samplerName );
	return m_pSamplerState[type] ? true : false;
}

bool CScriptedMaterial::SetDepthStencilState( IRenderer& renderer, const String& dsStateName )
{
	m_pDepthStencilState = renderer.CreateDepthStencilState( dsStateName );
	return m_pDepthStencilState ? true : false;
}

bool CScriptedMaterial::SetRasterizerState( IRenderer& renderer, const String& rsStateName )
{
	m_pRasterizerState = renderer.CreateRenderState( rsStateName );
	return m_pRasterizerState ? true : false;
}

bool CScriptedMaterial::SetBlendState( IRenderer & renderer, const String& blendStateName )
{
	m_pBlendState = renderer.CreateBlendState( blendStateName );
	return m_pBlendState ? true : false;
}

bool CMaterialLoader::LoadMaterials( IRenderer& renderer )
{
	CKeyValueReader keyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = keyValueReader.LoadKeyValueFromFile( MATERIAL_SCRIPT_FILE_NAME );

	if ( pKeyValues == nullptr )
	{
		DebugWarning( "Script file not exiest!!!\n" );
		return false;
	}

	const KeyValue* material = pKeyValues->GetChild( );

	if ( material == nullptr )
	{
		DebugWarning( "There is No Entering Point!!!\n" );
		return false;
	}

	for ( ; material != nullptr; material = material->GetNext( ) )
	{
		CreateScriptedMaterial( renderer, *material );
	}

	return true;
}

bool CMaterialLoader::CreateScriptedMaterial( IRenderer& renderer, const KeyValue& keyValue )
{
	std::unique_ptr<CScriptedMaterial> newMaterial = std::make_unique<CScriptedMaterial>();
	newMaterial->Init( renderer );
	CScriptedMaterial* pMaterial = newMaterial.get( );
	MaterialSystem::GetInstance( )->RegisterMaterial( keyValue.GetKey( ).c_str(), std::move( newMaterial ) );

	if ( const KeyValue* pShader = keyValue.Find( _T( "Shader" ) ) )
	{
		for ( auto shader = pShader->GetChild( ); shader != nullptr; shader = shader->GetNext( ) )
		{
			ON_FAIL_RETURN( pMaterial->SetShader( renderer, TranslateShaderType( shader->GetKey( ) ), shader->GetValue( ) ) );
		}
	}

	if ( const KeyValue* pRsState = keyValue.Find( _T( "RS_State" ) ) )
	{
		ON_FAIL_RETURN( pMaterial->SetRasterizerState( renderer, pRsState->GetValue( ) ) );
	}
	
	if ( const KeyValue* pDsState = keyValue.Find( _T( "DS_State" ) ) )
	{
		ON_FAIL_RETURN( pMaterial->SetDepthStencilState( renderer, pDsState->GetValue( ) ) );
	}
	
	if ( const KeyValue* pSamplers = keyValue.Find( _T( "Sampler" ) ) )
	{
		for ( auto sampler = pSamplers->GetChild( ); sampler != nullptr; sampler = sampler->GetNext( ) )
		{
			ON_FAIL_RETURN( pMaterial->SetSamplerState( renderer, TranslateShaderType( sampler->GetKey( ) ), sampler->GetValue( ) ) );
		}
	}
	
	if ( const KeyValue* pBlend = keyValue.Find( _T( "Blend" ) ) )
	{
		ON_FAIL_RETURN( pMaterial->SetBlendState( renderer, pBlend->GetValue( ) ) )
	}

	return true;
}

std::unique_ptr<CMaterialLoader> CreateMaterialLoader( )
{
	return std::make_unique<CMaterialLoader>( );
}