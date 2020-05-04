#include "stdafx.h"
#include "Render/MaterialManager.h"

#include "common.h"
#include "DataStructure/KeyValueReader.h"
#include "Render/IRenderer.h"
#include "Util.h"

#include <vector>

namespace
{
	constexpr TCHAR* MATERIAL_SCRIPT_FILE_NAME = _T( "./Scripts/Materials.txt" );

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

void CMaterialManager::OnDeviceLost( )
{
	m_materials.clear( );
	m_freeMaterial = nullptr;
	m_matLUT.clear( );
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

Material CMaterialManager::SearchMaterialByName( const TCHAR* pName )
{
	auto found = m_matLUT.find( pName );

	if ( found != m_matLUT.end( ) )
	{
		return found->second;
	}

	return INVALID_MATERIAL;
}

const CMaterial& CMaterialManager::GetConcrete( Material handle )
{
	assert( m_materials.size( ) > handle );
	return m_materials[handle];
}

void CMaterialManager::CreateScriptedMaterial( IRenderer& renderer, const KeyValue& keyValue )
{
	const TCHAR* materialName = keyValue.GetKey( ).c_str( );
	if ( SearchMaterialByName( materialName ) != INVALID_MATERIAL )
	{
		return;
	}

	CMaterial* pNewMat = nullptr;
	if ( m_freeMaterial == nullptr )
	{
		m_materials.emplace_back( );
		pNewMat = &m_materials.back( );
	}
	else
	{
		PopFrontInPlaceList( &m_freeMaterial, &pNewMat );
	}

	bool success = true;
	if ( const KeyValue* pShader = keyValue.Find( _T( "Shader" ) ) )
	{
		for ( auto shader = pShader->GetChild( ); shader != nullptr; shader = shader->GetNext( ) )
		{
			success = success && pNewMat->SetShader( renderer, TranslateShaderType( shader->GetKey( ) ), shader->GetValue( ) );
		}
	}

	if ( const KeyValue* pRsState = keyValue.Find( _T( "RS_State" ) ) )
	{
		success = success && pNewMat->SetRasterizerState( renderer, pRsState->GetValue( ) );
	}
	
	if ( const KeyValue* pDsState = keyValue.Find( _T( "DS_State" ) ) )
	{
		success = success && pNewMat->SetDepthStencilState( renderer, pDsState->GetValue( ) );
	}
	
	if ( const KeyValue* pSamplers = keyValue.Find( _T( "Sampler" ) ) )
	{
		for ( auto sampler = pSamplers->GetChild( ); sampler != nullptr; sampler = sampler->GetNext( ) )
		{
			success = success && pNewMat->SetSamplerState( renderer, TranslateShaderType( sampler->GetKey( ) ), sampler->GetValue( ) );
		}
	}
	
	if ( const KeyValue* pBlend = keyValue.Find( _T( "Blend" ) ) )
	{
		success = success && pNewMat->SetBlendState( renderer, pBlend->GetValue( ) );
	}

	if ( success )
	{
		m_matLUT.emplace( materialName, static_cast<Material>( pNewMat - &m_materials.front( ) ) );
	}
	else
	{
		PushFrontInPlaceList( &m_freeMaterial, pNewMat );
	}
}
