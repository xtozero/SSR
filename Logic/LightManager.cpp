#include "stdafx.h"
#include "Camera.h"
#include "LightManager.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/IMaterial.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/ConstantBufferDefine.h"
#include "../Shared/Util.h"

using namespace DirectX;

namespace
{
	constexpr TCHAR* CONST_BUFFER_NAME = _T( "Lights" );
	constexpr TCHAR* LIGHT_PROPERTY_FILE_NAME = _T( "../Script/SceneLight.txt" );

	void RegisterEnumString()
	{
		//Register enum string
		REGISTER_ENUM_STRING( LIGHT_TYPE::NONE );
		REGISTER_ENUM_STRING( LIGHT_TYPE::AMBIENT_LIGHT );
		REGISTER_ENUM_STRING( LIGHT_TYPE::DIRECTINAL_LIGHT );
		REGISTER_ENUM_STRING( LIGHT_TYPE::POINT_LIGHT );
		REGISTER_ENUM_STRING( LIGHT_TYPE::SPOT_LIGHT );
	}

	void AmbientColorHandler( CLightManager* owner, const String&, const KeyValue* keyValue )
	{
		KEYVALUE_VALUE_ASSERT( keyValue->GetValue( ), 4 );
		Stringstream stream( keyValue->GetValue( ) );
		CXMFLOAT4 ambientColor( 0.f, 0.f, 0.f, 0.f );

		stream >> ambientColor.x >> ambientColor.y >> ambientColor.z >> ambientColor.w;
		owner->SetGlobalAmbient( ambientColor );
	}

	void LightHandler( CLightManager* owner, const String&, const KeyValue* keyValue )
	{
		LightTrait trait;

		for ( auto key = keyValue->GetChild( ); key != nullptr; key = key->GetNext() )
		{
			if ( key->GetKey( ) == _T( "type" ) )
			{
				trait.m_type = static_cast<LIGHT_TYPE>( GetEnumStringMap().GetEnum( key->GetValue( ), static_cast<int>( LIGHT_TYPE::NONE ) ) );
			}
			else if ( key->GetKey( ) == _T( "onOff" ) )
			{
				trait.m_isOn = key->GetValue<int>() == 1;
			}
			else if ( key->GetKey( ) == _T( "theta" ) )
			{
				trait.m_theta = key->GetValue<float>();
			}
			else if ( key->GetKey( ) == _T( "phi" ) )
			{
				trait.m_phi = key->GetValue<float>();
			}
			else if ( key->GetKey( ) == _T( "direction" ) )
			{
				Stringstream stream( key->GetValue( ) );
				stream >> trait.m_direction.x >> trait.m_direction.y >> trait.m_direction.z;
			}
			else if ( key->GetKey( ) == _T( "range" ) )
			{
				trait.m_range = key->GetValue<float>();
			}
			else if ( key->GetKey( ) == _T( "fallOff" ) )
			{
				trait.m_fallOff = key->GetValue<float>();
			}
			else if ( key->GetKey( ) == _T( "attenuation" ) )
			{
				Stringstream stream( key->GetValue( ) );
				stream >> trait.m_attenuation.x >> trait.m_attenuation.y >> trait.m_attenuation.z;
			}
			else if ( key->GetKey( ) == _T( "position" ) )
			{
				Stringstream stream( key->GetValue( ) );
				stream >> trait.m_position.x >> trait.m_position.y >> trait.m_position.z;
			}
			else if ( key->GetKey( ) == _T( "m_diffuse" ) )
			{
				Stringstream stream( key->GetValue( ) );
				stream >> trait.m_diffuse.x >> trait.m_diffuse.y >> trait.m_diffuse.z >> trait.m_diffuse.w;
			}
			else if ( key->GetKey( ) == _T( "m_specular" ) )
			{
				Stringstream stream( key->GetValue( ) );
				stream >> trait.m_specular.x >> trait.m_specular.y >> trait.m_specular.z >> trait.m_specular.w;
			}
		}

		owner->PushLightTrait( trait );
	}
}

bool CLightManager::Initialize( IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& objectList )
{
	// 속성과 클래스를 연결
	for ( int i = 0; i < MAX_LIGHTS; ++i )
	{
		std::unique_ptr<CLight> newLight = std::make_unique<CLight>( );
		if ( newLight )
		{
			newLight->RegisterProperty( &m_shaderLightProperty.m_properties[i] );
			m_lights[i] = newLight.get( );
			objectList.emplace_back( std::move( newLight ) );
		}
	}

	LoadPropertyFromScript( );

	return renderer.CreateConstantBuffer(
		CONST_BUFFER_NAME,
		sizeof( ShaderLightTrait ),
		1,
		nullptr ) ? true : false;
}

void CLightManager::UpdateToRenderer( IRenderer& renderer, const CCamera& camera )
{
	SetCameraPosition( camera.GetOrigin( ) );

	if ( m_needUpdateToRenderer )
	{
		void* lights = renderer.MapConstantBuffer( CONST_BUFFER_NAME );

		if ( lights )
		{
			memcpy( lights, &m_shaderLightProperty, sizeof( ShaderLightTrait ) );
			renderer.UnMapConstantBuffer( CONST_BUFFER_NAME );
			renderer.SetConstantBuffer( CONST_BUFFER_NAME, static_cast<int>(PS_CONSTANT_BUFFER::LIGHT), SHADER_TYPE::PS );
		}

		m_needUpdateToRenderer = false;
	}
}

void CLightManager::SetCameraPosition( const CXMFLOAT3& cameraPos )
{
	if ( m_shaderLightProperty.m_cameraPos != cameraPos )
	{
		m_needUpdateToRenderer = true;
		m_shaderLightProperty.m_cameraPos = cameraPos;
	}
}

void CLightManager::SetGlobalAmbient( const CXMFLOAT4& globalAmbient )
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

CXMFLOAT4X4 CLightManager::GetPrimaryLightViewMatrix( )
{
	if ( m_lights.size() > m_primaryLight )
	{
		if ( m_lights[m_primaryLight] && m_lights[m_primaryLight]->IsOn( ) )
		{
			return m_lights[m_primaryLight]->GetViewMatrix( );
		}
	}
	
	return XMMatrixIdentity( );
}

CXMFLOAT4X4 CLightManager::GerPrimaryLightProjectionMatrix( )
{
	return XMMatrixPerspectiveFovLH( XM_PI / 4.f, 1.f, 1.f, 1500.f );
}

CLightManager::CLightManager( ) :
	m_needUpdateToRenderer( false ),
	m_primaryLight( 0 )
{
	RegisterHandler( _T( "globalAmbient" ), AmbientColorHandler );
	RegisterHandler( _T( "Light" ), LightHandler );

	RegisterEnumString( );
}

void CLightManager::LoadPropertyFromScript( )
{
	CKeyValueReader keyValueReader;

	std::unique_ptr<KeyValueGroupImpl> pKeyValues = keyValueReader.LoadKeyValueFromFile( LIGHT_PROPERTY_FILE_NAME );

	if ( pKeyValues )
	{
		LoadLightProperty( pKeyValues.get() );
	}
}

void CLightManager::LoadLightProperty( const KeyValueGroup* pKeyValues )
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
