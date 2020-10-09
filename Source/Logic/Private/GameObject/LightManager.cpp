#include "stdafx.h"
#include "GameObject/LightManager.h"

#include "Core/GameLogic.h"
#include "Components/CameraComponent.h"
#include "DataStructure/EnumStringMap.h"
#include "FileSystem/EngineFileSystem.h"
#include "Json/json.hpp"
//#include "Render/IRenderer.h"
//#include "Render/IRenderResourceManager.h"
#include "Scene/ConstantBufferDefine.h"
#include "Util.h"

#include <algorithm>
#include <cstddef>

using namespace DirectX;

namespace
{
	constexpr char* CONST_BUFFER_NAME = "Lights";
	constexpr char* LIGHT_PROPERTY_FILE_NAME = "./Scripts/WorldLight.json";

	constexpr char* OREN_NAYAR_TEX_NAME = "OrenNayarLookUP";
	constexpr char* OREN_NAYAR_SNAPSHOT_NAME = "DebugOrenNayarLookUP";

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
	//IRenderer& renderer = gameLogic.GetRenderer( );

	//if ( CreateOrenNayarLUTAndBind( renderer ) == false )
	//{
	//	return false;
	//}

	//return CreateDeviceDependentResource( renderer );

	// 속성과 클래스를 연결
	for ( int i = 0; i < MAX_LIGHTS; ++i )
	{
		Owner<CLight*> newLight = new CLight;
		newLight->RegisterProperty( &m_shaderLightProperty.m_properties[i] );
		m_lights[i] = newLight;
		gameLogic.SpawnObject( newLight );
	}

	IFileSystem* fileSystem = GetInterface<IFileSystem>( );
	FileHandle lightAsset = fileSystem->OpenFile( LIGHT_PROPERTY_FILE_NAME );

	if ( lightAsset.IsValid( ) == false )
	{
		return false;
	}

	unsigned long fileSize = fileSystem->GetFileSize( lightAsset );
	char* buffer = new char[fileSize];

	IFileSystem::IOCompletionCallback ParseLightAsset;
	ParseLightAsset.BindFunctor(
		[this, lightAsset]( const char* buffer, unsigned long bufferSize )
		{
			LoadProperty( buffer, static_cast<size_t>( bufferSize ) );

			delete[] buffer;
			GetInterface<IFileSystem>( )->CloseFile( lightAsset );
		}
	);

	bool result = fileSystem->ReadAsync( lightAsset, buffer, fileSize, &ParseLightAsset );
	if ( result == false )
	{
		delete[] buffer;
		GetInterface<IFileSystem>( )->CloseFile( lightAsset );
	}

	return result;
}

void CLightManager::UpdateToRenderer( IRenderer& renderer, const CameraComponent& camera )
{
	//SetCameraPosition( camera.GetOrigin( ) );

	//if ( m_needUpdateToRenderer )
	//{
	//	void* lights = renderer.LockBuffer( m_lightBuffer );

	//	if ( lights )
	//	{
	//		memcpy( lights, &m_shaderLightProperty, sizeof( ShaderLightTrait ) );
	//		renderer.UnLockBuffer( m_lightBuffer );
	//		renderer.BindConstantBuffer( SHADER_TYPE::PS, PS_CONSTANT_BUFFER::LIGHT, 1, &m_lightBuffer );
	//	}

	//	m_needUpdateToRenderer = false;
	//}
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
	//BUFFER_TRAIT trait = { sizeof( ShaderLightTrait ),
	//	1,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
	//	RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
	//	0,
	//	nullptr,
	//	0,
	//	0 };

	//m_lightBuffer = renderer.CreateBuffer( trait );

	//return m_lightBuffer != RE_HANDLE::InValidHandle( );
	return true;
}

