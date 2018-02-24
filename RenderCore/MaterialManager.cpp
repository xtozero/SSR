#include "stdafx.h"

#include "CommonRenderer/IRenderer.h"
#include "Material.h"
#include "MaterialManager.h"
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

	m_hShaders[type] = renderer.FindGraphicsShaderByName( shaderName.c_str( ) );
	return ( m_hShaders[type] != RE_HANDLE_TYPE::INVALID_HANDLE ) ? true : false;
}

bool CScriptedMaterial::SetSamplerState( IRenderer& renderer, const SHADER_TYPE type, const String& samplerName )
{
	if ( type >= SHADER_TYPE::MAX_SHADER || type < SHADER_TYPE::VS )
	{
		return false;
	}

	m_hSamplerState[type] = renderer.CreateSamplerState( samplerName );
	return ( m_hSamplerState[type] != RE_HANDLE_TYPE::INVALID_HANDLE ) ? true : false;
}

bool CScriptedMaterial::SetDepthStencilState( IRenderer& renderer, const String& dsStateName )
{
	m_hDepthStencilState = renderer.CreateDepthStencilState( dsStateName );
	return ( m_hDepthStencilState != RE_HANDLE_TYPE::INVALID_HANDLE ) ? true : false;
}

bool CScriptedMaterial::SetRasterizerState( IRenderer& renderer, const String& rsStateName )
{
	m_hRasterizerState = renderer.CreateRasterizerState( rsStateName );
	return ( m_hRasterizerState != RE_HANDLE_TYPE::INVALID_HANDLE ) ? true : false;
}

bool CScriptedMaterial::SetBlendState( IRenderer & renderer, const String& blendStateName )
{
	m_hBlendState = renderer.CreateBlendState( blendStateName );
	return ( m_hBlendState != RE_HANDLE_TYPE::INVALID_HANDLE ) ? true : false;
}

void CMaterialManager::OnDeviceLost( )
{
	m_materials.clear( );
}

bool CMaterialManager::LoadMaterials( IRenderer& renderer )
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

void CMaterialManager::RegisterMaterial( const TCHAR* pName, std::unique_ptr<IMaterial> pMaterial )
{
	if ( pName && pMaterial )
	{
		auto found = m_materials.find( pName );

		if ( found == m_materials.end( ) )
		{
			m_materials.emplace( pName, std::move( pMaterial ) );
		}
	}
}

IMaterial* CMaterialManager::SearchMaterialByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	auto found = m_materials.find( pName );

	if ( found != m_materials.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

bool CMaterialManager::CreateScriptedMaterial( IRenderer& renderer, const KeyValue& keyValue )
{
	const TCHAR* materialName = keyValue.GetKey( ).c_str( );
	if ( SearchMaterialByName( materialName ) != nullptr )
	{
		return false;
	}

	std::unique_ptr<CScriptedMaterial> newMaterial = std::make_unique<CScriptedMaterial>();
	newMaterial->Init( renderer );
	CScriptedMaterial* pMaterial = newMaterial.get( );
	m_materials.emplace( materialName, std::move( newMaterial ) );

	if ( const KeyValue* pShader = keyValue.Find( _T( "Shader" ) ) )
	{
		for ( auto shader = pShader->GetChild( ); shader != nullptr; shader = shader->GetNext( ) )
		{
			if ( !pMaterial->SetShader( renderer, TranslateShaderType( shader->GetKey( ) ), shader->GetValue( ) ) )
			{
				return false;
			}
		}
	}

	if ( const KeyValue* pRsState = keyValue.Find( _T( "RS_State" ) ) )
	{
		if ( !pMaterial->SetRasterizerState( renderer, pRsState->GetValue( ) ) )
		{
			return false;
		}
	}
	
	if ( const KeyValue* pDsState = keyValue.Find( _T( "DS_State" ) ) )
	{
		if ( !pMaterial->SetDepthStencilState( renderer, pDsState->GetValue( ) ) )
		{
			return false;
		}
	}
	
	if ( const KeyValue* pSamplers = keyValue.Find( _T( "Sampler" ) ) )
	{
		for ( auto sampler = pSamplers->GetChild( ); sampler != nullptr; sampler = sampler->GetNext( ) )
		{
			if ( !pMaterial->SetSamplerState( renderer, TranslateShaderType( sampler->GetKey( ) ), sampler->GetValue( ) ) )
			{
				return false;
			}
		}
	}
	
	if ( const KeyValue* pBlend = keyValue.Find( _T( "Blend" ) ) )
	{
		if ( !pMaterial->SetBlendState( renderer, pBlend->GetValue( ) ) )
		{
			return false;
		}
	}

	return true;
}
