#include "stdafx.h"
#include "GameObject/LightManager.h"

#include "Core/GameLogic.h"
#include "DataStructure/EnumStringMap.h"
#include "DataStructure/KeyValueReader.h"
#include "GameObject/Camera.h"
#include "Render/IRenderer.h"
#include "Render/IRenderResourceManager.h"
#include "Scene/ConstantBufferDefine.h"
#include "Util.h"

#include <algorithm>
#include <cstddef>

using namespace DirectX;

namespace
{
	constexpr TCHAR* CONST_BUFFER_NAME = _T( "Lights" );
	constexpr TCHAR* LIGHT_PROPERTY_FILE_NAME = _T( "../Script/SceneLight.txt" );

	constexpr TCHAR* OREN_NAYAR_TEX_NAME = _T( "OrenNayarLookUP" );
	constexpr TCHAR* OREN_NAYAR_SNAPSHOT_NAME = _T( "DebugOrenNayarLookUP" );

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

bool CLightManager::Initialize( CGameLogic& gameLogic )
{
	IRenderer& renderer = gameLogic.GetRenderer( );

	if ( CreateOrenNayarLUTAndBind( renderer ) == false )
	{
		return false;
	}

	return CreateDeviceDependentResource( renderer );
}

void CLightManager::SpawnLights( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& objectList )
{
	// 속성과 클래스를 연결
	for ( int i = 0; i < MAX_LIGHTS; ++i )
	{
		std::unique_ptr<CLight> newLight = std::make_unique<CLight>( );
		if ( newLight )
		{
			newLight->RegisterProperty( &m_shaderLightProperty.m_properties[i] );
			newLight->Initialize( gameLogic, static_cast<int>( objectList.size( ) ) );
			m_lights[i] = newLight.get( );
			objectList.emplace_back( std::move( newLight ) );
		}
	}

	LoadPropertyFromScript( );
}

void CLightManager::UpdateToRenderer( IRenderer& renderer, const CCamera& camera )
{
	SetCameraPosition( camera.GetOrigin( ) );

	if ( m_needUpdateToRenderer )
	{
		void* lights = renderer.LockBuffer( m_lightBuffer );

		if ( lights )
		{
			memcpy( lights, &m_shaderLightProperty, sizeof( ShaderLightTrait ) );
			renderer.UnLockBuffer( m_lightBuffer );
			renderer.BindConstantBuffer( SHADER_TYPE::PS, PS_CONSTANT_BUFFER::LIGHT, 1, &m_lightBuffer );
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

void CLightManager::PushLightTrait( const LightTrait& trait )
{
	int idx = m_shaderLightProperty.m_curLights;
	::memcpy( &m_shaderLightProperty.m_properties[idx], &trait, sizeof( trait ) );
	++m_shaderLightProperty.m_curLights;
}

CLightManager::CLightManager( ) :
	m_needUpdateToRenderer( false ),
	m_primaryLight( 0 )
{
	RegisterEnumString( );
}

bool CLightManager::CreateDeviceDependentResource( IRenderer& renderer )
{
	BUFFER_TRAIT trait = { sizeof( ShaderLightTrait ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
		0,
		nullptr,
		0,
		0 };

	m_lightBuffer = renderer.CreateBuffer( trait );

	return m_lightBuffer != RE_HANDLE::InValidHandle( );
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
			trait.m_direction = XMVector3Normalize( trait.m_direction );
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

bool CLightManager::CreateOrenNayarLUTAndBind( IRenderer& renderer )
{
	constexpr std::size_t lookupSize = 512;

	float* lookup = new float[lookupSize * lookupSize];

	for ( std::size_t i = 0; i < lookupSize; ++i )
	{
		for ( std::size_t j = 0; j < lookupSize; ++j )
		{
			float VdotN = static_cast<float>( i ) / lookupSize;
			float LdotN = static_cast<float>( j ) / lookupSize;

			VdotN *= 2.f;
			VdotN -= 1.f;

			LdotN *= 2.f;
			LdotN -= 1.f;

			float alpha = std::max( acosf( VdotN ), acosf( LdotN ) );
			float beta = std::min( acosf( VdotN ), acosf( LdotN ) );

			lookup[i + j * lookupSize] = sinf( alpha ) * tanf( beta );
		}
	}

	RESOURCE_INIT_DATA initData{ lookup, sizeof( float ) * lookupSize, 0 };

	IResourceManager& resourceMgr = renderer.GetResourceManager( );

	RE_HANDLE hTexture = resourceMgr.CreateTexture2D( OREN_NAYAR_TEX_NAME, OREN_NAYAR_TEX_NAME, &initData );
	if ( hTexture != RE_HANDLE::InValidHandle( ) )
	{
		RE_HANDLE hLUT = resourceMgr.CreateTextureShaderResource( hTexture, OREN_NAYAR_TEX_NAME );
		renderer.BindShaderResource( SHADER_TYPE::PS, 3, 1, &hLUT );
#ifdef _DEBUG
		renderer.TakeSnapshot2D( OREN_NAYAR_TEX_NAME, OREN_NAYAR_SNAPSHOT_NAME );
#endif
		delete[] lookup;
		return true;
	}

	delete[] lookup;
	return false;
}

void CLightManager::OnDeviceRestore( CGameLogic& gameLogic )
{
	m_needUpdateToRenderer = true;
	IRenderer& renderer = gameLogic.GetRenderer( );
	CreateDeviceDependentResource( renderer );
	CreateOrenNayarLUTAndBind( renderer );
}
