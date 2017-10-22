#include "stdafx.h"
#include "Camera.h"
#include "ConstantBufferDefine.h"
#include "LightManager.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../RenderCore/CommonRenderer/IBuffer.h"
#include "../RenderCore/CommonRenderer/IMaterial.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"
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

	BUFFER_TRAIT trait = { sizeof( ShaderLightTrait ),
							1,
							RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
							RESOURCE_TYPE::CONSTANT_BUFFER,
							0,
							nullptr,
							0,
							0 };

	m_lightBuffer = renderer.CreateBuffer( trait );
	return m_lightBuffer ? true : false;
}

void CLightManager::UpdateToRenderer( IRenderer& renderer, const CCamera& camera )
{
	SetCameraPosition( camera.GetOrigin( ) );

	if ( m_needUpdateToRenderer )
	{
		void* lights = m_lightBuffer->LockBuffer( );

		if ( lights )
		{
			memcpy( lights, &m_shaderLightProperty, sizeof( ShaderLightTrait ) );
			m_lightBuffer->UnLockBuffer( );
			m_lightBuffer->SetPSBuffer( PS_CONSTANT_BUFFER::LIGHT );
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
	RegisterEnumString( );
}

void CLightManager::LoadPropertyFromScript( )
{
	CKeyValueReader keyValueReader;

	std::unique_ptr<KeyValue> pKeyValues = keyValueReader.LoadKeyValueFromFile( LIGHT_PROPERTY_FILE_NAME );

	if ( pKeyValues )
	{
		LoadLightProperty( *pKeyValues.get() );
	}
}

void CLightManager::LoadLightProperty( const KeyValue& keyValue )
{
	if ( const KeyValue* pGlobalAmbient = keyValue.Find( _T( "globalAmbient" ) ) )
	{
		KEYVALUE_VALUE_ASSERT( pGlobalAmbient->GetValue( ), 4 );
		Stringstream stream( pGlobalAmbient->GetValue( ) );
		CXMFLOAT4 ambientColor( 0.f, 0.f, 0.f, 0.f );

		stream >> ambientColor.x >> ambientColor.y >> ambientColor.z >> ambientColor.w;
		SetGlobalAmbient( ambientColor );
	}

	for ( auto desc = keyValue.Find( _T( "Light" ) ); desc != nullptr; desc = desc->GetNext( ) )
	{
		LightTrait trait;

		if ( const KeyValue* pType = desc->Find( _T( "type" ) ) )
		{
			trait.m_type = static_cast<LIGHT_TYPE>( GetEnumStringMap( ).GetEnum( pType->GetValue( ), static_cast<int>( LIGHT_TYPE::NONE ) ) );
		}
		
		if ( const KeyValue* pOnOff = desc->Find( _T( "onOff" ) ) )
		{
			trait.m_isOn = pOnOff->GetValue<int>( ) == 1;
		}
		
		if ( const KeyValue* pTheta = desc->Find( _T( "theta" ) ) )
		{
			trait.m_theta = pTheta->GetValue<float>( );
		}
		
		if ( const KeyValue* pPhi = desc->Find( _T( "phi" ) ) )
		{
			trait.m_phi = pPhi->GetValue<float>( );
		}
		
		if ( const KeyValue* pDirection = desc->Find( _T( "direction" ) ) )
		{
			Stringstream stream( pDirection->GetValue( ) );
			stream >> trait.m_direction.x >> trait.m_direction.y >> trait.m_direction.z;
		}
		
		if ( const KeyValue* pRange = desc->Find( _T( "range" ) ) )
		{
			trait.m_range = pRange->GetValue<float>( );
		}
		
		if ( const KeyValue* pFallOff = desc->Find( _T( "fallOff" ) ) )
		{
			trait.m_fallOff = pFallOff->GetValue<float>( );
		}
		
		if ( const KeyValue* pAttenuation = desc->Find( _T( "attenuation" ) ) )
		{
			Stringstream stream( pAttenuation->GetValue( ) );
			stream >> trait.m_attenuation.x >> trait.m_attenuation.y >> trait.m_attenuation.z;
		}
		
		if ( const KeyValue* pPosition = desc->Find( _T( "position" ) ) )
		{
			Stringstream stream( pPosition->GetValue( ) );
			stream >> trait.m_position.x >> trait.m_position.y >> trait.m_position.z;
		}
		
		if ( const KeyValue* pDiffuse = desc->Find( _T( "diffuse" ) ) )
		{
			Stringstream stream( pDiffuse->GetValue( ) );
			stream >> trait.m_diffuse.x >> trait.m_diffuse.y >> trait.m_diffuse.z >> trait.m_diffuse.w;
		}
		
		if ( const KeyValue* pSpecular = desc->Find( _T( "specular" ) ) )
		{
			Stringstream stream( pSpecular->GetValue( ) );
			stream >> trait.m_specular.x >> trait.m_specular.y >> trait.m_specular.z >> trait.m_specular.w;
		}

		PushLightTrait( trait );
	}
}