void CLightManager::LoadProperty( const char* lightAsset, size_t assetSize )
{
	JSON::Value root( JSON::DataType::EMPTY );
	JSON::Reader reader;

	if ( reader.Parse( lightAsset, assetSize, root ) )
	{
		if ( const JSON::Value* pGlobalAmbient = root.Find( "globalAmbient" ) )
		{
			if ( pGlobalAmbient->Size( ) == 4 )
			{
				const JSON::Value& globalAmbient = *pGlobalAmbient;

				CXMFLOAT4 ambientColor( static_cast<float>( globalAmbient[0].AsReal( ) ),
					static_cast<float>( globalAmbient[1].AsReal( ) ),
					static_cast<float>( globalAmbient[2].AsReal( ) ),
					static_cast<float>( globalAmbient[3].AsReal( ) ) );
				SetGlobalAmbient( ambientColor );
			}
		}

		if ( const JSON::Value* pLights = root.Find( "Lights" ) )
		{
			for ( const JSON::Value& light : *pLights )
			{
				LightTrait trait;

				if ( const JSON::Value* pType = light.Find( "type" ) )
				{
					trait.m_type = GetEnum( pType->AsString( ), LIGHT_TYPE::NONE );
				}

				if ( const JSON::Value* pOnOff = light.Find( "onOff" ) )
				{
					trait.m_isOn = pOnOff->AsBool( );
				}

				if ( const JSON::Value* pTheta = light.Find( "theta" ) )
				{
					trait.m_theta = static_cast<float>( pTheta->AsReal( ) );
				}

				if ( const JSON::Value* pPhi = light.Find( "phi" ) )
				{
					trait.m_phi = static_cast<float>( pPhi->AsReal( ) );
				}

				if ( const JSON::Value* pDirection = light.Find( "direction" ) )
				{
					const JSON::Value& direction = *pDirection;

					if ( direction.Size( ) == 3 )
					{
						trait.m_direction.x = static_cast<float>( direction[0].AsReal( ) );
						trait.m_direction.y = static_cast<float>( direction[1].AsReal( ) );
						trait.m_direction.z = static_cast<float>( direction[2].AsReal( ) );
						trait.m_direction = XMVector3Normalize( trait.m_direction );
					}
				}

				if ( const JSON::Value* pRange = light.Find( "range" ) )
				{
					trait.m_range = static_cast<float>( pRange->AsReal( ) );
				}

				if ( const JSON::Value* pFallOff = light.Find( "fallOff" ) )
				{
					trait.m_fallOff = static_cast<float>( pFallOff->AsReal( ) );
				}

				if ( const JSON::Value* pAttenuation = light.Find( "attenuation" ) )
				{
					const JSON::Value& attenuation = *pAttenuation;

					if ( attenuation.Size( ) == 3 )
					{
						trait.m_attenuation.x = static_cast<float>( attenuation[0].AsReal( ) );
						trait.m_attenuation.y = static_cast<float>( attenuation[1].AsReal( ) );
						trait.m_attenuation.z = static_cast<float>( attenuation[2].AsReal( ) );
					}
				}

				if ( const JSON::Value* pPosition = light.Find( "position" ) )
				{
					const JSON::Value& position = *pPosition;

					if ( position.Size( ) == 3 )
					{
						trait.m_position.x = static_cast<float>( position[0].AsReal( ) );
						trait.m_position.y = static_cast<float>( position[1].AsReal( ) );
						trait.m_position.z = static_cast<float>( position[2].AsReal( ) );
					}
				}

				if ( const JSON::Value* pDiffuse = light.Find( "diffuse" ) )
				{
					const JSON::Value& diffuse = *pDiffuse;

					if ( diffuse.Size( ) == 4 )
					{
						trait.m_diffuse.x = static_cast<float>( diffuse[0].AsReal( ) );
						trait.m_diffuse.y = static_cast<float>( diffuse[1].AsReal( ) );
						trait.m_diffuse.z = static_cast<float>( diffuse[2].AsReal( ) );
						trait.m_diffuse.w = static_cast<float>( diffuse[3].AsReal( ) );
					}
				}

				if ( const JSON::Value* pSpecular = light.Find( "specular" ) )
				{
					const JSON::Value& specular = *pSpecular;

					if ( specular.Size( ) == 4 )
					{
						trait.m_specular.x = static_cast<float>( specular[0].AsReal( ) );
						trait.m_specular.y = static_cast<float>( specular[1].AsReal( ) );
						trait.m_specular.z = static_cast<float>( specular[2].AsReal( ) );
						trait.m_specular.w = static_cast<float>( specular[3].AsReal( ) );
					}
				}

				PushLightTrait( trait );
			}
		}
	}
}

bool CLightManager::CreateOrenNayarLUTAndBind( IRenderer& renderer )
{
//	constexpr std::size_t lookupSize = 512;
//
//	float* lookup = new float[lookupSize * lookupSize];
//
//	for ( std::size_t i = 0; i < lookupSize; ++i )
//	{
//		for ( std::size_t j = 0; j < lookupSize; ++j )
//		{
//			float VdotN = static_cast<float>( i ) / lookupSize;
//			float LdotN = static_cast<float>( j ) / lookupSize;
//
//			VdotN *= 2.f;
//			VdotN -= 1.f;
//
//			LdotN *= 2.f;
//			LdotN -= 1.f;
//
//			float alpha = std::max( acosf( VdotN ), acosf( LdotN ) );
//			float beta = std::min( acosf( VdotN ), acosf( LdotN ) );
//
//			lookup[i + j * lookupSize] = sinf( alpha ) * tanf( beta );
//		}
//	}
//
//	RESOURCE_INIT_DATA initData{ lookup, sizeof( float ) * lookupSize, 0 };
//
//	IResourceManager& resourceMgr = renderer.GetResourceManager( );
//
//	RE_HANDLE hTexture = resourceMgr.CreateTexture2D( OREN_NAYAR_TEX_NAME, OREN_NAYAR_TEX_NAME, &initData );
//	if ( hTexture != RE_HANDLE::InValidHandle( ) )
//	{
//		RE_HANDLE hLUT = resourceMgr.CreateTextureShaderResource( hTexture, OREN_NAYAR_TEX_NAME );
//		renderer.BindShaderResource( SHADER_TYPE::PS, 3, 1, &hLUT );
//#ifdef _DEBUG
//		renderer.TakeSnapshot2D( OREN_NAYAR_TEX_NAME, OREN_NAYAR_SNAPSHOT_NAME );
//#endif
//		delete[] lookup;
//		return true;
//	}
//
//	delete[] lookup;
//	return false;

	return true;
}

void CLightManager::OnDeviceRestore( CGameLogic& gameLogic )
{
	//m_needUpdateToRenderer = true;
	//IRenderer& renderer = gameLogic.GetRenderer( );
	//CreateDeviceDependentResource( renderer );
	//CreateOrenNayarLUTAndBind( renderer );
}
