#include "stdafx.h"
#include "Camera.h"
#include "LightManager.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/IMaterial.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/ConstantBufferDefine.h"
#include "../Shared/Util.h"

extern IRenderer* gRenderer;

namespace
{
	constexpr TCHAR* CONST_BUFFER_NAME = _T( "Lights" );
	constexpr TCHAR* LIGHT_PROPERTY_FILE_NAME = _T( "../Script/SceneLight.txt" );

	void AmbientColorHandler( CLightManager* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
	{
		KEYVALUE_VALUE_ASSERT( keyValue->GetString( ), 4 );
		Stringstream stream( keyValue->GetString( ) );
		D3DXCOLOR ambientColor;

		stream >> ambientColor.r >> ambientColor.g >> ambientColor.b >> ambientColor.a;
		owner->SetGlobalAmbient( ambientColor );
	}

	void LightHandler( CLightManager* owner, const String&, const std::shared_ptr<KeyValue>& keyValue )
	{
		LightTrait trait;

		for ( auto key = keyValue->GetChild( ); key != nullptr; key = key->GetNext() )
		{
			if ( key->GetKey( ) == _T( "type" ) )
			{
				trait.m_type = static_cast<LIGHT_TYPE>( GetEnumStringMap().GetEnum( key->GetString( ), static_cast<int>( LIGHT_TYPE::NONE ) ) );
			}
			else if ( key->GetKey( ) == _T( "onOff" ) )
			{
				trait.m_isOn = key->GetInt() == 1;
			}
			else if ( key->GetKey( ) == _T( "theta" ) )
			{
				trait.m_theta = key->GetFloat();
			}
			else if ( key->GetKey( ) == _T( "phi" ) )
			{
				trait.m_phi = key->GetFloat();
			}
			else if ( key->GetKey( ) == _T( "direction" ) )
			{
				Stringstream stream( key->GetString( ) );
				stream >> trait.m_direction.x >> trait.m_direction.y >> trait.m_direction.z;
			}
			else if ( key->GetKey( ) == _T( "range" ) )
			{
				trait.m_range = key->GetFloat();
			}
			else if ( key->GetKey( ) == _T( "fallOff" ) )
			{
				trait.m_fallOff = key->GetFloat();
			}
			else if ( key->GetKey( ) == _T( "attenuation" ) )
			{
				Stringstream stream( key->GetString( ) );
				stream >> trait.m_attenuation.x >> trait.m_attenuation.y >> trait.m_attenuation.z;
			}
			else if ( key->GetKey( ) == _T( "position" ) )
			{
				Stringstream stream( key->GetString( ) );
				stream >> trait.m_position.x >> trait.m_position.y >> trait.m_position.z;
			}
			else if ( key->GetKey( ) == _T( "m_diffuse" ) )
			{
				Stringstream stream( key->GetString( ) );
				stream >> trait.m_diffuse.r >> trait.m_diffuse.g >> trait.m_diffuse.b >> trait.m_diffuse.a;
			}
			else if ( key->GetKey( ) == _T( "m_specular" ) )
			{
				Stringstream stream( key->GetString( ) );
				stream >> trait.m_specular.r >> trait.m_specular.g >> trait.m_specular.b >> trait.m_specular.a;
			}
		}

		owner->PushLightTrait( trait );
	}
}

bool CLightManager::Initialize( std::vector<std::shared_ptr<CGameObject>>& objectList )
{
	// 속성과 클래스를 연결
	for ( int i = 0; i < MAX_LIGHTS; ++i )
	{
		m_lights[i] = std::make_shared<CLight>( );
		if ( m_lights[i] )
		{
			m_lights[i]->RegisterProperty( &m_shaderLightProperty.m_properties[i] );
			objectList.emplace_back( m_lights[i] );
		}
	}

	LoadPropertyFromScript( );

	return gRenderer->CreateConstantBuffer(
		CONST_BUFFER_NAME,
		sizeof( ShaderLightTrait ),
		1,
		nullptr ) ? true : false;
}

void CLightManager::UpdateToRenderer( const CCamera& camera )
{
	SetCameraPosition( camera.GetOrigin( ) );

	if ( m_needUpdateToRenderer )
	{
		void* lights = gRenderer->MapConstantBuffer( CONST_BUFFER_NAME );

		if ( lights )
		{
			memcpy( lights, &m_shaderLightProperty, sizeof( ShaderLightTrait ) );
			gRenderer->UnMapConstantBuffer( CONST_BUFFER_NAME );
			gRenderer->SetConstantBuffer( CONST_BUFFER_NAME, static_cast<int>(PS_CONSTANT_BUFFER::LIGHT), SHADER_TYPE::PS );
		}

		m_needUpdateToRenderer = false;
	}
}

void CLightManager::SetCameraPosition( const D3DXVECTOR3& cameraPos )
{
	if ( m_shaderLightProperty.m_cameraPos != cameraPos )
	{
		m_needUpdateToRenderer = true;
		m_shaderLightProperty.m_cameraPos = cameraPos;
	}
}

void CLightManager::SetGlobalAmbient( const D3DXCOLOR& globalAmbient )
{
	if ( m_shaderLightProperty.m_globalAmbient != globalAmbient )
	{
		m_needUpdateToRenderer = true;
		m_shaderLightProperty.m_globalAmbient = globalAmbient;
	}
}

void CLightManager::PushLightTrait( const LightTrait & trait )
{
	int idx = m_shaderLightProperty.m_curLights;
	::memcpy( &m_shaderLightProperty.m_properties[idx], &trait, sizeof( trait ) );
	++m_shaderLightProperty.m_curLights;
}

D3DXMATRIX CLightManager::GetPrimaryLightViewMatrix( )
{
	if ( m_lights.size() > m_primaryLight )
	{
		if ( m_lights[m_primaryLight] && m_lights[m_primaryLight]->IsOn( ) )
		{
			return m_lights[m_primaryLight]->GetViewMatrix( );
		}
	}
	
	D3DXMATRIX lightViewMatrix;
	D3DXMatrixIdentity( &lightViewMatrix );

	return lightViewMatrix;
}

D3DXMATRIX CLightManager::GerPrimaryLightProjectionMatrix( )
{
	D3DXMATRIX lightProjMatrix;
	D3DXMatrixPerspectiveFovLH( &lightProjMatrix, D3DX_PI / 4.0f, 1.f, 1.f, 1500.f );

	return lightProjMatrix;
}

CLightManager::CLightManager( ) :
	m_needUpdateToRenderer( false ),
	m_primaryLight( 0 )
{
	RegisterHandler( _T( "globalAmbient" ), AmbientColorHandler );
	RegisterHandler( _T( "Light" ), LightHandler );

	//Register enum string
	REGISTER_ENUM_STRING( LIGHT_TYPE::NONE );
	REGISTER_ENUM_STRING( LIGHT_TYPE::AMBIENT_LIGHT );
	REGISTER_ENUM_STRING( LIGHT_TYPE::DIRECTINAL_LIGHT );
	REGISTER_ENUM_STRING( LIGHT_TYPE::POINT_LIGHT );
	REGISTER_ENUM_STRING( LIGHT_TYPE::SPOT_LIGHT );
}

void CLightManager::LoadPropertyFromScript( )
{
	CKeyValueReader keyValueReader;

	auto pKeyValues = keyValueReader.LoadKeyValueFromFile( LIGHT_PROPERTY_FILE_NAME );

	if ( pKeyValues )
	{
		LoadLightProperty( pKeyValues );
	}
}

void CLightManager::LoadLightProperty( const std::shared_ptr<KeyValueGroup>& pKeyValues )
{
	if ( pKeyValues == nullptr )
	{
		return;
	}

	auto keyValue = pKeyValues->FindKeyValue( _T( "Lights" ) );

	if ( keyValue == nullptr )
	{
		DebugWarning( "Load Light Property Fail!!!\n" );
		return;
	}

	for ( auto desc = keyValue->GetChild( ); desc != nullptr; desc = desc->GetNext( ) )
	{
		Handle( desc->GetKey( ), desc );
	}
}
